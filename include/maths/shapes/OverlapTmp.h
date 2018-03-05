#ifndef OVERLAPTMP_H
#define OVERLAPTMP_H

#include "../maths_config.h"
#include "../Vec2.h"
#include "types/ObjFunc.h"
#include "ContactManifold.h"
#include "GJK.h"
#define WITHOUT_IMPL
#   include "Ray.h"
#   include "Circle.h"
#undef WITHOUT_IMPL

namespace grynca {

    struct OverlapTmp {
        OverlapTmp() {}
        OverlapTmp(const OverlapTmp& src) { memcpy(this, &src, sizeof(OverlapTmp)); }

        union {
            struct { RayHitType rht; f32 t1, t2; bool t1_in, t2_in; Ray transformed_r; } arect_ray_;  // also used for ray-rect
            struct { f32 op_l, op_r, overlap_x, op_t, op_b, overlap_y; } arect_arect_;
            struct { u8 fid; Vec2 r_halfe, bc, dv, v; f32 cr; Circle transformed_c; } arect_circle_;  // also used for rect-circle
            struct { Vec2 rv1; f32 t;} ray_ray_;
            struct { RayHitType rht; Vec2 rv; f32 t1, t2; bool t1_in, t2_in; } ray_circle_;
            struct { Vec2 dv; f32 cc; f32 rs; } circle_circle_;
            struct { Vec2 cA, cB, hA, hB, dA, dB, faceA, faceB; } rect_rect_;
            struct { f32 pen[MATHS_MAX_PGON_SIZE]; Dir2 nearest_pt_dv_n; f32 nearest_pt_d; u32 nearest_pt_id;} pgon_circle_;
            struct { Vec2 rv; f32 t1; u32 edge_id; Vec2 edge_vector; } pgon_ray_;
            struct { GJK2D<Pgon, ARect> gjk; } pgon_arect_;
            struct { GJK2D<Pgon, Rect> gjk; } pgon_rect_;
            struct { GJK2D<Pgon, Pgon> gjk; } pgon_pgon_;
        };
    };

}

#endif //OVERLAPTMP_H
