#include "OverlapInfo.h"


namespace grynca {

    inline OverlapInfo::OverlapInfo()
     : depth_(0), intersections_count_(0)
    {}

    inline const Vec2& OverlapInfo::getDirOut()const{
        return dir_out_;
    }

    inline float OverlapInfo::getDepth()const{
        return depth_;
    }

    inline uint32_t OverlapInfo::getIntersectionsCount()const{
        return intersections_count_;
    }

    inline const Vec2& OverlapInfo::getIntersection(uint32_t id)const{
        ASSERT(id < intersections_count_, "Intersection point not defined.");
        return intersections_[id];
    }

    inline void OverlapInfo::addIntersection_(const Vec2& ip) {
        ASSERT(intersections_count_ < MAX_INTERSECTIONS, "Max intersections exceeded.");
        intersections_[intersections_count_++] = ip;
    }

    inline void OverlapInfo::setDepth(float d) {
        depth_ = d;
    }

    inline void OverlapInfo::setDirOut(const Vec2& d) {
        dir_out_ = d;
    }

}