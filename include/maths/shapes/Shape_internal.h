#ifndef SHAPE_INTERNAL_H
#define SHAPE_INTERNAL_H

#include "ShapeTypes.h"
#include "Shape.h"

namespace grynca {
    // fw
    class Shape;
    class OverlapInfo;
    class Mat3;

    namespace internal {

        typedef bool (*overlapFunc)(const Shape& s1, const Shape& s2);
        typedef bool (*overlapWithInfoFunc)(const Shape& s1, const Shape& s2, OverlapInfo& oi);
        typedef void (*transformFunc)(Shape& s, const Mat3& tr);
        typedef std::ostream& (*printFunc)(Shape& s, std::ostream& os);

        struct OverlapFuncs {
            overlapFunc funcs[ShapeTypes::getTypesCount()][ShapeTypes::getTypesCount()];
            overlapWithInfoFunc funcsWithInfo[ShapeTypes::getTypesCount()][ShapeTypes::getTypesCount()];
            transformFunc transformFuncs[ShapeTypes::getTypesCount()];
            printFunc printFuncs[ShapeTypes::getTypesCount()];
        };

        class ShapeHelper {
        public:
            ShapeHelper() {
                createFuncs<ShapeTypes>();
            }

            template<typename TP>
            IF_EMPTY(TP) createFuncs() {}
            template<typename TP>
            IF_NOT_EMPTY(TP) createFuncs() {
                funcs.transformFuncs[ShapeTypes::pos<HEAD(TP)>()] = transformFunc<HEAD(TP)>;
                funcs.printFuncs[ShapeTypes::pos<HEAD(TP)>()] = printFunc<HEAD(TP)>;
                createFuncsInner<HEAD(TP), ShapeTypes>();
                createFuncs<TAIL(TP)>();
            }

            template<typename T, typename TP>
            IF_EMPTY(TP) createFuncsInner() {};
            template<typename T, typename TP>
            IF_NOT_EMPTY(TP) createFuncsInner() {
                funcs.funcs[ShapeTypes::pos<T>()][ShapeTypes::pos<HEAD(TP)>()] = overlapFunc<T, HEAD(TP)>;
                funcs.funcsWithInfo[ShapeTypes::pos<T>()][ShapeTypes::pos<HEAD(TP)>()] = overlapFuncWithInfo<T, HEAD(TP)>;
                createFuncsInner<T, TAIL(TP)>();
            };


            template<class S1, class S2>
            static bool overlapFunc(const Shape& s1, const Shape& s2) {
                return s1.get<S1>().overlaps(s2.get<S2>());
            }

            template<class S1, class S2>
            static bool overlapFuncWithInfo(const Shape& s1, const Shape& s2, OverlapInfo& oi) {
                return s1.get<S1>().overlaps(s2.get<S2>(), oi);
            }

            template<class S>
            static void transformFunc(Shape& s, const Mat3& tr) {
                s.get<S>().transform(tr);
            }

            template<class S>
            static std::ostream& printFunc(Shape& s, std::ostream& os) {
                return operator<<(os, s.get<S>());
            }

            OverlapFuncs funcs;
        };
    }
}

#endif //SHAPE_INTERNAL_H
