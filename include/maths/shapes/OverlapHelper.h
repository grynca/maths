#ifndef OVERLAPHELPER_H
#define OVERLAPHELPER_H

#include "../Transform.h"
#include "OverlapTmp.h"
#include "ContactManifold.h"

namespace grynca {

    // fw
    class Shape;

    class OverlapHelper {
    public:
        OverlapHelper();

        void set(const Shape& shapeA, const Shape& shapeB, const Transform& shapeA_tr, const Transform& shapeB_tr);
        void setShapeA(const Shape& shape, const Transform& tr);
        void setShapeB(const Shape& shape, const Transform& tr);

        bool overlaps();
        void calcContactG();        // calculates global manifold
        void calcContactGL();       // calculates both global and local manifolds

        const ContactManifold& getContactManifoldG()const;
        ContactManifold& accContactManifoldG();
        const ContactManifold& getContactManifoldA()const;
        ContactManifold& accContactManifoldA();
        const ContactManifold& getContactManifoldB()const;
        ContactManifold& accContactManifoldB();
    private:
        void changeRefShape_();
        template <typename RefShapeT, typename TargetShapeT, typename TargetShapeRsltT = TargetShapeT>
        bool overlapsInner_();

        u32 ref_shape_id_;
        f32 normal_mult_;
        const Shape* shapes_[2];
        Shape tr_target_shape_;     // transformed target shape

        Transform shape_tr_[2];
        Transform target_to_ref_tr_;

        OverlapTmp otmp_;
        ContactManifold cm_l_[2];   // local contact manifolds
        ContactManifold cm_g_;      // global contact manifold
    };

}

#include "OverlapHelper.inl"
#endif //OVERLAPHELPER_H
