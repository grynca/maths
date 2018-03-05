#ifndef MATHS_FUNCS_H
#define MATHS_FUNCS_H

#include "Vec2.h"

namespace grynca {
    namespace maths {
        inline static f32 getLength(f32 val) {
            return fabsf(val);
        }

        inline static f32 getLength(const Vec2& val) {
            return val.getLen();
        }

        inline static bool isZero(f32 val) {
            return val == 0;
        }

        inline static bool isZero(const Vec2& val) {
            return val.isZero();
        }

        template <typename T>
        inline bool betwZeroOne(const T& v, f32 eps = maths::EPS) {
            return eps<v && v<(1.f-eps);
        }

        inline static f32 calcDistanceFromLine(const Vec2& pt, const Vec2& line_pt, const Dir2& line_normal) {
            return dot(pt-line_pt, line_normal);
        }

        // returns point id
        inline static u32 calcSupport(const Vec2* points, u32 points_cnt, const Dir2& dir) {
            ASSERT(points_cnt != 0);

            u32 best_id = 0;
            f32 best_proj = dot(points[0], dir);
            for (u32 i=1; i<points_cnt; ++i) {
                f32 proj = dot(points[i], dir);
                if (proj > best_proj) {
                    best_proj = proj;
                    best_id = i;
                }
            }
            return best_id;
        }

        // calcs also worst-id (support in negative dir)
        inline static void calcSupportBestWorst(Vec2* points, u32 points_cnt, const Dir2& dir, u32& best_id, u32& worst_id) {
            ASSERT(points_cnt != 0);

            best_id = 0;
            worst_id = 0;
            f32 best_proj = dot(points[0], dir);
            f32 worst_proj = best_proj;

            for (u32 i=1; i<points_cnt; ++i) {
                f32 proj = dot(points[i], dir);
                if (proj > best_proj) {
                    best_proj = proj;
                    best_id = i;
                }
                else if (proj < worst_proj) {
                    worst_proj = proj;
                    worst_id = i;
                }
            }
        }

        // v1 & v2 are two vectors of triangle sharing vertex
        // for positive result v1 -> v2 must be clockwise rotation
        inline static f32 calcTriangleArea(const Vec2& v1, const Vec2& v2) {
            return cross(v1, v2)*0.5f;
        }

        // clips points behind clipper_line
        inline static void clipLineByLine1(Vec2& clipped_line_start_io, Vec2& clipped_line_end_io, const Vec2& clipper_line_pt, const Dir2& clipper_line_n) {
            f32 d1 = maths::calcDistanceFromLine(clipped_line_start_io, clipper_line_pt, clipper_line_n);
            f32 d2 = maths::calcDistanceFromLine(clipped_line_end_io, clipper_line_pt, clipper_line_n);

            if (d1*d2 < 0.0f) {
                // If the points are on different sides of the plane
                // push intersection point
                f32 alpha = d1 / (d1 - d2);
                if (d1 < 0.0f) {
                    clipped_line_start_io = clipped_line_start_io + alpha * (clipped_line_end_io - clipped_line_start_io);
                } else {
                    clipped_line_end_io = clipped_line_start_io + alpha * (clipped_line_end_io - clipped_line_start_io);
                }
            }
            else if (d1 < 0.0f) {
                // both < 0
                clipped_line_start_io -= clipper_line_n*d1;
                clipped_line_end_io -= clipper_line_n*d2;
            }
        }

        // also returns distance from clipper
        inline static void clipLineByLine2(Vec2& clipped_line_start_io, Vec2& clipped_line_end_io, const Vec2& clipper_line_pt, const Dir2& clipper_line_n, f32 &d1_out, f32 &d2_out) {
            d1_out = maths::calcDistanceFromLine(clipped_line_start_io, clipper_line_pt, clipper_line_n);
            d2_out = maths::calcDistanceFromLine(clipped_line_end_io, clipper_line_pt, clipper_line_n);

            if (d1_out*d2_out < 0.0f) {
                // If the points are on different sides of the plane
                // push intersection point
                f32 alpha = d1_out / (d1_out - d2_out);
                if (d1_out < 0.0f) {
                    clipped_line_start_io = clipped_line_start_io + alpha * (clipped_line_end_io - clipped_line_start_io);
                    d1_out = 0.0f;
                } else {
                    clipped_line_end_io = clipped_line_start_io + alpha * (clipped_line_end_io - clipped_line_start_io);
                    d2_out = 0.0f;
                }
            }
            else if (d1_out < 0.0f) {
                // both < 0
                clipped_line_start_io -= clipper_line_n*d1_out;
                clipped_line_end_io -= clipper_line_n*d2_out;
                d1_out = d2_out = 0.0f;
            }
        }

        template <u32 N>
        f32 dotFloats(f32* vec1, f32* vec2) {
            f32 rslt = 0.0f;
            for (u32 i=0; i<N; ++i) {
                rslt += vec1[i]*vec2[i];
            }
            return rslt;
        }
    }
}

#endif //MATHS_FUNCS_H
