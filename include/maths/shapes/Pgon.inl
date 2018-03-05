#include "shapes_h.h"
#include "../Transform.h"
#include "../maths_funcs.h"
#include "functions/sort_utils.h"
#include "OverlapTmp.h"
#include "ContactManifold.h"

namespace grynca {

    inline Pgon::Pgon()
    {}

    inline Pgon::Pgon(const Vec2* points, u32 points_cnt) {
        points_.resize(points_cnt);
        for (u32 i=0; i<points_cnt; ++i) {
            points_[i] = points[i];
        }
        dirty_normals_.set();
    }

    inline Pgon::Pgon(const Pgon& p)
     : dirty_normals_(p.dirty_normals_),
       points_(p.points_),
       normals_(p.normals_)
    {
    }

    inline ARect Pgon::calcARectBound()const {
        return ARect(&points_[0], u32(points_.size()));
    }

    inline void Pgon::transform(const Mat3& tr) {
        for (u32 i=0; i<points_.size(); ++i) {
            points_[i] = tr*points_[i];
        }
        if (!normals_.empty() || dirty_normals_.any()) {
            invalidateNormals();
        }
    }

    inline void Pgon::transform(const Transform& tr) {
        transform(tr.calcMatrix());
    }

    inline Pgon Pgon::transformOut(const Mat3& tr)const {
        Pgon rslt;
        rslt.points_.resize(points_.size());
        for (u32 i=0; i<points_.size(); ++i) {
            rslt.points_[i] = tr*points_[i];
        }
        if (!normals_.empty() || dirty_normals_.any()) {
            rslt.invalidateNormals();
        }
        return rslt;
    }

    inline Pgon Pgon::transformOut(const Transform& tr)const {
        return transformOut(tr.calcMatrix());
    }

    inline bool Pgon::isPointInside(const Vec2& p)const {
        ASSERT(isClockwise());

        u32 s = getSize();
        for (u32 i=0; i< s-1; ++i) {
            if (isLeftFromLine(p, getPoint(i), getPoint(i+1))) {
                return false;
            }
        }

        // last vertex
        return !isLeftFromLine(p, getPoint(s-1), getPoint(0));
    }

    inline f32 Pgon::calcArea()const {
        ASSERT(getSize() > 2);

        f32 area = 0.0f;
        u32 s = getSize();
        for (u32 i=1; i<(s-1); ++i) {
            area += points_[i].getX()*(points_[i+1].getY() - points_[i-1].getY());
        }
        area += points_[s-1].getX()*(points_[0].getY() - points_[s-2].getY());
        area += points_[0].getX()*(points_[1].getY() - points_[s-1].getY());
        return area*0.5f;
    }

    inline f32 Pgon::calcInertia()const {
        //https://www.physicsforums.com/threads/calculating-polygon-inertia.25293/page-2#post-210675
        // this "should" calc moment of inertia around polygon local (0,0)
        // if some other centroid is required you must use parallel axis theorem on result
        ASSERT(getSize() > 1);

        f32 denom = 0.0f;
        f32 numer = 0.0f;

        u32 sz = getSize();

        for(u32 j = sz-1, i = 0; i < sz; j = i, i ++) {
            const Vec2 p1 = getPoint(j);
            const Vec2 p2 = getPoint(i);

            f32 a = fabsf(cross(p1, p2));
            f32 b = dot(p2, p2) + dot(p2, p1) + dot(p1, p1);

            denom += a*b;
            numer += a;
        }
        return (1.0f/6.0f) * (denom / numer);
    }

    inline bool Pgon::isEmpty()const {
        return points_.empty();
    }

    inline u32 Pgon::getSize()const {
        return u32(points_.size());
    }

    inline Vec2& Pgon::accPoint(u32 id) {
        dirty_normals_.set(id);
        dirty_normals_.set(wrap(i32(id-1), u32(points_.size())));
        return points_[id];
    }

    inline const Vec2& Pgon::getPoint(u32 id)const {
        return points_[id];
    }

