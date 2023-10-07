#include "tcp_sender.hh"
#include "tcp_config.hh"

#include <random>

using namespace std;

/* TCPSender constructor (uses a random ISN if none given) */
TCPSender::TCPSender( uint64_t initial_RTO_ms, optional<Wrap32> fixed_isn )
  : isn_( fixed_isn.value_or( Wrap32 { random_device()() } ) ), initial_RTO_ms_( initial_RTO_ms ), current_RTO_ms_{initial_RTO_ms_}, t{},
    ackno_{nullopt}, window_size_{0},  pushed_segments_{}, segments_in_flight_{}, time_alive_{0}, num_consecutive_retransmissions_{0},
    b_syn_sent_{false}, b_fin_sent_{false}, absolute_seqno_{0}, b_retransmit_{false}
{}

uint64_t TCPSender::sequence_numbers_in_flight() const
{
  uint64_t amount_in_flight = 0;

  for(auto out_seg : segments_in_flight_) {
      amount_in_flight += out_seg.tcp_message_.sequence_length();
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
      segments_in_flight_.front().timestamp_ = time_alive_;
      return segments_in_flight_.front().tcp_message_;
  }
  if(!pushed_segments_.empty()) {
      segment_to_send = pushed_segments_.front();
      pushed_segments_.pop_front();


      segments_in_flight_.emplace_back(*segment_to_send, time_alive_);

      return segments_in_flight_.back().tcp_message_;
  }

  return segment_to_send;
}

void TCPSender::push( Reader& outbound_stream )
{
  // Your code here.
  (void)outbound_stream;
}

TCPSenderMessage TCPSender::send_empty_message() const
{
  TCPSenderMessage message_to_send;
  message_to_send.seqno = Wrap32::wrap(absolute_seqno_, isn_);

  return message_to_send;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  ackno_ = msg.ackno;
  window_size_ = msg.window_size;
}

void TCPSender::tick( const size_t ms_since_last_tick )
{
  // Your code here.
  (void)ms_since_last_tick;
}
