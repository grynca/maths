#ifndef SHAPE_H
#define SHAPE_H

#include "types/Variant.h"
#include "ShapeTypes.h"

namespace grynca {
    // fw
    namespace internal { class ShapeHelper; }

    // in-place polymorphic shape type
    class Shape : public Variant<ShapeTypes> {
    public:
        bool overlaps(const Shape& s, OverlapInfo& oi)const;
        bool overlaps(const Shape& s)const;
    private:
        internal::ShapeHelper& getHelper_()const;
    };

}

#include "Shape.inl"
#endif //SHAPE_H
