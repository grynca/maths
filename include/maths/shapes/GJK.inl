#include "GJK.h"
#include "maths/maths_funcs.h"
#include "ClipVertex.h"

#define G2D_TPL template <typename ST1, typename ST2>
#define G2D_TYPE GJK2D<ST1, ST2>

namespace grynca {

    // http://vec3.ca/gjk/implementation/

    G2D_TPL
    inline G2D_TYPE::GJK2D()
     : s1_(NULL), s2_(NULL)
    {}

    G2D_TPL
    inline void G2D_TYPE::setShapes(const ST1& s1, const ST2& s2) {
        s1_ = &s1;
        s2_ = &s2;
    }

    G2D_TPL
    inline bool G2D_TYPE::isOverlapping() {
        v_.set(1, 0); //some arbitrary starting vector

        c_.set(*s1_, *s2_, v_);
        if(dot(c_.supp, v_) < maths::EPS) {
            return false;
        }

        // 0-simplex - point
        v_ = -c_.supp;
        b_.set(*s1_, *s2_, v_);

        // 1-simplex - line segment
        if(dot(b_.supp, v_) < maths::EPS) {
            return false;
        }

        Vec2 ab = c_.supp-b_.supp;
        f32 zero_on_left = (cross(-b_.supp, ab) > 0);
        if (!zero_on_left) {
            v_ = ab.perpL();
        }
        else {
            v_ = ab.perpR();
            std::swap(b_,c_);       // so that triangle will be clockwise winded
        }

        static constexpr f32 EPS = 2*maths::EPS;        // use more tolerant EPS
        // 2-simplex - triangle
        for(u32 iterations = 0; iterations < MATHS_GJK_MAX_ITERATIONS; iterations++ ) {
            // limit iterations (it can cycle when rounding errors happen)
            Supp a;
            a.set(*s1_, *s2_, v_);   // third triangle point

            if(dot(a.supp, v_) < EPS) {
                return false;
            }

            Vec2 ao = -a.supp;       // to origin

            // edges vectors
            Vec2 ab = b_.supp - a.supp;
            Vec2 ac = c_.supp - a.supp;

            //compute a vector within the plane of the triangle,
            //pointing away from the edge ab
            // out-pointing vector of AB
            Vec2 abp = ab.perpL();
            if(dot(abp, ao) > -EPS) {
                //the origin lies outside the triangle,
                //near the edge ab
                c_ = a;
                v_ = abp;
                continue;
            }

            //perform a similar test for the edge ac
            Vec2 acp = ac.perpR();
            if(dot(acp, ao) > -EPS) {
                b_ = a;
                v_ = acp;
                continue;
            }

            //if we get here, then the origin must be within the triangle
            a_ = a;     // is needed for EPA
            return true;
        }

        //out of iterations (probably rounding errors)
        // be strict
        return false;
    }

    G2D_TPL
    inline void G2D_TYPE::calcPenetrationInfo() {
        //http://www.dyn4j.org/2010/05/epa-expanding-polytope-algorithm/
        // we must find point on Minkowski diff. polygon that is nearest to origin
        // we already have triangle simplex
        //  -> calc distances to origin for all 3 edges and find nearest

        Edges edges;
        calcEdgeCtx_(a_, b_, edges.pool_[0]);
        calcEdgeCtx_(b_, c_, edges.pool_[1]);
        calcEdgeCtx_(c_, a_, edges.pool_[2]);
        edges.size_ = 3;

        sortThree(edges.pool_[0], edges.pool_[1], edges.pool_[2], edges.edges_, typename EdgeCtx::GreaterThan());

        // try to expand polygon in best_n dir and find some other point on Mink. diff border
        for(u32 iterations = 0; iterations < MAX_EPA_ITERATIONS; iterations++ ) {
            EdgeCtx* last_e = edges.edges_[edges.size_-1];   // last edge has best distance to zero
            Dir2 n = last_e->normal;
            Supp pt;
            pt.set(*s1_, *s2_, n);
            f32 pt_dist = dot(pt.supp, n);
            if (pt_dist - last_e->dist < maths::EPS) {
                // we could not expand in that direction (meaning we already reached Mink.diff border in that dir)
                // -> we are done
                setPenInfo_(*last_e);
                return;
            }

            // this edge is not on Min.diff. border
            //  -> expand edge to 2 new edges (going through "pt")
            Supp a = last_e->pt1;
            Supp b = last_e->pt2;

            EdgeCtx* e1 = last_e;   // reuse previous edge memory for first
            EdgeCtx* e2 = &edges.pool_[edges.size_];
            calcEdgeCtx_(a, pt, *e1);
            calcEdgeCtx_(pt, b, *e2);

            // keep new edges sorted
            --edges.size_;
            u32 ins_pos = bisectFindInsert(edges, *e1, typename EdgeCtx::GreaterThan());
            arrayInsert(edges.edges_, ins_pos, 1, edges.size_, MATHS_EPA_MAX_EDGES);
            edges.edges_[ins_pos] = e1;
            ++edges.size_;
            ins_pos = bisectFindInsert(edges, *e2, typename EdgeCtx::GreaterThan());
            arrayInsert(edges.edges_, ins_pos, 1, edges.size_, MATHS_EPA_MAX_EDGES);
            edges.edges_[ins_pos] = e2;
            ++edges.size_;
        }
        // run out of iterations (this should happen really rarely - probably due to floating point errors)
        setPenInfo_(*edges.edges_[edges.size_-1]);
    }

