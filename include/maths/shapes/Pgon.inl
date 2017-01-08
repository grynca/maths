#include "Pgon.h"
#include "Ray.h"
#include "Rect.h"
#include "Circle.h"
#include "ARect.h"
#include "OverlapInfo.h"
#include "functions/sort_utils.h"

#ifndef MAX_PGON_SIZE
#   define MAX_PGON_SIZE 15
#endif

namespace grynca {

    inline ARect Pgon::calcARectBound()const {
        return ARect(&points_[0], points_.size());
    }

    inline bool Pgon::overlaps(const Ray& r)const {
        NEVER_GET_HERE("Not yet implemented.");
        return false;
    }

    inline bool Pgon::overlaps(const Ray& r, OverlapInfo& oi)const {
        NEVER_GET_HERE("Not yet implemented.");
        return false;
    }

    inline bool Pgon::overlaps(const Rect& r)const {
        NEVER_GET_HERE("Not yet implemented.");
        return false;
    }

    inline bool Pgon::overlaps(const Rect& r, OverlapInfo& oi)const {
        NEVER_GET_HERE("Not yet implemented.");
        return false;
    }

    inline bool Pgon::overlaps(const Circle& c)const {
        NEVER_GET_HERE("Not yet implemented.");
        return false;
    }

    inline bool Pgon::overlaps(const Circle& c, OverlapInfo& oi)const {
        NEVER_GET_HERE("Not yet implemented.");
        return false;
    }

    inline bool Pgon::overlaps(const ARect& r)const {
        NEVER_GET_HERE("Not yet implemented.");
        return false;
    }

    inline bool Pgon::overlaps(const ARect& r, OverlapInfo& oi)const {
        NEVER_GET_HERE("Not yet implemented.");
        return false;
    }

    inline bool Pgon::overlaps(const Pgon& p)const {
        NEVER_GET_HERE("Not yet implemented.");
        return false;
    }

    inline bool Pgon::overlaps(const Pgon& p, OverlapInfo& oi)const {
        NEVER_GET_HERE("Not yet implemented.");
        return false;
    }

    inline bool Pgon::isEmpty()const {
        return points_.empty();
    }

    inline u32 Pgon::getSize()const {
        return points_.size();
    }

    inline Vec2& Pgon::getPoint(u32 id) {
        return points_[id];
    }

    inline const Vec2& Pgon::getPoint(u32 id)const {
        return points_[id];
    }

    inline void Pgon::addPoint(const Vec2& p) {
        points_.push_back(p);
    }

    inline void Pgon::insertPoint(u32 pos, const Vec2& p) {
        points_.insert(points_.begin()+pos, p);
    }

    inline void Pgon::reverse() {
        std::reverse(points_.begin(), points_.end());
    }

    inline bool Pgon::isClockwise() {
        f32 area = 0;
        u32 psize = points_.size();
        for (u32 i=0; i<(psize-1); ++i) {
            Vec2& p1 = points_[i];
            Vec2& p2 = points_[i+1];
            area += (p2.getX()-p1.getX())*(p1.getY()+p2.getY());
        }

        Vec2& p1 = points_[psize-1];
        Vec2& p2 = points_[0];
        area += (p2.getX()-p1.getX())*(p1.getY()+p2.getY());
        return area < 0;
    }

    inline u32 Pgon::getSupportId(const Vec2& dir) {
        f32 best_proj = -std::numeric_limits<f32>::max();
        u32 best_i = u32(-1);
        for (u32 i=0; i<points_.size(); ++i) {
            f32 proj = dot(points_[i], dir);
            if (proj > best_proj) {
                best_proj = proj;
                best_i = i;
            }
        }
        return best_i;
    }

    inline ARect Pgon::calcTightAABB() const {
        return ARect(&points_[0], points_.size());
    }

    inline ARect Pgon::calcFatAABB(const Vec2& fatness) const {
        ARect r = calcTightAABB();
        r.accBounds()[0] -= fatness;
        r.accBounds()[1] += fatness;
        return r;
    }

    inline bool Pgon::isSimple() {
        // TODO: jde vylepsit na O(logN *N)
        //      http://geomalgorithms.com/a09-_intersect-3.html

        u32 psize = points_.size();
        fast_vector<Ray> edges(psize);
        for (u32 i=0; i<psize-1; ++i) {
            edges[i] = Ray(points_[i], points_[i+1]);
        }
        edges[psize-1] = Ray(points_[psize-1], points_[0]);

        for (u32 j=2; j<psize-1; ++j) {
            if (edges[0].overlaps(edges[j])) {
                return false;
            }
        }

        for (u32 i=1; i<psize; ++i) {
            for (u32 j=i+2; j<psize; ++j) {
                if (edges[i].overlaps(edges[j])) {
                    return false;
                }
            }
        }
        return true;
    }

