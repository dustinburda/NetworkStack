//
// Created by Dustin Burda on 9/28/23.
//

#ifndef CIRCBUFFER_INTERVALS_H
#define CIRCBUFFER_INTERVALS_H

#include <vector>
#include <set>


using Interval = std::vector<uint64_t>; //TODO used hard coded fixed size array
using IntervalIt = std::set<Interval>::iterator;


class IntervalSet{
public:
    IntervalSet() : interval_set {} {}

    IntervalSet(const std::set<Interval>& i_set) : interval_set{i_set} {}

    void merge_and_insert(const Interval& i) {
        std::vector<IntervalIt> mergeable;

        auto contained = get_mergeable(i, mergeable);

        if(i[0] == i[1]) //empty interval
            return;

        if(contained)
            return;

        auto interval_start = i[0];
        auto interval_end = i[1];

        // TODO don't need loop, set is sorted
        for(auto interval_it : mergeable) {
            interval_start = std::min(interval_start, (*interval_it)[0]);
            interval_end = std::max(interval_end, (*interval_it)[1]);

            interval_set.erase(interval_it);
        }

        interval_set.insert({interval_start, interval_end});
    }

    static void show_interval(const Interval& i) {
        std::cout << "[" << i[0] << ", " << i[1] << ") ";
    }

    void show_set() {
        for( const auto& interval : interval_set ) {
            show_interval(interval);
        }
        std::cout << "\n";
    }

    const std::set<Interval>& return_set() const { return interval_set; }
    std::set<Interval>& return_modifiable_set() { return interval_set; }

    size_t size() const { return interval_set.size(); }
    bool empty() const { return interval_set.empty(); }

private:

    static bool contains(uint64_t val, uint64_t start, uint64_t end) {
        return (val >= start && val <= end);
    }

    // @return - true if Interval i is contained entirely in some interval in interval_set
    bool get_mergeable(const Interval& i, std::vector<IntervalIt>& mergeable) {
        // TODO limit search space of loop
        for(auto it = interval_set.begin(); it != interval_set.end(); it++) {
            auto end_points = *it;

            auto start = i[0];
            auto end = i[1];

            auto si = end_points[0];
            auto ei = end_points[1];

            if(contains(start, si, ei) && contains(end, si, ei))
                return true;

            if(contains(si, start, end) || contains(ei, start, end)) {
                mergeable.push_back(it);
            }
        }

        return false;
    }

    std::set<Interval> interval_set;
};

[[maybe_unused]]
static std::ostream& operator<<(std::ostream& os, const IntervalSet& is) {
    for( const auto& interval : is.return_set()) {
        os << "[" << interval[0] << ", " << interval[1] << ") ";
    }
    os << "\n";
    return os;
}


#endif //CIRCBUFFER_INTERVALS_H
