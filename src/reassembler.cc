#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring, Writer& output )
{
    /*
        0. If is_last_substring == true, then last_index = first_index + data.size()      -------------------------> DONE

        1. trim data in range of [first_unassembled_index, first_unnacceptable_index)     -------------------------> DONE
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




====================================================
        3. Merge overlapping strings in temp storage

        4. Pop all contiguous bytes off temp storage

        (2, 4 will slow down reassembler TODO try to optimize after tests pass)

        5. If Writer.bytes_pushed() == last_index_, call Writer.close()
===================================================
     * */


    // TODO Set size of Circular Buffer because it's not known without Reassembler

    // 0
    if(is_last_substring)
        last_index_ = first_index + data.size();

    // 1
    first_unpopped_index_ = output.reader().bytes_popped();
    first_unassembled_index_ = output.bytes_pushed();
    first_unacceptable_index_ = first_unassembled_index_ + output.available_capacity();

    // 1.1
    fixed_buffer_.set_size(output.capacity());

    // 1.2
    fixed_buffer_.set_first_unpopped(first_unpopped_index_);

    uint64_t start { first_index };
    uint64_t end { first_index + data.size() };

    start = std::min(start, first_unacceptable_index_);
    start = std::max(start, first_unassembled_index_);

    end = std::min(end, first_unacceptable_index_);
    end = std::max(end, first_unassembled_index_);


    if( end - start <= 0 )
        return;

    data.resize( end - start );

    // 2
    fixed_buffer_.insert(start, data);

    // 3
    intervalset_.merge_and_insert({start, end});

    //4
    Interval smallest = *( intervalset_.return_set().begin() );

    if(smallest[0] == first_unpopped_index_) {
        intervalset_.return_modifiable_set().erase( intervalset_.return_set().begin() );

        size_t length = smallest[1] - smallest[0];

        std::string buffer;
        fixed_buffer_.pop(smallest[0], length, buffer);

        output.push(buffer);
    }


    // 5
    if(b_last_substring_recieved_ && output.bytes_pushed() == last_index_)
        output.close();


  // Your code here.
  (void)first_index;
  (void)data;
  (void)is_last_substring;
  (void)output;
}

uint64_t Reassembler::bytes_pending() const
{
  return reassembler_size_;
}