    inline bool Pgon::isConvex() {
        ASSERT(isClockwise());

        // first vertex
        u32 s = getSize();
        // first vertex
        bool is_reflex = isLeftFromLine(getPoint(s-1), getPoint(0), getPoint(1));
        if (is_reflex)
            return false;

        // inner vertices
        for (u32 i=1; i< s-1; ++i) {
            is_reflex = isLeftFromLine(getPoint(i-1), getPoint(i), getPoint(i+1));
            if (is_reflex)
                return false;
        }

        // last vertex
        is_reflex = isLeftFromLine(getPoint(s-2), getPoint(s-1), getPoint(0));
        if (is_reflex)
            return false;

        return true;
    }

    inline PgonModifier::PgonModifier() {}
    inline PgonModifier::PgonModifier(fast_vector<Pgon>& pgons_io)
     : pgons_(&pgons_io)
    {}

    inline void PgonModifier::setPgons(fast_vector<Pgon>& pgons_io) {
        pgons_ = &pgons_io;
    }

    inline void PgonModifier::simplify(u32 pos) {
        // TODO: mozna vylepsit na O(logN *N) kdyz tohle bude pomaly
        //      http://geomalgorithms.com/a09-_intersect-3.html
        Pgon& pgon = (*pgons_)[pos];
        u32 pgons_cnt_prev = pgons_->size();
        u32 psize = pgon.points_.size();

        fast_vector<Ray> edges;
        edges.reserve(psize);
        for (u32 i=0; i<psize-1; ++i) {
            edges.emplace_back(pgon.points_[i], pgon.points_[i+1]);
        }
        edges.emplace_back(pgon.points_[psize-1], pgon.points_[0]);

        simplifyInnerRec_(pos, edges);

        u32 pgons_cnt_new = pgons_->size();
        for (u32 i=pgons_cnt_prev; i<pgons_cnt_new; ++i) {
            simplify(i);
        }
    }

    inline void PgonModifier::convexize(u32 pos) {
        Pgon& pgon = (*pgons_)[pos];
        ASSERT(pgon.isSimple());
        ASSERT(pgon.isClockwise());
        u32 s = pgon.getSize();
        ASSERT(s >= 3);

        // first point
        if (decomposeInnerRec_(pos, IdTriplet{s-1, 0, 1})) {
            return;
        }
        // other points
        for (u32 i=1; i<s-1; ++i) {
            if (decomposeInnerRec_(pos, IdTriplet{i-1, i, i+1})) {
                return;
            }
        }
        // last point
        if (decomposeInnerRec_(pos, IdTriplet{s-2, s-1, 0})) {
            return;
        }
    }

    inline void PgonModifier::half(u32 pos) {
        Pgon& pgon = (*pgons_)[pos];
        ASSERT(pgon.isConvex());

        u32 half_id = pgon.getSize()/2;

        pgons_->emplace_back();
        Pgon& new_pgon = pgons_->back();
        Pgon& old_pgon = (*pgons_)[pos];

        new_pgon.points_.insert(new_pgon.points_.end(), old_pgon.points_.begin()+half_id, old_pgon.points_.end());
        new_pgon.points_.push_back(old_pgon.points_[0]);
        old_pgon.points_.erase(old_pgon.points_.begin()+half_id+1, old_pgon.points_.end());
    }

    inline void PgonModifier::simplifyInnerRec_(u32 pos, fast_vector<Ray>& edges_io) {
        Pgon& pgon = (*pgons_)[pos];

        // first edge
        for (u32 j=2; j<pgon.points_.size()-1; ++j) {
            OverlapInfo oi;
            if (edges_io[0].overlaps(edges_io[j], oi)) {
                splitEdge_(pos, oi.getIntersection(0), edges_io, 0, j);
                simplifyInnerRec_(pos, edges_io);
                return;
            }
        }

        // other edges
        for (u32 i=1; i<pgon.points_.size()-1; ++i) {
            for (u32 j=i+2; j<pgon.points_.size(); ++j) {
                OverlapInfo oi;
                if (edges_io[i].overlaps(edges_io[j], oi)) {
                    splitEdge_(pos, oi.getIntersection(0), edges_io, i, j);
                    simplifyInnerRec_(pos, edges_io);
                    return;
                }
            }
        }
    }

