#pragma once

#include <queue>
#include <deque>
#include <stdexcept>
#include <string>
#include <string_view>
#include "buffer.hh"
#include <iostream>

class Reader;
class Writer;

class ByteStream
{
protected:
  uint64_t capacity_;
  // Please add any additional state to the ByteStream here, and not to the Writer and Reader interfaces.
  size_t current_size_;
  std::deque<Buffer> q_;
  bool b_closed_;
  bool b_error_;
  size_t bytes_pushed_;
  size_t bytes_popped_;

public:
  explicit ByteStream( uint64_t capacity );

  // Helper functions (provided) to access the ByteStream's Reader and Writer interfaces
  Reader& reader();
  const Reader& reader() const;
  Writer& writer();
  const Writer& writer() const;

  void print_bytestream() const {
      std::cout << "Capacity: " << capacity_ << "\n"
                << "Current Size: " << current_size_ << "\n"
                << "Is Closed: " << b_closed_ << "\n"
                << "Error Set?: " << b_error_ << "\n"
                << "Bytes Pushed: " << bytes_pushed_ << "\n"
                << "Bytes Popped: " << bytes_popped_ << "\n";

      std::cout << "Buffer:  " << "\n";
      for(const Buffer& buffer : q_) {
          std::cout << static_cast<std::string >(buffer) << "\n";
      }
      std::cout << std::endl;
  }

  uint64_t capacity() const { return capacity_; }
};

class Writer : public ByteStream
{
public:
  void push( std::string data ); // Push data to stream, but only as much as available capacity allows.

  void close();     // Signal that the stream has reached its ending. Nothing more will be written.
  void set_error(); // Signal that the stream suffered an error.

  bool is_closed() const;              // Has the stream been closed?
  uint64_t available_capacity() const; // How many bytes can be pushed to the stream right now?
  uint64_t bytes_pushed() const;       // Total number of bytes cumulatively pushed to the stream
};

class Reader : public ByteStream
{
public:
  std::string_view peek() const; // Peek at the next bytes in the buffer
  void pop( uint64_t len );      // Remove `len` bytes from the buffer

  bool is_finished() const; // Is the stream finished (closed and fully popped)?
  bool has_error() const;   // Has the stream had an error?

  uint64_t bytes_buffered() const; // Number of bytes currently buffered (pushed and not popped)
  uint64_t bytes_popped() const;   // Total number of bytes cumulatively popped from stream
};

/*
 * read: A (provided) helper function thats peeks and pops up to `len` bytes
 * from a ByteStream Reader into a string;
 */
void read( Reader& reader, uint64_t len, std::string& out );