    inline const Vec2* Pgon::getPointsData()const {
        return points_.begin();
    }

    inline u32 Pgon::wrapPointId(u32 pt_id)const {
        return wrap(pt_id, (u32)points_.size());
    }

    inline Vec2* Pgon::accPointsData() {
        return points_.begin();
    }

    inline void Pgon::addPoint(const Vec2& p) {
        ASSERT(points_.size() < MATHS_MAX_PGON_SIZE);

        points_.push_back(p);
        u32 cnt = u32(points_.size());
        dirty_normals_.set(cnt-1);
        dirty_normals_.set(wrap(i32(cnt-2), u32(cnt)));
    }

    inline void Pgon::insertPoint(u32 pos, const Vec2& p) {
        ASSERT(points_.size() < MATHS_MAX_PGON_SIZE);
        points_.insert(points_.begin()+pos, p);

        if (!normals_.empty()) {
            normals_.insert(normals_.begin()+pos);
        }

        dirty_normals_.insert(pos, 1);      // shift dirty normals
        dirty_normals_.set(wrap(i32(pos-1), u32(points_.size())));
        dirty_normals_.set(pos);
    }

    inline void Pgon::insertPoints(u32 pos, const Vec2* points, u32 count) {
        ASSERT((points_.size()+count) <= MATHS_MAX_PGON_SIZE);
        points_.insert(points_.begin()+pos, points, points+count);

        if (!normals_.empty()) {
            normals_.insert(normals_.begin()+pos, count, Dir2());
        }

        dirty_normals_.insert(pos, count);    // shift dirty normals
        dirty_normals_.set(wrap(i32(pos-1), u32(points_.size())));
        for (u32 i=0; i<count; ++i) {
            dirty_normals_.set(pos+i);
        }
    }

    inline void Pgon::removePoint(u32 pos) {
        points_.erase(points_.begin()+pos);
        if (!normals_.empty()) {
            normals_.erase(normals_.begin()+pos);
        }

        dirty_normals_.remove(pos, 1);      // shift dirty normals
        dirty_normals_.set(pos);
    }

    inline void Pgon::removePoints(u32 pos, u32 count) {
        points_.erase(points_.begin()+pos, points_.begin()+pos+count);
        if (!normals_.empty()) {
            normals_.erase(normals_.begin()+pos, normals_.begin()+pos+count);
        }

        dirty_normals_.remove(pos, count);      // shift dirty normals
        dirty_normals_.set(pos);
    }

    template <typename Func>
    inline void Pgon::loopPoints(const Func& cb) {
        u32 psize = u32(points_.size());
        for (u32 i=0; i<psize; ++i) {
            cb(points_[i]);
        }
    }
    template <typename Func>
    inline void Pgon::loopPoints(const Func& cb)const {
        u32 psize = u32(points_.size());
        for (u32 i=0; i<psize; ++i) {
            cb(points_[i]);
        }
    }

    template <typename Func>
    inline void Pgon::loopEdges(const Func& cb) {
        u32 psize = u32(points_.size());
        u32 i;
        for (i=0; i<(psize-1); ++i) {
            cb(points_[i], points_[i+1], i);
        }
        if (i<psize) {
            // last edge
            cb(points_[psize-1], points_[0], i);
        }
    }

    template <typename Func>
    inline void Pgon::loopEdges(const Func& cb)const {
        u32 psize = u32(points_.size());
        u32 i;
        for (i=0; i<(psize-1); ++i) {
            cb(points_[i], points_[i+1], i);
        }
        if (i<psize) {
            // last edge
            cb(points_[psize-1], points_[0], i);
        }
    }

    inline void Pgon::reverse() {
        std::reverse(points_.begin(), points_.end());
        std::reverse(normals_.begin(), normals_.end());
        dirty_normals_.reverse();
    }

    inline Vec2 Pgon::calcSupport(const Dir2& dir)const {
        ASSERT_M(points_.size(), "dont call on empty pgon");
        u32 pt_id = maths::calcSupport(points_.begin(), (u32)points_.size(), dir);
        return points_[pt_id];
    }

