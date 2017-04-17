#include "ARect.h"
#include "OverlapInfo.h"
#include "Circle.h"
#include "Ray.h"
#include "Pgon.h"

namespace grynca {

    inline ARect::ARect(const Vec2& lt, const Vec2& size) {
        bounds_[0] = lt;
        bounds_[1] = lt+size;
    }

    inline ARect::ARect(const Vec2 bounds[2]) {
        bounds_[0] = bounds[0];
        bounds_[1] = bounds[1];
    }

    inline ARect::ARect(const Vec2* points, u32 points_cnt) {
        ASSERT(points_cnt > 0);

        bounds_[0] = bounds_[1] = points[0];

        for (u32 i=1; i<points_cnt; ++i) {
            const Vec2& p = points[i];
            if (p.getX() < bounds_[0].getX()) {
                bounds_[0].setX(p.getX());
            }
            else if (p.getX() > bounds_[1].getX()) {
                bounds_[1].setX(p.getX());
            }

            if (p.getY() < bounds_[0].getY()) {
                bounds_[0].setY(p.getY());
            }
            else if (p.getY() > bounds_[1].getY()) {
                bounds_[1].setY(p.getY());
            }
        }
    }

    inline f32 ARect::getWidth()const {
        return getRightBot().getX() - getLeftTop().getX();
    }

    inline f32 ARect::getHeight()const {
        return getRightBot().getY() - getLeftTop().getY();
    }

    inline f32 ARect::getX()const {
        return getLeftTop().getX();
    }

    inline f32 ARect::getY()const {
        return getLeftTop().getY();
    }

    inline Vec2 ARect::getSize()const {
        return getRightBot() - getLeftTop();
    }

    inline f32* ARect::getDataPtr() {
        return &bounds_[0].val(0);
    }

    inline const f32* ARect::getDataPtr() const {
        return bounds_[0].getDataPtr();
    }

    inline const Vec2& ARect::getLeftTop()const {
        return bounds_[0];
    }

    inline Vec2 ARect::getRightTop()const {
        return getLeftTop() + Vec2(getWidth(), 0);
    }

    inline const Vec2& ARect::getRightBot()const {
        return bounds_[1];
    }

    inline Vec2 ARect::getLeftBot()const {
        return getLeftTop() + Vec2(0, getHeight());
    }

    inline Vec2 ARect::getCenter()const {
        return getLeftTop() + getSize()*0.5f;
    }

    inline const Vec2* ARect::getBounds()const {
        return bounds_;
    }

    inline Vec2* ARect::accBounds() {
        return bounds_;
    }

    inline bool ARect::isZero() {
        return getSize().isZero();
    }

    inline void ARect::setLeftTop(const Vec2& lt) {
        bounds_[0] = lt;
    }

    inline void ARect::setRightBot(const Vec2& rb) {
        bounds_[1] = rb;
    }

    inline void ARect::setSize(const Vec2& s) {
        bounds_[1] = bounds_[0] + s;
    }

    inline void ARect::expand(const ARect& r) {
        bounds_[0].accX() = std::min(bounds_[0].getX(), r.bounds_[0].getX());
        bounds_[0].accY() = std::min(bounds_[0].getY(), r.bounds_[0].getY());
        bounds_[1].accX() = std::max(bounds_[1].getX(), r.bounds_[1].getX());
        bounds_[1].accY() = std::max(bounds_[1].getY(), r.bounds_[1].getY());
    }

    inline ARect ARect::calcARectBound()const {
        return *this;
    }

    inline void ARect::transform(const Mat3& tr) {
        Vec2 pts[4] = {
                tr * getLeftTop(),
                tr * getRightTop(),
                tr * getRightBot(),
                tr * getLeftBot()
        };
        *this = ARect(&pts[0], 4);
    }

    inline bool ARect::overlaps(const Ray& r)const {
        NEVER_GET_HERE("Not implemented");
        return false;
    }

    inline bool ARect::overlaps(const Ray& r, OverlapInfo& oi)const {
        NEVER_GET_HERE("Not implemented");
        return false;
    }

    inline bool ARect::overlaps(const ARect& r)const {
        return (fabs(getLeftTop().getX() - r.getLeftTop().getX()) * 2 < (getWidth() + r.getWidth()))
               && (fabs(getLeftTop().getY() - r.getLeftTop().getY()) * 2 < (getHeight() + r.getHeight()));
    }

    inline bool ARect::overlaps(const ARect& r, OverlapInfo& oi)const {
        f32 hw1 = getWidth()*0.5f;
        f32 hw2 = r.getWidth()*0.5f;
        Vec2 d = getCenter() - r.getCenter();
        f32 x_overlap = hw1 + hw2 - fabsf(d.getX());
        if (x_overlap > 0) {
            f32 hh1 = getHeight()*0.5f;
            f32 hh2 = r.getHeight()*0.5f;
            f32 y_overlap = hh1 + hh2 - fabsf(d.getY());
            if (y_overlap > 0) {
                // find out which axis is axis of least penetration
                if (x_overlap < y_overlap) {
                    // Point towards B knowing that n points from A to B
                    if(d.getX() < 0)
                        oi.dir_out_ = {-1, 0};
                    else
                        oi.dir_out_ = {1, 0};
                    oi.depth_ = x_overlap;
                }
                else {
                    // Point toward B knowing that n points from A to B
                    if(d.getY() < 0)
                        oi.dir_out_ = {0, -1};
                    else
                        oi.dir_out_ = {0, 1};
                    oi.depth_ = y_overlap;
                }
                return true;
            }
        }
        return false;
    }

    inline bool ARect::overlaps(const Circle& c)const {
        return c.overlaps(*this);
    }

    inline bool ARect::overlaps(const Circle& c, OverlapInfo& oi)const {
        bool rslt = c.overlaps(*this, oi);
        oi.dir_out_ = - oi.dir_out_;
        return rslt;
    }

    inline bool ARect::overlaps(const Rect& r)const {
        assert(!"Not implemented yet!");
        return false;
    }

    inline bool ARect::overlaps(const Rect& r, OverlapInfo& oi)const {
        assert(!"Not implemented yet!");
        return false;
    }

    inline bool ARect::overlaps(const Pgon& p)const {
        assert(!"Not implemented yet!");
        return false;
    }

    inline bool ARect::overlaps(const Pgon& p, OverlapInfo& oi)const {
        assert(!"Not implemented yet!");
        return false;
    }

    inline bool ARect::isPointInside(const Vec2& p) {
        return (p.getX() > getX())
               && (p.getX() < (getX()+getWidth()))
               && (p.getY() > getY())
               && (p.getY() < (getY()+getHeight()));

    }

    inline Circle ARect::calcCircleBound() {
        return Circle(getCenter(), (getSize()*0.5f).getLen());
    }

    inline std::ostream& operator<<(std::ostream& os, ARect& ar) {
        os << "ARect= p:" << ar.getCenter() << ", s:" << ar.getSize() << std::endl;
        return os;

    }

    inline bool operator==(const ARect& lhs, const ARect& rhs) {
        return (lhs.getLeftTop() == rhs.getLeftTop()) && (lhs.getRightBot() == rhs.getRightBot());
    }

}