    G2D_TPL
    const ContactManifold& G2D_TYPE::getContactManifold()const {
        return cm_;
    }

    G2D_TPL
    inline void G2D_TYPE::Supp::set(const ST1& s1, const ST2& s2, const Dir2& d) {
        supp_a = s1.calcSupport(d, supp_a_id);
        supp_b = s2.calcSupport(-d, supp_b_id);
        supp = supp_a-supp_b;
    }

    G2D_TPL
    inline void G2D_TYPE::EdgeCtx::calcContactManifold(const ST1& s1, const ST2& s2, ContactManifold& cm_out) {
        // shape 1 is referent
        Vec2 ref_edge[] = {pt1.supp_a, pt2.supp_a};
        ClipVertex inc_edge[2];

        u32 ref_edge_id = pt1.supp_a_id;
        u32 ref_neg_edge_id = s1.wrapPointId(pt1.supp_a_id - 1);
        u32 ref_pos_edge_id = s1.wrapPointId(pt1.supp_a_id + 1);

        u32 inc_pt_id = pt1.supp_b_id;
        u32 inc_pt_neg_id = s2.wrapPointId(inc_pt_id - 1);
        u32 inc_pt_pos_id;

        Dir2 inc1_n = s2.getNormal(inc_pt_id);
        Dir2 inc2_n = s2.getNormal(inc_pt_neg_id);

        if (dot(inc1_n, normal) < dot(inc2_n, normal)) {
            // use next edge
            inc_pt_pos_id = s2.wrapPointId(inc_pt_id + 1);

            inc_edge[0].v = pt1.supp_b;
            inc_edge[1].v = s2.getPoint(inc_pt_pos_id);
        } else {
            // use prev edge
            inc_pt_pos_id = inc_pt_id;
            inc_pt_id = inc_pt_neg_id;
            inc_pt_neg_id = s2.wrapPointId(inc_pt_neg_id - 1);

            inc_edge[0].v = s2.getPoint(inc_pt_id);
            inc_edge[1].v = pt1.supp_b;
        }

        inc_edge[0].fp.e.in_edge2 = u8(inc_pt_neg_id)+1;
        inc_edge[0].fp.e.out_edge2 = u8(inc_pt_id)+1;
        inc_edge[1].fp.e.in_edge2 = u8(inc_pt_id)+1;
        inc_edge[1].fp.e.out_edge2 = u8(inc_pt_pos_id)+1;

        Dir2 ref_dir = s1.getEdgeDir(ref_edge_id);
        f32 o1 = dot(ref_edge[0], ref_dir);
        clip_(inc_edge, ref_dir, o1, u8(ref_neg_edge_id));
        f32 o2 = dot(ref_edge[1], ref_dir);
        clip_(inc_edge, -ref_dir, -o2, u8(ref_pos_edge_id));

        cm_out.normal = -normal;
        cm_out.size = 0;
        for (u32 i = 0; i < 2; ++i) {
            ContactPoint& cp = cm_out.points[cm_out.size];
            cp.penetration = maths::calcDistanceFromLine(inc_edge[i].v, ref_edge[0], cm_out.normal);
            if (cp.penetration > -maths::EPS) {
                // push cp on referent edge
                cp.position = inc_edge[i].v - cp.penetration * cm_out.normal;
                cp.feature_id = inc_edge[i].fp.value;
                ++cm_out.size;
            }
        }
    }

