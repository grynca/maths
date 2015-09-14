#include "Ray.h"
#include "OverlapInfo.h"
#include "Circle.h"
#include "ARect.h"
#include "Rect.h"

namespace grynca {


    inline Ray::Ray() {}

    inline Ray::Ray(const Vec2& start, const Vec2& dir, float length)
     : start_(start), dir_(dir), length_(length)
    {}

    inline const Vec2& Ray::getStart()const {
        return start_;
    }

    inline const Vec2& Ray::getDir()const {
        return dir_;
    }

    inline Vec2 Ray::getEnd()const {
        return start_+dir_*length_;
    }

    inline Vec2 Ray::getToEndVec()const {
        return dir_*length_;
    }

    inline float Ray::getLength()const {
        return length_;
    }

    inline void Ray::setStart(const Vec2& s) {
        start_ = s;
    }

    inline void Ray::setDir(const Vec2& d) {
        dir_ = d;
    }

    inline void Ray::setLength(float l) {
        length_ = l;
    }

    inline bool Ray::overlaps(const Ray& r)const {
        Vec2 rv1 = getToEndVec();
        Vec2 rv2 = r.getToEndVec();
        // http://www.codeproject.com/Tips/862988/Find-the-Intersection-Point-of-Two-Line-Segments
        float dxd = cross(rv1, rv2);

        if (dxd == 0.0f)
            // parallel or collinear
            return false;

        //float sxd = Vec2::cross(r.start_-start_, dir_);
        // sxd can be checked for null if we want to distinguish between parallel and collinear

        float t = cross(r.start_-start_, rv2)/dxd;
        float u = cross(r.start_-start_, rv1)/dxd;

        return (0 <=t && t <= 1) && (0 <= u && u <= 1);
    }

    inline bool Ray::overlaps(const Ray& r, OverlapInfo& oi)const {
        Vec2 rv1 = getToEndVec();
        Vec2 rv2 = r.getToEndVec();
        float dxd = cross(rv1, rv2);

        if (dxd == 0)
            // parallel or collinear
            return false;

        float t = cross(r.start_-start_, rv2)/dxd;
        float u = cross(r.start_-start_, rv1)/dxd;

        if ( (0 <=t && t <= 1) && (0 <= u && u <= 1) ) {
            oi.addIntersection_(start_+t*rv1);
            oi.depth_ = t*length_;
            oi.dir_out_ = -dir_;
            return true;
        }
        return false;
    }

    inline bool Ray::overlaps(const Circle& c)const {
        NEVER_GET_HERE("Not implemented.");
        return false;
    }

    inline bool Ray::overlaps(const Circle& c, OverlapInfo& oi)const {
        NEVER_GET_HERE("Not implemented.");
        return false;
    }

    inline bool Ray::overlaps(const ARect& r)const {
        NEVER_GET_HERE("Not implemented.");
        return false;
    }

    inline bool Ray::overlaps(const ARect& r, OverlapInfo& oi)const {
        NEVER_GET_HERE("Not implemented.");
        return false;
    }

    inline bool Ray::overlaps(const Rect& r)const {
        Vec2 wd = r.getWidthDir();
        Vec2 hd = r.getHeightDir();
        Ray rays[4] = {
                Ray{r.getLeftTop(), wd, r.getSize().getX()},
                Ray{r.getRightTop(), hd, r.getSize().getY()},
                Ray{r.getLeftBot(), wd, r.getSize().getX()},
                Ray{r.getLeftTop(), hd, r.getSize().getY()}
        };


        for (uint32_t i=0; i<4; ++i) {
            if (r.overlaps(rays[i]))
                return true;
        }
        return false;
    }

    inline bool Ray::overlaps(const Rect& r, OverlapInfo& oi)const {
        Vec2 wd = r.getWidthDir();
        Vec2 hd = r.getHeightDir();
        Ray rays[4] = {
                Ray{r.getLeftTop(), wd, r.getSize().getX()},
                Ray{r.getRightTop(), hd, r.getSize().getY()},
                Ray{r.getLeftBot(), wd, r.getSize().getX()},
                Ray{r.getLeftTop(), hd, r.getSize().getY()}
        };

        // TODO: depth pocitat nejak lip, aby k necemu byla
        bool overlap_rslt = false;
        float prev_depth = 0.0f;
        for (uint32_t i=0; i<4; ++i) {
            if (overlaps(rays[i], oi)) {
                overlap_rslt = true;
                if (oi.getIntersectionsCount() == 1) {
                    prev_depth = oi.depth_;
                }
                else {
                    if (prev_depth < oi.depth_) {
                        oi.depth_ = prev_depth;
                    }
                    else {
                        // make nearest intersection first
                        std::swap(oi.intersections_[0], oi.intersections_[1]);
                    }
                    // max 2 intersections
                    return true;
                }
            }
        }
        return overlap_rslt;
    }
}