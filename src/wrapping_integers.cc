#include "wrapping_integers.hh"
#include <cmath>

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
    return zero_point + static_cast<uint32_t>(n);
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{

    /*
        1. Compute offset from ISN

        2. Compute nearest multiple "k" of 2**32 to checkpoint

        3. Consider (k-1) * 2**32 + offset, k * 2**32 + offset, (k+1) * 2**32 + offset

        4. Pick among them, the value closest to checkpoint
     * */
    uint64_t offset = raw_value_ - zero_point.raw_value_;

    [[maybe_unused]] uint64_t nearest_multiple = checkpoint / static_cast<uint64_t>(std::pow(2, 32));

    uint64_t k_minus_1 = (nearest_multiple - 1) * static_cast<uint64_t>(std::pow(2,32)) + offset;
    uint64_t k = (nearest_multiple) * static_cast<uint64_t>(std::pow(2,32)) + offset;
    uint64_t k_plus_1 = (nearest_multiple + 1) * static_cast<uint64_t>(std::pow(2,32)) + offset;

    uint64_t nums[3] = {k_minus_1, k, k_plus_1};

    uint64_t result = 0;
    uint64_t distance = std::numeric_limits<uint64_t>::max();

    for(int i = 0; i < 3; i++) {
        uint64_t cur_distance = (checkpoint > nums[i]) ? checkpoint - nums[i] : nums[i] - checkpoint;
        if(cur_distance < distance) {
            distance = cur_distance;
            result = nums[i];
        }
    }

    return result;

  // Your code here.
  (void)zero_point;
  (void)checkpoint;
  return {};
}
