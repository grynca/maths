#ifndef GJK_H
#define GJK_H

#include "ContactManifold.h"

namespace grynca {

    template <typename ShapeT1, typename ShapeT2>
    class GJK2D {
    public:
        GJK2D();

        void setShapes(const ShapeT1& s1, const ShapeT2& s2);

        bool isOverlapping();
        void calcPenetrationInfo();

        const ContactManifold& getContactManifold()const;
    private:
        static constexpr u32 MAX_EPA_ITERATIONS = MATHS_EPA_MAX_EDGES - 3; // we start at 3 edges and each iteration 1 edge is added

        struct Supp {
            Supp() {}
            void set(const ShapeT1& s1, const ShapeT2& s2, const Dir2& d);

            u32 supp_a_id, supp_b_id;
            Vec2 supp_a;
            Vec2 supp_b;
            Vec2 supp;
        };

        struct EdgeCtx {
            f32 dist;
            Dir2 normal;
            Supp pt1, pt2;

            void calcContactManifold(const ShapeT1& s1, const ShapeT2& s2, ContactManifold& cm_out);
            void calcContactManifoldFlip(const ShapeT1& s1, const ShapeT2& s2, ContactManifold& cm_out);

            void clip_(ClipVertex* points, const Dir2& dir, f32 offset, u8 clip_edge);

            struct GreaterThan {
                bool operator()(const EdgeCtx& e1, const EdgeCtx& e2) {
                    return e1.dist > e2.dist;
                }
            };
        };

        struct Edges {
            const EdgeCtx& operator[](u32 i)const { ASSERT(i<size_); return *edges_[i]; }
            u32 size()const { return size_; }

            EdgeCtx pool_[MATHS_EPA_MAX_EDGES];
            EdgeCtx* edges_[MATHS_EPA_MAX_EDGES];        // keep these sorted by dist (descending)
            u32 size_;
        };


        void calcEdgeDistToZeroInner_(const Vec2& a, const Vec2& b, f32& dist_out, Dir2& norm_out);
        void calcEdgeCtx_(const Supp& a, const Supp& b, EdgeCtx& edge_out);
        void setPenInfo_(EdgeCtx& edge);

        const ShapeT1* s1_;
        const ShapeT2* s2_;

        Vec2 v_;
        Supp a_, b_, c_;

        // penetration info
        ContactManifold cm_;
    };

}

#include "GJK.inl"
#endif //GJK_H
