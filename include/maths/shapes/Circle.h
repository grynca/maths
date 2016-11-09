#ifndef CIRCLE_H
#define CIRCLE_H

#include "../Vec2.h"

namespace grynca {
    // fw
    class OverlapInfo;
    class Rect;
    class ARect;
    class Ray;
    class Pgon;

    class Circle {
        friend std::ostream& operator << (std::ostream& os, Circle& c);
    public:
        Circle(const Vec2& center = {0,0}, f32 radius = 0);

        void setCenter(const Vec2& c);
        void setRadius(f32 r);

        const Vec2& getCenter()const;
        f32 getRadius()const;

        ARect calcARectBound()const;

        bool overlaps(const Ray& r)const;
        bool overlaps(const Ray& r, OverlapInfo& oi)const;
        bool overlaps(const Circle& c)const;
        bool overlaps(const Circle& c, OverlapInfo& oi)const;
        bool overlaps(const ARect& r)const;
        bool overlaps(const ARect& r, OverlapInfo& oi)const;
        bool overlaps(const Rect& r)const;
        bool overlaps(const Rect& r, OverlapInfo& oi)const;
        bool overlaps(const Pgon& p)const;
        bool overlaps(const Pgon& p, OverlapInfo& oi)const;
    private:
        Vec2 c_;
        f32 r_;
    };

}

#include "Circle.inl"
#endif //CIRCLE_H
