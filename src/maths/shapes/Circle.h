#ifndef CIRCLE_H
#define CIRCLE_H

#include "../Vec2.h"

namespace grynca {
    // fw
    class OverlapInfo;
    class Rect;
    class ARect;

    class Circle {
        friend std::ostream& operator << (std::ostream& os, Circle& c);
    public:
        Circle(const Vec2& center = {0,0}, float radius = 0);

        void setCenter(const Vec2& c);
        void setRadius(float r);

        const Vec2& getCenter()const;
        float getRadius()const;

        ARect calcARectBound()const;

        bool overlaps(const Circle& c)const;
        bool overlaps(const Circle& c, OverlapInfo& oi)const;
        bool overlaps(const ARect& r)const;
        bool overlaps(const ARect& r, OverlapInfo& oi)const;
        bool overlaps(const Rect& r)const;
        bool overlaps(const Rect& r, OverlapInfo& oi)const;
    private:
        Vec2 c_;
        float r_;
    };

}

#include "Circle.inl"
#endif //CIRCLE_H
