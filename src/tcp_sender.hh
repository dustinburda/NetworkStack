#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"
#include "Timer.h"
#include <deque>

class TCPSender
{
  Wrap32 isn_;
  uint64_t initial_RTO_ms_;
  uint64_t current_RTO_ms_;

  Timer t;

  std::optional<Wrap32> ackno_;
  uint16_t window_size_;

  struct OutSeg {
      TCPSenderMessage tcp_message_;
      uint64_t timestamp_;
  };
  std::deque<TCPSenderMessage> pushed_segments_;
  std::deque<OutSeg> segments_in_flight_;

  uint64_t time_alive_;

  uint64_t num_consecutive_retransmissions_;

  bool b_syn_sent_;
  bool b_fin_sent_;


  uint64_t absolute_seqno_;

  bool b_retransmit_;

  bool dont_back_off_rto_;


public:
  /* Construct TCP sender with given default Retransmission Timeout and possible ISN */
  TCPSender( uint64_t initial_RTO_ms, std::optional<Wrap32> fixed_isn );

  /* Push bytes from the outbound stream */
  void push( Reader& outbound_stream );

  /* Send a TCPSenderMessage if needed (or empty optional otherwise) */
  std::optional<TCPSenderMessage> maybe_send();

  /* Generate an empty TCPSenderMessage */
  TCPSenderMessage send_empty_message() const;

  /* Receive an act on a TCPReceiverMessage from the peer's receiver */
  void receive( const TCPReceiverMessage& msg );

  /* Time has passed by the given # of milliseconds since the last time the tick() method was called. */
  void tick( uint64_t ms_since_last_tick );

  /* Accessors for use in testing */
  uint64_t sequence_numbers_in_flight() const;  // How many sequence numbers are outstanding?
  uint64_t consecutive_retransmissions() const; // How many consecutive *re*transmissions have happened?
};
