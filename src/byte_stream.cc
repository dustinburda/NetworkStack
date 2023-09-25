#include <stdexcept>

#include "byte_stream.hh"
#include <cstring> //for memmove

using namespace std;

#define LOGGING 0

ByteStream::ByteStream( uint64_t capacity )
    : capacity_( capacity ), current_size_(0), q_(), b_closed_(false), b_error_(false), bytes_pushed_(0), bytes_popped_(0) {}

void Writer::push( string data )
{
#if LOGGING
    std::cout << "Before Push" << "\n";
    print_bytestream();
#endif // LOGGING

  size_t num_bytes = std::min(available_capacity(), static_cast<uint64_t>(data.size()));

  bytes_pushed_ += num_bytes;
  current_size_ += num_bytes;

  data.resize(num_bytes);

  if(num_bytes > 0) {
      Buffer b{std::move(data)};
      q_.push_back(b);
  }


#if LOGGING
  std::cout << "After Push" << "\n";
  print_bytestream();
#endif //LOGGING
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
#if LOGGING
    std::cout << "Before Peek: " << "\n";
    print_bytestream();
#endif //LOGGING

  std::string_view s{""};
  if(!q_.empty()) {
      auto buffer = q_.front();
      s = static_cast<std::string_view>(buffer);
  }

#if LOGGING
  std::cout << "After Peek" << "\n";
  print_bytestream();
#endif // LOGGING
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
#if LOGGING
    std::cout << "Before Pop: " << "\n";
    print_bytestream();
#endif // LOGGING

  size_t num_bytes = std::min(len, static_cast<uint64_t>(current_size_));
#if LOGGING
  std::cout << "In Pop" << "\n";
#endif // LOGGING
  while(!q_.empty() && num_bytes > 0) {
      auto buffer = q_.front();
      size_t pop_length = std::min(num_bytes, buffer.size());
      std::string& s_buffer = static_cast<std::string&>(buffer);
#if LOGGING
      std::cout << "Buffer: " << s_buffer << "\n";
#endif // LOGGING

      bool shrink {false};
      // TODO move length
      // assert( !(buffer.size() < pop_length) );
      if(buffer.size() > pop_length) {
#if LOGGING
          std::cout << "In Buffer Shrink" << "\n";
#endif // LOGGING
          std::memmove((void *) s_buffer.data(), (void*) (s_buffer.data() + pop_length), buffer.size() - pop_length);
          s_buffer.resize(buffer.size() - pop_length);
          shrink = true;

#if LOGGING
          std::cout << "Buffer After Shrink: " << s_buffer << "\n";
          std::cout << "Buffer Object Size: " << buffer.size() << "\n";
          std::cout << "Buffer Object Length: " << buffer.length() << "\n";
#endif // LOGGING
      }

      //if(s_buffer.size() == 0)
      if(!shrink)
        q_.pop_front();

      bytes_popped_ += pop_length;
      num_bytes -= pop_length;
      current_size_ -= pop_length;
      //TODO add to bytes_popped, subtract from num_bytes
  }
#if LOGGING
  std::cout << "In Pop" << "\n";

  std::cout << "After Pop: " << "\n";
  print_bytestream();
#endif // LOGGING
}

uint64_t Reader::bytes_buffered() const
{
  return current_size_;
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_;
}
