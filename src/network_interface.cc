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
  datagram_qs_{}, arp_request_times_{}, ethernet_q_{}
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
    [[maybe_unused]] uint32_t dgram_src_addr = dgram.header.src;
    if(ip_eth_map_.find(next_hop_addr) != ip_eth_map_.end()) {
        EthernetAddress dst = ip_eth_map_[next_hop_addr];
        auto eth_frame = frame_create(ethernet_address_, dst, EthernetHeader::TYPE_IPv4, serialize(dgram));
        ethernet_q_.push_back(eth_frame);
    } else{
        datagram_qs_[next_hop_addr].push_back(dgram);
        if(arp_request_times_.find(next_hop_addr) == arp_request_times_.end() ||  time_alive_ - arp_request_times_[next_hop_addr] > 5000) {
            auto arp_message = arp_create(ARPMessage::OPCODE_REQUEST, ethernet_address_, std::to_string(ip_address_.ipv4_numeric()),
                                          {}, std::to_string(next_hop.ipv4_numeric()));
            auto arp_eth_frame = frame_create(ethernet_address_, ETHERNET_BROADCAST, EthernetHeader::TYPE_ARP, serialize(arp_message));
            ethernet_q_.push_back(arp_eth_frame);

            arp_request_times_[next_hop_addr] = time_alive_;
        }
    }
}

// frame: the incoming Ethernet frame
optional<InternetDatagram> NetworkInterface::recv_frame( const EthernetFrame& frame )
{

    std::optional<InternetDatagram> ret = nullopt;


    if(frame.header.dst != ethernet_address_ && frame.header.dst != ETHERNET_BROADCAST) {
        return ret;
    }


   if(frame.header.type == EthernetHeader::TYPE_IPv4) {
        // send immediately
        InternetDatagram dgram;
        bool parse_success = parse(dgram, frame.payload);
        if(parse_success)
            ret = dgram;

        return ret;
   }

   if(frame.header.type == EthernetHeader::TYPE_ARP) {
       ARPMessage arp_msg;
       bool parse_success = parse(arp_msg, frame.payload);
       if(parse_success) {
           // learn mapping from both request and reply
            ip_eth_map_[arp_msg.sender_ip_address] = arp_msg.sender_ethernet_address;

            MapTimeEntry mte;
            mte.expiration_time = time_alive_ + 30000;
            mte.it = ip_eth_map_.find(arp_msg.sender_ip_address);

            time_entries_.insert(mte);

            // send out all datagrams to sender ip address and sender ethernet address
            for(auto& dgram : datagram_qs_[arp_msg.sender_ip_address]) {
                auto eth_frame = frame_create(ethernet_address_, arp_msg.sender_ethernet_address, EthernetHeader::TYPE_IPv4, serialize(dgram));
                ethernet_q_.push_back(eth_frame);
            }
            datagram_qs_[arp_msg.sender_ip_address].clear();

            arp_request_times_.erase(arp_msg.sender_ip_address);

           if(arp_msg.opcode == ARPMessage::OPCODE_REQUEST) {
               if(arp_msg.target_ip_address != ip_address_.ipv4_numeric())
                   return ret;
                ARPMessage arp_reply = arp_create(ARPMessage::OPCODE_REPLY, ethernet_address_, std::to_string(ip_address_.ipv4_numeric()), arp_msg.sender_ethernet_address, to_string(arp_msg.sender_ip_address));
               auto eth_frame = frame_create(ethernet_address_, arp_msg.sender_ethernet_address, EthernetHeader::TYPE_ARP, serialize(arp_reply));
               ethernet_q_.push_back(eth_frame);
           }
           if(arp_msg.opcode == ARPMessage::OPCODE_REPLY) {
               // nothing for now
           }
       }


       // for mapping,
   }
  return ret;
}

// ms_since_last_tick: the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
    time_alive_ += ms_since_last_tick;
    while(!time_entries_.empty()) {
        auto time_entry = time_entries_.begin();

        if(time_entry->expiration_time <= time_alive_) {
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