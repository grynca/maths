#include "shapes_h.h"
#include "../maths_funcs.h"
#include "../Transform.h"
#include "OverlapTmp.h"
#include "ContactManifold.h"
#include "Rect.h"

namespace grynca {

    inline Rect::Rect()
     : position_(0,0), size_(0, 0), offset_(0,0), rotation_(0), rot_dir_(1.0f, 0.0f)
    {}

    inline Rect::Rect(const Vec2& position, const Vec2& size)
     : position_(position), size_(size), offset_(0,0), rotation_(0), rot_dir_(1.0f, 0.0f)
    {}

    inline Rect::Rect(const Vec2& position, const Vec2& size, const Vec2& offset, Angle rot)
     : position_(position), size_(size), offset_(offset), rotation_(rot), rot_dir_(rot.getDir())
    {
    }

    inline Rect::Rect(const Vec2& position, const Vec2& size, const Vec2& offset, const Dir2& rot_dir)
     : position_(position), size_(size), offset_(offset), rotation_(rot_dir.getAngle()),
       rot_dir_(rot_dir)
    {}

    inline Rect::Rect(const Vec2& position, const Vec2& size, const Vec2& offset, Angle rot, const Dir2& rot_dir)
     : position_(position), size_(size), offset_(offset), rotation_(rot),
       rot_dir_(rot_dir)
    {
#ifdef DEBUG_BUILD
        ASSERT(fabsf(rot_dir.getAngle() - rot.normalize()) < maths::EPS);
#endif
    }

    inline Rect::Rect(const ARect& arect)
     : position_(0, 0), size_(arect.getSize()), offset_(arect.getLeftTop()), rotation_(0.0f), rot_dir_(1.0f, 0.0f)
    {
    }

    inline Vec2 Rect::getLeftTop()const {
        return getLT_(offset_.rotate(rot_dir_));
    }

    inline Vec2 Rect::getRightTop()const {
        return getRT_(offset_.rotate(rot_dir_));
    }

    inline Vec2 Rect::getRightBot()const {
        return getRB_(offset_.rotate(rot_dir_));
    }

    inline Vec2 Rect::getLeftBot()const {
        return getLB_(offset_.rotate(rot_dir_));
    }

    inline Vec2 Rect::getCenter()const {
        return position_ + (size_*0.5f + offset_).rotate(rot_dir_);
    }

    inline void Rect::getCorners(Vec2* corners)const {
        Vec2 rotated_offset = offset_.rotate(rot_dir_);
        corners[0] = getLT_(rotated_offset);
        corners[1] = corners[0]+(getWidthDir()*getSize().getX());
        corners[2] = corners[1]+(getHeightDir()*getSize().getY());
        corners[3] = corners[0]+(getHeightDir()*getSize().getY());
    }

    inline void Rect::getEdges(Vec2& w_out, Vec2& h_out)const {
        w_out = getWidthDir()*getSize().getX();
        h_out = getHeightDir()*getSize().getY();
    }

    inline const Dir2& Rect::getWidthDir()const {
        return rot_dir_;
    }

    inline Dir2 Rect::getHeightDir()const {
        return rot_dir_.perpR();
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
        rot_dir_ = rot.getDir();
    }

    inline ARect Rect::calcARectBound()const {
        Vec2 corners[4];
        getCorners(corners);
        return ARect(corners, 4);
    }

    inline void Rect::transform(const Mat3& tr) {
        transform(Transform(tr));
    }

    inline void Rect::transform(const Transform& tr) {
        position_ += tr.getPosition();
        size_ *= tr.getScale();
        offset_ *= tr.getScale();
        rotation_ += tr.getRotation();
        rot_dir_ = rotation_.getDir();
    }

    inline Rect Rect::transformOut(const Mat3& tr)const {
        return transformOut(Transform(tr));
    }

