#include "network_interface.hh"

#include "ethernet_frame.hh"

using namespace std;

InternetDatagram datagram_create( const std::string& src_ip, const std::string& dst_ip );
ARPMessage arp_create( const uint16_t opcode,
                              const EthernetAddress sender_ethernet_address,
                              const std::string& sender_ip_address,
                              const EthernetAddress target_ethernet_address,
                              const std::string& target_ip_address );

EthernetFrame frame_create( const EthernetAddress& src,
                                   const EthernetAddress& dst,
                                   const uint16_t type,
                                   std::vector<Buffer> payload );

// ethernet_address: Ethernet (what ARP calls "hardware") address of the interface
// ip_address: IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( const EthernetAddress& ethernet_address, const Address& ip_address )
  : ethernet_address_( ethernet_address ), ip_address_( ip_address ), time_alive_{0}, ip_eth_map_ {}, time_entries_{},
  datagram_q_{}, arp_request_times_{}, ethernet_q_{}
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address_ ) << " and IP address "
       << ip_address.ip() << "\n";
}

// dgram: the IPv4 datagram to be sent
// next_hop: the IP address of the interface to send it to (typically a router or default gateway, but
// may also be another host if directly connected to the same network as the destination)

// Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) by using the
// Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{
    uint32_t next_hop_addr = next_hop.ipv4_numeric();
    uint32_t dgram_src_addr = dgram.header.src;
    if(ip_eth_map_.find(next_hop_addr) != ip_eth_map_.end()) {
        EthernetAddress dst = ip_eth_map_[next_hop_addr];
        auto eth_frame = frame_create(ethernet_address_, dst, EthernetHeader::TYPE_IPv4, serialize(dgram));
        ethernet_q_.push_back(eth_frame);
    } else{
        if(arp_request_times_.find(dgram_src_addr) == arp_request_times_.end() ||  arp_request_times_[dgram_src_addr] - time_alive_ > 5) {
            auto arp_message = arp_create(ARPMessage::OPCODE_REQUEST, ethernet_address_, ip_address_.to_string(), ETHERNET_BROADCAST, next_hop.to_string());
            auto eth_frame = frame_create(ethernet_address_, ETHERNET_BROADCAST, EthernetHeader::TYPE_ARP, serialize(arp_message));
            ethernet_q_.push_back(eth_frame);

            datagram_q_.push_back(dgram);
            arp_request_times_[dgram_src_addr] = time_alive_;
        }
    }
}

// frame: the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame( const EthernetFrame& frame )
{

    // when ARP reply is recieved for some datagram in datagram_deque, remove it and send ethernet frame
  (void)frame;
  return {};
}

// ms_since_last_tick: the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
    time_alive_ += ms_since_last_tick;
    while(!time_entries_.empty()) {
        auto time_entry = time_entries_.begin();

        if(time_entry->expiration_time <= time_alive_ + 30) {
            ip_eth_map_.erase(time_entry->it);
            time_entries_.erase(time_entry);
        } else {
            break;
        }
    }
}

optional<EthernetFrame> NetworkInterface::maybe_send()
{
    std::optional<EthernetFrame> ret = nullopt;
    if(!ethernet_q_.empty()) {
        ret = ethernet_q_.front();
        ethernet_q_.pop_front();
    }
    return ret;
}

//
//InternetDatagram datagram_create( const std::string& src_ip, const std::string& dst_ip ) // NOLINT(*-swappable-*)
//{
//    InternetDatagram dgram;
//    dgram.header.src = Address( src_ip, 0 ).ipv4_numeric();
//    dgram.header.dst = Address( dst_ip, 0 ).ipv4_numeric();
//    dgram.payload.emplace_back( "hello" );
//    dgram.header.len = static_cast<uint64_t>( dgram.header.hlen ) * 4 + dgram.payload.front().size();
//    dgram.header.compute_checksum();
//    return dgram;
//}

ARPMessage arp_create( const uint16_t opcode,
                              const EthernetAddress sender_ethernet_address,
                              const std::string& sender_ip_address,
                              const EthernetAddress target_ethernet_address,
                              const std::string& target_ip_address )
{
    ARPMessage arp;
    arp.opcode = opcode;
    arp.sender_ethernet_address = sender_ethernet_address;
    arp.sender_ip_address = Address( sender_ip_address, 0 ).ipv4_numeric();
    arp.target_ethernet_address = target_ethernet_address;
    arp.target_ip_address = Address( target_ip_address, 0 ).ipv4_numeric();
    return arp;
}

EthernetFrame frame_create( const EthernetAddress& src,
                                   const EthernetAddress& dst,
                                   const uint16_t type,
                                   std::vector<Buffer> payload )
{
    EthernetFrame frame;
    frame.header.src = src;
    frame.header.dst = dst;
    frame.header.type = type;
    frame.payload = std::move( payload );
    return frame;
}