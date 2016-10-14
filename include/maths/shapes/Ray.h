#ifndef RAY_H
#define RAY_H

#include "../Vec2.h"
#include <bitset>

namespace grynca {

    // fw
    class OverlapInfo;
    class Circle;
    class ARect;
    class Rect;
    class Pgon;

    class Ray {
    public:
        Ray();
        Ray(const Vec2& start, const Vec2& end);                    // not normalized
        Ray(const Vec2& start, const Vec2& dir, float length);      // dir normalized

        bool isDirNormalized()const;
        bool isDirInfoCalculated()const;
        void normalizeDirIfNotAlready();
        void calcDirInfoIfNotAlready();
        const Vec2& getStart()const;
        const Vec2& getDir()const;
        Vec2 getEnd()const;     //start_ + dir_*length_;
        Vec2 getToEndVec()const;
        float getLength()const;         // 1 when dir was not normalized yet

        void setStart(const Vec2& s);
        void setDir(const Vec2& d);     // setting non-normalized dir
        void setDirN(const Vec2& d);    // setting normalied dir
        void setLength(float l);        // must be called on normalized dir

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
        enum BitFlags {
            bDirNormalized,
            bDirInfoComputed,
            bDirXSign,
            bDirYSign,

            bfCount
        };

        static void normalize_(Vec2& dir_io, float& len_out, std::bitset<bfCount>& flags_out);
        static void calcDirInfo_(const Vec2& dir, Vec2& inv_dir_out, std::bitset<bfCount>& flags_out);

        Vec2 start_;
        Vec2 dir_;
        Vec2 inv_dir_;
        float length_;
        std::bitset<bfCount> flags_;
    };
}

#include "Ray.inl"
#endif //RAY_H
