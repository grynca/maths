#ifndef SHAPE_H
#define SHAPE_H

#include "ShapeTypes.h"
#include "types/containers/VArray.h"

namespace grynca {

    class Shape : public Variant<ShapeTypes> {
        typedef Variant<ShapeTypes> V;
        friend std::ostream& operator<<(std::ostream& os, Shape& s);
    public:
        ARect calcARectBound()const;
        void transform(const Mat3& tr);
        void transform(const Transform& tr);
        bool isPointInside(const Vec2& pt)const;
        f32 calcArea()const;
        f32 calcInertia()const;

        bool overlaps(const Shape& sh)const;
        bool overlaps(const Shape& sh, OverlapTmp& otmp)const;
        void calcContact(const Shape& sh, OverlapTmp& otmp, ContactManifold& cm_out)const;
        Vec2 calcSupport(const Dir2& dir)const;
    private:
        TEMPLATED_FUNCTOR(CalcARectBoundF_, (T* sh) { return sh->calcARectBound(); })
        TEMPLATED_FUNCTOR(Transform1F_, (T* sh, Shape& out, const Mat3& tr) { out.set(sh->transformOut(tr)); })
        TEMPLATED_FUNCTOR(Transform2F_, (T* sh, Shape& out, const Transform& tr) { out.set(sh->transformOut(tr)); })
        TEMPLATED_FUNCTOR(IsPointInsideF_, (T* sh, const Vec2& pt) { return sh->isPointInside(pt); })
        TEMPLATED_FUNCTOR(CalcAreaF_, (T* sh) { return sh->calcArea(); })
        TEMPLATED_FUNCTOR(CalcInertiaF_, (T* sh) { return sh->calcInertia(); })
        TEMPLATED_FUNCTOR(PrintF_, (T* sh, std::ostream& os) { os << *sh; })
        template <typename T2>
        TEMPLATED_FUNCTOR(OverlapsInnerF_, (T* sh2, T2* sh1) { return sh1->overlaps(*sh2); })
        TEMPLATED_FUNCTOR(OverlapsF_, (T* sh1, const Shape& sh2) { return sh2.callFunctor<OverlapsInnerF_<T> >(sh1); })
        template <typename T2>
        TEMPLATED_FUNCTOR(Overlaps2InnerF_, (T* sh2, T2* sh1, OverlapTmp& otmp) { return sh1->overlaps(*sh2, otmp); })
        TEMPLATED_FUNCTOR(Overlaps2F_, (T* sh1, const Shape& sh2, OverlapTmp& otmp) { return sh2.callFunctor<Overlaps2InnerF_<T> >(sh1, otmp); })
        template <typename T2>
        TEMPLATED_FUNCTOR(CalcContactInnerF_, (T* sh2, T2* sh1, OverlapTmp& otmp, ContactManifold& cm_out) { sh1->calcContact(*sh2, otmp, cm_out); })
        TEMPLATED_FUNCTOR(CalcContactF_, (T* sh1, const Shape& sh2, OverlapTmp& otmp, ContactManifold& cm_out) { sh2.callFunctor<CalcContactInnerF_<T> >(sh1, otmp, cm_out); })
        TEMPLATED_FUNCTOR(CalcSupportF_, (T* sh, const Dir2& dir) { return sh->calcSupport(dir); })
    };
}

#include "Shape.inl"
#endif //SHAPE_H
