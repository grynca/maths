#include "Ray.h"
#include "OverlapInfo.h"
#include "Circle.h"
#include "ARect.h"
#include "Rect.h"
#include "Pgon.h"
#include "functions/common.h"

namespace grynca {

    inline Ray::Ray() {}

    inline Ray::Ray(const Vec2& start, const Vec2& end)
     : start_(start), dir_(end-start), length_(1.0f), flags_(0)
    {
    }

    inline Ray::Ray(const Vec2& start, const Vec2& dir, f32 length)
     : start_(start), dir_(dir), length_(length), flags_(0)
    {
        flags_[bDirNormalized] = true;
    }

    inline bool Ray::isDirNormalized()const {
        return flags_[bDirNormalized];
    }

    inline bool Ray::isDirInfoCalculated()const {
        return flags_[bDirInfoComputed];
    }

    inline void Ray::normalizeDirIfNotAlready() {
        if (!isDirNormalized()) {
            normalize_(dir_, length_, flags_);
            if (flags_[bDirInfoComputed]) {
                inv_dir_.setX(1.0f/dir_.getX());
                inv_dir_.setY(1.0f/dir_.getY());
            }
        }
    }

    inline void Ray::calcDirInfoIfNotAlready() {
        if (!isDirInfoCalculated()) {
            calcDirInfo_(dir_, inv_dir_, flags_);
        }
    }

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

    inline f32 Ray::getLength()const {
        return length_;
    }

    inline void Ray::setStart(const Vec2& s) {
        start_ = s;
    }

    inline void Ray::setDir(const Vec2& d) {
        dir_ = d;
        flags_[bDirNormalized] = false;
        if (isDirInfoCalculated()) {
            calcDirInfo_(dir_, inv_dir_, flags_);
        }
    }

    inline void Ray::setDirN(const Vec2& d) {
        ASSERT(d.getLen() == 1.0f);
        dir_ = d;
        flags_[bDirNormalized] = true;
        if (isDirInfoCalculated()) {
            calcDirInfo_(dir_, inv_dir_, flags_);
        }
    }

    inline void Ray::setLength(f32 l) {
        ASSERT(isDirNormalized());
        length_ = l;
    }

    inline ARect Ray::calcARectBound()const {
        Vec2 start = start_;
        Vec2 size = dir_;
        if (size.getX() < 0) {
            start.accX() += size.getX();
            size.accX() = - size.getX();
        }
        if (size.getY() < 0) {
            start.accY() += size.getY();
            size.accY() = - size.getY();
        }
        return ARect(start, size);
    }

    inline bool Ray::overlaps(const Ray& r)const {
        Vec2 rv1 = getToEndVec();
        Vec2 rv2 = r.getToEndVec();
        // http://www.codeproject.com/Tips/862988/Find-the-Intersection-Point-of-Two-Line-Segments
        f32 dxd = cross(rv1, rv2);

        if (dxd == 0.0f)
            // parallel or collinear
            return false;

        //f32 sxd = Vec2::cross(r.start_-start_, dir_);
        // sxd can be checked for null if we want to distinguish between parallel and collinear

        f32 t = cross(r.start_-start_, rv2)/dxd;
        f32 u = cross(r.start_-start_, rv1)/dxd;

        return (0 <=t && t <= 1) && (0 <= u && u <= 1);
    }

    inline bool Ray::overlaps(const Ray& r, OverlapInfo& oi)const {
        Vec2 rv1 = getToEndVec();
        Vec2 rv2 = r.getToEndVec();
        f32 dxd = cross(rv1, rv2);

        if (dxd == 0)
            // parallel or collinear
            return false;

        f32 t = cross(r.start_-start_, rv2)/dxd;
        f32 u = cross(r.start_-start_, rv1)/dxd;

        if ( (0 <=t && t <= 1) && (0 <= u && u <= 1) ) {
            oi.addIntersection_(start_+t*rv1);
            oi.depth_ = (1.0f-t)*length_;
            oi.dir_out_ = -dir_;
            return true;
        }
        return false;
    }

    inline bool Ray::overlaps(const Circle& circle)const {
        Vec2 d = dir_*length_;
        Vec2 f = start_ - circle.getCenter();

        f32 a = d.getSqrLen();
        f32 b = 2*dot(f, d);
        f32 c = f.getSqrLen() - circle.getRadius()*circle.getRadius();

        f32 discriminant = b*b-4*a*c;
        if (discriminant >= 0) {
            discriminant = (f32)sqrt(discriminant);

            f32 t1 = (-b - discriminant)/(2*a);
            f32 t2 = (-b + discriminant)/(2*a);

            if( t1 >= 0 && t1 <= 1 )
                return true;

            if( t2 >= 0 && t2 <= 1 )
                return true;
        }
        return false;
    }

