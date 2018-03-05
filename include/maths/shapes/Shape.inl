#include "Shape.h"

namespace grynca {

    inline std::ostream& operator<<(std::ostream& os, Shape& s) {
        s.callFunctor<Shape::PrintF_>(os);
        return os;
    }

    inline ARect Shape::calcARectBound()const {
        return V::callFunctor<CalcARectBoundF_>();
    }

    inline void Shape::transform(const Mat3& tr) {
        V::callFunctor<Transform1F_>(*this, tr);
    }

    inline void Shape::transform(const Transform& tr) {
        V::callFunctor<Transform2F_>(*this, tr);
    }

    inline bool Shape::isPointInside(const Vec2& pt)const {
        return V::callFunctor<IsPointInsideF_>(pt);
    }

    inline f32 Shape::calcArea()const {
        return V::callFunctor<CalcAreaF_>();
    }

    inline f32 Shape::calcInertia()const {
        return V::callFunctor<CalcInertiaF_>();
    }

    inline bool Shape::overlaps(const Shape& sh)const {
        return V::callFunctor<OverlapsF_>(sh);
    }

    inline bool Shape::overlaps(const Shape& sh, OverlapTmp& otmp)const {
        return V::callFunctor<Overlaps2F_>(sh, otmp);
    }

    inline void Shape::calcContact(const Shape& sh, OverlapTmp& otmp, ContactManifold& cm_out)const {
        V::callFunctor<CalcContactF_>(sh, otmp, cm_out);
    }

    inline Vec2 Shape::calcSupport(const Dir2& dir)const {
        return V::callFunctor<CalcSupportF_>(dir);
    }
}