    inline Rect Rect::transformOut(const Transform& tr)const {
        // TODO: find out why this does not work
//        f32 new_sin = sinr_*tr.getRotCos() + cosr_*tr.getRotSin();
//        f32 new_cos = cosr_*tr.getRotCos() + sinr_*tr.getRotSin();
        return Rect(position_ + tr.getPosition(), size_*tr.getScale(), offset_*tr.getScale(), rotation_+tr.getRotation());
    }

    inline Vec2 Rect::calcSupport(const Dir2& dir)const {
        // rotate dir to local rect space & then check as if with arect
        Vec2 rdir = dir.rotateInverse(rot_dir_);

        if (rdir.getX() >= 0) {
            if (rdir.getY() >= 0) {
                return getRightBot();
            }
            else {
                return getRightTop();
            }
        }
        if (rdir.getY() >= 0) {
            return getLeftBot();
        }
        return getLeftTop();
    }

    inline Vec2 Rect::calcSupport(const Dir2& dir, u32& pt_id_out)const {
        Vec2 rdir = dir.rotateInverse(rot_dir_);
        if (rdir.getX() >= 0) {
            if (rdir.getY() >= 0) {
                pt_id_out = 2;
                return getRightBot();
            }
            else {
                pt_id_out = 1;
                return getRightTop();
            }
        }
        if (rdir.getY() >= 0) {
            pt_id_out = 3;
            return getLeftBot();
        }
        pt_id_out = 0;
        return getLeftTop();
    }

    inline Vec2 Rect::getPoint(u32 pt_id)const {
        switch (pt_id) {
            case 0: return getLeftTop();
            case 1: return getRightTop();
            case 2: return getRightBot();
            case 3: return getLeftBot();
        }
        return getPoint(PMOD(pt_id, 4));
    }

    inline u32 Rect::wrapPointId(u32 pt_id)const {
        return PMOD(pt_id, 4);
    }

    inline Dir2 Rect::getNormal(u32 pt_id)const {
        switch (pt_id) {
            case 0: return rot_dir_.perpL();
            case 1: return rot_dir_;
            case 2: return rot_dir_.perpR();
            case 3: return -rot_dir_;
        }
        return getNormal(PMOD(pt_id, 4));
    }

    inline Dir2 Rect::getEdgeDir(u32 pt_id)const {
        switch (pt_id) {
            case 0: return rot_dir_;
            case 1: return rot_dir_.perpR();
            case 2: return -rot_dir_;
            case 3: return rot_dir_.perpL();
        }
        return getEdgeDir(PMOD(pt_id, 4));
    }

    inline bool Rect::isPointInside(const Vec2& p)const {
        Dir2 wd = getWidthDir();
        Dir2 hd = getHeightDir();

        Ray top{getLeftTop(), wd, getSize().getX()};
        if (!isRightFromLine(p, top.getStart(), top.getEnd()))
            return false;
        Ray right{getRightTop(), hd, getSize().getY()};
        if (!isRightFromLine(p, right.getStart(), right.getEnd()))
            return false;
        Ray bottom{getRightBot(), -wd, getSize().getX()};
        if (!isRightFromLine(p, bottom.getStart(), bottom.getEnd()))
            return false;
        Ray left{getLeftBot(), -hd, getSize().getY()};
        return isRightFromLine(p, left.getStart(), left.getEnd());
    }

    inline f32 Rect::calcArea()const {
        return size_.getX()*size_.getY();
    }

    f32 Rect::calcInertia()const {
        // rotation around rect center of mass, I = 1/12 * mass * (w*w + h*h)
        f32 moi = (1.0f/12)*(size_.getX()*size_.getX() + size_.getY()*size_.getY());
        // recalc inertia for rotation around dif. axis (parallel axis theorem)
        Vec2 to_center = Vec2(size_.getX()*0.5f, size_.getY()*0.5f) + offset_;
        moi += to_center.getLen();
        return moi;
    }

    template <typename ShapeT>
    inline bool Rect::overlaps(const ShapeT& sh)const {
        OverlapTmp unused;
        return overlaps(sh, unused);
    }

