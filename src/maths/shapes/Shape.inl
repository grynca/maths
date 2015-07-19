#include "Shape.h"
#include "Shape_internal.h"

namespace grynca {

    inline bool Shape::overlaps(const Shape& s, OverlapInfo& oi)const {
        return getHelper_().funcs.funcsWithInfo[getCurrentType()][s.getCurrentType()](*this, s, oi);
    }

    inline bool Shape::overlaps(const Shape& s)const {
        return getHelper_().funcs.funcs[getCurrentType()][s.getCurrentType()](*this, s);
    }

    inline internal::ShapeHelper& Shape::getHelper_()const {
        static internal::ShapeHelper helper;
        return helper;
    }

}