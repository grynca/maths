#ifndef RECT_H
#define RECT_H

#include "shapes_fw.h"
#include "ClipVertex.h"

namespace grynca {

    // general (unaligned) rectangle
    // offset is from position to left-top corner
    //      e.g offset of {-w/2, -h/2} makes rectangle positioned and rotated on/around its center
    class Rect {
        friend std::ostream& operator << (std::ostream& os, const Rect& r);
    public:
        Rect();
        Rect(const Vec2& position, const Vec2& size);       // offset = {0,0} - position at left-top corner
        Rect(const Vec2& position, const Vec2& size , const Vec2& offset, Angle rot = 0);
        Rect(const Vec2& position, const Vec2& size, const Vec2& offset, const Dir2& rot_dir);
        Rect(const Vec2& position, const Vec2& size, const Vec2& offset, Angle rot, const Dir2& rot_dir);
        Rect(const ARect& arect);

        Vec2 getLeftTop()const;
        Vec2 getRightTop()const;
        Vec2 getRightBot()const;
        Vec2 getLeftBot()const;
        Vec2 getCenter()const;
        void getCorners(Vec2* corners)const;       // lt rt rb lb
        void getEdges(Vec2& w_out, Vec2& h_out)const;
        const Dir2& getWidthDir()const;
        Dir2 getHeightDir()const;

        const Vec2& getPosition()const;
        const Vec2& getSize()const;
        const Vec2& getOffset()const;
        const Angle& getRotation()const;

        bool isZero();

        void setPosition(const Vec2& p);
        void setSize(const Vec2& s);
        void setOffset(const Vec2& o);
        void setRotation(const Angle& rot);

        ARect calcARectBound()const;
        // changes "this"
        void transform(const Mat3& tr);
        void transform(const Transform& tr);
        // returns new transformed obj
        Rect transformOut(const Mat3& tr)const;
        Rect transformOut(const Transform& tr)const;
        Vec2 calcSupport(const Dir2& dir)const;
        Vec2 calcSupport(const Dir2& dir, u32& pt_id_out)const;
        Vec2 getPoint(u32 pt_id)const;
        Dir2 getNormal(u32 pt_id)const;
        Dir2 getEdgeDir(u32 pt_id)const;
        u32 wrapPointId(u32 pt_id)const;

        bool isPointInside(const Vec2& p)const;

        f32 calcArea()const;
        f32 calcInertia()const;

        template <typename ShapeT>
        bool overlaps(const ShapeT& sh)const;

        bool overlaps(const ARect& ar, OverlapTmp& otmp)const;
        void calcContact(const ARect& ar, OverlapTmp& otmp, ContactManifold& cm_out)const;

        bool overlaps(const Circle& c, OverlapTmp& otmp)const;
        void calcContact(const Circle& c, OverlapTmp& otmp, ContactManifold& cm_out)const;

        bool overlaps(const Ray& r, OverlapTmp& otmp)const;
        void calcContact(const Ray& r, OverlapTmp& otmp, ContactManifold& cm_out)const;

        bool overlaps(const Rect& r, OverlapTmp& otmp)const;
        void calcContact(const Rect& r, OverlapTmp& otmp, ContactManifold& cm_out)const;

        bool overlaps(const Pgon& p, OverlapTmp& otmp)const;
        void calcContact(const Pgon& p, OverlapTmp& otmp, ContactManifold& cm_out)const;

    private:
        enum Axis {
            aFaceAX,
            aFaceAY,
            aFaceBX,
            aFaceBY
        };

        enum EdgeNumber {
            NO_EDGE = 0,
            EDGE1,
            EDGE2,
            EDGE3,
            EDGE4
        };

        Vec2 getLT_(const Vec2& rot_offset)const;
        Vec2 getRT_(const Vec2& rot_offset)const;
        Vec2 getRB_(const Vec2& rot_offset)const;
        Vec2 getLB_(const Vec2& rot_offset)const;

        void computeIncidentEdge_(ClipVertex *c, const Vec2 &h, const Vec2 &center, const Dir2 &rot_dir,
                                  const Vec2 &normal)const;
        void clipPointsByLine_(ClipVertex points[2], const Dir2& normal, f32 offset, u8 clip_edge)const;
//        void clipPointsByRefEdge_(Vec2& cp1, Vec2& cp2, const Vec2& clipper_pt, const Dir2& clipper_n, f32& d1_out, f32& d2_out, u32& pts_cnt_out)const;
//        u32 clipIncidentPoints_(Vec2 *inc_pts_io, const Vec2* ref_corners, const Dir2* ref_normals, u32 ref_edge_id, f32* dists)const;

        Vec2 position_;
        Vec2 size_;
        Vec2 offset_;  // vector offsetting rotation center (from left-top)
        Angle rotation_;
        Dir2 rot_dir_;      // (cos, sin), first col. of 2x2 rot. matrix
    };
}

#endif //RECT_H

#if !defined(RECT_INL) && !defined(WITHOUT_IMPL)
#define RECT_INL
# include "Rect.inl"
#endif // RECT_INL