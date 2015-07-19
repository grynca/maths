#ifndef SHAPE_INTERNAL_H
#define SHAPE_INTERNAL_H

#include "ShapeTypes.h"
#include "Shape.h"

namespace grynca {
    // fw
    class Shape;
    class OverlapInfo;

    namespace internal {

        typedef bool (*overlapFunc)(const Shape& s1, const Shape& s2);
        typedef bool (*overlapWithInfoFunc)(const Shape& s1, const Shape& s2, OverlapInfo& oi);

        struct OverlapFuncs {
            overlapFunc funcs[ShapeTypes::getTypesCount()][ShapeTypes::getTypesCount()];
            overlapWithInfoFunc funcsWithInfo[ShapeTypes::getTypesCount()][ShapeTypes::getTypesCount()];
        };

        class ShapeHelper {
        public:
            ShapeHelper() {
                createOverlapFuncs<ShapeTypes>();
            }

            template<typename TP>
            IF_EMPTY(TP) createOverlapFuncs() {}
            template<typename TP>
            IF_NOT_EMPTY(TP) createOverlapFuncs() {
                createOverlapFuncsInner<HEAD(TP), ShapeTypes>();
                createOverlapFuncs<TAIL(TP)>();
            }

            template<typename T, typename TP>
            IF_EMPTY(TP) createOverlapFuncsInner() {};
            template<typename T, typename TP>
            IF_NOT_EMPTY(TP) createOverlapFuncsInner() {
                funcs.funcs[ShapeTypes::pos<T>()][ShapeTypes::pos<HEAD(TP)>()] = overlapFunc<T, HEAD(TP)>;
                funcs.funcsWithInfo[ShapeTypes::pos<T>()][ShapeTypes::pos<HEAD(TP)>()] = overlapFuncWithInfo<T, HEAD(TP)>;
                createOverlapFuncsInner<T, TAIL(TP)>();
            };


            template<class S1, class S2>
            static bool overlapFunc(const Shape& s1, const Shape& s2) {
                return s1.get<S1>().overlaps(s2.get<S2>());
            }

            template<class S1, class S2>
            static bool overlapFuncWithInfo(const Shape& s1, const Shape& s2, OverlapInfo& oi) {
                return s1.get<S1>().overlaps(s2.get<S2>(), oi);
            }

            OverlapFuncs funcs;
        };
    }
}

#endif //SHAPE_INTERNAL_H
