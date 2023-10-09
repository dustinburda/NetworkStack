#include "tcp_sender.hh"
#include "tcp_config.hh"

#include <random>
#include <algorithm>

using namespace std;

static void show_deque(const std::deque<TCPSenderMessage>& d) {
    // front to back
    std::cout << "Pushed Segment======\nOldest\n";
    uint64_t i = 0;
    for(auto it = d.begin(); it != d.end(); it++) {
        it->show(std::to_string(i));
    }
    std::cout << "Newest\nPushed Segment======\n\n";
}

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
      std::cout << "Retransmitting.... ";
      b_retransmit_ = false;
      //front is oldest, back is newest
      segments_in_flight_.front().timestamp_ = time_alive_;
      // if(window_size_ > 0) TODO, uncomment this?
        // num_consecutive_retransmissions_++;
        std::cout << " Num Retransmission: " << num_consecutive_retransmissions_ << "\n";
      std::cout << "Maybe Send End======================================\n\n";
      return segments_in_flight_.front().tcp_message_;

  }
  if(!pushed_segments_.empty()) {

      // front to back
      show_deque(pushed_segments_);
//      std::cout << "Pushed Segment======\n";
//      uint64_t i = 0;
//      for(auto it = pushed_segments_.begin(); it != pushed_segments_.end(); it++) {
//          it->show(std::to_string(i));
//      }
//      std::cout << "Pushed Segment======\n";


      std::cout << "Sending new segment.... \n";
      segment_to_send = pushed_segments_.front();
      pushed_segments_.pop_front();



//      std::cout << "Pushing onto segments_inflight_: Segment seqno: " << (*segment_to_send).seqno.unwrap(isn_, absolute_seqno_) << "  payload: " << (std::string)((*segment_to_send).payload) << "payload size: " << (*segment_to_send).payload.size()
//      << " syn: " << (*segment_to_send).SYN << " fin: " <<  (*segment_to_send).FIN << "\n\n";
      (*segment_to_send).show("Push segments_in_flight_");

      if(!pushed_segments_.empty()) {
          const auto& back_segment = pushed_segments_.back();
          back_segment.show("Back");
      } else {
          std::cout << "No Back\n";
      }

      // segments_in_flight_.emplace_back(*segment_to_send, time_alive_);
      segments_in_flight_.push_back({*segment_to_send, time_alive_});
      // absolute_seqno_ += segments_in_flight_.back().tcp_message_.sequence_length();

      if(!t.is_running())
          t.start(time_alive_, current_RTO_ms_);

      auto& sending_segment = segments_in_flight_.back().tcp_message_;

      sending_segment.show("Sending");
//      std::cout << "Sending: Segment seqno: " << (sending_segment).seqno.unwrap(isn_, absolute_seqno_) << "  payload: " << (std::string)((sending_segment).payload) << "payload size: " << sending_segment.payload.size()
//                << " syn: " << (sending_segment).SYN << " fin: " <<  (sending_segment).FIN << "\n";

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
//          std::cout << "Pushing Segment, seqno: " << segment.seqno.unwrap(isn_, absolute_seqno_)  << " payload size: " << segment.payload.size() << " payload: "
//                    << std::string(segment.payload) << " syn: " << segment.SYN << " fin: " << segment.FIN << "\n\n";
        segment.show("Push into pushed_segments_");
          pushed_segments_.push_back(segment);
      }


      if(outbound_stream.bytes_buffered() == 0)
          break;
  }

    show_deque(pushed_segments_);

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

//    // TODO check that ackno not greater than abs seqno + 1 when unwrapped
    if(msg.ackno->unwrap(isn_, absolute_seqno_) > absolute_seqno_)
        return;

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

      std::cout << "Popped Segment Seqno: " << segment.seqno.unwrap(isn_, absolute_seqno_) << " Payload: " << (std::string)segment.payload
                << " syn: " << segment.SYN << " fin:  " << segment.FIN << "\n";
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
    std::cout << "Tick Begin=====================================\n";
    std::cout << "Old Time: " << time_alive_ << "\n";
    time_alive_ += ms_since_last_tick;
    std::cout << "New Time: " << time_alive_ << "\n";

    if(t.is_expired(time_alive_)) {
        std::cout << "Timer Expired\n";
        if(!segments_in_flight_.empty()) {
            std::cout << "Setting b_transmit to true\n";
            b_retransmit_ = true;
        }

        if(window_size_ > 0) {
            std::cout << "Doubling Retransmission Timeout\n";
            // TODO: increment num_consecutive_retransmissions_ here or in maybe_send?
            current_RTO_ms_ *= 2;
            num_consecutive_retransmissions_++;
        }

        t.start(time_alive_, current_RTO_ms_);

    }
    std::cout << "Tick End========================================\n\n";
}
