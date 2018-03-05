#ifndef CONTACTMANIFOLD_H
#define CONTACTMANIFOLD_H

#include "../Vec2.h"

namespace grynca {

    struct ContactPoint {
        ContactPoint() : feature_id(InvalidId()) {}

        Vec2 position;
        f32 penetration;
        u32 feature_id;     // for e.g. contacts caching
    };

    struct ContactManifold {
        static constexpr u32 MAX_SIZE = 2;

        bool isCached()const {
            return points[0].feature_id != u32(-1);
        }

        u32 size;
        ContactPoint points[MAX_SIZE];
        Dir2 normal;
    };

}

#endif //CONTACTMANIFOLD_H
