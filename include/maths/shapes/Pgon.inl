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

    inline uint32_t Pgon::getSize()const {
        return points_.size();
    }

    inline Vec2& Pgon::getPoint(uint32_t id) {
        return points_[id];
    }

    inline const Vec2& Pgon::getPoint(uint32_t id)const {
        return points_[id];
    }

    inline void Pgon::addPoint(const Vec2& p) {
        points_.push_back(p);
    }

    inline void Pgon::insertPoint(uint32_t pos, const Vec2& p) {
        points_.insert(points_.begin()+pos, p);
    }

    inline void Pgon::reverse() {
        std::reverse(points_.begin(), points_.end());
    }

    inline bool Pgon::isClockwise() {
        float area = 0;
        uint32_t psize = points_.size();
        for (uint32_t i=0; i<(psize-1); ++i) {
            Vec2& p1 = points_[i];
            Vec2& p2 = points_[i+1];
            area += (p2.getX()-p1.getX())*(p1.getY()+p2.getY());
        }

        Vec2& p1 = points_[psize-1];
        Vec2& p2 = points_[0];
        area += (p2.getX()-p1.getX())*(p1.getY()+p2.getY());
        return area < 0;
    }

    inline uint32_t Pgon::getSupportId(const Vec2& dir) {
        float best_proj = -std::numeric_limits<float>::max();
        uint32_t best_i = uint32_t(-1);
        for (uint32_t i=0; i<points_.size(); ++i) {
            float proj = dot(points_[i], dir);
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

        uint32_t psize = points_.size();
        Ray edges[psize];
        for (uint32_t i=0; i<psize-1; ++i) {
            edges[i] = Ray(points_[i], points_[i+1]);
        }
        edges[psize-1] = Ray(points_[psize-1], points_[0]);

        for (uint32_t j=2; j<psize-1; ++j) {
            if (edges[0].overlaps(edges[j])) {
                return false;
            }
        }

        for (uint32_t i=1; i<psize; ++i) {
            for (uint32_t j=i+2; j<psize; ++j) {
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
        uint32_t s = getSize();
        // first vertex
        bool is_reflex = isLeftFromLine(getPoint(s-1), getPoint(0), getPoint(1));
        if (is_reflex)
            return false;

        // inner vertices
        for (uint32_t i=1; i< s-1; ++i) {
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

    inline void PgonModifier::simplify(uint32_t pos) {
        // TODO: mozna vylepsit na O(logN *N) kdyz tohle bude pomaly
        //      http://geomalgorithms.com/a09-_intersect-3.html
        Pgon& pgon = (*pgons_)[pos];
        uint32_t pgons_cnt_prev = pgons_->size();
        uint32_t psize = pgon.points_.size();

        fast_vector<Ray> edges;
        edges.reserve(psize);
        for (uint32_t i=0; i<psize-1; ++i) {
            edges.emplace_back(pgon.points_[i], pgon.points_[i+1]);
        }
        edges.emplace_back(pgon.points_[psize-1], pgon.points_[0]);

        simplifyInnerRec_(pos, edges);

        uint32_t pgons_cnt_new = pgons_->size();
        for (uint32_t i=pgons_cnt_prev; i<pgons_cnt_new; ++i) {
            simplify(i);
        }
    }

    inline void PgonModifier::convexize(uint32_t pos) {
        Pgon& pgon = (*pgons_)[pos];
        ASSERT(pgon.isSimple());
        ASSERT(pgon.isClockwise());
        uint32_t s = pgon.getSize();
        ASSERT(s >= 3);

        // first point
        if (decomposeInnerRec_(pos, IdTriplet{s-1, 0, 1})) {
            return;
        }
        // other points
        for (uint32_t i=1; i<s-1; ++i) {
            if (decomposeInnerRec_(pos, IdTriplet{i-1, i, i+1})) {
                return;
            }
        }
        // last point
        if (decomposeInnerRec_(pos, IdTriplet{s-2, s-1, 0})) {
            return;
        }
    }

    inline void PgonModifier::half(uint32_t pos) {
        Pgon& pgon = (*pgons_)[pos];
        ASSERT(pgon.isConvex());

        uint32_t half_id = pgon.getSize()/2;

        pgons_->push_back();
        Pgon& new_pgon = pgons_->back();
        Pgon& old_pgon = (*pgons_)[pos];

        new_pgon.points_.insert(new_pgon.points_.end(), old_pgon.points_.begin()+half_id, old_pgon.points_.end());
        new_pgon.points_.push_back(old_pgon.points_[0]);
        old_pgon.points_.erase(old_pgon.points_.begin()+half_id+1, old_pgon.points_.end());
    }

    inline void PgonModifier::simplifyInnerRec_(uint32_t pos, fast_vector<Ray>& edges_io) {
        Pgon& pgon = (*pgons_)[pos];

        // first edge
        for (uint32_t j=2; j<pgon.points_.size()-1; ++j) {
            OverlapInfo oi;
            if (edges_io[0].overlaps(edges_io[j], oi)) {
                splitEdge_(pos, oi.getIntersection(0), edges_io, 0, j);
                simplifyInnerRec_(pos, edges_io);
                return;
            }
        }

        // other edges
        for (uint32_t i=1; i<pgon.points_.size()-1; ++i) {
            for (uint32_t j=i+2; j<pgon.points_.size(); ++j) {
                OverlapInfo oi;
                if (edges_io[i].overlaps(edges_io[j], oi)) {
                    splitEdge_(pos, oi.getIntersection(0), edges_io, i, j);
                    simplifyInnerRec_(pos, edges_io);
                    return;
                }
            }
        }
    }

    inline void PgonModifier::splitEdge_(uint32_t pos, const Vec2& split_point, fast_vector<Ray>& edges_io, uint32_t edge1_id, uint32_t edge2_id) {
        pgons_->push_back();
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

    inline bool PgonModifier::decomposeInnerRec_(uint32_t pos, const IdTriplet& v) {
        Pgon& pgon = (*pgons_)[pos];
        Vec2& vert_p = pgon.getPoint(v.p_id);
        Vec2& vert_n = pgon.getPoint(v.n_id);
        Vec2& vert = pgon.getPoint(v.id);
        bool is_reflex = isLeftFromLine(vert_n, vert_p, vert);
        if (!is_reflex)
            return false;

        uint32_t s = pgon.getSize();
        // https://mpen.ca/406/bayazit
        IntCtx lower, upper;

        // first vertex
        checkIntersectionCandidate_(pgon, v, IdTriplet{s-1, 0, 1}, lower, upper);
        // inner vertices
        for (uint32_t i=1; i< s-1; ++i) {
            checkIntersectionCandidate_(pgon, v, IdTriplet{i-1, i, i+1}, lower, upper);
        }
        // last vertex
        checkIntersectionCandidate_(pgon, v, IdTriplet{s-2, s-1, 0}, lower, upper);

        if (lower.v_id == (upper.v_id+1)%s) {
            // no vertices to connect to, create steiner vertex in the middle
            Vec2 steiner_pt((lower.int_point+upper.int_point)/2);

            pgons_->push_back();
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
        else {
            // connect to the closest point within the triangle
            uint32_t n = pgon.getSize();
            if (lower.v_id > upper.v_id) {
                upper.v_id += n;
            }

            dists_.clear();
            for (uint32_t i = lower.v_id; i <= upper.v_id; ++i) {
                Vec2& pt = pgon.getPoint(i%n);
                float d = (pt - vert).getSqrLen();
                dists_.push_back(d);
            }

            indirectSort(dists_.begin(), dists_.end(), dists_order_, std::less<float>());

            uint32_t i;
            uint32_t best_pt_id;
            for (i=0; i<dists_order_.size(); ++i) {
                best_pt_id = (dists_order_[i]+lower.v_id)%n;
                Vec2& pt = pgon.getPoint(best_pt_id);

                bool in_triangle = isRightFromLine(pt, vert_p, vert)
                                   && isLeftFromLine(pt, vert_n, vert);
                if (!in_triangle)
                    continue;

                if (v.id<best_pt_id) {
                    if (checkIfCrossed(pgon, v.id, best_pt_id)) {
                        continue;
                    }
                }
                else {
                    if (checkIfCrossed(pgon, best_pt_id, v.id)) {
                        continue;
                    }
                }
                // good point
                break;
            }

            ASSERT(i!=dists_order_.size() && "Suitable point not found.");

            pgons_->push_back();
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

        // split smallest first
        uint32_t last_id = pgons_->size()-1;
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
            float t;
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
            float t;
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

    inline bool PgonModifier::checkIfCrossed(Pgon& pgon, uint32_t v_id, uint32_t target_id) {
        Vec2& v = pgon.getPoint(v_id);
        Vec2& t = pgon.getPoint(target_id);

        for (uint32_t j=v_id; j<target_id; ++j) {
            Vec2& pt = pgon.getPoint(j);
            if (isRightFromLine(pt, v, t)) {
                return true;
            }
        }
        return false;
    }

    inline PolygonMesh::PolygonMesh()
     : mod_(polygons_)
    {}


    inline uint32_t PolygonMesh::getPgonsCount()const {
        return polygons_.size();
    }

    inline Pgon& PolygonMesh::getPgon(uint32_t id) {
        return polygons_[id];
    }

    inline const Pgon& PolygonMesh::getPgon(uint32_t id)const {
        polygons_[id];
    }


    inline void PolygonMesh::fix() {
        // TODO: mit vektor tech co jsou dirty a projet jenom ty
        //       kdyz bude polygon empty tak ho removnout ( kdyz bude moc malej, tak ho nekam pridat?, co kdyz bude invalidni (< 3 pointy) ?)
        //
        uint32_t s = polygons_.size();
        for (uint32_t i=0; i<s; ++i) {
            ASSERT(polygons_[i].isSimple());
            mod_.convexize(i);
        }
        for (uint32_t i=0; i<polygons_.size(); ++i) {
            while (polygons_[i].getSize() > MAX_PGON_SIZE) {
                mod_.half(i);
            }
        }
    }
}