    inline void PgonModifier::splitEdge_(u32 pos, const Vec2& split_point, fast_vector<Ray>& edges_io, u32 edge1_id, u32 edge2_id) {
        pgons_->emplace_back();
        Pgon& new_pgon = pgons_->back();
        Pgon& old_pgon = (*pgons_)[pos];

        edges_io[edge1_id] = Ray(old_pgon.points_[edge1_id], split_point);
        edges_io[edge2_id] = Ray(split_point, old_pgon.points_[(edge2_id+1)%old_pgon.points_.size()]);
        edges_io.erase(edges_io.begin()+edge1_id+1, edges_io.begin()+edge2_id);

        new_pgon.points_.push_back(split_point);
        new_pgon.points_.insert(new_pgon.points_.end(), old_pgon.points_.begin()+edge1_id+1, old_pgon.points_.begin()+edge2_id+1);

        old_pgon.points_[edge1_id+1] = split_point;
        old_pgon.points_.erase(old_pgon.points_.begin()+edge1_id+2, old_pgon.points_.begin()+edge2_id+1);
    }

    inline bool PgonModifier::decomposeInnerRec_(u32 pos, const IdTriplet& v) {
        Pgon& pgon = (*pgons_)[pos];
        Vec2& vert_p = pgon.getPoint(v.p_id);
        Vec2& vert_n = pgon.getPoint(v.n_id);
        Vec2& vert = pgon.getPoint(v.id);
        bool is_reflex = isLeftFromLine(vert_n, vert_p, vert);
        if (!is_reflex)
            return false;

        u32 s = pgon.getSize();
        // https://mpen.ca/406/bayazit
        IntCtx lower, upper;

        // first vertex
        checkIntersectionCandidate_(pgon, v, IdTriplet{s-1, 0, 1}, lower, upper);
        // inner vertices
        for (u32 i=1; i< s-1; ++i) {
            checkIntersectionCandidate_(pgon, v, IdTriplet{i-1, i, i+1}, lower, upper);
        }
        // last vertex
        checkIntersectionCandidate_(pgon, v, IdTriplet{s-2, s-1, 0}, lower, upper);

        // no vertices to connect to, create steiner vertex in the middle
        bool create_point = (lower.v_id == (upper.v_id+1)%s);
        if (!create_point) {
            // connect to the closest point within the triangle
            u32 n = pgon.getSize();
            if (lower.v_id > upper.v_id) {
                upper.v_id += n;
            }

            dists_.clear();
            for (u32 i = lower.v_id; i <= upper.v_id; ++i) {
                Vec2& pt = pgon.getPoint(i%n);
                f32 d = (pt - vert).getSqrLen();
                dists_.push_back(d);
            }

            indirectSort(dists_.begin(), dists_.end(), dists_order_, std::less<f32>());

            u32 i;
            u32 best_pt_id = InvalidId();
            for (i=0; i<dists_order_.size(); ++i) {
                best_pt_id = (dists_order_[i]+lower.v_id)%n;
                Vec2& pt = pgon.getPoint(best_pt_id);

                bool in_triangle = isRightFromLine(pt, vert_p, vert)
                                   && isLeftFromLine(pt, vert_n, vert);
                if (!in_triangle)
                    continue;

                if (checkIfCrossed(pgon, v.id, best_pt_id))
                    continue;

                // good point
                break;
            }

            bool point_found = (i!=dists_order_.size());
            if (point_found) {
                pgons_->emplace_back();
                Pgon& new_pgon = pgons_->back();
                Pgon& old_pgon = (*pgons_)[pos];

                if (v.id<best_pt_id) {
                    new_pgon.points_.insert(new_pgon.points_.end(), old_pgon.points_.begin()+v.id, old_pgon.points_.begin()+best_pt_id+1);
                    old_pgon.points_.erase(old_pgon.points_.begin()+v.id+1, old_pgon.points_.begin()+best_pt_id);
                }
                else {
                    new_pgon.points_.insert(new_pgon.points_.end(), old_pgon.points_.begin()+best_pt_id, old_pgon.points_.begin()+v.id+1);
                    old_pgon.points_.erase(old_pgon.points_.begin()+best_pt_id+1, old_pgon.points_.begin()+v.id);
                }
            }
            else {
                create_point = true;
            }
        }

        if (create_point) {
            Vec2 steiner_pt((lower.int_point+upper.int_point)/2);

            pgons_->emplace_back();
            Pgon& new_pgon = pgons_->back();
            Pgon& old_pgon = (*pgons_)[pos];

            if (v.id < lower.v_id) {
                new_pgon.points_.insert(new_pgon.points_.end(), old_pgon.points_.begin()+v.id, old_pgon.points_.begin()+lower.v_id);
                new_pgon.points_.push_back(steiner_pt);
                old_pgon.points_.erase(old_pgon.points_.begin()+v.id+1, old_pgon.points_.begin()+lower.v_id);
                old_pgon.points_.insert(old_pgon.points_.begin()+v.id+1, steiner_pt);
            }
            else {
                new_pgon.points_.push_back(steiner_pt);
                new_pgon.points_.insert(new_pgon.points_.end(), old_pgon.points_.begin()+lower.v_id, old_pgon.points_.begin()+v.id+1);
                old_pgon.points_.erase(old_pgon.points_.begin()+lower.v_id, old_pgon.points_.begin()+v.id);
                old_pgon.points_.insert(old_pgon.points_.begin()+lower.v_id, steiner_pt);
            }
        }

        // split smallest first
        u32 last_id = u32(pgons_->size())-1;
        if ((*pgons_)[pos].getSize() < (*pgons_)[last_id].getSize()) {
            convexize(pos);
            convexize(last_id);
        }
        else {
            convexize(last_id);
            convexize(pos);
        }
        return true;
    }

