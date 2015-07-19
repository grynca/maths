#ifndef ARECT_H
#define ARECT_H

#include "../Vec2.h"

namespace grynca {
    // fw
    class OverlapInfo;
    class Circle;
    class Rect;

    class ARect {
        friend std::ostream& operator<<(std::ostream& os, ARect& ar);
        friend bool operator==(const ARect& lhs, const ARect& rhs);
    public:
        ARect(const Vec2& lt = {0,0}, const Vec2& size = {0, 0});

        float getWidth()const;
        float getHeight()const;
        float getX()const;
        float getY()const;
        const Vec2& getSize()const;

        const Vec2& getLeftTop()const;
        Vec2 getRightTop()const;
        Vec2 getRightBot()const;
        Vec2 getLeftBot()const;
        Vec2 getCenter()const;

        bool isZero();

        void setLeftTop(const Vec2& lt);
        void setSize(const Vec2& s);

        bool overlaps(const ARect& r)const;
        bool overlaps(const ARect& r, OverlapInfo& oi)const;
        bool overlaps(const Circle& c)const;
        bool overlaps(const Circle& c, OverlapInfo& oi)const;
        bool overlaps(const Rect& r)const;
        bool overlaps(const Rect& r, OverlapInfo& oi)const;

        bool isPointInside(const Vec2& p);

        Circle calcCircleBound();
    private:
        Vec2 lefttop_;
        Vec2 size_;
    };
}


#include "ARect.inl"
#endif //ARECT_H