    inline Vec2 Pgon::calcSupport(const Dir2& dir, u32& pt_id_out)const {
        ASSERT_M(points_.size(), "dont call on empty pgon");
        pt_id_out = maths::calcSupport(points_.begin(), (u32)points_.size(), dir);
        return points_[pt_id_out];
    }

    inline bool Pgon::isClockwise()const {
        f32 area = 0;
        loopEdges([&area](const Vec2& p1, const Vec2& p2, u32&) {
           area += (p2.getX()-p1.getX())*(p1.getY()+p2.getY());
        });
        return area < 0;
    }

    inline u32 Pgon::findNearestPointTo(const Vec2& target_pt, f32& dist_sqr_out)const {
        ASSERT(!points_.empty());

        u32 nearest = 0;
        dist_sqr_out = (target_pt - getPoint(0)).getSqrLen();
        for (u32 i=1; i<getSize(); ++i) {
            f32 sqrd = (target_pt - getPoint(i)).getSqrLen();
            if (sqrd < dist_sqr_out) {
                dist_sqr_out = sqrd;
                nearest = i;
            }
        }
        return nearest;
    }

    inline ARect Pgon::calcTightAABB() const {
        return ARect(&points_[0], u32(points_.size()));
    }

    inline ARect Pgon::calcFatAABB(const Vec2& fatness) const {
        ARect r = calcTightAABB();
        r.accBounds()[0] -= fatness;
        r.accBounds()[1] += fatness;
        return r;
    }

    inline Vec2 Pgon::calcCenter()const {
        Vec2 c;
        for (u32 i=0; i<points_.size(); ++i) {
            c += points_[i];
        }
        return c/points_.size();
    }

    inline bool Pgon::isSimple()const {
        // TODO: jde asi vylepsit na O(logN *N) (Bentley–Ottmann algorithm)
        //      http://geomalgorithms.com/a09-_intersect-3.html


        u32 psize = u32(points_.size());
        Vec2 edges_starts[MATHS_MAX_PGON_SIZE];
        Vec2 edges_vectors[MATHS_MAX_PGON_SIZE];

        loopEdges([&edges_starts, &edges_vectors](const Vec2& p1, const Vec2& p2, u32& i) {
            edges_starts[i] = p1;
            edges_vectors[i] = p2 - p1;
        });

        f32 t, u;
        for (u32 j=2; j<psize-1; ++j) {
            if (overlapLineSegVSLineSeg(edges_starts[0], edges_vectors[0], edges_starts[j], edges_vectors[j], t, u)
                && maths::betwZeroOne(t) && maths::betwZeroOne(u))
            {
                return false;
            }
        }

        for (u32 i=1; i<psize; ++i) {
            for (u32 j=i+2; j<psize; ++j) {
                if (overlapLineSegVSLineSeg(edges_starts[i], edges_vectors[i], edges_starts[j], edges_vectors[j], t, u)
                    && maths::betwZeroOne(t) && maths::betwZeroOne(u))
                {
                    return false;
                }
            }
        }
        return true;
    }

    inline bool Pgon::isConvex()const {
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
        return !is_reflex;
    }

    inline void Pgon::invalidateNormals() {
        dirty_normals_.set();
    }

    inline void Pgon::calculateNormalsIfNeeded()const {
        if (dirty_normals_.any()) {
            calculateNormals_();
            dirty_normals_.clear();
        }
    }

    inline const Dir2& Pgon::getNormal(u32 id)const {
        ASSERT_M(!dirty_normals_.any(), "Call calculateNormalsIfNeeded() first.");
        return normals_[id];
    }

    inline Dir2 Pgon::getEdgeDir(u32 id)const {
        return getNormal(id).perpR();
    }

    inline f32 Pgon::calcEdgeLength(u32 id)const {
        ASSERT(id != getSize()-1);
        Dir2 edge_d = getEdgeDir(id);
        Vec2 edge_v = getPoint(id+1) - getPoint(id);
        return dot(edge_d, edge_v);
    }