    inline void PgonModifier::checkIntersectionCandidate_(Pgon& pgon, const IdTriplet& reflex_v, const IdTriplet& cand_v, IntCtx& lower_out, IntCtx& upper_out) {
        Vec2& ref = pgon.getPoint(reflex_v.id);
        Vec2& ref_n = pgon.getPoint(reflex_v.n_id);
        Vec2& ref_p = pgon.getPoint(reflex_v.p_id);
        Vec2& cp = pgon.getPoint(cand_v.id);
        Vec2& cp_n = pgon.getPoint(cand_v.n_id);
        Vec2& cp_p = pgon.getPoint(cand_v.p_id);

        if (isRightFromLine(cp, ref_p, ref) && isLeftFromLine(cp_p, ref_p, ref)) {
            f32 t;
            overlapLines(ref_p, ref, cp, cp_p, t);
            if (t < lower_out.dist) {
                Vec2 int_point = ref_p + t*(ref-ref_p);
                if (isLeftFromLine(int_point, ref_n, ref)) {
                    lower_out.dist = t;
                    lower_out.int_point = int_point;
                    lower_out.v_id = cand_v.id;
                }
            }
        }
        if (isRightFromLine(cp_n, ref_n, ref) && isLeftFromLine(cp, ref_n, ref)) {
            f32 t;
            overlapLines(ref_n, ref, cp, cp_n, t);
            if (t < upper_out.dist) {
                Vec2 int_point = ref_n + t*(ref-ref_n);
                if (isRightFromLine(int_point, ref_p, ref)) {
                    upper_out.dist = t;
                    upper_out.int_point = int_point;
                    upper_out.v_id = cand_v.id;
                }
            }
        }
    }

    inline bool PgonModifier::checkIfCrossed(Pgon& pgon, u32 v_id, u32 target_id) {
        u32 p1_id = v_id, p2_id = target_id;
        if (v_id > target_id) {
            p1_id = target_id;
            p2_id = v_id;
        }

        Vec2& p1 = pgon.getPoint(p1_id);
        Vec2& p2 = pgon.getPoint(p2_id);

        for (u32 i=p1_id+1; i<p2_id; ++i) {
            Vec2& pt = pgon.getPoint(i);
            if (isRightFromLine(pt, p1, p2)) {
                return true;
            }
        }
        u32 ps = pgon.getSize();
        for (u32 i = (p2_id+1)%ps; i!= p1_id; i=(i+1)%ps) {
            Vec2& pt = pgon.getPoint(i);
            if (isRightFromLine(pt, p2, p1)) {
                return true;
            }
        }
        return false;
    }

    inline PolygonMesh::PolygonMesh()
     : mod_(polygons_)
    {}


    inline u32 PolygonMesh::getPgonsCount()const {
        return polygons_.size();
    }

    inline Pgon& PolygonMesh::getPgon(u32 id) {
        return polygons_[id];
    }

    inline const Pgon& PolygonMesh::getPgon(u32 id)const {
        return polygons_[id];
    }


    inline void PolygonMesh::fix() {
        // TODO: mit vektor tech co jsou dirty a projet jenom ty
        //       kdyz bude polygon empty tak ho removnout ( kdyz bude moc malej, tak ho nekam pridat?, co kdyz bude invalidni (< 3 pointy) ?)
        //
        u32 s = polygons_.size();
        for (u32 i=0; i<s; ++i) {
            ASSERT(polygons_[i].isSimple());
            mod_.convexize(i);
        }
        for (u32 i=0; i<polygons_.size(); ++i) {
            while (polygons_[i].getSize() > MAX_PGON_SIZE) {
                mod_.half(i);
            }
        }
    }
}