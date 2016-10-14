#ifndef INTERVAL_H
#define INTERVAL_H

namespace grynca {

    class Interval {
    public:
        Interval(float min = 0.0f, float max = 0.0f);

        float getMin()const;
        float getMax()const;
        void setMin(float m);
        void setMax(float m);

        bool overlaps(const Interval& i);
        bool overlaps(const Interval& i, float& penetration);
        bool contains(const Interval& i);

    private:
        float min_;
        float max_;
    };
}

#include "Interval.inl"
#endif //INTERVAL_H
