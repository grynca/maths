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
        uint32_t getSize()const;
        Vec2& getPoint(uint32_t id);
        const Vec2& getPoint(uint32_t id)const;
        void addPoint(const Vec2& p);
        void insertPoint(uint32_t pos, const Vec2& p);
        void reverse();
        uint32_t getSupportId(const Vec2& dir);     // returns id of vert with longest projection in dir
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

        void simplify(uint32_t pos);        // splits to simple (non overlapping polygons)
        void convexize(uint32_t pos);       // splits to convex polygons
        void half(uint32_t pos);            // splits polygon to 2 halfs
    private:
        struct IdTriplet {
            uint32_t p_id, id, n_id;    // 3 neighboring vertex ids
        };
        struct IntCtx {
            IntCtx() : dist(std::numeric_limits<float>::max()) {}

            float dist;
            Vec2 int_point;
            uint32_t v_id;
        };

        void simplifyInnerRec_(uint32_t pos, fast_vector<Ray>& edges_io);
        void splitEdge_(uint32_t pos, const Vec2& split_point, fast_vector<Ray>& edges_io, uint32_t edge1_id, uint32_t edge2_id);

        bool decomposeInnerRec_(uint32_t pos, const IdTriplet& v);
        void checkIntersectionCandidate_(Pgon& pgon, const IdTriplet& reflex_v, const IdTriplet& cand_v, IntCtx& lower_out, IntCtx& upper_out);
        bool checkIfCrossed(Pgon& pgon, uint32_t v_id, uint32_t target_id);

        fast_vector<Pgon>* pgons_;
        fast_vector<float> dists_;
        fast_vector<uint32_t> dists_order_;
    };

    class PolygonMesh {
    public:
        PolygonMesh();

        uint32_t getPgonsCount()const;
        Pgon& getPgon(uint32_t id);
        const Pgon& getPgon(uint32_t id)const;

        void fix();
    private:
        fast_vector<Pgon> polygons_;
        PgonModifier mod_;
    };

}


#include "Pgon.inl"
#endif //POLYGON_H
