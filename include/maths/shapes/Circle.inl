#include "shapes_h.h"
#include "../Transform.h"
#include "OverlapTmp.h"
#include "ContactManifold.h"

namespace grynca {

    inline Circle::Circle(const Vec2& center, f32 radius)
     : c_(center), r_(radius)
    {
    }

    inline void Circle::setCenter(const Vec2& c) {
        c_ = c;
    }

    inline void Circle::setRadius(f32 r) {
        r_ = r;
    }

    inline const Vec2& Circle::getCenter()const {
        return c_;
    }

    inline f32 Circle::getRadius()const {
        return r_;
    }

    inline ARect Circle::calcARectBound()const {
        f32 d = r_*2;
        return {c_-Vec2(r_, r_), Vec2(d, d)};
    }

    inline void Circle::transform(const Mat3& tr) {
        Dir2 trad = tr*Dir2(getRadius(), 0);
        c_ = tr*getCenter();
        r_ = trad.getLen();
    }

    inline void Circle::transform(const Transform& tr) {
        ASSERT_M(tr.getScale().getX() == tr.getScale().getY(),
                "use only uniform scale.");

        c_ += tr.getPosition();
        r_ = getRadius()*tr.getScale().getX();
    }

    inline Circle Circle::transformOut(const Mat3& tr)const {
        Dir2 trad = tr*Dir2(getRadius(), 0);
        return Circle(tr*getCenter(), trad.getLen());
    }

    inline Circle Circle::transformOut(const Transform& tr)const {
        ASSERT_M(tr.getScale().getX() == tr.getScale().getY(),
                 "use only uniform scale.");

        return Circle(getCenter()+tr.getPosition(), getRadius()*tr.getScale().getX());
    }

    inline Vec2 Circle::calcSupport(const Dir2& dir)const {
        ASSERT_M((dir.getLen() - 1.0f) < maths::EPS, "dir must be normalized.");
        Vec2 supp = c_ + dir * r_;
        return supp;
    }

    inline bool Circle::isPointInside(const Vec2& p)const {
        return (p-getCenter()).getSqrLen() < (getRadius()*getRadius());
    }

    inline f32 Circle::calcArea()const {
        return Angle::Pi*r_*r_;
    }

    inline f32 Circle::calcInertia()const {
        return getRadius()*getRadius()*0.5f;
    }

    template <typename ShapeT>
    inline bool Circle::overlaps(const ShapeT& s)const {
        OverlapTmp unused;
        return overlaps(s, unused);
    }

    inline bool Circle::overlaps(const ARect& ar, OverlapTmp& otmp)const {
        return ar.overlaps(*this, otmp);
    }

    inline void Circle::calcContact(const ARect& ar, OverlapTmp& otmp, ContactManifold& cm_out)const {
        ar.calcContact(*this, otmp, cm_out);
        cm_out.normal *= -1;
    }

    inline bool Circle::overlaps(const Circle& c, OverlapTmp& otmp)const {
        otmp.circle_circle_.dv = getCenter()-c.getCenter();
        otmp.circle_circle_.cc = otmp.circle_circle_.dv.getSqrLen();
        otmp.circle_circle_.rs = getRadius()+c.getRadius();

        return (otmp.circle_circle_.cc - (otmp.circle_circle_.rs*otmp.circle_circle_.rs)) < (-maths::EPS_SQRT);
    }

    inline void Circle::calcContact(const Circle& c, OverlapTmp& otmp, ContactManifold& cm_out)const {
        f32 d = sqrtf(otmp.circle_circle_.cc);

        cm_out.size = 1;
        // must check if circles are on different positions (for avoiding division by zero)
        if (d==0.0f) {
            cm_out.normal.set(0, -1);
            cm_out.points[0].penetration = getRadius()+c.getRadius();
            cm_out.points[0].position = getCenter();
            return;
        }
        cm_out.normal = otmp.circle_circle_.dv/d;
        cm_out.points[0].penetration = getRadius()+c.getRadius() - d;
        // contact is in middle between surface contact points of both circles
        cm_out.points[0].position = getCenter() +(cm_out.points[0].penetration*0.5f - getRadius()) *cm_out.normal;
    }

    inline bool Circle::overlaps(const Ray& r, OverlapTmp& otmp)const {
        return r.overlaps(*this, otmp);
    }

    inline void Circle::calcContact(const Ray& r, OverlapTmp& otmp, ContactManifold& cm_out)const {
        r.calcContact(*this, otmp, cm_out);
        cm_out.normal *= -1;
    }

    inline bool Circle::overlaps(const Rect& r, OverlapTmp& otmp)const {
        return r.overlaps(*this, otmp);
    }

    inline void Circle::calcContact(const Rect& r, OverlapTmp& otmp, ContactManifold& cm_out)const {
        r.calcContact(*this, otmp, cm_out);
        cm_out.normal *= -1;
    }

    inline bool Circle::overlaps(const Pgon& p, OverlapTmp& otmp)const {
        return p.overlaps(*this, otmp);
    }

    inline void Circle::calcContact(const Pgon& p, OverlapTmp& otmp, ContactManifold& cm_out)const {
        p.calcContact(*this, otmp, cm_out);
        cm_out.normal *= -1;
    }

    inline std::ostream& operator << (std::ostream& os, Circle& c) {
        os << "Circle= c:" << c.getCenter() << ", r:" << c.r_;
        return os;
    }
}