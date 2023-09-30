#include "reassembler.hh"

#define LOGGING 0

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring, Writer& output )
{
    /*
        0. If is_last_substring == true, then last_index = first_index + data.size()

        1. trim data in range of [first_unassembled_index, first_unnacceptable_index)
            a. first_unassembled_index = Writer.bytes_pushed() + 1
            b. first_unacceptable_index = first_unassembled_index + Writer.available_capacity()

        (1.1. Set size of circular buffer just once, can't do it without constructor)
        (1.2. Update first unpopped index for pretty printing)

        2. Push data in circular buffer

        3. Push interval into interval set

        4. If the interval with lowest index has start == first_unassembled_index
                pop this interval off,
                record index (start) and length(end - start)
                Pop "length" amount of bytes at index from circular buffer
                Push this string into Writer

           else return

        5. If Writer.bytes_pushed() == last_index_, call Writer.close()
     * */

    // 0
    if(is_last_substring){
        last_index_ = first_index + data.size();
        b_last_substring_recieved_ = true;
    }


    // 1
    first_unpopped_index_ = output.reader().bytes_popped();
    first_unassembled_index_ = output.bytes_pushed();
    first_unacceptable_index_ = first_unassembled_index_ + output.available_capacity();


    // 1.1
    if(output.capacity() > 0)
        fixed_buffer_.set_size(output.capacity());


    // 1.2
    fixed_buffer_.set_first_unpopped(first_unpopped_index_);

    uint64_t old_start { first_index };
    [[maybe_unused]] uint64_t old_end { first_index + data.size() };

    uint64_t start { first_index };
    uint64_t end { first_index + data.size() };

    start = std::min(start, first_unacceptable_index_);
    start = std::max(start, first_unassembled_index_);

    end = std::min(end, first_unacceptable_index_);
    end = std::max(end, first_unassembled_index_);

#if LOGGING
    std::cout << "Data: " << data << " Data Size: " << data.size() << " First Index: " << first_index << "\n";
    std::cout << "Start: " << start << " End: " << end << "\n";
#endif // LOGGING

    if(end < start )
        return;

    if(start < old_start) {
#if LOGGING
        std::cout << "old_start: " << old_start << "\n";
        std::cout << "start <= old_start\n";
#endif // LOGGING
        return;
    }


    if(start > old_start)
        std::memmove(data.data(), data.data() + start - old_start, end - start);
    data.resize( end - start );
    // TODO can't just resize

    // return;

#if LOGGING
    std::cout << "New Data: " << data << "\n";
#endif // LOGGING

    // 2
    fixed_buffer_.insert(start, data);

    // 3
    intervalset_.merge_and_insert({start, end});

    //4

    if(!intervalset_.empty()) {
        Interval smallest = *( intervalset_.return_set().begin() );

        if(smallest[0] == first_unassembled_index_) {
            intervalset_.return_modifiable_set().erase( intervalset_.return_set().begin() );

            size_t length = smallest[1] - smallest[0];

            std::string buffer;
            fixed_buffer_.pop(smallest[0], length, buffer);

            output.push(buffer);
        }
    }



    // 5
    if(b_last_substring_recieved_ && output.bytes_pushed() == last_index_) {
#if LOGGING
        std::cout << "BS bytes_pushed: " << output.bytes_pushed() << "  last_index: " << last_index_ << "\n";
#endif // LOGGING
        output.close();
#if LOGGING
        std::cout << "BS output.is_closed: " << output.is_closed() << " BS reader is_finished: " << output.reader().is_finished() << "\n";
#endif // LOGGING
    }
}

uint64_t Reassembler::bytes_pending() const
{
  uint64_t pending_size = 0;

  for(const Interval& i : intervalset_.return_set()) {
        pending_size += (i[1] - i[0]);
  }

  return pending_size;
}
