#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream )
{

    if(!b_syn_recv_ && message.SYN) {
        isn_ = message.seqno;
        b_syn_recv_ = true;
    }

    if(b_syn_recv_) {
        Wrap32 seqno = message.seqno;
        if(message.SYN)
            seqno = seqno + static_cast<uint32_t>(1);

        uint64_t stream_index = seqno.unwrap(isn_, inbound_stream.bytes_pushed()) - 1;

        bool is_last_substring = message.FIN;
        b_fin_recv_ |= message.FIN;
        Buffer payload = message.payload;

        reassembler.insert(stream_index, static_cast<std::string&>(payload), is_last_substring, inbound_stream);
    }
}

TCPReceiverMessage TCPReceiver::send( const Writer& inbound_stream ) const
{
  TCPReceiverMessage response;

  response.ackno = (b_syn_recv_) ? std::optional<Wrap32>(Wrap32::wrap(inbound_stream.bytes_pushed() + 1 + (b_fin_recv_ && inbound_stream.is_closed()), isn_)) : nullopt;
  response.window_size = std::min(inbound_stream.available_capacity(), static_cast<uint64_t>(UINT16_MAX));

  return response;
}
