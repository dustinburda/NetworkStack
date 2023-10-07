//
// Created by Dustin Burda on 10/7/23.
//

#ifndef TIMER_TIMER_H
#define TIMER_TIMER_H

#include <cstdlib>

class Timer {
public:
    Timer() : time_start_ {0}, timeout_ {0}, b_running_{false} {}

    void start(uint64_t time_start, uint64_t timeout) {
        time_start_ = time_start;
        timeout_ = timeout;
        b_running_ = true;
    }

    void stop() { b_running_ = false; }

    bool is_expired(uint64_t current_time) const { return (b_running_) && current_time >= time_start_ + timeout_; }
    bool is_running() const { return b_running_; }


    //accessors
    uint64_t time_start() const { return time_start_; }
    uint64_t timeout() const { return timeout_; }

private:
    uint64_t time_start_;
    uint64_t timeout_;
    bool b_running_;
};


#endif //TIMER_TIMER_H
