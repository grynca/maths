#ifndef RAY_H
#define RAY_H

#include "../Vec2.h"

namespace grynca {

    // fw
    class OverlapInfo;
    class Circle;
    class ARect;
    class Rect;

    class Ray {
    public:
        Ray();
        Ray(const Vec2& start, const Vec2& dir, float length);       // dir should be normalized

        const Vec2& getStart()const;
        const Vec2& getDir()const;
        Vec2 getEnd()const;     //start_ + dir_*length_;
        Vec2 getToEndVec()const;
        float getLength()const;

        void setStart(const Vec2& s);
        void setDir(const Vec2& d);
        void setLength(float l);

        bool overlaps(const Ray& r)const;
        bool overlaps(const Ray& r, OverlapInfo& oi)const;
        bool overlaps(const Circle& c)const;
        bool overlaps(const Circle& c, OverlapInfo& oi)const;
        bool overlaps(const ARect& r)const;
        bool overlaps(const ARect& r, OverlapInfo& oi)const;
        bool overlaps(const Rect& r)const;
        bool overlaps(const Rect& r, OverlapInfo& oi)const;
    private:
        Vec2 start_;
        Vec2 dir_;
        float length_;
    };
}

#include "Ray.inl"
#endif //RAY_H
