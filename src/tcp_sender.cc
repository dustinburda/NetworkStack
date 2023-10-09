#include "tcp_sender.hh"
#include "tcp_config.hh"

#include <random>
#include <algorithm>

using namespace std;


/* TCPSender constructor (uses a random ISN if none given) */
TCPSender::TCPSender( uint64_t initial_RTO_ms, optional<Wrap32> fixed_isn )
  : isn_( fixed_isn.value_or( Wrap32 { random_device()() } ) ), initial_RTO_ms_( initial_RTO_ms ), current_RTO_ms_{initial_RTO_ms_}, t{},
    ackno_{isn_}, window_size_{1},  pushed_segments_{}, segments_in_flight_{}, time_alive_{0}, num_consecutive_retransmissions_{0},
    b_syn_sent_{false}, b_fin_sent_{false}, absolute_seqno_{0}, b_retransmit_{false}, dont_back_off_rto_{false}
{}

uint64_t TCPSender::sequence_numbers_in_flight() const
{
  uint64_t amount_in_flight = 0;

  for(auto out_seg : segments_in_flight_) {
      amount_in_flight += out_seg.tcp_message_.sequence_length();
  }

  for(auto push_seg : pushed_segments_) {
      amount_in_flight += push_seg.sequence_length();
  }

  return amount_in_flight;
}

uint64_t TCPSender::consecutive_retransmissions() const
{
  return num_consecutive_retransmissions_;
}

optional<TCPSenderMessage> TCPSender::maybe_send()
{

  std::optional<TCPSenderMessage> segment_to_send = nullopt;
  if(b_retransmit_) {
      b_retransmit_ = false;

      segments_in_flight_.front().timestamp_ = time_alive_;
      return segments_in_flight_.front().tcp_message_;

  }
  if(!pushed_segments_.empty()) {

      segment_to_send = pushed_segments_.front();
      pushed_segments_.pop_front();

      segments_in_flight_.push_back({*segment_to_send, time_alive_});


      if(!t.is_running())
          t.start(time_alive_, current_RTO_ms_);


      return segments_in_flight_.back().tcp_message_;

  }

  return segment_to_send;
}

void TCPSender::push( Reader& outbound_stream )
{
    TCPSenderMessage segment;

    uint64_t total_bytes_to_push = window_size_ - (absolute_seqno_ - ackno_->unwrap(isn_, absolute_seqno_));

  if(!b_syn_sent_) {
      b_syn_sent_ = true;
      segment.SYN = true;
      total_bytes_to_push -= 1;
      absolute_seqno_ += 1;

  }

  while(total_bytes_to_push > 0 || (segment.SYN)) {

      auto segment_size = std::min<size_t>({TCPConfig::MAX_PAYLOAD_SIZE, total_bytes_to_push, outbound_stream.bytes_buffered()});


      // segment.payload = Buffer
      Buffer b;
      read(outbound_stream, segment_size, b);
      segment.payload = b;

      total_bytes_to_push -= segment.payload.size();
      segment.seqno = Wrap32::wrap(absolute_seqno_ - segment.SYN, isn_);

      absolute_seqno_ += segment.payload.size();


      if(!b_fin_sent_ && outbound_stream.is_finished() && (total_bytes_to_push > 0 || (total_bytes_to_push == 0 && segment.SYN))) {
          b_fin_sent_ |= true;
          total_bytes_to_push -= 1;
          absolute_seqno_ += 1;
          segment.FIN = true;
      }

      if( (segment_size > 0) || (segment.FIN) || (segment.SYN)) {
          pushed_segments_.push_back(segment);
      }


      if(outbound_stream.bytes_buffered() == 0)
          break;
  }
}

TCPSenderMessage TCPSender::send_empty_message() const
{
  TCPSenderMessage message_to_send;
  message_to_send.seqno = Wrap32::wrap(absolute_seqno_, isn_);

  return message_to_send;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
    if(msg.ackno->unwrap(isn_, absolute_seqno_) > absolute_seqno_)
        return;



  auto old_ackno = ackno_;
  ackno_ = msg.ackno;
  if(msg.window_size > 0) {
      window_size_ = msg.window_size;
      dont_back_off_rto_ = false;
  }
  else {
      window_size_ = 1;
      dont_back_off_rto_ = true;
  }

  while(segments_in_flight_.size()) {
      auto& segment = segments_in_flight_.front().tcp_message_;

      auto abs_seqno_segment = segment.seqno.unwrap(isn_, absolute_seqno_);
      auto length_segment = segment.sequence_length();
      if(abs_seqno_segment + length_segment  > ackno_->unwrap(isn_,absolute_seqno_))
        break;

      segments_in_flight_.pop_front();
  }

  if(segments_in_flight_.empty())
      t.stop();


  if(old_ackno->unwrap(isn_, absolute_seqno_) < ackno_->unwrap(isn_, absolute_seqno_)) {
      current_RTO_ms_ = initial_RTO_ms_;
      num_consecutive_retransmissions_ = 0;

      if(!segments_in_flight_.empty()) {
          t.start(time_alive_, current_RTO_ms_);
      }
  }
}

void TCPSender::tick( const size_t ms_since_last_tick )
{
    time_alive_ += ms_since_last_tick;

    if(t.is_expired(time_alive_)) {
        if(!segments_in_flight_.empty()) {
            b_retransmit_ = true;
        }

        if(window_size_ > 0) {
            if(!dont_back_off_rto_) {
                current_RTO_ms_ *= 2;
            }
            num_consecutive_retransmissions_++;
        }

        t.start(time_alive_, current_RTO_ms_);

    }
}
