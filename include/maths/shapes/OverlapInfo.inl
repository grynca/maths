#include "OverlapInfo.h"


namespace grynca {

    inline OverlapInfo::OverlapInfo()
     : depth_(0), intersections_count_(0)
    {}

    inline const Dir2& OverlapInfo::getDirOut()const{
        return dir_out_;
    }

    inline f32 OverlapInfo::getDepth()const{
        return depth_;
    }

    inline u32 OverlapInfo::getIntersectionsCount()const{
        return intersections_count_;
    }

    inline const Vec2& OverlapInfo::getIntersection(u32 id)const{
        ASSERT_M(id < intersections_count_, "Intersection point not defined.");
        return intersections_[id];
    }

    inline void OverlapInfo::addIntersection_(const Vec2& ip) {
        ASSERT_M(intersections_count_ < MAX_INTERSECTIONS, "Max intersections exceeded.");
        intersections_[intersections_count_++] = ip;
    }

    inline void OverlapInfo::setDepth(f32 d) {
        depth_ = d;
    }

    inline void OverlapInfo::setDirOut(const Dir2& d) {
        dir_out_ = d;
    }

    inline Vec2 OverlapInfo::getVecOut() {
        return dir_out_*depth_;
    }

}