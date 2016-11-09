#ifndef POLYGON_H
#define POLYGON_H

namespace grynca {

    //fw
    class Ray;
    class Rect;
    class Circle;
    class ARect;
    class OverlapInfo;

    class Pgon {
    public:
        ARect calcARectBound()const;

        bool overlaps(const Ray& r)const;
        bool overlaps(const Ray& r, OverlapInfo& oi)const;
        bool overlaps(const Rect& r)const;
        bool overlaps(const Rect& r, OverlapInfo& oi)const;
        bool overlaps(const Circle& c)const;
        bool overlaps(const Circle& c, OverlapInfo& oi)const;
        bool overlaps(const ARect& r)const;
        bool overlaps(const ARect& r, OverlapInfo& oi)const;
        bool overlaps(const Pgon& p)const;
        bool overlaps(const Pgon& p, OverlapInfo& oi)const;

        bool isEmpty()const;
        u32 getSize()const;
        Vec2& getPoint(u32 id);
        const Vec2& getPoint(u32 id)const;
        void addPoint(const Vec2& p);
        void insertPoint(u32 pos, const Vec2& p);
        void reverse();
        u32 getSupportId(const Vec2& dir);     // returns id of vert with longest projection in dir
        ARect calcTightAABB() const;
        ARect calcFatAABB(const Vec2& fatness) const;

        bool isClockwise();
        bool isSimple();        // false if self-intersecting
        bool isConvex();
    private:
        friend class PgonModifier;

        fast_vector<Vec2> points_;
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

        void simplifyInnerRec_(u32 pos, fast_vector<Ray>& edges_io);
        void splitEdge_(u32 pos, const Vec2& split_point, fast_vector<Ray>& edges_io, u32 edge1_id, u32 edge2_id);

        bool decomposeInnerRec_(u32 pos, const IdTriplet& v);
        void checkIntersectionCandidate_(Pgon& pgon, const IdTriplet& reflex_v, const IdTriplet& cand_v, IntCtx& lower_out, IntCtx& upper_out);
        bool checkIfCrossed(Pgon& pgon, u32 v_id, u32 target_id);

        fast_vector<Pgon>* pgons_;
        fast_vector<f32> dists_;
        fast_vector<u32> dists_order_;
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


#include "Pgon.inl"
#endif //POLYGON_H