    inline f32 Pgon::calcLastEdgeLength()const {
        u32 id = getSize()-1;
        Dir2 edge_d = getEdgeDir(id);
        Vec2 edge_v = getPoint(0) - getPoint(id);
        return dot(edge_d, edge_v);
    }

    inline void Pgon::calcEdgeLengths(f32 (&lengths_out)[MATHS_MAX_PGON_SIZE])const {
        u32 i=0;
        for (; i<getSize()-1; ++i) {
            lengths_out[i] = calcEdgeLength(i);
        }
        lengths_out[i] = calcLastEdgeLength();
    }


    inline Dir2& Pgon::accNormal(u32 id) {
        ASSERT_M(!dirty_normals_.any(), "Call calculateNormalsIfNeeded() first.");
        return normals_[id];
    }

    template <typename ShapeT>
    inline bool Pgon::overlaps(const ShapeT& sh)const {
        OverlapTmp unused;
        return overlaps(sh, unused);
    }

    inline bool Pgon::overlaps(const ARect& ar, OverlapTmp& otmp)const {
        otmp.pgon_arect_.gjk.setShapes(*this, ar);
        return otmp.pgon_arect_.gjk.isOverlapping();
    }

    inline void Pgon::calcContact(const ARect& ar, OverlapTmp& otmp, ContactManifold& cm_out)const {
        UNUSED(ar);
        calculateNormalsIfNeeded();
        otmp.pgon_arect_.gjk.calcPenetrationInfo();
        cm_out = otmp.pgon_arect_.gjk.getContactManifold();
    }

    inline bool Pgon::overlaps(const Circle& c, OverlapTmp& otmp)const {
        //http://www.sevenson.com.au/actionscript/sat/
        ASSERT(getSize() > 2);
        calculateNormalsIfNeeded();
        // pgon edges
        for (u32 i = 0; i<getSize(); ++i) {
            const Vec2& vert = getPoint(i);
            const Dir2& normal = getNormal(i);
            f32 vert_proj = dot(normal, vert);
            f32 cc_proj = dot(normal, c.getCenter());

            f32 pen1 = vert_proj - (cc_proj+c.getRadius());
            f32 pen2 = vert_proj - (cc_proj-c.getRadius());
            otmp.pgon_circle_.pen[i] = std::max(pen1, pen2);
            if (otmp.pgon_circle_.pen[i] < maths::EPS) {
                return false;
            }
        }

        // project on axis between circle center & nearest point
        otmp.pgon_circle_.nearest_pt_id = findNearestPointTo(c.getCenter(), otmp.pgon_circle_.nearest_pt_d);
        otmp.pgon_circle_.nearest_pt_d = sqrtf(otmp.pgon_circle_.nearest_pt_d);
        const Vec2& nearest_pt = getPoint(otmp.pgon_circle_.nearest_pt_id);
        Vec2 dv = nearest_pt - c.getCenter();
        otmp.pgon_circle_.nearest_pt_dv_n = dv/otmp.pgon_circle_.nearest_pt_d;
        for (u32 i = 0; i<getSize(); ++i) {
            f32 proj = dot(otmp.pgon_circle_.nearest_pt_dv_n, getPoint(i)-c.getCenter());
            if (proj < c.getRadius())
                return true;
        }
        return false;
    }

    inline void Pgon::calcContact(const Circle& c, OverlapTmp& otmp, ContactManifold& cm_out)const {
        // min penetration for projections on normals
        u32 min_id = u32(std::min_element(otmp.pgon_circle_.pen, otmp.pgon_circle_.pen+getSize()) - otmp.pgon_circle_.pen);
        f32 min_pen = otmp.pgon_circle_.pen[min_id];

        cm_out.size = 1;
        // project point on closest edge
        Dir2 edge_dir = getEdgeDir(min_id);
        f32 d = dot(edge_dir, c.getCenter() - getPoint(min_id));
        if (d > 0.0f) {
            Vec2 cp = getPoint(min_id) + d*edge_dir;
            // check if projection is inside polygon
            const Vec2& next_edge_start = getPoint(wrap(min_id+1, getSize()));
            const Vec2& next_edge_end = getPoint(wrap(min_id+2, getSize()));
            if (isRightFromLine(cp, next_edge_start, next_edge_end)) {
                cm_out.normal = -getNormal(min_id);
                cm_out.points[0].penetration = min_pen;
                cm_out.points[0].position = cp;
                return;
            }
        }

        // check distance for circle from nearest point on pgon
        cm_out.normal = otmp.pgon_circle_.nearest_pt_dv_n;
        cm_out.points[0].penetration = c.getRadius() - otmp.pgon_circle_.nearest_pt_d;
        cm_out.points[0].position = getPoint(otmp.pgon_circle_.nearest_pt_id);
    }