    inline bool Ray::overlaps(const Circle& circle, OverlapInfo& oi)const {
        Vec2 d = dir_*length_;
        Vec2 f = start_ - circle.getCenter();

        f32 a = d.getSqrLen();
        f32 b = 2*dot(f, d);
        f32 c = f.getSqrLen() - circle.getRadius()*circle.getRadius();

        f32 discriminant = b*b-4*a*c;
        if (discriminant >= 0) {
            // ray didn't totally miss sphere, so there is a solution to the equation.
            discriminant = (f32)sqrt(discriminant);

            // either solution may be on or off the ray so need to test both t1 is
            // always the smaller value, because BOTH discriminant and a are nonnegative.
            f32 t1 = (-b - discriminant)/(2*a);
            f32 t2 = (-b + discriminant)/(2*a);

            // 3x HIT cases:
            //          -o->             --|-->  |            |  --|->
            // Impale(t1 hit,t2 hit), Poke(t1 hit,t2>1), ExitWound(t1<0, t2 hit),

            // 3x MISS cases:
            //       ->  o                     o ->              | -> |
            // FallShort (t1>1,t2>1), Past (t1<0,t2<0), CompletelyInside(t1<0, t2>1)

            if( t1 >= 0 && t1 <= 1 ) {
                // t1 is the intersection, and it's closer than t2
                // (since t1 uses -b - discriminant)
                // Impale
                oi.addIntersection_(start_ + t1*d);
                if( t2 >= 0 && t2 <= 1 ) {
                    // Poke
                    oi.addIntersection_(start_ + t2*d);
                }
                oi.depth_ = (1.0f - t1)*length_;
                oi.dir_out_ = -dir_;
                return true;
            }

            if( t2 >= 0 && t2 <= 1 ) {
                // ExitWound
                oi.addIntersection_(start_ + t2*d);
                oi.depth_ = t2*length_;
                oi.dir_out_ = dir_;
                return true;
            }

            // no intn: FallShort, Past, CompletelyInside
        }
        // no intersection
        return false;
    }

    inline bool Ray::overlaps(const ARect& rect)const {
        ASSERT(isDirInfoCalculated() && "Ray must have its dir info calculated.");

        //http://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection

        Vec2 rect_bounds[2];
        rect_bounds[0] = rect.getLeftTop();
        rect_bounds[1] = rect.getRightBot();

        f32 tmin = (rect_bounds[flags_[bDirXSign]].getX() - start_.getX()) * inv_dir_.getX();
        f32 tymax = (rect_bounds[1-(int)flags_[bDirYSign]].getY() - start_.getY()) * inv_dir_.getY();
        if (tmin > tymax)
            return false;

        f32 tmax = (rect_bounds[1-(int)flags_[bDirXSign]].getX() - start_.getX()) * inv_dir_.getX();
        f32 tymin = (rect_bounds[flags_[bDirYSign]].getY() - start_.getY()) * inv_dir_.getY();

        return (tymin <= tmax);
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


        for (u32 i=0; i<4; ++i) {
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
        f32 prev_depth = 0.0f;
        for (u32 i=0; i<4; ++i) {
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

    inline bool Ray::overlaps(const Pgon& p)const {
        return false;
    }

    inline bool Ray::overlaps(const Pgon& p, OverlapInfo& oi)const {
        return false;
    }

    inline void Ray::normalize_(Vec2& dir_io, f32& len_out, std::bitset<bfCount>& flags_out) {
    // static
        len_out = dir_io.getLen();
        dir_io = dir_io/len_out;
        flags_out[bDirNormalized] = true;
    }

    inline void Ray::calcDirInfo_(const Vec2& dir, Vec2& inv_dir_out, std::bitset<bfCount>& flags_out) {
    // static
        inv_dir_out.setX(1.0f/dir.getX());
        inv_dir_out.setY(1.0f/dir.getY());
        flags_out[bDirXSign] = sign(dir.getX());
        flags_out[bDirYSign] = sign(dir.getY());
        flags_out[bDirInfoComputed] = true;
    }
}