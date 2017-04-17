#include "Shape.h"
#include "Shape_internal.h"

namespace grynca {

    inline bool Shape::overlaps(const Shape& s, OverlapInfo& oi)const {
        return getHelper_().funcs.funcsWithInfo[getTypeId()][s.getTypeId()](*this, s, oi);
    }

    inline bool Shape::overlaps(const Shape& s)const {
        return getHelper_().funcs.funcs[getTypeId()][s.getTypeId()](*this, s);
    }

    void Shape::transform(const Mat3& tr) {
        getHelper_().funcs.transformFuncs[getTypeId()](*this, tr);
    }

    inline internal::ShapeHelper& Shape::getHelper_()const {
        static internal::ShapeHelper helper;
        return helper;
    }

    inline std::ostream& operator<<(std::ostream& os, Shape& s) {
        return s.getHelper_().funcs.printFuncs[s.getTypeId()](s, os);
    }
}