    inline bool Pgon::overlaps(const Ray& r, OverlapTmp& otmp)const {
        otmp.pgon_ray_.rv = r.getToEndVec();

        bool rslt = false;
        loopEdges([&otmp, &rslt, &r](const Vec2& e_start, const Vec2& e_end, u32& i) {
            otmp.pgon_ray_.edge_vector = e_end-e_start;
            f32 u;
            if (overlapLineSegVSLineSeg(r.getStart(), otmp.pgon_ray_.rv, e_start, otmp.pgon_ray_.edge_vector, otmp.pgon_ray_.t1, u)
                && maths::betwZeroOne(otmp.pgon_ray_.t1) && maths::betwZeroOne(u))
            {
                rslt = true;
                otmp.pgon_ray_.edge_id = i;
                i = InvalidEdgeId;
            }
        });
        return rslt;
    }

    inline void Pgon::calcContact(const Ray& r, OverlapTmp& otmp, ContactManifold& cm_out)const {
        // try to find second intersection
        f32 t2;
        f32 u;
        u32 first_hit_edge_id = otmp.pgon_ray_.edge_id;
        ++otmp.pgon_ray_.edge_id;

        auto add_impale_f = [](const Ray& r, OverlapTmp& otmp, ContactManifold& cm_out, f32 t2) {
            // two intersections ... Impale
            if (t2<otmp.pgon_ray_.t1) {
                std::swap(otmp.pgon_ray_.t1, t2);
            }

            cm_out.size = 2;
            cm_out.normal = r.getDir();
            cm_out.points[0].penetration = (1.0f-otmp.pgon_ray_.t1)*r.getLength();
            cm_out.points[0].position = r.getStart()+otmp.pgon_ray_.t1*otmp.pgon_ray_.rv;
            cm_out.points[1].penetration = (1.0f-t2)*r.getLength();
            cm_out.points[1].position = r.getStart()+t2*otmp.pgon_ray_.rv;
        };

        for (; otmp.pgon_ray_.edge_id<getSize()-1; ++otmp.pgon_ray_.edge_id) {
            const Vec2& edge_start = getPoint(otmp.pgon_ray_.edge_id);
            const Vec2& edge_end = getPoint(otmp.pgon_ray_.edge_id+1);
            if (overlapLineSegVSLineSeg(r.getStart(), otmp.pgon_ray_.rv, edge_start, (edge_end-edge_start), t2, u)
                && maths::betwZeroOne(t2) && maths::betwZeroOne(u))
            {
                add_impale_f(r, otmp, cm_out, t2);
                return;
            }
        }
        if (otmp.pgon_ray_.edge_id<getSize()) {
            // last edge
            const Vec2& edge_start = getPoint(otmp.pgon_ray_.edge_id);
            const Vec2& edge_end = getPoint(0);
            if (overlapLineSegVSLineSeg(r.getStart(), otmp.pgon_ray_.rv, edge_start, (edge_end-edge_start), t2, u)
                && maths::betwZeroOne(t2) && maths::betwZeroOne(u))
            {
                add_impale_f(r, otmp, cm_out, t2);
                return;
            }
        }
        // one intersection ... Poke or ExitWound
        cm_out.size = 1;
        const Vec2& edge_start = getPoint(first_hit_edge_id);
        if (isRightFromLine(r.getStart(), edge_start, edge_start+otmp.pgon_ray_.edge_vector)) {
            //ExitWound
            cm_out.normal = -r.getDir();
            cm_out.points[0].penetration = otmp.pgon_ray_.t1*r.getLength();
        }
        else {
            // Poke
            cm_out.normal = r.getDir();
            cm_out.points[0].penetration = (1.0f-otmp.pgon_ray_.t1)*r.getLength();
        }
        cm_out.points[0].position = r.getStart()+otmp.pgon_ray_.t1*otmp.pgon_ray_.rv;
    }

