#include "Interval.h"
#include <algorithm>

namespace grynca {

    inline Interval::Interval(float min, float max)
     : min_(min), max_(max)
    {}

    inline float Interval::getMin()const {
        return min_;
    }

    inline float Interval::getMax()const {
        return max_;
    }

    inline void Interval::setMin(float m) {
        min_ = m;
    }
    inline void Interval::setMax(float m) {
        max_ = m;
    }

    inline bool Interval::overlaps(const Interval& i) {
        return max_ > i.min_ && min_ < i.max_;
    }

    inline bool Interval::overlaps(const Interval& i, float& penetration) {
        float p1 = max_ - i.min_;
        if (p1 <= 0)
            return false;
        float p2 = i.max_ - min_;
        if (p2 <= 0)
            return false;

        penetration = std::min(p1, p2);
        return true;
    }

    inline bool Interval::contains(const Interval& i) {
        return i.min_ > min_ && i.max_ < max_;
    }


}