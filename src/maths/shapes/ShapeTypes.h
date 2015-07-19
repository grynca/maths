#ifndef SHAPETYPES_H
#define SHAPETYPES_H

#include "types/Type.h"
#include "Circle.h"
#include "ARect.h"

namespace grynca {

    typedef TypesPack<
            Circle,
            ARect,
            Rect
    > ShapeTypes;

}

#endif //SHAPETYPES_H
