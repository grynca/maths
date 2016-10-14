#include "Circle.h"
#include "OverlapInfo.h"
#include "ARect.h"
#include "Rect.h"
#include "Ray.h"
#include "Pgon.h"

namespace grynca {

    inline Circle::Circle(const Vec2& center, float radius)
     : c_(center), r_(radius)
    {}

    inline void Circle::setCenter(const Vec2& c) {
        c_ = c;
    }

    inline void Circle::setRadius(float r) {
        r_ = r;
    }

    inline const Vec2& Circle::getCenter()const {
        return c_;
    }

    inline float Circle::getRadius()const {
        return r_;
    }

    inline ARect Circle::calcARectBound()const {
        float d = r_*2;
        return {c_-Vec2(r_, r_), Vec2(d, d)};
    }

    inline bool Circle::overlaps(const Ray& r)const {
        return r.overlaps(*this);
    }

    inline bool Circle::overlaps(const Ray& r, OverlapInfo& oi)const {
        bool rslt = r.overlaps(*this, oi);
        oi.dir_out_ = -oi.dir_out_;
        return rslt;
    }

    inline bool Circle::overlaps(const Circle& c)const {
        float radsum_sqr = getRadius()+c.getRadius();
        radsum_sqr *= radsum_sqr;
        return (getCenter() - c.getCenter()).getSqrLen() < radsum_sqr;
    }

    inline bool Circle::overlaps(const Circle& c, OverlapInfo& oi)const {
        Vec2 dv = getCenter()-c.getCenter();
        float cc = dv.getSqrLen();
        float radsum_sqr = getRadius()+c.getRadius();
        radsum_sqr *= radsum_sqr;
        if (cc > radsum_sqr)
            return false;
        float d = sqrt(cc);
        // must check if circles are on different positions (for avoiding division by zero)
        if (d!=0) {
            oi.depth_ = (getRadius()+c.getRadius()) - d;
            oi.dir_out_ = dv/d;
            return true;
        }

        // on same position
        oi.depth_ = r_;
        oi.dir_out_ = {1, 0}; // can be whatever dir
        return true;

    }

    inline bool Circle::overlaps(const ARect& r)const {
        Vec2 r_halfe = r.getSize()*0.5f;
        Vec2 v = getCenter()-r.getCenter();
        // clamp to half extent
        if (v.getX() > 0.0f)
            v.setX(std::min(v.getX(), r_halfe.getX()));
        else
            v.setX(std::max(v.getX(), -r_halfe.getX()));
        if (v.getY() > 0.0f)
            v.setY(std::min(v.getY(), r_halfe.getY()));
        else
            v.setY(std::max(v.getY(), -r_halfe.getY()));

        // closes point on rect to circle
        Vec2 bc = r.getCenter()+v;

        Vec2 dv = getCenter()-bc;
        return dv.getSqrLen() < (getRadius()*getRadius());
    }

    inline bool Circle::overlaps(const ARect& r, OverlapInfo& oi)const {
        Vec2 r_halfe = r.getSize()*0.5f;
        Vec2 v = getCenter()-r.getCenter();
        // clamp to half extent
        if (v.getX() > 0.0f)
            v.setX(std::min(v.getX(), r_halfe.getX()));
        else
            v.setX(std::max(v.getX(), -r_halfe.getX()));
        if (v.getY() > 0.0f)
            v.setY(std::min(v.getY(), r_halfe.getY()));
        else
            v.setY(std::max(v.getY(), -r_halfe.getY()));

        // closes point on rect to circle
        Vec2 bc = r.getCenter()+v;

        Vec2 dv = getCenter()-bc;
        float cr = dv.getSqrLen();
        if (cr > (getRadius()*getRadius()))
            return false;
        float d = sqrt(cr);
        if (d!=0) {
            oi.depth_ = getRadius() - d;
            oi.dir_out_ = dv/d;
            return true;
        }

        // on same position
        oi.depth_ = getRadius();
        oi.dir_out_ = {1, 0}; // can be whatever dir
        return true;

    }

    inline bool Circle::overlaps(const Rect& r)const {
        ARect ar(r.getOffset(), r.getSize());
        // transform center to rect space
        Vec2 tc = getCenter()-r.getPosition();
        tc = tc.rotate(r.getSinr(), r.getCosr());
        Circle c(tc, getRadius());
        return c.overlaps(ar);

    }

    inline bool Circle::overlaps(const Rect& r, OverlapInfo& oi)const {
        ARect ar(r.getOffset(), r.getSize());
        // transform center to rect space
        Vec2 tc = getCenter()-r.getPosition();
        tc = tc.rotate(r.getSinr(), r.getCosr());
        Circle c(tc, getRadius());
        if (!c.overlaps(ar, oi))
            return false;

        // rotate penetration normal back
        oi.dir_out_ = oi.dir_out_.rotate(-r.getSinr(), r.getCosr());
        return true;
    }

    inline bool Circle::overlaps(const Pgon& p)const {
        assert(!"Not implemented yet!");
        return false;
    }

    inline bool Circle::overlaps(const Pgon& p, OverlapInfo& oi)const {
        assert(!"Not implemented yet!");
        return false;
    }

    inline std::ostream& operator << (std::ostream& os, Circle& c) {
        os << "Circle[c=(" << c.c_.getX() << ", " << c.c_.getY() << "), r=" << c.r_ << "]" << std::endl;
        return os;
    }
}