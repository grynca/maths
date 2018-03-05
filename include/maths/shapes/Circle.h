#ifndef CIRCLE_H
#define CIRCLE_H

#include "shapes_fw.h"

namespace grynca {

    class Circle {
        friend std::ostream& operator << (std::ostream& os, Circle& c);
    public:
        Circle(const Vec2& center = {0,0}, f32 radius = 0);

        void setCenter(const Vec2& c);
        void setRadius(f32 r);

        const Vec2& getCenter()const;
        f32 getRadius()const;

        ARect calcARectBound()const;

        // only uniform scale works
        // changes "this"
        void transform(const Mat3& tr);
        void transform(const Transform& tr);

        Circle transformOut(const Mat3& tr)const;
        Circle transformOut(const Transform& tr)const;
        Vec2 calcSupport(const Dir2& dir)const;

        bool isPointInside(const Vec2& p)const;

        f32 calcArea()const;

        f32 calcInertia()const;

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
        Vec2 c_;
        f32 r_;
    };

}
#endif //CIRCLE_H

#if !defined(CIRCLE_INL) && !defined(WITHOUT_IMPL)
#define CIRCLE_INL
# include "Circle.inl"
#endif // CIRCLE_INL
