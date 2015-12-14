#include "Rect.h"
#include "OverlapInfo.h"
#include "Ray.h"

namespace grynca {

    inline Rect::Rect(const Vec2& position, const Vec2& size, const Vec2& offset, Angle rot)
     : position_(position), size_(size), offset_(offset), rotation_(rot),
       sinr_(rot.getSin()), cosr_(rot.getCos())
    {}

    inline Vec2 Rect::getLeftTop()const {
        return getLT_(offset_.rotate(sinr_, cosr_));
    }

    inline Vec2 Rect::getRightTop()const {
        return getRT_(offset_.rotate(sinr_, cosr_));
    }

    inline Vec2 Rect::getRightBot()const {
        return getRB_(offset_.rotate(sinr_, cosr_));
    }

    inline Vec2 Rect::getLeftBot()const {
        return getLB_(offset_.rotate(sinr_, cosr_));
    }
    inline void Rect::getCorners(Vec2* corners)const {
        Vec2 rotated_offset = offset_.rotate(sinr_, cosr_);
        corners[0] = getLT_(rotated_offset);
        corners[1] = getRT_(rotated_offset);
        corners[2] = getRB_(rotated_offset);
        corners[3] = getLB_(rotated_offset);
    }

    inline Vec2 Rect::getWidthDir()const {
        return {cosr_, sinr_};
    }

    inline Vec2 Rect::getHeightDir()const {
        return {-sinr_, cosr_};
    }

    inline Vec2 Rect::getSizeDir()const {
        return getWidthDir()+getHeightDir();
    }

    inline const Vec2& Rect::getPosition()const {
        return position_;
    }

    inline const Vec2& Rect::getSize()const {
        return size_;
    }

    inline const Vec2& Rect::getOffset()const {
        return offset_;
    }

    inline const Angle& Rect::getRotation()const {
        return rotation_;
    }

    inline float Rect::getSinr()const {
        return sinr_;
    }

    inline float Rect::getCosr()const {
        return cosr_;
    }

    inline bool Rect::isZero() {
        return size_.isZero();
    }

    inline void Rect::setPosition(const Vec2& p) {
        position_ = p;
    }

    inline void Rect::setSize(const Vec2& s) {
        size_ = s;
    }

    inline void Rect::setOffset(const Vec2& o) {
        offset_ = o;
    }

    inline void Rect::setRotation(const Angle& rot) {
        rotation_ = rot;
        sinr_ = rot.getSin();
        cosr_ = rot.getCos();
    }

    inline ARect Rect::calcARectBound()const {
        Vec2 corners[4];
        getCorners(corners);
        auto Xs = {corners[0].getX(), corners[1].getX(), corners[2].getX(), corners[3].getX()};
        auto Ys = {corners[0].getY(), corners[1].getY(), corners[2].getY(), corners[3].getY()};
        Vec2 lefttop(std::min(Xs), std::min(Ys));
        Vec2 rightbot(std::max(Xs), std::max(Ys));
        return ARect(lefttop, rightbot-lefttop);
    }

    inline bool Rect::overlaps(const Ray& r)const {
        return r.overlaps(*this);
    }

    inline bool Rect::overlaps(const Ray& r, OverlapInfo& oi)const {
        bool rslt = r.overlaps(*this, oi);
        oi.dir_out_ = - oi.dir_out_;
        return rslt;
    }

    inline bool Rect::overlaps(const Rect& r)const {
        // SAT
        Vec2 axes[4] = {
            getWidthDir(),
            getHeightDir(),
            r.getWidthDir(),
            r.getHeightDir()
        };

        Vec2 corners1[4];
        Vec2 corners2[4];
        getCorners(corners1);
        r.getCorners(corners2);

        for (size_t i=0; i<4; ++i) {
            // project corners on axis
            Interval i1 = axes[i].projectPoints(corners1, 4);
            Interval i2 = axes[i].projectPoints(corners2, 4);
            if (!i1.overlaps(i2))
                // is separation axis
                return false;
        }

        return true;
    }

    inline bool Rect::overlaps(const Rect& r, OverlapInfo& oi)const {
        // SAT
        Vec2 axes[4] = {
                getWidthDir(),
                getHeightDir(),
                r.getWidthDir(),
                r.getHeightDir()
        };

        Vec2 corners1[4];
        Vec2 corners2[4];
        getCorners(corners1);
        r.getCorners(corners2);

        int penetration_axis = -1;
        float smallest_overlap = std::numeric_limits<float>::max();
        for (int i=0; i<4; ++i) {
            // project corners on axis
            Interval i1 = axes[i].projectPoints(corners1, 4);
            Interval i2 = axes[i].projectPoints(corners2, 4);
            float o;
            if (!i1.overlaps(i2, o))
                // it is separation axis
                return false;
            if (i1.contains(i2) || i2.contains(i1)) {
                // check containment
                float mins = fabs(i1.getMin()-i2.getMin());
                float maxs = fabs(i1.getMax()-i2.getMax());
                if (mins < maxs)
                    o += mins;
                else
                    o += maxs;
            }

            if (o < smallest_overlap) {
                smallest_overlap = o;
                penetration_axis = i;
            }
        }

        oi.depth_ = smallest_overlap;
        oi.dir_out_ = axes[penetration_axis];
        return true;
    }

    inline bool Rect::overlaps(const Circle& c)const {
        return c.overlaps(*this);
    }

    inline bool Rect::overlaps(const Circle& c, OverlapInfo& oi)const {
        bool rslt = c.overlaps(*this, oi);
        oi.dir_out_ = - oi.dir_out_;
        return rslt;
    }

    inline bool Rect::overlaps(const ARect& r)const {
        NEVER_GET_HERE("Not implemented");
        return false;
    }

    inline bool Rect::overlaps(const ARect& r, OverlapInfo& oi)const {
        NEVER_GET_HERE("Not implemented");
        return false;
    }

    inline Vec2 Rect::getLT_(const Vec2& rot_offset)const {
        return position_+rot_offset;
    }

    inline Vec2 Rect::getRT_(const Vec2& rot_offset)const {
        return getLT_(rot_offset)+(getWidthDir()*getSize().getX());
    }

    inline Vec2 Rect::getRB_(const Vec2& rot_offset)const {
        return getRT_(rot_offset)+(getHeightDir()*getSize().getY());
    }

    inline Vec2 Rect::getLB_(const Vec2& rot_offset)const {
        return getLT_(rot_offset)+(getHeightDir()*getSize().getY());
    }

    inline std::ostream& operator << (std::ostream& os, const Rect& r) {
        os << "Rect[p=(" << r.position_.getX() << ", " << r.position_.getY() << "), s=("
        << r.size_.getX() << ", " << r.size_.getY() << "), o=(" << r.offset_.getX() << ", " << r.offset_.getY()
        << "), r=" << r.rotation_ << "]" << std::endl;
        return os;
    }

}