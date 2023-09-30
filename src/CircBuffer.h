//
// Created by Dustin Burda on 9/28/23.
//

#ifndef CIRCBUFFER_CIRCBUFFER_H
#define CIRCBUFFER_CIRCBUFFER_H


#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <algorithm>

class CircBuffer {
public:
    CircBuffer() : buffer_{}, first_unpopped_index_{0}, size_{0} {}

    explicit CircBuffer(size_t size) : buffer_{}, first_unpopped_index_{0}, size_{size} {
        buffer_.resize(size);
        std::memset(buffer_.data(), ' ', size_);
    }

    void set_size(size_t size) {
        buffer_.resize(size);
        std::memset(buffer_.data(), ' ', size_);
    }

    void insert(uint64_t first_index, std::string data) {
        uint64_t index = (first_index) % size_; // Convert absolute index to
        size_t first_size = size_ - index; // Space left from index to end of buffer

        // std::string first, second;
        // TODO replace with std::memcpy calls
        if( first_size < data.size() ) {
//            first = data.substr(0, first_size);
//            second = data.substr(first_size, data.size() - first_size);

//            for(int i = 0; i < first_size; i++) {
//                buffer_[index + i] = first[i];
//            }
            std::memcpy(buffer_.data() + index, data.data(), first_size);


//            for(int i = 0; i < data.size() - first_size; i++) {
//                buffer_[i] = second[i];
//            }

            std::memcpy(buffer_.data(), data.data() + first_size, data.size() - first_size);

        }
        else {
//            for(int i = 0; i < data.size(); i++) {
//                buffer_[index + i] = data[i];
//            }

            std::memcpy(buffer_.data() + index, data.data(), data.size());
        }
    }

    void pop_bs(size_t delta) {
        first_unpopped_index_ += delta;
    }

    // @outparam data
    void pop(uint64_t index, size_t amount, std::string& data) {
        data.resize(amount);
        std::memset(data.data(), ' ', amount); //resize and zero out;

        size_t distance_to_end = size_ - (index % size_);

        if(distance_to_end < amount) {
            std::memcpy(data.data(), buffer_.data() + (index % size_), distance_to_end);
            std::memset(buffer_.data() + (index % size_), ' ', distance_to_end);

            std::memcpy(data.data() + distance_to_end, buffer_.data(), data.size() - distance_to_end);
            std::memset(buffer_.data(), ' ', data.size() - distance_to_end);
        } else {
            // TODO   Likely a mistake here
            std::memcpy(data.data(), buffer_.data() + (index % size_), amount);
            std::memset(buffer_.data() + (index % size_), ' ', amount);
        }
    }

    void set_first_unpopped( uint64_t new_unpopped_index) { first_unpopped_index_ = new_unpopped_index; }

    // Accessors
    const std::vector<char>& buffer() const { return buffer_; }
    size_t size() const { return buffer_.size(); }
    size_t capacity() const { return size_; }
    uint64_t first_unpopped() const { return first_unpopped_index_; }

    bool empty() const { return  buffer_.size() == 0; }
private:
    std::vector<char> buffer_;
    uint64_t first_unpopped_index_; // only to print
    size_t size_;
};

[[maybe_unused]]
static std::ostream& operator<<(std::ostream& os, const CircBuffer& c) {
    for(size_t i = 0; i < 4 * c.size() + 1; i++) {
        std::cout << "=";
    }
    std::cout << "\n";

    int offset = c.first_unpopped() % c.capacity();

    for(size_t i = 0; i < c.capacity(); i++) {
        if(i == static_cast<size_t>(offset) )
            os <<"| * ";
        os << "|   ";
    }
    os << "\n";


    for(size_t i = 0; i < c.capacity(); i++) {
        if( i < static_cast<size_t>(offset) ) {
            os << "|" << (c.first_unpopped() - offset + i + c.capacity());
        } else {
            os << "|" << (c.first_unpopped() - offset + i);
        }

    }
    if(!c.empty())
        os << "|";
    std::cout << "\n";


    int i = 0;
    std::for_each(c.buffer().begin(), c.buffer().end(), [&os, &i]([[maybe_unused]] char elem) {
        os << "| " << i;
        if(i < 10)
            os << " ";
        i++;
    });
    if(!c.empty())
        os << "|";
    os << "\n";

    std::for_each(c.buffer().begin(), c.buffer().end(), [&os](char elem) {
        os << "| " << elem << " ";
    });
    if(!c.empty())
        os << "|";
    os << "\n";

    for(size_t j = 0; j < 4 * c.size() + 1; j++) {
        std::cout << "=";
    }
    std::cout << "\n";


    std::cout << "\n";

    return os;
}


#endif //CIRCBUFFER_CIRCBUFFER_H
