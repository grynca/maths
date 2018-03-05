#ifndef RAY_H
#define RAY_H

#include "shapes_fw.h"
#include <bitset>

namespace grynca {

    // 3x HIT cases:
    //          -o->             --|-->  |            |  --|->
    // Impale(t1 hit,t2 hit), Poke(t1 hit,t2>1), ExitWound(t1<0, t2 hit),

    // 4x MISS cases:
    // regular miss +
    //       ->  o                     o ->              | -> |
    // FallShort (t1>1,t2>1), Past (t1<0,t2<0), CompletelyInside(t1<0, t2>1)

    enum RayHitType {
        // misses
        rhtMiss,
        rhtFallShort,
        rhtPast,
        rhtCompletelyInside,
        // hits
        rhtImpale,
        rhtPoke,
        rhtExitWound
    };

    class Ray {
        friend std::ostream& operator << (std::ostream& os, Ray& r);
    public:
        enum BitFlags {
            bDirNormalized,
            bDirInfoComputed,
            bDirXSign,
            bDirYSign,

            bfCount
        };

        struct CalcDirInfo {};
    public:
        Ray();
        Ray(const Vec2& start, const Vec2& end);                    // not normalized
        Ray(const Vec2& start, const Dir2& dir, f32 length);      // dir normalized

        Ray(const Vec2& start, const Vec2& end, CalcDirInfo);                    // not normalized
        Ray(const Vec2& start, const Dir2& dir, f32 length, CalcDirInfo);      // dir normalized

        bool isDirNormalized()const;
        bool isDirInfoCalculated()const;
        void normalizeDirIfNotAlready();
        void calcDirInfoIfNotAlready();
        const Vec2& getStart()const;
        const Dir2& getDir()const;
        const Vec2& getInvDir()const;
        Vec2 getEnd()const;     //start_ + dir_*length_;
        Vec2 getToEndVec()const;
        f32 getLength()const;         // 1 when dir was not normalized yet
        const std::bitset<bfCount>& getFlags()const;

        void setStart(const Vec2& s);
        void setDir(const Dir2& d);     // setting non-normalized dir
        void setDirN(const Dir2& d);    // setting normalied dir
        void setLength(f32 l);        // must be called on normalized dir

        ARect calcARectBound()const;
        // changes "this"
        void transform(const Mat3& tr);
        void transform(const Transform& tr);
        // returns new transformed obj
        Ray transformOut(const Mat3& tr)const;
        Ray transformOut(const Transform& tr)const;

        Vec2 calcSupport(const Dir2& dir)const;

        // eps is tolerance in distance from ray direction
        bool isPointInside(const Vec2& p, f32 eps = maths::EPS)const;

        f32 calcArea()const;
        f32 calcInertia()const;

        static bool calcHitType(f32 t1, f32 t2, f32 ray_len, RayHitType& rht_out);

        template <typename ShapeT>
        bool overlaps(const ShapeT& s)const;

        bool overlaps(const ARect& ar, OverlapTmp& otmp)const;
        void calcContact(const ARect& ar, OverlapTmp& otmp, ContactManifold& cm_out)const;

        bool overlaps(const Circle& c, OverlapTmp& otmp)const;
        void calcContact(const Circle& c, OverlapTmp& otmp, ContactManifold& cm_out)const;

        bool overlaps(const Ray& r, OverlapTmp& otmp)const;
        void calcContact(const Ray& r, OverlapTmp& otmp, ContactManifold& cm_out)const;

        bool overlaps(const Rect& r, OverlapTmp& otmp)const;
        void calcContact(const Rect& r, OverlapTmp& otmp, ContactManifold& cm_out)const;

        bool overlaps(const Pgon& p, OverlapTmp& otmp)const;
        void calcContact(const Pgon& p, OverlapTmp& otmp, ContactManifold& cm_out)const;

    private:
        static void normalize_(Dir2& dir_io, f32& len_out, std::bitset<bfCount>& flags_out);
        static void calcDirInfo_(const Dir2& dir, Vec2& inv_dir_out, std::bitset<bfCount>& flags_out);

        Vec2 start_;
        Dir2 dir_;
        Vec2 inv_dir_;
        f32 length_;
        std::bitset<bfCount> flags_;
    };
}
#endif //RAY_H

#if !defined(RAY_INL) && !defined(WITHOUT_IMPL)
#define RAY_INL
# include "Ray.inl"
#endif // RAY_INL

