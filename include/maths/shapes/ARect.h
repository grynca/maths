#ifndef ARECT_H
#define ARECT_H

#include "../Vec2.h"

namespace grynca {
    // fw
    class OverlapInfo;
    class Circle;
    class Rect;
    class Ray;
    class Pgon;

    class ARect {
        friend std::ostream& operator<<(std::ostream& os, ARect& ar);
        friend bool operator==(const ARect& lhs, const ARect& rhs);
    public:
        ARect(const Vec2& lt = {0,0}, const Vec2& size = {0, 0});
        ARect(const Vec2 bounds[2]);
        ARect(const Vec2* points, u32 points_cnt);     // calculates bound for points

        f32 getWidth()const;
        f32 getHeight()const;
        f32 getX()const;
        f32 getY()const;
        Vec2 getSize()const;

        // 4 f32s - left, top, right, bottom
        f32* getDataPtr();
        const f32* getDataPtr() const;

        const Vec2& getLeftTop()const;
        Vec2 getRightTop()const;
        const Vec2& getRightBot()const;
        Vec2 getLeftBot()const;
        Vec2 getCenter()const;
        const Vec2* getBounds()const;
        Vec2* accBounds();
        bool isZero();

        void setLeftTop(const Vec2& lt);
        void setRightBot(const Vec2& rb);
        void setSize(const Vec2& s);

        void expand(const ARect& r);

        ARect calcARectBound()const;

        bool overlaps(const Ray& r)const;
        bool overlaps(const Ray& r, OverlapInfo& oi)const;
        bool overlaps(const ARect& r)const;
        bool overlaps(const ARect& r, OverlapInfo& oi)const;
        bool overlaps(const Circle& c)const;
        bool overlaps(const Circle& c, OverlapInfo& oi)const;
        bool overlaps(const Rect& r)const;
        bool overlaps(const Rect& r, OverlapInfo& oi)const;
        bool overlaps(const Pgon& p)const;
        bool overlaps(const Pgon& p, OverlapInfo& oi)const;

        bool isPointInside(const Vec2& p);

        Circle calcCircleBound();

    private:
        Vec2 bounds_[2];
    };
}


#include "ARect.inl"
#endif //ARECT_H
