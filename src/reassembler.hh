#pragma once

#include "byte_stream.hh"

#include <string>
#include <iostream>
#include <map>


class Reassembler
{
public:
    Reassembler() : first_unassembled_index_{0}, temp_storage_{}, last_index_{0}, b_last_substring_recieved_{false} {}

  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring, Writer& output );

  // How many bytes are stored in the Reassembler itself?
  uint64_t bytes_pending() const;


  /*
   * Accessors
   * */
  uint64_t first_unassembled_index() const { return first_unassembled_index_; }
  const std::map<uint64_t, Buffer>& temp_storage() const { return temp_storage_; }
  size_t last_index() const { return last_index_; }

private:
    uint64_t first_unassembled_index_;
    std::map<uint64_t, Buffer> temp_storage_;
    size_t last_index_;
    bool b_last_substring_recieved_;
};

[[maybe_unused]]
static std::ostream& operator<<(std::ostream& os, const Reassembler& r) {
    os << "===== Reassembler Begin ====" << "\n";
    os << "First Unassembled Index: " << r.first_unassembled_index() << "\n";

    os << "Temp Storage: \n";
    for(const auto& [index, buffer] : r.temp_storage() ) {
        os << index << " ---> " << static_cast<std::string_view>(buffer) << "\n";
    }

    os << "===== Reassembler End ====" << "\n";
    return os;
}