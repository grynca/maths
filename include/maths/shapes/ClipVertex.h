#ifndef CLIPVERTEX_H
#define CLIPVERTEX_H

#include "../Vec2.h"

namespace grynca {

    struct ClipVertex {
        ClipVertex() { fp.value = 0; }
        void swap() {
            std::swap(fp.e.in_edge1, fp.e.in_edge2);
            std::swap(fp.e.out_edge1, fp.e.out_edge2);
        }

        Vec2 v;
        union {
            struct {
                u8 in_edge1;
                u8 out_edge1;
                u8 in_edge2;
                u8 out_edge2;
            } e;
            u32 value;
        } fp;
    };

}

#endif //CLIPVERTEX_H