    inline bool Pgon::overlaps(const Rect& r, OverlapTmp& otmp)const {
        otmp.pgon_rect_.gjk.setShapes(*this, r);
        return otmp.pgon_rect_.gjk.isOverlapping();
    }

    inline void Pgon::calcContact(const Rect& r, OverlapTmp& otmp, ContactManifold& cm_out)const {
        UNUSED(r);
        calculateNormalsIfNeeded();
        otmp.pgon_rect_.gjk.calcPenetrationInfo();
        cm_out = otmp.pgon_rect_.gjk.getContactManifold();
    }

    inline bool Pgon::overlaps(const Pgon& p, OverlapTmp& otmp)const {
        otmp.pgon_pgon_.gjk.setShapes(*this, p);
        return otmp.pgon_pgon_.gjk.isOverlapping();
    }

    inline void Pgon::calcContact(const Pgon& p, OverlapTmp& otmp, ContactManifold& cm_out)const {
        UNUSED(p);
        calculateNormalsIfNeeded();
        p.calculateNormalsIfNeeded();
        otmp.pgon_pgon_.gjk.calcPenetrationInfo();
        cm_out = otmp.pgon_pgon_.gjk.getContactManifold();
    }

    inline void Pgon::calculateNormals_()const {
        ASSERT(dirty_normals_.any());
        ASSERT(points_.size()>1);
        ASSERT(isClockwise());

        u32 pts_cnt = u32(points_.size());
        normals_.resize(pts_cnt);

        // remove invalid dirty flags (for nn existant points)
        while (true) {
            u32 last_id = u32(dirty_normals_.findLastSetBit());
            if (last_id >= pts_cnt) {
                dirty_normals_.reset(last_id);
                if (!dirty_normals_.any())
                    return;
            }
            else break;
        }

        u32 dirty_cnt = 0;
        u32 dirty_ids[MATHS_MAX_PGON_SIZE];
        LOOP_SET_BITS(dirty_normals_, it) {
            dirty_ids[dirty_cnt] = it.getPos();
            ++dirty_cnt;
        }

        // check if last normal is dirty
        bool last_dirty = (dirty_ids[dirty_cnt-1] == u32(pts_cnt-1));
        if (last_dirty) {
            // handle last normal
            const Vec2& pt1 = points_[pts_cnt-1];
            const Vec2& pt2 = points_[0];
            normals_[pts_cnt-1]=normalize((pt2-pt1).perpL());
            --dirty_cnt;
        }

        // handle other normals
        for (u32 i=0; i<dirty_cnt; ++i) {
            u32 nid = dirty_ids[i];
            const Vec2& pt1 = points_[nid];
            const Vec2& pt2 = points_[nid+1];
            normals_[nid]=normalize((pt2-pt1).perpL());
        }
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
        u32 pgons_cnt_prev = u32(pgons_->size());
        u32 psize = u32(pgon.getSize());

        Edges edges;
        edges.size = psize;
        for (u32 i=0; i<psize-1; ++i) {
            edges.starts[i] = pgon.getPoint(i);
            edges.vectors[i] = pgon.getPoint(i+1) - edges.starts[i];
        }
        edges.starts[psize-1] = pgon.getPoint(psize-1);
        edges.vectors[psize-1] = pgon.getPoint(0) - edges.starts[psize-1];

        simplifyInnerRec_(pos, edges);

        u32 pgons_cnt_new = u32(pgons_->size());
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

        new_pgon.insertPoints(new_pgon.getSize(), old_pgon.getPointsData()+half_id, pgon.getSize() - half_id);
        new_pgon.addPoint(old_pgon.getPoint(0));
        old_pgon.removePoints(half_id+1, pgon.getSize()-half_id-1);
    }