    inline bool Rect::overlaps(const ARect& ar, OverlapTmp& otmp)const {
        return ar.overlaps(*this, otmp);
    }

    inline void Rect::calcContact(const ARect& ar, OverlapTmp& otmp, ContactManifold& cm_out)const {
        ar.calcContact(*this, otmp, cm_out);
        cm_out.normal *= -1;
    }

    inline bool Rect::overlaps(const Circle& c, OverlapTmp& otmp)const {
        // make Aligned Rectangle centered on {0,0}
        ARect ar(getOffset(), getSize());
        // transform center to rect space
        Vec2 transformed_center = c.getCenter()-getPosition();
        transformed_center = transformed_center.rotateInverse(rot_dir_);
        otmp.arect_circle_.transformed_c = Circle(transformed_center, c.getRadius());
        return ar.overlaps(otmp.arect_circle_.transformed_c, otmp);
    }

    inline void Rect::calcContact(const Circle& c, OverlapTmp& otmp, ContactManifold& cm_out)const {
        ARect ar(getOffset(), getSize());
        ar.calcContact(otmp.arect_circle_.transformed_c, otmp, cm_out);

        // rotate normal back
        cm_out.normal = cm_out.normal.rotate(rot_dir_);
        // transform contact points back
        for (u32 i=0; i<cm_out.size; ++i) {
            cm_out.points[i].position = cm_out.points[i].position.rotate(rot_dir_);
            cm_out.points[i].position += getPosition();
        }
    }

    inline bool Rect::overlaps(const Ray& r, OverlapTmp& otmp)const {
        // make Aligned Rectangle centered on {0,0}
        ARect ar(getOffset(), getSize());

        // transform Ray to its local space

        Vec2 transformed_start = r.getStart() - getPosition();
        transformed_start = transformed_start.rotateInverse(rot_dir_);
        Dir2 transformed_dir = r.getDir().rotateInverse(rot_dir_);

        otmp.arect_ray_.transformed_r = Ray(transformed_start, transformed_dir, r.getLength(), Ray::CalcDirInfo());

        return ar.overlaps(otmp.arect_ray_.transformed_r, otmp);
    }

    inline void Rect::calcContact(const Ray& r, OverlapTmp& otmp, ContactManifold& cm_out)const {
        // make Aligned Rectangle centered on {0,0}
        ARect ar(getOffset(), getSize());
        ar.calcContact(otmp.arect_ray_.transformed_r, otmp, cm_out);

        // rotate normal back
        cm_out.normal = cm_out.normal.rotate(rot_dir_);
        // transform contact points back
        for (u32 i=0; i<cm_out.size; ++i) {
            cm_out.points[i].position = cm_out.points[i].position.rotate(rot_dir_);
            cm_out.points[i].position += getPosition();
        }
    }

    inline bool Rect::overlaps(const Rect& r, OverlapTmp& otmp)const {
        // SAT
        auto& tmp = otmp.rect_rect_;

        tmp.hA = getSize()*0.5f;
        tmp.hB = r.getSize()*0.5f;

        Dir2 irA = Angle::invertRotDir(rot_dir_);
        Dir2 irB = Angle::invertRotDir(r.rot_dir_);

        tmp.cA = getCenter();
        tmp.cB = r.getCenter();

        Vec2 dp = tmp.cB - tmp.cA;
        tmp.dA = dp.rotate(irA);     // rotate by A inv.rot
        tmp.dB = dp.rotate(irB);     // rotate by B inv.rot

        Dir2 rBA = Angle::combineRotations(irA, r.rot_dir_);  // rotation B->A

        Vec2 C1(fabsf(rBA.getX()), fabsf(rBA.getY()));
        Vec2 C2(C1.getY(), C1.getX());

        Vec2 dA_abs(fabsf(tmp.dA.getX()), fabsf(tmp.dA.getY()));
        Vec2 dB_abs(fabsf(tmp.dB.getX()), fabsf(tmp.dB.getY()));

        // Box A faces
        tmp.faceA = dA_abs - tmp.hA - Vec2(dot(C1, tmp.hB), dot(C2, tmp.hB));
        if (tmp.faceA.getX() > -maths::EPS || tmp.faceA.getY() > -maths::EPS) {
            return false;
        }

        // Box B faces
        tmp.faceB = dB_abs - tmp.hB - Vec2(dot(C1, tmp.hA), dot(C2, tmp.hA));
        return !(tmp.faceB.getX() > -maths::EPS || tmp.faceB.getY() > -maths::EPS);
    }

