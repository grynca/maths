#include "shapes_h.h"
#include "functions/common.h"
#include "maths/maths_funcs.h"
#include "../Transform.h"
#include "OverlapTmp.h"
#include "ContactManifold.h"

namespace grynca {

    inline Ray::Ray() {}

    inline Ray::Ray(const Vec2& start, const Vec2& end)
     : start_(start), dir_(end-start), length_(1.0f), flags_(0)
    {
    }

    inline Ray::Ray(const Vec2& start, const Dir2& dir, f32 length)
     : start_(start), dir_(dir), length_(length), flags_(0)
    {
        flags_[bDirNormalized] = true;
    }

    inline Ray::Ray(const Vec2& start, const Vec2& end, CalcDirInfo)
     : Ray(start, end)
    {
        calcDirInfo_(dir_, inv_dir_, flags_);
    }

    inline Ray::Ray(const Vec2& start, const Dir2& dir, f32 length, CalcDirInfo)
     : Ray(start, dir, length)
    {
        calcDirInfo_(dir_, inv_dir_, flags_);
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
            if (isDirInfoCalculated()) {
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

    inline const Dir2& Ray::getDir()const {
        return dir_;
    }

    inline const Vec2& Ray::getInvDir()const {
        return inv_dir_;
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

    inline const std::bitset<Ray::bfCount>& Ray::getFlags()const {
        return flags_;
    }

    inline void Ray::setStart(const Vec2& s) {
        start_ = s;
    }

    inline void Ray::setDir(const Dir2& d) {
        dir_ = d;
        flags_[bDirNormalized] = false;
        if (isDirInfoCalculated()) {
            calcDirInfo_(dir_, inv_dir_, flags_);
        }
    }

    inline void Ray::setDirN(const Dir2& d) {
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

    inline void Ray::transform(const Mat3& tr) {
        *this = transformOut(tr);
    }

    inline void Ray::transform(const Transform& tr) {
        *this = transformOut(tr);
    }

    inline Ray Ray::transformOut(const Mat3& tr)const {
        Ray rslt(tr*getStart(), tr*getEnd());
        if (isDirNormalized()) {
            rslt.normalize_(rslt.dir_, rslt.length_, rslt.flags_);
            if (isDirInfoCalculated()) {
                rslt.calcDirInfo_(rslt.dir_, rslt.inv_dir_, rslt.flags_);
            }
        }
        return rslt;
    }

    inline Ray Ray::transformOut(const Transform& tr)const {
        return transformOut(tr.calcMatrix());
    }

    inline Vec2 Ray::calcSupport(const Dir2& dir)const {
        f32 proj = dot(dir, dir_);
        return (proj>0.0f)
                ?getEnd()
                :getStart();
    }

    inline bool Ray::isPointInside(const Vec2& p, f32 eps)const {
        Dir2 dir = getDir();
        Dir2 n = dir.perpL();
        f32 dst = maths::calcDistanceFromLine(p, getStart(), n);
        if (fabsf(dst) >= eps)
            return false;
        dst = maths::calcDistanceFromLine(p, getStart(), dir);
        return dst > 0.0f && dst < getLength();
    }

    inline f32 Ray::calcArea()const {
        return 0.0f;
    }

    inline f32 Ray::calcInertia()const {
        return 0.0f;
    }

    inline bool Ray::calcHitType(f32 t1, f32 t2, f32 ray_len, RayHitType& rht_out) {
        // static
#define T1_MIN (t1 > maths::EPS)
#define T1_MAX (t1 < (ray_len-maths::EPS))
#define T2_MIN (t2 > maths::EPS)
#define T2_MAX (t2 < (ray_len-maths::EPS))

        if (T1_MIN) {
            if (T1_MAX) {
                rht_out = (T2_MAX)?rhtImpale:rhtPoke;
            }
            else {
                rht_out = rhtFallShort;
                return false;
            }
        }
        else if (T2_MIN) {
            if (T2_MAX) {
                rht_out = rhtExitWound;
            }
            else {
                rht_out = rhtCompletelyInside;
                return false;
            }
        }
        else {
            rht_out = rhtPast;
            return false;
        }

        return true;
#undef T1_MIN
#undef T1_MAX
#undef T2_MIN
#undef T2_MAX
    }

    template <typename ShapeT>
    inline bool Ray::overlaps(const ShapeT& s)const {
        OverlapTmp unused;
        return overlaps(s, unused);
    }

    inline bool Ray::overlaps(const ARect& ar, OverlapTmp& otmp)const {
        return ar.overlaps(*this, otmp);
    }

    inline void Ray::calcContact(const ARect& ar, OverlapTmp& otmp, ContactManifold& cm_out)const {
        ar.calcContact(*this, otmp, cm_out);
        cm_out.normal *= -1;
    }

    inline bool Ray::overlaps(const Circle& circle, OverlapTmp& otmp)const {
        otmp.ray_circle_.rv = getToEndVec();
        Vec2 f = getStart() - circle.getCenter();

        f32 a = otmp.ray_circle_.rv.getSqrLen();
        f32 b = 2*dot(f, otmp.ray_circle_.rv);
        f32 c = f.getSqrLen() - circle.getRadius()*circle.getRadius();

        f32 discriminant = b*b-4*a*c;
        if (discriminant >= 0) {
            discriminant = (f32)sqrt(discriminant);

            // t1 is the intersection, and it's closer than t2
            // (since t1 uses -b - discriminant)
            otmp.ray_circle_.t1 = (-b - discriminant)/(2*a);
            otmp.ray_circle_.t2 = (-b + discriminant)/(2*a);

            return calcHitType(otmp.ray_circle_.t1, otmp.ray_circle_.t2, 1.0f, otmp.ray_circle_.rht);
        }
        // miss
        return false;
    }

    inline void Ray::calcContact(const Circle& circle, OverlapTmp& otmp, ContactManifold& cm_out)const {
        switch (otmp.ray_circle_.rht) {
            case rhtPoke:
                cm_out.size = 1;
                cm_out.normal = -getDir();
                cm_out.points[0].penetration = (1.0f - otmp.ray_circle_.t1) * getLength();
                cm_out.points[0].position = getStart() + otmp.ray_circle_.t1 * otmp.ray_circle_.rv;
                break;
            case rhtImpale:
                cm_out.size = 2;
                cm_out.normal = -getDir();
                cm_out.points[0].penetration = (1.0f - otmp.ray_circle_.t1) * getLength();
                cm_out.points[0].position = getStart() + otmp.ray_circle_.t1 * otmp.ray_circle_.rv;
                cm_out.points[1].penetration = (1.0f - otmp.ray_circle_.t2) * getLength();
                cm_out.points[1].position = getStart() + otmp.ray_circle_.t2 * otmp.ray_circle_.rv;
                break;
            case rhtExitWound:
                cm_out.size = 1;
                cm_out.normal = getDir();
                cm_out.points[0].penetration = otmp.ray_circle_.t2 * getLength();
                cm_out.points[0].position = getStart() + otmp.ray_circle_.t2 * otmp.ray_circle_.rv;
                break;
            default:
                NEVER_GET_HERE("shouldnt call for miss cases.");
                return;
        }
    }

    inline bool Ray::overlaps(const Ray& r, OverlapTmp& otmp)const {
        otmp.ray_ray_.rv1 = getToEndVec();

        f32 u;
        if (!overlapLineSegVSLineSeg(getStart(), otmp.ray_ray_.rv1, r.getStart(), r.getToEndVec(), otmp.ray_ray_.t, u))
            return false;

        return maths::betwZeroOne(otmp.ray_ray_.t) && maths::betwZeroOne(u);
    }

    inline void Ray::calcContact(const Ray& r, OverlapTmp& otmp, ContactManifold& cm_out)const {
        cm_out.size = 1;
        cm_out.normal = -getDir();
        cm_out.points[0].penetration = (1.0f - otmp.ray_ray_.t) * getLength();
        cm_out.points[1].position = getStart() + otmp.ray_ray_.t * otmp.ray_ray_.rv1;
    }

    inline bool Ray::overlaps(const Rect& r, OverlapTmp& otmp)const {
        return r.overlaps(*this, otmp);
    }

    inline void Ray::calcContact(const Rect& r, OverlapTmp& otmp, ContactManifold& cm_out)const {
        r.calcContact(*this, otmp, cm_out);
        cm_out.normal *= -1;
    }

    inline bool Ray::overlaps(const Pgon& p, OverlapTmp& otmp)const {
        return p.overlaps(*this, otmp);
    }

    inline void Ray::calcContact(const Pgon& p, OverlapTmp& otmp, ContactManifold& cm_out)const {
        p.calcContact(*this, otmp, cm_out);
        cm_out.normal *= -1;
    }

    inline void Ray::normalize_(Dir2& dir_io, f32& len_out, std::bitset<bfCount>& flags_out) {
    // static
        len_out = dir_io.getLen();
        dir_io = dir_io/len_out;
        flags_out[bDirNormalized] = true;
    }

    inline void Ray::calcDirInfo_(const Dir2& dir, Vec2& inv_dir_out, std::bitset<bfCount>& flags_out) {
    // static
        inv_dir_out.setX(1.0f/dir.getX());
        inv_dir_out.setY(1.0f/dir.getY());
        flags_out[bDirXSign] = (dir.getX() < 0);
        flags_out[bDirYSign] = (dir.getY() < 0);
        flags_out[bDirInfoComputed] = true;
    }

    inline std::ostream& operator << (std::ostream& os, Ray& r) {
        os << "Ray= s:" << r.getStart() << ", e:" << r.getEnd() << ", d:" <<r.getDir();
        if (r.isDirNormalized()) {
            os << ", l:" << r.getLength();
        }
        return os;
    }
}