    inline void PgonModifier::simplifyInnerRec_(u32 pos, Edges& edges) {
        Pgon& pgon = (*pgons_)[pos];

        //first edge
        for (u32 j=2; j<pgon.getSize()-1; ++j) {
            f32 t1, t2;
            if (overlapLineSegVSLineSeg(edges.starts[0], edges.vectors[0], edges.starts[j], edges.vectors[j], t1, t2)
                && maths::betwZeroOne(t1) && maths::betwZeroOne(t2))
            {
                Vec2 cp = edges.starts[0] + edges.vectors[0]*t1;
                splitEdge_(pos, cp, edges, 0, j);
                simplifyInnerRec_(pos, edges);
                return;
            }
        }

        // other edges
        for (u32 i=1; i<pgon.getSize()-1; ++i) {
            for (u32 j=i+2; j<pgon.getSize(); ++j) {
                f32 t1, t2;
                if (overlapLineSegVSLineSeg(edges.starts[i], edges.vectors[i], edges.starts[j], edges.vectors[j], t1, t2)
                    && maths::betwZeroOne(t1) && maths::betwZeroOne(t2))
                {
                    Vec2 cp = edges.starts[i] + edges.vectors[i]*t1;
                    splitEdge_(pos, cp, edges, i, j);
                    simplifyInnerRec_(pos, edges);
                    return;
                }
            }
        }
    }

    inline void PgonModifier::splitEdge_(u32 pos, const Vec2& split_point, Edges& edges, u32 edge1_id, u32 edge2_id) {
        pgons_->emplace_back();
        Pgon& new_pgon = pgons_->back();
        Pgon& old_pgon = (*pgons_)[pos];

        edges.starts[edge1_id] = old_pgon.getPoint(edge1_id);
        edges.vectors[edge1_id] = split_point - edges.starts[edge1_id];
        edges.starts[edge2_id] = split_point;
        edges.vectors[edge2_id] = old_pgon.getPoint((edge2_id+1)%old_pgon.getSize()) - split_point;
        u32 cnt = edge2_id - edge1_id - 1;
        arrayErase(edges.starts, edge1_id+1, cnt, edges.size);
        arrayErase(edges.vectors, edge1_id+1, cnt, edges.size);
        edges.size -= cnt;
        new_pgon.addPoint(split_point);
        new_pgon.insertPoints(new_pgon.getSize(), old_pgon.getPointsData()+edge1_id+1, (edge2_id - edge1_id));
        old_pgon.accPoint(edge1_id+1) = split_point;
        old_pgon.removePoints(edge1_id+2, edge2_id - edge1_id -1);
    }

    inline bool PgonModifier::decomposeInnerRec_(u32 pos, const IdTriplet& v) {
        Pgon& pgon = (*pgons_)[pos];
        const Vec2& vert_p = pgon.accPoint(v.p_id);
        const Vec2& vert_n = pgon.accPoint(v.n_id);
        const Vec2& vert = pgon.accPoint(v.id);
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
                const Vec2& pt = pgon.accPoint(i%n);
                f32 d = (pt - vert).getSqrLen();
                dists_.push_back(d);
            }

            struct {
                bool operator()(f32* d1, f32* d2) { return *d1 < *d2; }
            } cmp;
            indirectSort(dists_.begin(), dists_.end(), sorted_dists_, cmp);

            u32 i;
            u32 best_pt_id = InvalidId();
            for (i=0; i<sorted_dists_.size(); ++i) {
                u32 dist_id = u32(sorted_dists_[i] - dists_.begin());
                best_pt_id = (dist_id+lower.v_id)%n;
                const Vec2& pt = pgon.accPoint(best_pt_id);

                bool in_triangle = isRightFromLine(pt, vert_p, vert)
                                   && isLeftFromLine(pt, vert_n, vert);
                if (!in_triangle)
                    continue;

                if (checkIfCrossed(pgon, v.id, best_pt_id))
                    continue;

                // good point
                break;
            }

