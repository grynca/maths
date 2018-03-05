#ifndef POLYGON_H
#define POLYGON_H

#include "../maths_config.h"
#include "types/Mask.h"
#include "shapes_fw.h"

namespace grynca {

    class Pgon {
        friend std::ostream& operator << (std::ostream& os, Pgon& p);
    public:
        static const u32 InvalidEdgeId = MATHS_MAX_PGON_SIZE;

        Pgon();
        Pgon(const Vec2* points, u32 points_cnt);
        Pgon(const Pgon& p);
        Pgon(Pgon&& p) = default;

        ARect calcARectBound()const;
        // changes "this"
        void transform(const Mat3& tr);
        void transform(const Transform& tr);
        // returns new transformed obj
        Pgon transformOut(const Mat3& tr)const;
        Pgon transformOut(const Transform& tr)const;

        bool isPointInside(const Vec2& p)const;
        f32 calcArea()const;
        f32 calcInertia()const;

        bool isEmpty()const;
        u32 getSize()const;
        Vec2& accPoint(u32 id);     // also dirties normals around the point
        const Vec2& getPoint(u32 id)const;
        u32 wrapPointId(u32 pt_id)const;

        // access to linear points memory
        const Vec2* getPointsData()const;
        Vec2* accPointsData();      // warn: this bypasses normal dirtying

        void addPoint(const Vec2& p);
        void insertPoint(u32 pos, const Vec2& p);
        void insertPoints(u32 pos, const Vec2* points, u32 count);
        void removePoint(u32 pos);
        void removePoints(u32 pos, u32 count);
        // f(Vec2& pt)
        template <typename Func>
        void loopPoints(const Func& cb);
        // f(const Vec2& pt)
        template <typename Func>
        void loopPoints(const Func& cb)const;

        // f(Vec2& e_start, Vec2& e_end, u32& edge_id)                  // for early exit store InvalidEdgeId to edge_id
        template <typename Func>
        void loopEdges(const Func& cb);
        // f(const Vec2& e_start, const Vec2& e_end, u32& edge_id)      // for early exit store InvalidEdgeId to edge_id
        template <typename Func>
        void loopEdges(const Func& cb)const;

        void reverse();
        Vec2 calcSupport(const Dir2& dir)const;
        Vec2 calcSupport(const Dir2& dir, u32& pt_id_out)const;
        u32 findNearestPointTo(const Vec2& target_pt, f32& dist_sqr_out)const;
        ARect calcTightAABB() const;
        ARect calcFatAABB(const Vec2& fatness) const;
        Vec2 calcCenter()const;

        bool isClockwise()const;
        bool isSimple()const;        // false if self-intersecting
        bool isConvex()const;

        // normals pointing out of polygon
        void invalidateNormals();
        void calculateNormalsIfNeeded()const;
        const Dir2& getNormal(u32 id)const;
        Dir2 getEdgeDir(u32 id)const;
        f32 calcEdgeLength(u32 id)const;        // id != getSize()-1
        f32 calcLastEdgeLength()const;
        void calcEdgeLengths(f32 (&lengths_out)[MATHS_MAX_PGON_SIZE])const;
        Dir2& accNormal(u32 id);

        template <typename ShapeT>
        bool overlaps(const ShapeT& sh)const;

        bool overlaps(const ARect& ar, OverlapTmp& otmp)const;
        void calcContact(const ARect& ar, OverlapTmp& otmp, ContactManifold& cm_out)const;

        bool overlaps(const Circle& c, OverlapTmp& otmp)const;
        void calcContact(const Circle& c,OverlapTmp& otmp, ContactManifold& cm_out)const;

        bool overlaps(const Ray& r, OverlapTmp& otmp)const;
        void calcContact(const Ray& r, OverlapTmp& otmp, ContactManifold& cm_out)const;

        bool overlaps(const Rect& r, OverlapTmp& otmp)const;
        void calcContact(const Rect& r, OverlapTmp& otmp, ContactManifold& cm_out)const;

        bool overlaps(const Pgon& p, OverlapTmp& otmp)const;
        void calcContact(const Pgon& p, OverlapTmp& otmp, ContactManifold& cm_out)const;
    private:
        void calculateNormals_()const;

        static constexpr u32 BitsForEdgeId_ = floorLog2(MATHS_MAX_PGON_SIZE);

        mutable Mask<MATHS_MAX_PGON_SIZE> dirty_normals_;

        fast_vector<Vec2> points_;
        mutable fast_vector<Dir2> normals_;
    };

    class PgonModifier {
    public:
        PgonModifier();
        PgonModifier(fast_vector<Pgon>& pgons_io);
        void setPgons(fast_vector<Pgon>& pgons_io);

        void simplify(u32 pos);        // splits to simple (non overlapping polygons)
        void convexize(u32 pos);       // splits to convex polygons
        void half(u32 pos);            // splits polygon to 2 halfs
    private:
        struct IdTriplet {
            u32 p_id, id, n_id;    // 3 neighboring vertex ids
        };
        struct IntCtx {
            IntCtx() : dist(std::numeric_limits<f32>::max()) {}

            f32 dist;
            Vec2 int_point;
            u32 v_id;
        };
        struct Edges {
            Vec2 starts[MATHS_MAX_PGON_SIZE];
            Vec2 vectors[MATHS_MAX_PGON_SIZE];
            u32 size;
        };

        void simplifyInnerRec_(u32 pos, Edges& edges);
        void splitEdge_(u32 pos, const Vec2& split_point, Edges& edges, u32 edge1_id, u32 edge2_id);

        bool decomposeInnerRec_(u32 pos, const IdTriplet& v);
        void checkIntersectionCandidate_(Pgon& pgon, const IdTriplet& reflex_v, const IdTriplet& cand_v, IntCtx& lower_out, IntCtx& upper_out);
        bool checkIfCrossed(Pgon& pgon, u32 v_id, u32 target_id);

        fast_vector<Pgon>* pgons_;
        fast_vector<f32> dists_;
        fast_vector<f32*> sorted_dists_;
    };

    class PolygonMesh {
    public:
        PolygonMesh();

        u32 getPgonsCount()const;
        Pgon& getPgon(u32 id);
        const Pgon& getPgon(u32 id)const;

        void fix();
    private:
        fast_vector<Pgon> polygons_;
        PgonModifier mod_;
    };

}
#endif //POLYGON_H

#if !defined(PGON_INL) && !defined(WITHOUT_IMPL)
#define PGON_INL
# include "Pgon.inl"
#endif // PGON_INL
