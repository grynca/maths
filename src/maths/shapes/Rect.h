#ifndef RECT_H
#define RECT_H

#include "../Vec2.h"

namespace grynca {
    // fw
    class ARect;
    class Circle;
    class OverlapInfo;
    class Ray;

    // general (unaligned) rectangle
    class Rect {
        friend std::ostream& operator << (std::ostream& os, const Rect& r);
    public:
        Rect(const Vec2& position = {0,0}, const Vec2& size = {0,0} , const Vec2& offset = {0, 0}, Angle rot = {0});

        Vec2 getLeftTop()const;
        Vec2 getRightTop()const;
        Vec2 getRightBot()const;
        Vec2 getLeftBot()const;
        void getCorners(Vec2* corners)const;       // lt rt rb lb
        Vec2 getWidthDir()const;
        Vec2 getHeightDir()const;
        Vec2 getSizeDir()const;

        const Vec2& getPosition()const;
        const Vec2& getSize()const;
        const Vec2& getOffset()const;
        const Angle& getRotation()const;
        float getSinr()const;
        float getCosr()const;

        bool isZero();

        void setPosition(const Vec2& p);
        void setSize(const Vec2& s);
        void setOffset(const Vec2& o);
        void setRotation(const Angle& rot);

        ARect calcARectBound()const;

        bool overlaps(const Ray& r)const;
        bool overlaps(const Ray& r, OverlapInfo& oi)const;
        bool overlaps(const Rect& r)const;
        bool overlaps(const Rect& r, OverlapInfo& oi)const;
        bool overlaps(const Circle& c)const;
        bool overlaps(const Circle& c, OverlapInfo& oi)const;
        bool overlaps(const ARect& r)const;
        bool overlaps(const ARect& r, OverlapInfo& oi)const;

    private:

        Vec2 getLT_(const Vec2& rot_offset)const;
        Vec2 getRT_(const Vec2& rot_offset)const;
        Vec2 getRB_(const Vec2& rot_offset)const;
        Vec2 getLB_(const Vec2& rot_offset)const;

        Vec2 position_;
        Vec2 size_;
        Vec2 offset_;  // vector offsetting rotation center (from left-top)
        Angle rotation_;
        float sinr_, cosr_; // stored sinus and cosinus of rotation
    };
}

#include "Rect.inl"
#endif //RECT_H
