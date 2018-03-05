#ifndef ARECT_H
#define ARECT_H

#include "shapes_fw.h"

namespace grynca {

    class ARect {
        friend std::ostream& operator<<(std::ostream& os, ARect& ar);
        friend bool operator==(const ARect& lhs, const ARect& rhs);
    public:
        ARect(const Vec2& lt = {0,0}, const Vec2& size = {0, 0});
        ARect(const Vec2 bounds[2]);
        ARect(const Vec2* points, u32 points_cnt);     // calculates bound for points

        static ARect createCentered(const Vec2& size);

        f32 getWidth()const;
        f32 getHeight()const;
        f32 getX()const;
        f32 getY()const;
        Vec2 getSize()const;

        // 4 f32s - left, top, right, bottom
        f32* getDataPtr();
        const f32* getDataPtr() const;

        const Vec2& getLeftTop()const;
        Vec2 getRightTop()const;
        const Vec2& getRightBot()const;
        Vec2 getLeftBot()const;
        Vec2 getCenter()const;
        const Vec2* getBounds()const;
        Vec2* accBounds();
        bool isZero();

        void setLeftTop(const Vec2& lt);
        void setRightBot(const Vec2& rb);
        void setSize(const Vec2& s);

        void expand(const ARect& r);
        void expand(const ARect& r, bool& expanded_out);

        ARect calcARectBound()const;
        ARect calcRotInvBound()const;

        // changes "this"
        // (if transform contains rotation, it will change ARect size to contain its rotated version)
        void transform(const Mat3& tr);
        void transform(const Transform& tr);
        void transformWithoutRotation(const Transform& tr);

        // this changes ARect to Rect, possibly rotated
        Rect transformOut(const Mat3& tr)const;
        Rect transformOut(const Transform& tr)const;

        Vec2 calcSupport(const Dir2& dir)const;
        Vec2 calcSupport(const Dir2& dir, u32& pt_id_out)const;

        Vec2 getPoint(u32 pt_id)const;
        u32 wrapPointId(u32 pt_id)const;
        Dir2 getNormal(u32 pt_id)const;
        Dir2 getEdgeDir(u32 pt_id)const;

        bool isPointInside(const Vec2& p)const;

        Circle calcCircleBound()const;

        f32 calcArea()const;
        f32 calcInertia()const;

        template <typename ShapeT>
        bool overlaps(const ShapeT& s)const;

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
        Vec2 bounds_[2];
    };
}
#endif //ARECT_H

#if !defined(ARECT_INL) && !defined(WITHOUT_IMPL)
#define ARECT_INL
# include "ARect.inl"
#endif // ARECT_INL
