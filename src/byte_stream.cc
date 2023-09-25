#include <stdexcept>

#include "byte_stream.hh"
#include <cstring> //for memmove

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
    : capacity_( capacity ), current_size_(0), q_(), b_closed_(false), b_error_(false), bytes_pushed_(0), bytes_popped_(0) {}

void Writer::push( string data )
{
  size_t num_bytes = std::min(available_capacity(), static_cast<uint64_t>(data.size()));

  bytes_pushed_ += num_bytes;

  data.resize(num_bytes);

  Buffer b{std::move(data)};
  q_.push(b);
}

void Writer::close()
{
  b_closed_ = true;
}

void Writer::set_error()
{
  b_error_ = true;
}

bool Writer::is_closed() const
{
  return b_closed_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - current_size_;
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_;
}

string_view Reader::peek() const
{
  std::string_view s{""};
  if(!q_.empty()) {
      auto buffer = q_.front();
      s = static_cast<std::string_view>(buffer);
  }
    return s;
}

bool Reader::is_finished() const
{
  return ( b_closed_ && (current_size_ == 0) );
}

bool Reader::has_error() const
{
  return b_error_;
}

void Reader::pop( uint64_t len )
{
  size_t num_bytes = std::min(len, static_cast<uint64_t>(current_size_));
  while(!q_.empty() && num_bytes > 0) {
      auto buffer = q_.front();
      size_t pop_length = std::min(num_bytes, buffer.size());
      std::string s_buffer = static_cast<std::string>(buffer);

      // TODO move length
      // assert( !(buffer.size() < pop_length) );
      if(buffer.size() > pop_length) {
          std::memmove((void *) s_buffer.data(), (void*) (s_buffer.data() + pop_length), buffer.size() - pop_length);
          s_buffer.resize(buffer.size() - pop_length);
      }

      if(s_buffer.empty())
          q_.pop();

      bytes_popped_ += pop_length;
      num_bytes -= pop_length;
      //TODO add to bytes_popped, subtract from num_bytes
  }
}

uint64_t Reader::bytes_buffered() const
{
  return current_size_;
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_;
}