    G2D_TPL
    inline void G2D_TYPE::EdgeCtx::calcContactManifoldFlip(const ST1& s1, const ST2& s2, ContactManifold& cm_out) {
        // shape 2 is referent
        Vec2 ref_edge[] = {pt1.supp_b, pt2.supp_b};
        ClipVertex inc_edge[2];

        u32 ref_edge_id = pt1.supp_b_id;
        u32 ref_neg_edge_id = s2.wrapPointId(pt1.supp_b_id - 1);
        u32 ref_pos_edge_id = s2.wrapPointId(pt1.supp_b_id + 1);

        u32 inc_pt_id = pt1.supp_a_id;
        u32 inc_pt_neg_id = s1.wrapPointId(inc_pt_id - 1);
        u32 inc_pt_pos_id;

        Dir2 inc1_n = s1.getNormal(inc_pt_id);
        Dir2 inc2_n = s1.getNormal(inc_pt_neg_id);

        if (dot(inc1_n, -normal) < dot(inc2_n, -normal)) {
            // use next edge
            inc_pt_pos_id = s1.wrapPointId(inc_pt_id + 1);

            inc_edge[0].v = s1.getPoint(inc_pt_pos_id);
            inc_edge[1].v = pt1.supp_a;
        } else {
            // use prev edge
            inc_pt_pos_id = inc_pt_id;
            inc_pt_id = inc_pt_neg_id;
            inc_pt_neg_id = s1.wrapPointId(inc_pt_neg_id - 1);

            inc_edge[0].v = pt1.supp_a;
            inc_edge[1].v = s1.getPoint(inc_pt_id);
        }

        inc_edge[1].fp.e.in_edge2 = u8(inc_pt_neg_id)+1;
        inc_edge[1].fp.e.out_edge2 = u8(inc_pt_id)+1;
        inc_edge[0].fp.e.in_edge2 = u8(inc_pt_id)+1;
        inc_edge[0].fp.e.out_edge2 = u8(inc_pt_pos_id)+1;

        Dir2 ref_dir = s2.getEdgeDir(ref_edge_id);
        f32 o1 = dot(ref_edge[0], ref_dir);
        clip_(inc_edge, ref_dir, o1, u8(ref_neg_edge_id));
        f32 o2 = dot(ref_edge[1], ref_dir);
        clip_(inc_edge, -ref_dir, -o2, u8(ref_pos_edge_id));

        cm_out.normal = -normal;
        cm_out.size = 0;
        for (u32 i = 0; i < 2; ++i) {
            ContactPoint& cp = cm_out.points[cm_out.size];
            cp.penetration = maths::calcDistanceFromLine(inc_edge[i].v, ref_edge[0], normal);
            if (cp.penetration > -maths::EPS) {
                cp.position = inc_edge[i].v;
                cp.feature_id = inc_edge[i].fp.value;
                ++cm_out.size;
            }
        }
    }

    G2D_TPL
    inline void G2D_TYPE::EdgeCtx::clip_(ClipVertex* points, const Dir2& dir, f32 offset, u8 clip_edge) {
        f32 d1 = dot(points[0].v, dir) - offset;
        f32 d2 = dot(points[1].v, dir) - offset;
        if (d1*d2 < 0.0f) {
            f32 alpha = d1 / (d1 - d2);
            if (d1 < 0.0f) {
                points[0].v = points[0].v + alpha * (points[1].v - points[0].v);
                points[0].fp.e.in_edge1 = clip_edge+1;
                points[0].fp.e.in_edge2 = 0;
            } else {
                points[1].v = points[0].v + alpha * (points[1].v - points[0].v);
                points[1].fp.e.out_edge1 = clip_edge+1;
                points[1].fp.e.out_edge2 = 0;
            }
        }
        else if (d1 < 0.0f) {
            // both negative
            points[0].v -= dir*d1;
            points[1].v -= dir*d2;
        }
    }

    G2D_TPL
    inline void G2D_TYPE::calcEdgeDistToZeroInner_(const Vec2& a, const Vec2& b, f32& dist_out, Dir2& norm_out) {
        Vec2 ab = b-a;
        norm_out = normalize(ab.perpL());
        dist_out = dot(a, norm_out);
    }

    G2D_TPL
    inline void G2D_TYPE::calcEdgeCtx_(const Supp& a, const Supp& b, EdgeCtx& edge_out) {
        edge_out.pt1 = a;
        edge_out.pt2 = b;
        calcEdgeDistToZeroInner_(a.supp, b.supp, edge_out.dist, edge_out.normal);
    }

    G2D_TPL
    inline void G2D_TYPE::setPenInfo_(EdgeCtx& edge) {
        if (edge.pt1.supp_a_id == edge.pt2.supp_a_id) {
            edge.calcContactManifoldFlip(*s1_, *s2_, cm_);
        } else {
            edge.calcContactManifold(*s1_, *s2_, cm_);
        }
        ASSERT(cm_.size > 0);
    }
}

#undef G2D_TPL
#undef G2D_TYPE