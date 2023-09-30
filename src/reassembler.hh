#pragma once

#include "byte_stream.hh"

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include "Intervals.h"
#include "CircBuffer.h"


class Reassembler
{
public:
    Reassembler() : first_unassembled_index_{0}, first_unacceptable_index_{0}, first_unpopped_index_{0},
                    fixed_buffer_{}, intervalset_{},  reassembler_size_{0}, last_index_{0}, b_last_substring_recieved_{false} {
        std::cout << "============================== TEST BEGIN ===========================\n";
    }

    ~Reassembler() {
        std::cout << "============================== TEST END ===========================\n\n";
    }

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
  uint64_t first_unacceptable_index() const { return first_unacceptable_index_; }
  uint64_t first_unpopped_index() const { return first_unpopped_index_; }
  const CircBuffer& fixed_buffer() const { return fixed_buffer_; }
  const IntervalSet& interval_set() const { return intervalset_; }
  size_t reassembler_size() const { return reassembler_size_; }
  size_t last_index() const { return last_index_; }
  bool last_substring_recieved() const { return b_last_substring_recieved_; }

private:
    uint64_t first_unassembled_index_;
    uint64_t first_unacceptable_index_;
    uint64_t first_unpopped_index_;

    CircBuffer fixed_buffer_;
    IntervalSet intervalset_;

    size_t reassembler_size_;

    size_t last_index_; // close bytestream when Writer.bytes_pushed() == last_index_
    bool b_last_substring_recieved_; // TODO Do we need this?
};

[[maybe_unused]]
static std::ostream& operator<<(std::ostream& os, const Reassembler& r) {
    os << "===== Reassembler Begin ====" << "\n";
    os << "First Unassembled Index: " << r.first_unassembled_index() << "\n";
    os << "First Unacceptable Index: " << r.first_unacceptable_index() << "\n";
    // TODO Print all member variables

    os << "Buffer: \n";
    os << r.fixed_buffer();
    std::cout << "\n";

    // TODO Show interval_set;

    os << "Interval Set: \n";
    os << r.interval_set();

    os << "Reassembler Size: " << r.reassembler_size() << "\n";
    os << "Last Substring Recieved?: " << r.last_substring_recieved() << "  Last Index: " << r.last_index() << "\n";
    os << "===== Reassembler End ====" << "\n";
    return os;
}