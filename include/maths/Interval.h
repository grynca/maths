#ifndef INTERVAL_H
#define INTERVAL_H

namespace grynca {

    class Interval {
    public:
        Interval(f32 min = 0.0f, f32 max = 0.0f);

        f32 getMin()const;
        f32 getMax()const;
        void setMin(f32 m);
        void setMax(f32 m);

        bool overlaps(const Interval& i);
        bool overlaps(const Interval& i, f32& penetration_out);
        bool overlaps(const Interval& i, f32& penetration_out, f32& dir_out);   // finds shortest overlap dir
        bool contains(const Interval& i);

    private:
        f32 min_;
        f32 max_;
    };
}

#include "Interval.inl"
#endif //INTERVAL_H