    inline void Rect::calcContact(const Rect& r, OverlapTmp& otmp, ContactManifold& cm_out)const {
        auto& tmp = otmp.rect_rect_;

        Dir2 rAY = rot_dir_.perpR();
        Dir2 rBY = r.rot_dir_.perpR();
        // Find best axis
        // Box A faces
        u8 axis = aFaceAX;
        f32 sep = tmp.faceA.getX();
        Dir2 normal = (tmp.dA.getX() > 0.0f) ? rot_dir_ : -rot_dir_;
        static const f32 relative_tol = 0.95f;
        static const f32 absolute_tol = 0.01f;

        if (tmp.faceA.getY() > (sep * relative_tol + absolute_tol * tmp.hA.getY())) {
            axis = aFaceAY;
            sep = tmp.faceA.getY();
            normal = (tmp.dA.getY() > 0.0f) ? rAY : -rAY;
        }

        // Box B faces
        if (tmp.faceB.getX() > (sep * relative_tol + absolute_tol * tmp.hB.getX())) {
            axis = aFaceBX;
            sep = tmp.faceB.getX();
            normal = (tmp.dB.getX() > 0.0f) ? r.rot_dir_ : -r.rot_dir_;
        }

        if (tmp.faceB.getY() > (sep * relative_tol + absolute_tol * tmp.hB.getY())) {
            axis = aFaceBY;
            //sep = tmp.faceB.getY();
            normal = (tmp.dB.getY() > 0.0f) ? rBY : -rBY;
        }

        Dir2 front_normal, side_normal;
        ClipVertex incident_edge[2];
        f32 front, neg_side, pos_side;
        u8 ref_neg_edge, ref_pos_edge;

        // Compute the clipping lines and the line segment to be clipped.
        switch (axis) {
            case aFaceAX: {
                front_normal = normal;
                front = dot(tmp.cA, front_normal) + tmp.hA.getX();
                side_normal = rAY;
                f32 side = dot(tmp.cA, side_normal);
                neg_side = -side + tmp.hA.getY();
                pos_side =  side + tmp.hA.getY();
                ref_neg_edge = EDGE1;
                ref_pos_edge = EDGE3;
                computeIncidentEdge_(incident_edge, tmp.hB, tmp.cB, r.rot_dir_, front_normal);
            }break;
            case aFaceAY: {
                front_normal = normal;
                front = dot(tmp.cA, front_normal) + tmp.hA.getY();
                side_normal = rot_dir_;
                f32 side = dot(tmp.cA, side_normal);
                neg_side = -side + tmp.hA.getX();
                pos_side =  side + tmp.hA.getX();
                ref_neg_edge = EDGE4;
                ref_pos_edge = EDGE2;
                computeIncidentEdge_(incident_edge, tmp.hB, tmp.cB, r.rot_dir_, front_normal);
            }break;
            case aFaceBX: {
                front_normal = -normal;
                front = dot(tmp.cB, front_normal) + tmp.hB.getX();
                side_normal = rBY;
                f32 side = dot(tmp.cB, side_normal);
                neg_side = -side + tmp.hB.getY();
                pos_side =  side + tmp.hB.getY();
                ref_neg_edge = EDGE1;
                ref_pos_edge = EDGE3;
                computeIncidentEdge_(incident_edge, tmp.hA, tmp.cA, rot_dir_, front_normal);
            }break;
            case aFaceBY: {
                front_normal = -normal;
                front = dot(tmp.cB, front_normal) + tmp.hB.getY();
                side_normal = r.rot_dir_;;
                f32 side = dot(tmp.cB, side_normal);
                neg_side = -side + tmp.hB.getX();
                pos_side =  side + tmp.hB.getX();
                ref_neg_edge = EDGE4;
                ref_pos_edge = EDGE2;
                computeIncidentEdge_(incident_edge, tmp.hA, tmp.cA, rot_dir_, front_normal);
            }break;
        }

        // Clip to box side 1
        clipPointsByLine_(incident_edge, side_normal, neg_side, ref_neg_edge);
        // Clip to negative box side 1
        clipPointsByLine_(incident_edge, -side_normal, pos_side, ref_pos_edge);


        cm_out.normal = -normal;
        cm_out.size = 0;
        if (axis == aFaceBX || axis == aFaceBY) {
            incident_edge[0].swap();
            incident_edge[1].swap();

            for (u32 i=0; i<2; ++i) {
                f32 pen = front - dot(front_normal, incident_edge[i].v);
                if (pen > 0) {
                    ContactPoint& cp = cm_out.points[cm_out.size];
                    cp.penetration = pen;
                    cp.position = incident_edge[i].v;
                    cp.feature_id = incident_edge[i].fp.value;
                    ++cm_out.size;
                }
            }
        }
        else {
            for (u32 i=0; i<2; ++i) {
                f32 pen = front - dot(front_normal, incident_edge[i].v);
                if (pen > 0) {
                    ContactPoint& cp = cm_out.points[cm_out.size];
                    cp.penetration = pen;
                    // slide contact point onto reference face (easy to cull)
                    cp.position = incident_edge[i].v + cp.penetration * front_normal;
                    cp.feature_id = incident_edge[i].fp.value;
                    ++cm_out.size;
                }
            }
        }

        ASSERT(cm_out.size > 0);
    }

