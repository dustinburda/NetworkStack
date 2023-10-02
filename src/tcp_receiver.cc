#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream )
{

    if(!b_syn_recv_ && message.SYN) {
        isn_ = message.seqno;
        b_syn_recv_ = true;
    }

    if(b_syn_recv_) {

    }
    /*
        1. If !b_syn_recieved (hence no ISN and no way to properly unwrap seqno in packet), DO NOTHING
            UNLESS bool SYN set in TCPSenderMessage message
        2.
        3.
        4.
        5.
     * */
//  // Your code here.
//  (void)message;
//  (void)reassembler;
//  (void)inbound_stream;
}

TCPReceiverMessage TCPReceiver::send( const Writer& inbound_stream ) const
{
  TCPReceiverMessage response;

  response.ackno = (b_syn_recv_) ? std::optional<Wrap32>(Wrap32::wrap(inbound_stream.bytes_pushed(), isn_)) : nullopt;
  response.window_size = inbound_stream.available_capacity();

  return response;
}
