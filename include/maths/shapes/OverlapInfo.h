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
        static constexpr uint32_t MAX_INTERSECTIONS = 8;

        OverlapInfo();

        const Vec2& getDirOut()const;
        float getDepth()const;
        uint32_t getIntersectionsCount()const;
        const Vec2& getIntersection(uint32_t id)const;
        void setDepth(float d);
        void setDirOut(const Vec2& d);
    private:
        void addIntersection_(const Vec2& ip);

        Vec2 dir_out_;
        float depth_;
        uint32_t intersections_count_;
        Vec2 intersections_[MAX_INTERSECTIONS];
    };

}

#include "OverlapInfo.inl"
#endif //OVERLAPINFO_H