    inline bool Rect::overlaps(const Pgon& p, OverlapTmp& otmp)const {
        return p.overlaps(*this, otmp);
    }

    inline void Rect::calcContact(const Pgon& p, OverlapTmp& otmp, ContactManifold& cm_out)const {
        p.calcContact(*this, otmp, cm_out);
        cm_out.normal *= -1;
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

    inline void Rect::computeIncidentEdge_(ClipVertex *c, const Vec2 &h, const Vec2 &center, const Dir2 &rot_dir, const Vec2 &normal)const {
        static constexpr f32 th = 0.707106781f;
        f32 cos = dot(rot_dir, normal);
        f32 sin = cross(rot_dir, normal);
        Dir2 d = Dir2(cos, sin).rotate({th, th});
        Dir2 down_dir = rot_dir.perpR();

        if (d.getX() > 0.0f) {
            if (d.getY() > 0.0f) {
                // Edge 4
                Vec2 midp = center - rot_dir*h.getX();
                c[0].v = midp + down_dir*h.getY();
                c[1].v = midp - down_dir*h.getY();

                c[0].fp.e.in_edge2 = EDGE3;
                c[0].fp.e.out_edge2 = EDGE4;

                c[1].fp.e.in_edge2 = EDGE4;
                c[1].fp.e.out_edge2 = EDGE1;
            }
            else {
                // Edge 3
                Vec2 midp = center + down_dir*h.getY();
                c[0].v = midp + rot_dir*h.getX();
                c[1].v = midp - rot_dir*h.getX();

                c[0].fp.e.in_edge2 = EDGE2;
                c[0].fp.e.out_edge2 = EDGE3;

                c[1].fp.e.in_edge2 = EDGE3;
                c[1].fp.e.out_edge2 = EDGE4;
            }
        }
        else {
            if (d.getY() > 0.0f) {
                // Edge 1
                Vec2 midp = center - down_dir*h.getY();
                c[0].v = midp - rot_dir*h.getX();
                c[1].v = midp + rot_dir*h.getX();

                c[0].fp.e.in_edge2 = EDGE4;
                c[0].fp.e.out_edge2 = EDGE1;

                c[1].fp.e.in_edge2 = EDGE1;
                c[1].fp.e.out_edge2 = EDGE2;
            }
            else {
                // Edge 2
                Vec2 midp = center + rot_dir*h.getX();
                c[0].v = midp - down_dir*h.getY();
                c[1].v = midp + down_dir*h.getY();

                c[0].fp.e.in_edge2 = EDGE1;
                c[0].fp.e.out_edge2 = EDGE2;

                c[1].fp.e.in_edge2 = EDGE2;
                c[1].fp.e.out_edge2 = EDGE3;
            }
        }
    }

    inline void Rect::clipPointsByLine_(ClipVertex points[2], const Dir2& normal, f32 offset, u8 clip_edge)const {
        f32 d1 = dot(points[0].v, normal) + offset;
        f32 d2 = dot(points[1].v, normal) + offset;

        if (d1*d2 < 0.0f) {
            // If the points are on different sides of the plane
            // push intersection point
            f32 alpha = d1 / (d1 - d2);
            if (d1 < 0.0f) {
                points[0].v = points[0].v + alpha * (points[1].v - points[0].v);
                points[0].fp.e.in_edge1 = clip_edge;
                points[0].fp.e.in_edge2 = NO_EDGE;
            } else {
                points[1].v = points[0].v + alpha * (points[1].v - points[0].v);
                points[1].fp.e.out_edge1 = clip_edge;
                points[1].fp.e.out_edge2 = NO_EDGE;
            }
        }
        else if (d1 < 0.0f) {
            // both negative
            points[0].v -= normal*d1;
            points[1].v -= normal*d2;
        }
    }

//    inline void Rect::clipPointsByRefEdge_(Vec2& cp1, Vec2& cp2, const Vec2& clipper_pt, const Dir2& clipper_n, f32& d1_out, f32& d2_out, u32& pts_cnt_out)const {
//        d1_out = maths::calcDistanceFromLine(cp1, clipper_pt, clipper_n);
//        d2_out = maths::calcDistanceFromLine(cp2, clipper_pt, clipper_n);
//
//        if (d1_out*d2_out < 0.0f) {
//            // remove point behind clipper
//            pts_cnt_out = 1;
//            if (d1_out < 0.0f) {
//                d1_out = d2_out;
//                cp1 = cp2;
//            }
//        }
//        else {
//            pts_cnt_out = 2;
//        }
//    }
//
//    inline u32 Rect::clipIncidentPoints_(Vec2 *inc_pts_io, const Vec2* ref_corners, const Dir2* ref_normals, u32 ref_edge_id, f32* dists)const {
//        const Dir2& ref_normal = ref_normals[ref_edge_id];
//        const Dir2& ref_prev_normal = ref_normals[PMOD(ref_edge_id-1, 4)];
//        const Dir2& ref_next_normal = ref_normals[PMOD(ref_edge_id+1, 4)];
//
//        const Vec2& ref_pt1 = ref_corners[ref_edge_id];
//        const Vec2& ref_pt2 = ref_corners[PMOD(ref_edge_id+1, 4)];
//
//        // clip points of incident edge by neighboring edges' lines of ref.edge
//        clipPointsByNeighborEdge_(inc_pts_io[0], inc_pts_io[1], ref_pt1, ref_prev_normal);
//        clipPointsByNeighborEdge_(inc_pts_io[0], inc_pts_io[1], ref_pt2, ref_next_normal);
//
//        u32 points_cnt;
//        // keep only points behind reference edge
//        clipPointsByRefEdge_(inc_pts_io[0], inc_pts_io[1], ref_pt1, ref_normal, dists[0], dists[1], points_cnt);
//        return points_cnt;
//    }

    inline std::ostream& operator << (std::ostream& os, const Rect& r) {
        os << "Rect= p:" << r.getPosition() << ", s:" << r.getSize() << ", o:" << r.getOffset() << ", r:" << r.getRotation();
        return os;
    }

}