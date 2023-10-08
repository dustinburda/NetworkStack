#include "tcp_sender.hh"
#include "tcp_config.hh"

#include <random>
#include <algorithm>

using namespace std;

/* TCPSender constructor (uses a random ISN if none given) */
TCPSender::TCPSender( uint64_t initial_RTO_ms, optional<Wrap32> fixed_isn )
  : isn_( fixed_isn.value_or( Wrap32 { random_device()() } ) ), initial_RTO_ms_( initial_RTO_ms ), current_RTO_ms_{initial_RTO_ms_}, t{},
    ackno_{isn_}, window_size_{1},  pushed_segments_{}, segments_in_flight_{}, time_alive_{0}, num_consecutive_retransmissions_{0},
    b_syn_sent_{false}, b_fin_sent_{false}, absolute_seqno_{0}, b_retransmit_{false}
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

    std::cout << "Maybe Send Begin======================================\n";
  std::optional<TCPSenderMessage> segment_to_send = nullopt;
  if(b_retransmit_) {
      std::cout << "Retransmitting.... \n";
      b_retransmit_ = false;
      //front is oldest, back is newest
      segments_in_flight_.front().timestamp_ = time_alive_;
      // if(window_size_ > 0) TODO, uncomment this?
        num_consecutive_retransmissions_++;
      std::cout << "Maybe Send End======================================\n\n";
      return segments_in_flight_.front().tcp_message_;

  }
  if(!pushed_segments_.empty()) {
      std::cout << "Sending new segment.... ";
      segment_to_send = pushed_segments_.front();
      pushed_segments_.pop_front();

      std::cout << " Segment seqno: " << (*segment_to_send).seqno.unwrap(isn_, absolute_seqno_) << "  payload:  " << (std::string)((*segment_to_send).payload)
      << " syn: " << (*segment_to_send).SYN << " fyn: " <<  (*segment_to_send).FIN << "\n";

      segments_in_flight_.emplace_back(*segment_to_send, time_alive_);

      // absolute_seqno_ += segments_in_flight_.back().tcp_message_.sequence_length();

      if(!t.is_running())
          t.start(time_alive_, current_RTO_ms_);

      std::cout << "Maybe Send End======================================\n\n";
      return segments_in_flight_.back().tcp_message_;

  }
    std::cout << "Maybe Send End======================================\n\n";
  return segment_to_send;
}

void TCPSender::push( Reader& outbound_stream )
{
    std::cout << "Push Begin============================\n";
    TCPSenderMessage segment;

    uint64_t total_bytes_to_push = window_size_ - (absolute_seqno_ - ackno_->unwrap(isn_, absolute_seqno_)); // TODO, account for ack (test send_transmit)

    std::cout << "Total Bytes to Push Before Syn: " << total_bytes_to_push << "\n";

  if(!b_syn_sent_) {
      b_syn_sent_ = true;
      segment.SYN = true;
      total_bytes_to_push -= 1;
      absolute_seqno_ += 1;

  }

    std::cout << "Total Bytes to Push After Syn: " << total_bytes_to_push << "\n";
  while(total_bytes_to_push > 0 || (segment.SYN)) {
      std::cout << "In Loop, Buffer: " << outbound_stream.bytes_buffered() << "\n";

      auto segment_size = std::min<size_t>({TCPConfig::MAX_PAYLOAD_SIZE, total_bytes_to_push, outbound_stream.bytes_buffered()});

      read(outbound_stream, segment_size, segment.payload);

      total_bytes_to_push -= segment.payload.size();
      segment.seqno = Wrap32::wrap(absolute_seqno_ - segment.SYN, isn_);

      absolute_seqno_ += segment.payload.size();


      if(!b_fin_sent_ && outbound_stream.is_finished() && total_bytes_to_push > 0) {
          b_fin_sent_ |= true;
          total_bytes_to_push -= 1;
          absolute_seqno_ += 1;
          segment.FIN = true;
      }

      if( (segment_size > 0) || (segment.FIN) || (segment.SYN)) {
          std::cout << "Pushing Segment, payload size: " << segment.payload.size() << " payload: " << std::string(segment.payload) << " syn: " << segment.SYN << " fin: " << segment.FIN << "\n";
          pushed_segments_.push_back(segment);
      }


      if(outbound_stream.bytes_buffered() == 0)
          break;
  }
  std::cout << "Push End==========================\n\n";
}

TCPSenderMessage TCPSender::send_empty_message() const
{
  TCPSenderMessage message_to_send;
  message_to_send.seqno = Wrap32::wrap(absolute_seqno_, isn_);

  return message_to_send;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
    std::cout << "Receive Begin=========================================\n";
  auto old_ackno = ackno_;
  ackno_ = msg.ackno;
  window_size_ = msg.window_size;

  std::cout << "Old Ackno: " << old_ackno->unwrap(isn_, absolute_seqno_) << "  New Ackno: " << ackno_->unwrap(isn_, absolute_seqno_) << "\n";

  while(segments_in_flight_.size()) {
      auto& segment = segments_in_flight_.front().tcp_message_;

      auto abs_seqno_segment = segment.seqno.unwrap(isn_, absolute_seqno_);
      auto length_segment = segment.sequence_length();
      if(abs_seqno_segment + length_segment  > ackno_->unwrap(isn_,absolute_seqno_))
        break;

      std::cout << "Popped Segment: " << segment.seqno.unwrap(isn_, absolute_seqno_) << "\n";
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
    std::cout << "Receive End=========================================\n\n";
}

void TCPSender::tick( const size_t ms_since_last_tick )
{
    time_alive_ += ms_since_last_tick;

    if(t.is_expired(time_alive_)) {
        if(!segments_in_flight_.empty()) {
            b_retransmit_ = true;
        }

        if(window_size_ > 0) {
            // TODO: increment num_consecutive_retransmissions_ here or in maybe_send?
            current_RTO_ms_ *= 2;
        }

        t.start(time_alive_, current_RTO_ms_);

    }
}