            bool point_found = (i!=sorted_dists_.size());
            if (point_found) {
                pgons_->emplace_back();
                Pgon& new_pgon = pgons_->back();
                Pgon& old_pgon = (*pgons_)[pos];

                if (v.id<best_pt_id) {
                    u32 cnt = best_pt_id + 1 - v.id;
                    new_pgon.insertPoints(new_pgon.getSize(), old_pgon.getPointsData()+v.id, cnt);
                    cnt = best_pt_id - v.id - 1;
                    old_pgon.removePoints(v.id+1, cnt);
                }
                else {
                    u32 cnt = v.id + 1 - best_pt_id;
                    new_pgon.insertPoints(new_pgon.getSize(), old_pgon.getPointsData()+best_pt_id, cnt);
                    cnt = v.id - best_pt_id - 1;
                    old_pgon.removePoints(best_pt_id+1, cnt);
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
                u32 cnt = lower.v_id - v.id;
                new_pgon.insertPoints(new_pgon.getSize(), old_pgon.getPointsData()+v.id, cnt);
                new_pgon.addPoint(steiner_pt);
                cnt = lower.v_id - v.id - 1;
                old_pgon.removePoints(v.id+1, cnt);
                old_pgon.insertPoint(v.id+1, steiner_pt);
            }
            else {
                new_pgon.addPoint(steiner_pt);
                u32 cnt = v.id + 1 - lower.v_id;
                new_pgon.insertPoints(new_pgon.getSize(), old_pgon.getPointsData()+lower.v_id, cnt);
                cnt = v.id - lower.v_id;
                old_pgon.removePoints(lower.v_id, cnt);
                old_pgon.insertPoint(lower.v_id, steiner_pt);
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
        const Vec2& ref = pgon.accPoint(reflex_v.id);
        const Vec2& ref_n = pgon.accPoint(reflex_v.n_id);
        const Vec2& ref_p = pgon.accPoint(reflex_v.p_id);
        const Vec2& cp = pgon.accPoint(cand_v.id);
        const Vec2& cp_n = pgon.accPoint(cand_v.n_id);
        const Vec2& cp_p = pgon.accPoint(cand_v.p_id);

        if (isRightFromLine(cp, ref_p, ref) && isLeftFromLine(cp_p, ref_p, ref)) {
            f32 t;
            overlapLineSegVSLine(ref_p, (ref-ref_p), cp, (cp_p-cp), t);
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
            overlapLineSegVSLine(ref_n, (ref-ref_n), cp, (cp_n-cp), t);
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

        const Vec2& p1 = pgon.getPoint(p1_id);
        const Vec2& p2 = pgon.getPoint(p2_id);

        for (u32 i=p1_id+1; i<p2_id; ++i) {
            const Vec2& pt = pgon.getPoint(i);
            if (isRightFromLine(pt, p1, p2)) {
                return true;
            }
        }
        u32 ps = pgon.getSize();
        for (u32 i = (p2_id+1)%ps; i!= p1_id; i=(i+1)%ps) {
            const Vec2& pt = pgon.getPoint(i);
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
        return u32(polygons_.size());
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
        u32 s = u32(polygons_.size());
        for (u32 i=0; i<s; ++i) {
            ASSERT(polygons_[i].isSimple());
            mod_.convexize(i);
        }
        for (u32 i=0; i<polygons_.size(); ++i) {
            while (polygons_[i].getSize() > MATHS_MAX_PGON_SIZE) {
                mod_.half(i);
            }
        }
    }

    inline std::ostream& operator << (std::ostream& os, Pgon& p) {
        os << "Pgon= ";
        if (!p.points_.empty()) {
            os << p.points_[0];
            for (u32 i=1; i<p.points_.size(); ++i) {
                os << ", " << p.points_[i];
            }
        }
        return os;
    }
}