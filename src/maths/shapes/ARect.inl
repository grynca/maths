#include "ARect.h"
#include "OverlapInfo.h"
#include "Circle.h"
#include "Ray.h"

namespace grynca {

    inline ARect::ARect(const Vec2& lt, const Vec2& size)
     : lefttop_(lt), size_(size)
    {}

    inline float ARect::getWidth()const {
        return size_.getX();
    }

    inline float ARect::getHeight()const {
        return size_.getY();
    }

    inline float ARect::getX()const {
        return lefttop_.getX();
    }

    inline float ARect::getY()const {
        return lefttop_.getY();
    }

    inline const Vec2& ARect::getSize()const {
        return size_;
    }

    inline const Vec2& ARect::getLeftTop()const {
        return lefttop_;
    }

    inline Vec2 ARect::getRightTop()const {
        return lefttop_ + Vec2(size_.getX(), 0);
    }

    inline Vec2 ARect::getRightBot()const {
        return lefttop_ + size_;
    }

    inline Vec2 ARect::getLeftBot()const {
        return lefttop_ + Vec2(0, size_.getY());
    }

    inline Vec2 ARect::getCenter()const {
        return lefttop_ + size_*0.5f;
    }

    inline bool ARect::isZero() {
        return size_.isZero();
    }

    inline void ARect::setLeftTop(const Vec2& lt) {
        lefttop_ = lt;
    }

    inline void ARect::setSize(const Vec2& s) {
        size_ = s;
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
        return (fabs(lefttop_.getX() - r.lefttop_.getX()) * 2 < (getWidth() + r.getWidth()))
               && (fabs(lefttop_.getY() - r.lefttop_.getY()) * 2 < (getHeight() + r.getHeight()));
    }

    inline bool ARect::overlaps(const ARect& r, OverlapInfo& oi)const {
        float hw1 = getWidth()*0.5f;
        float hw2 = r.getWidth()*0.5f;
        Vec2 d = lefttop_ - r.lefttop_;
        float x_overlap = hw1 + hw2 - fabs(d.getX());
        if (x_overlap > 0) {
            float hh1 = getHeight()*0.5f;
            float hh2 = r.getHeight()*0.5f;
            float y_overlap = hh1 + hh2 - fabs(d.getY());
            if (y_overlap > 0) {
                // find out which axis is axis of least penetration
                if (x_overlap < y_overlap) {
                    // Point towards B knowing that n points from A to B
                    if(d.getX() < 0)
                        oi.dir_out_ = {-1, 0};
                    else
                        oi.dir_out_ = {0, 0};
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

    inline bool ARect::isPointInside(const Vec2& p) {
        return (p.getX() > getX())
               && (p.getX() < (getX()+getWidth()))
               && (p.getY() > getY())
               && (p.getY() < (getY()+getHeight()));

    }

    inline Circle ARect::calcCircleBound() {
        return Circle(getCenter(), (size_*0.5f).getLen());
    }

    inline std::ostream& operator<<(std::ostream& os, ARect& ar) {
        os << "ARect[p=(" << ar.getX() << ", " << ar.getY() << "), s=(" << ar.getWidth() << ", " << ar.getHeight() << ")]" << std::endl;
        return os;

    }

    inline bool operator==(const ARect& lhs, const ARect& rhs) {
        return (lhs.lefttop_ == rhs.lefttop_) && (lhs.size_ == rhs.size_);
    }

}