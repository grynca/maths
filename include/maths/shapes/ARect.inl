#include "shapes_h.h"
#include "../Transform.h"
#include "OverlapTmp.h"
#include "ContactManifold.h"

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

    inline ARect ARect::createCentered(const Vec2& size) {
        // static
        f32 w = size.getX(), h = size.getY();
        return ARect({-w*0.5f, -h*0.5f}, size);
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

    inline void ARect::expand(const ARect& r, bool& expanded_out) {
        expanded_out = false;

        if (r.bounds_[0].getX() < bounds_[0].getX()) {
            expanded_out = true;
            bounds_[0].accX() = r.bounds_[0].getX();
        }
        if (r.bounds_[0].getY() < bounds_[0].getY()) {
            expanded_out = true;
            bounds_[0].accY() = r.bounds_[0].getY();
        }
        if (r.bounds_[1].getX() > bounds_[1].getX()) {
            expanded_out = true;
            bounds_[1].accX() = r.bounds_[1].getX();
        }
        if (r.bounds_[1].getY() > bounds_[1].getY()) {
            expanded_out = true;
            bounds_[1].accY() = r.bounds_[1].getY();
        }
    }

    inline ARect ARect::calcARectBound()const {
        return *this;
    }

    inline ARect ARect::calcRotInvBound()const {
        Vec2 s = getSize();
        f32 diag_len = sqrtf(s.getX()*s.getX() + s.getY()*s.getY());
        Vec2 c = getLeftTop() + s*0.5f;
        Vec2 off(diag_len*0.5f, diag_len*0.5f);
        return ARect(c - off, off*2);
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

    inline void ARect::transform(const Transform& tr) {
        Vec2 old_size = getSize();
        f32 cos = fabsf(tr.getRotDir().getX());
        f32 sin = fabsf(tr.getRotDir().getY());
        f32 sx = fabsf(tr.getScale().getX());
        f32 sy = fabsf(tr.getScale().getY());

        f32 new_w = old_size.getY()*sin + old_size.getX()*cos;
        f32 new_h = old_size.getX()*sin + old_size.getY()*cos;

        Vec2 new_c = getLeftTop() + old_size*0.5f + tr.getPosition();
        Vec2 new_hs(new_w*sx*0.5f, new_h*sy*0.5f);

        bounds_[0] = new_c - new_hs;
        bounds_[1] = new_c + new_hs;
    }

    inline void ARect::transformWithoutRotation(const Transform& tr) {
        Vec2 new_size = getSize() * tr.getScale();
        bounds_[0] = bounds_[0]*tr.getScale() + tr.getPosition();
        bounds_[1] = bounds_[0] + new_size;
    }

    inline Rect ARect::transformOut(const Mat3& tr)const {
        return transformOut(Transform(tr));
    }

    inline Rect ARect::transformOut(const Transform& tr)const {
        Vec2 offset(getLeftTop());
        return Rect(tr.getPosition(), getSize()*tr.getScale(), offset*tr.getScale(), tr.getRotation(), tr.getRotDir());
    }

    inline Vec2 ARect::calcSupport(const Dir2& dir)const {
        Vec2 supp(dir.getX()>= 0 ? bounds_[1].getX() : bounds_[0].getX(),
                  dir.getY()>=0 ? bounds_[1].getY() : bounds_[0].getY());
        return supp;
    }

    inline Vec2 ARect::calcSupport(const Dir2& dir, u32& pt_id_out)const {
        if (dir.getX()>=0) {
            if (dir.getY()>=0) {
                pt_id_out = 2;
                return getRightBot();
            }
            else {
                pt_id_out = 1;
                return getRightTop();
            }
        }
        if (dir.getY()>=0) {
            pt_id_out = 3;
            return getLeftBot();
        }
        pt_id_out = 0;
        return getLeftTop();
    }

    inline Vec2 ARect::getPoint(u32 pt_id)const {
        switch (pt_id) {
            case 0: return getLeftTop();
            case 1: return getRightTop();
            case 2: return getRightBot();
            case 3: return getLeftBot();
        }
        return getPoint(PMOD(pt_id, 4));
    }

    inline u32 ARect::wrapPointId(u32 pt_id)const {
        return PMOD(pt_id, 4);
    }

    inline Dir2 ARect::getNormal(u32 pt_id)const {
        switch (pt_id) {
            case 0: return {0, -1};
            case 1: return {1, 0};
            case 2: return {0, 1};
            case 3: return {-1, 0};
        }
        return getNormal(PMOD(pt_id, 4));
    }

    inline Dir2 ARect::getEdgeDir(u32 pt_id)const {
        switch (pt_id) {
            case 0: return {1, 0};
            case 1: return {0, 1};
            case 2: return {-1, 0};
            case 3: return {0, -1};
        }
        return getEdgeDir(PMOD(pt_id, 4));
    }

    inline bool ARect::isPointInside(const Vec2& p)const {
        return (p.getX() > getX())
               && (p.getX() < (getX()+getWidth()))
               && (p.getY() > getY())
               && (p.getY() < (getY()+getHeight()));

    }

    inline Circle ARect::calcCircleBound()const {
        Vec2 hs = getSize()*0.5f;
        return Circle(getLeftTop()+hs, hs.getLen());
    }

    inline f32 ARect::calcArea()const {
        Vec2 s = getSize();
        return s.getX()*s.getY();
    }

    inline f32 ARect::calcInertia()const {
        const Vec2& offset = getLeftTop();
        Vec2 size = getSize();
        // rotation around rect center of mass, I = 1/12 * mass * (w*w + h*h)
        f32 moi = (1.0f/12)*(size.getX()*size.getX() + size.getY()*size.getY());
        // recalc inertia for rotation around dif. axis (parallel axis theorem)
        Vec2 to_center = Vec2(size.getX()*0.5f, size.getY()*0.5f) + offset;
        moi += to_center.getLen();
        return moi;
    }

    template <typename ShapeT>
    inline bool ARect::overlaps(const ShapeT& s)const {
        OverlapTmp unused;
        return overlaps(s, unused);
    }

    inline bool ARect::overlaps(const ARect& ar, OverlapTmp& otmp)const {
        otmp.arect_arect_.op_l = std::max(getLeftTop().getX(), ar.getLeftTop().getX());
        otmp.arect_arect_.op_r = std::min(getRightBot().getX(), ar.getRightBot().getX());
        otmp.arect_arect_.overlap_x = otmp.arect_arect_.op_r - otmp.arect_arect_.op_l;
        if (otmp.arect_arect_.overlap_x < maths::EPS)
            return false;

        otmp.arect_arect_.op_t = std::max(getLeftTop().getY(), ar.getLeftTop().getY());
        otmp.arect_arect_.op_b = std::min(getRightBot().getY(), ar.getRightBot().getY());
        otmp.arect_arect_.overlap_y = otmp.arect_arect_.op_b - otmp.arect_arect_.op_t;
        return otmp.arect_arect_.overlap_y > maths::EPS;
    }

    inline void ARect::calcContact(const ARect& ar, OverlapTmp& otmp, ContactManifold& cm_out)const {
        // TODO: maybe this could be optimized to fewer ifs
        cm_out.size = 2;
        if (otmp.arect_arect_.overlap_x < otmp.arect_arect_.overlap_y) {
            bool my_y_bigger = ar.getSize().getY() < getSize().getY();
            if (getCenter().getX() > ar.getCenter().getX()) {
                cm_out.normal.set(1, 0);
                cm_out.points[0].penetration = ar.getRightBot().getX() - getLeftTop().getX();
                cm_out.points[1].penetration = cm_out.points[0].penetration;

                // pick contact points on smaller of rect boundaries
                if (my_y_bigger) {
                    cm_out.points[0].position.set(otmp.arect_arect_.op_l, otmp.arect_arect_.op_t);
                    cm_out.points[1].position.set(otmp.arect_arect_.op_l, otmp.arect_arect_.op_b);
                }
                else {
                    cm_out.points[0].position.set(otmp.arect_arect_.op_r, otmp.arect_arect_.op_t);
                    cm_out.points[1].position.set(otmp.arect_arect_.op_r, otmp.arect_arect_.op_b);
                }
            }
            else {
                cm_out.normal.set(-1, 0);
                cm_out.points[0].penetration = getRightBot().getX() - ar.getLeftTop().getX();
                cm_out.points[1].penetration = cm_out.points[0].penetration;
                if (my_y_bigger) {
                    cm_out.points[0].position.set(otmp.arect_arect_.op_r, otmp.arect_arect_.op_t);
                    cm_out.points[1].position.set(otmp.arect_arect_.op_r, otmp.arect_arect_.op_b);
                }
                else {
                    cm_out.points[0].position.set(otmp.arect_arect_.op_l, otmp.arect_arect_.op_t);
                    cm_out.points[1].position.set(otmp.arect_arect_.op_l, otmp.arect_arect_.op_b);
                }
            }
        }
        else {
            bool my_x_bigger = ar.getSize().getX() < getSize().getX();
            if (getCenter().getY() > ar.getCenter().getY()) {
                cm_out.normal.set(0, 1);
                cm_out.points[0].penetration = ar.getRightBot().getY() - getLeftTop().getY();
                cm_out.points[1].penetration = cm_out.points[0].penetration;
                if (my_x_bigger) {
                    cm_out.points[0].position.set(otmp.arect_arect_.op_l, otmp.arect_arect_.op_b);
                    cm_out.points[1].position.set(otmp.arect_arect_.op_r, otmp.arect_arect_.op_b);
                }
                else {
                    cm_out.points[0].position.set(otmp.arect_arect_.op_l, otmp.arect_arect_.op_t);
                    cm_out.points[1].position.set(otmp.arect_arect_.op_r, otmp.arect_arect_.op_t);
                }
            }
            else {
                cm_out.normal.set(0, -1);
                cm_out.points[0].penetration = getRightBot().getY() - ar.getLeftTop().getY();
                cm_out.points[1].penetration = cm_out.points[0].penetration;
                if (my_x_bigger) {
                    cm_out.points[0].position.set(otmp.arect_arect_.op_l, otmp.arect_arect_.op_t);
                    cm_out.points[1].position.set(otmp.arect_arect_.op_r, otmp.arect_arect_.op_t);
                }
                else {
                    cm_out.points[0].position.set(otmp.arect_arect_.op_l, otmp.arect_arect_.op_b);
                    cm_out.points[1].position.set(otmp.arect_arect_.op_r, otmp.arect_arect_.op_b);
                }
            }
        }
    }

    inline bool ARect::overlaps(const Circle& c, OverlapTmp& otmp)const {
        otmp.arect_circle_.r_halfe = getSize()*0.5f;
        otmp.arect_circle_.v = c.getCenter()-getCenter();
        // clamp to half extent
        if (otmp.arect_circle_.v.getX() > 0.0f) {
            otmp.arect_circle_.fid = 0;
            otmp.arect_circle_.v.setX(std::min(otmp.arect_circle_.v.getX(), otmp.arect_circle_.r_halfe.getX()));
        }
        else {
            otmp.arect_circle_.fid = 1;
            otmp.arect_circle_.v.setX(std::max(otmp.arect_circle_.v.getX(), -otmp.arect_circle_.r_halfe.getX()));
        }
        if (otmp.arect_circle_.v.getY() > 0.0f) {
            otmp.arect_circle_.v.setY(std::min(otmp.arect_circle_.v.getY(), otmp.arect_circle_.r_halfe.getY()));
        }
        else {
            otmp.arect_circle_.fid |= (1<<1);
            otmp.arect_circle_.v.setY(std::max(otmp.arect_circle_.v.getY(), -otmp.arect_circle_.r_halfe.getY()));
        }

        // closes point on rect to circle
        otmp.arect_circle_.bc = getCenter()+otmp.arect_circle_.v;

        otmp.arect_circle_.dv = otmp.arect_circle_.bc - c.getCenter();
        otmp.arect_circle_.cr = otmp.arect_circle_.dv.getSqrLen();

        return (otmp.arect_circle_.cr - (c.getRadius()*c.getRadius())) < (-maths::EPS_SQRT);
    }

    inline void ARect::calcContact(const Circle& c, OverlapTmp& otmp, ContactManifold& cm_out)const {
        cm_out.size = 1;
        f32 d = sqrtf(otmp.arect_circle_.cr);
        if (d>maths::EPS) {
            cm_out.normal = otmp.arect_circle_.dv/d;
            cm_out.points[0].penetration = c.getRadius() - d;
        }
        else {
            // circle center inside rectangle
            f32 x_overlap = otmp.arect_circle_.r_halfe.getX() - fabsf(otmp.arect_circle_.v.getX());
            f32 y_overlap = otmp.arect_circle_.r_halfe.getY() - fabsf(otmp.arect_circle_.v.getY());

            if (x_overlap < y_overlap) {
                if (otmp.arect_circle_.v.getX() > 0) {
                    cm_out.normal.set(-1, 0);
                    otmp.arect_circle_.bc.setX(getRightBot().getX());
                } else {
                    cm_out.normal.set(1, 0);
                    otmp.arect_circle_.bc.setX(getLeftTop().getX());
                }
                cm_out.points[0].penetration = x_overlap + c.getRadius();
            } else {
                if (otmp.arect_circle_.v.getY() > 0) {
                    cm_out.normal.set(0, -1);
                    otmp.arect_circle_.bc.setY(getRightBot().getY());
                } else {
                    cm_out.normal.set(0, 1);
                    otmp.arect_circle_.bc.setY(getLeftTop().getY());
                }
                cm_out.points[0].penetration = y_overlap + c.getRadius();
            }
        }
        cm_out.points[0].position = otmp.arect_circle_.bc;
    }

    inline bool ARect::overlaps(const Ray& r, OverlapTmp& otmp)const {
        ASSERT(r.isDirInfoCalculated() && "Ray must have its dir info calculated.");

        //http://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection

        const auto& rf = r.getFlags();

        f32 tx1 = (bounds_[rf[Ray::bDirXSign]].getX() - r.getStart().getX()) * r.getInvDir().getX();
        f32 ty2 = (bounds_[1-(int)rf[Ray::bDirYSign]].getY() - r.getStart().getY()) * r.getInvDir().getY();
        if (tx1 > ty2) {
            otmp.arect_ray_.rht = rhtMiss;
            return false;
        }

        f32 tx2 = (bounds_[1-(int)rf[Ray::bDirXSign]].getX() - r.getStart().getX()) * r.getInvDir().getX();
        f32 ty1 = (bounds_[rf[Ray::bDirYSign]].getY() - r.getStart().getY()) * r.getInvDir().getY();

        if (ty1 > tx2) {
            otmp.arect_ray_.rht = rhtMiss;
            return false;
        }

        f32* order[4];
        sortFour(tx1, tx2, ty1, ty2, order);

        otmp.arect_ray_.t1 = *order[1];
        otmp.arect_ray_.t2 = *order[2];

        return r.calcHitType(otmp.arect_ray_.t1, otmp.arect_ray_.t2, r.getLength(), otmp.arect_ray_.rht);
    }

    inline void ARect::calcContact(const Ray& r, OverlapTmp& otmp, ContactManifold& cm_out)const {
        switch (otmp.arect_ray_.rht) {
            case rhtPoke:
                cm_out.size = 1;
                cm_out.normal = r.getDir();
                cm_out.points[0].penetration = r.getLength() - otmp.arect_ray_.t1;
                cm_out.points[0].position = r.getStart() + otmp.arect_ray_.t1 * r.getDir();
                break;
            case rhtImpale:
                cm_out.size = 2;
                cm_out.normal = r.getDir();
                cm_out.points[0].penetration = r.getLength() - otmp.arect_ray_.t1;
                cm_out.points[0].position = r.getStart() + otmp.arect_ray_.t1 * r.getDir();
                cm_out.points[1].penetration = r.getLength() - otmp.arect_ray_.t2;
                cm_out.points[1].position= r.getStart() + otmp.arect_ray_.t2 * r.getDir();
                break;
            case rhtExitWound:
                cm_out.size = 1;
                cm_out.normal = -r.getDir();
                cm_out.points[0].penetration = otmp.arect_ray_.t2;
                cm_out.points[0].position = r.getStart() + otmp.arect_ray_.t2*r.getDir();
                break;
            default:
                NEVER_GET_HERE("shouldnt call for miss cases.");
                return;
        }
    }

    inline bool ARect::overlaps(const Rect& r, OverlapTmp& otmp)const {
        // convert ARect to Rect
        Rect me(getLeftTop(), getSize());
        return me.overlaps(r, otmp);
    }

    inline void ARect::calcContact(const Rect& r, OverlapTmp& otmp, ContactManifold& cm_out)const {
        // convert ARect to Rect
        Rect me(getLeftTop(), getSize());
        me.calcContact(r, otmp, cm_out);
    }

    inline bool ARect::overlaps(const Pgon& p, OverlapTmp& otmp)const {
        return p.overlaps(*this, otmp);
    }

    inline void ARect::calcContact(const Pgon& p, OverlapTmp& otmp, ContactManifold& cm_out)const {
        p.calcContact(*this, otmp, cm_out);
        cm_out.normal *= -1;
    }

    inline std::ostream& operator<<(std::ostream& os, ARect& ar) {
        os << "ARect= c:" << ar.getCenter() << ", s:" << ar.getSize();
        return os;

    }

    inline bool operator==(const ARect& lhs, const ARect& rhs) {
        return (lhs.getLeftTop() == rhs.getLeftTop()) && (lhs.getRightBot() == rhs.getRightBot());
    }

}