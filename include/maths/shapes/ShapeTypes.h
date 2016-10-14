#ifndef SHAPETYPES_H
#define SHAPETYPES_H

#include "types/Type.h"
#include "Ray.h"
#include "Circle.h"
#include "ARect.h"
#include "Rect.h"
#include "Pgon.h"

namespace grynca {

    typedef TypesPack<
            Ray,
            Circle,
            ARect,
            Rect,
            Pgon
    > ShapeTypes;

}

#endif //SHAPETYPES_H
