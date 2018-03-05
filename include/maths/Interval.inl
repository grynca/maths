#include "Interval.h"
#include <algorithm>

namespace grynca {

    inline Interval::Interval(f32 min, f32 max)
     : min_(min), max_(max)
    {}

    inline f32 Interval::getMin()const {
        return min_;
    }

    inline f32 Interval::getMax()const {
        return max_;
    }

    inline void Interval::setMin(f32 m) {
        min_ = m;
    }
    inline void Interval::setMax(f32 m) {
        max_ = m;
    }

    inline bool Interval::overlaps(const Interval& i) {
        return max_ > i.min_ && min_ < i.max_;
    }

    inline bool Interval::overlaps(const Interval& i, f32& penetration_out) {
        f32 p1 = max_ - i.min_;
        if (p1 < maths::EPS)
            return false;
        f32 p2 = i.max_ - min_;
        if (p2 < maths::EPS)
            return false;

        penetration_out = p1;
        return true;
    }

    inline bool Interval::overlaps(const Interval& i, f32& penetration_out, f32& dir_out) {
        f32 p1 = max_ - i.min_;
        if (p1 < maths::EPS)
            return false;
        f32 p2 = i.max_ - min_;
        if (p2 < maths::EPS)
            return false;

        if (p2 < p1) {
            dir_out = -1.0f;
            penetration_out = p2;
        }
        else {
            dir_out = 1.0f;
            penetration_out = p1;
        }
        return true;
    }

    inline bool Interval::contains(const Interval& i) {
        return i.min_ > min_ && i.max_ < max_;
    }


}