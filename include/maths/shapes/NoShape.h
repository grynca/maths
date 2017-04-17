#ifndef NOSHAPE_H
#define NOSHAPE_H

#include "ARect.h"

namespace grynca {

    // fw
    class OverlapInfo;
    class Rect;
    class ARect;
    class Ray;
    class Pgon;
    class Circle;
    class Mat3;

    class NoShape {
        friend std::ostream& operator << (std::ostream& os, NoShape&);
    public:
        ARect calcARectBound()const { return ARect(); }
        void transform(const Mat3& tr) {}

        bool overlaps(const Ray& r)const { return false; }
        bool overlaps(const Ray& r, OverlapInfo& oi)const { return false; }
        bool overlaps(const ARect& r)const { return false; }
        bool overlaps(const ARect& r, OverlapInfo& oi)const { return false; }
        bool overlaps(const Circle& c)const { return false; }
        bool overlaps(const Circle& c, OverlapInfo& oi)const { return false; }
        bool overlaps(const Rect& r)const { return false; }
        bool overlaps(const Rect& r, OverlapInfo& oi)const { return false; }
        bool overlaps(const Pgon& p)const { return false; }
        bool overlaps(const Pgon& p, OverlapInfo& oi)const { return false; }
        bool overlaps(const NoShape& p)const { return false; }
        bool overlaps(const NoShape& p, OverlapInfo& oi)const { return false; }
    };


    inline std::ostream& operator<<(std::ostream& os, NoShape&) {
        os << "NoShape" << std::endl;
        return os;
    }
}

#endif //NOSHAPE_H
