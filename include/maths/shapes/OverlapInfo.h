#ifndef OVERLAPINFO_H
#define OVERLAPINFO_H

#include "../Vec2.h"

namespace grynca {

    class OverlapInfo {
        friend class Ray;
        friend class Circle;
        friend class ARect;
        friend class Rect;
    public:
        static constexpr u32 MAX_INTERSECTIONS = 8;

        OverlapInfo();

        const Vec2& getDirOut()const;
        f32 getDepth()const;
        u32 getIntersectionsCount()const;
        const Vec2& getIntersection(u32 id)const;
        void setDepth(f32 d);
        void setDirOut(const Vec2& d);
    private:
        void addIntersection_(const Vec2& ip);

        Vec2 dir_out_;
        f32 depth_;
        u32 intersections_count_;
        Vec2 intersections_[MAX_INTERSECTIONS];
    };

}

#include "OverlapInfo.inl"
#endif //OVERLAPINFO_H
