#include "OverlapHelper.h"
#include "Shape.h"
#define TID(ST) Shape::getTypeIdOf<ST>()

namespace grynca {

    inline OverlapHelper::OverlapHelper()
     : ref_shape_id_(0), normal_mult_(1.0f)
    {
     shapes_[0] = shapes_[1] = NULL;
    }

    inline void OverlapHelper::set(const Shape& shapeA, const Shape& shapeB, const Transform& shapeA_tr, const Transform& shapeB_tr) {
        shapes_[0] = &shapeA;
        shape_tr_[0] = shapeA_tr;
        shapes_[1] = &shapeB;
        shape_tr_[1] = shapeB_tr;
        ref_shape_id_ = 0;
        normal_mult_ = 1.0f;
    }

    inline void OverlapHelper::setShapeA(const Shape& shape, const Transform& tr) {
        shapes_[0] = &shape;
        shape_tr_[0] = tr;
        ref_shape_id_ = 0;
        normal_mult_ = 1.0f;
    }

    inline void OverlapHelper::setShapeB(const Shape& shape, const Transform& tr) {
        shapes_[1] = &shape;
        shape_tr_[1] = tr;
        ref_shape_id_ = 0;
        normal_mult_ = 1.0f;
    }

    inline bool OverlapHelper::overlaps() {
        PROFILE_BLOCK("OverlapHelper::overlaps()");
        // one shape will be transformed to other shape's frame
        // this switch makes sure that it transforms shape that is cheaper to transform
        // then it calls overlaps() func directly

        const Shape* rs = shapes_[ref_shape_id_];
        const Shape* ts = shapes_[1-ref_shape_id_];
        i32 rs_tid = rs->getTypeId();
        i32 ts_tid = ts->getTypeId();
        switch (rs_tid) {
            case TID(Ray): {
                switch (ts_tid) {
                    case TID(Ray): return overlapsInner_<Ray, Ray>();
                    case TID(Circle): return overlapsInner_<Ray, Circle>();
                    case TID(ARect): changeRefShape_(); return overlapsInner_<ARect, Ray>();
                    case TID(Rect): changeRefShape_(); return overlapsInner_<Rect, Ray>();
                    case TID(Pgon): changeRefShape_(); return overlapsInner_<Pgon, Ray>();
                }
            }break;
            case TID(Circle): {
                switch (ts_tid) {
                    case TID(Circle): return overlapsInner_<Circle, Circle>();
                    case TID(Ray): changeRefShape_(); return overlapsInner_<Ray, Circle>();
                    case TID(ARect): changeRefShape_(); return overlapsInner_<ARect, Circle>();
                    case TID(Rect): changeRefShape_(); return overlapsInner_<Rect, Circle>();
                    case TID(Pgon): changeRefShape_(); return overlapsInner_<Pgon, Circle>();
                }
            }break;
            case TID(ARect): {
                switch (ts_tid) {
                    case TID(Ray): return overlapsInner_<ARect, Ray>();
                    case TID(Circle): return overlapsInner_<ARect, Circle>();
                    case TID(ARect):
                        // make Rect from target ARect so that it can be transformed (rotated) to ref. frame
                        return overlapsInner_<ARect, ARect, Rect>();
                    case TID(Rect): return overlapsInner_<ARect, Rect>();
                    case TID(Pgon):
                        // make Rect from target ARect so that it can be transformed (rotated) to ref. frame
                        changeRefShape_(); return  overlapsInner_<Pgon, ARect, Rect>();
                }
            }break;
            case TID(Rect): {
                switch (ts_tid) {
                    case TID(Ray): return overlapsInner_<Rect, Ray>();
                    case TID(Circle): return overlapsInner_<Rect, Circle>();
                    case TID(ARect): changeRefShape_(); return overlapsInner_<ARect, Rect>();
                    case TID(Rect): return overlapsInner_<Rect, Rect>();
                    case TID(Pgon): changeRefShape_(); return overlapsInner_<Pgon, Rect>();
                }
            }break;
            case TID(Pgon): {
                switch (ts_tid) {
                    case TID(Ray):return overlapsInner_<Pgon, Ray>();
                    case TID(Circle): return overlapsInner_<Pgon, Circle>();
                        // make Rect from ARect so that it can be transformed (rotated) to Pgon frame
                    case TID(ARect): return overlapsInner_<Pgon, ARect, Rect>();
                    case TID(Rect): return overlapsInner_<Pgon, Rect>();
                    case TID(Pgon): return overlapsInner_<Pgon, Pgon>();
                }
            }break;
        }
        NEVER_GET_HERE("unknown types");
        return false;
    }

    inline void OverlapHelper::calcContactG() {
        PROFILE_BLOCK("OverlapHelper::calcContactG()");
        ContactManifold& ref_cm = cm_l_[ref_shape_id_];
        shapes_[ref_shape_id_]->calcContact(tr_target_shape_, otmp_, ref_cm);
        ref_cm.normal *= normal_mult_;

        Mat3 trm = shape_tr_[ref_shape_id_].calcMatrix();
        cm_g_ = ref_cm;
        cm_g_.normal = trm*ref_cm.normal;
        for (u32 i=0; i<cm_g_.size; ++i) {
            cm_g_.points[i].position = trm * ref_cm.points[i].position;
        }
    }

    inline void OverlapHelper::calcContactGL() {
        calcContactG();

        ContactManifold& ref_cm = cm_l_[ref_shape_id_];
        Transform ref_to_target_tr = -target_to_ref_tr_;
        Mat3 trm = ref_to_target_tr.calcMatrix();
        ContactManifold& tgt_cm = cm_l_[1-ref_shape_id_];
        tgt_cm = ref_cm;
        tgt_cm.normal = trm*ref_cm.normal;
        for (u32 i=0; i<tgt_cm.size; ++i) {
            tgt_cm.points[i].position = trm * ref_cm.points[i].position;
        }
    }

    inline const ContactManifold& OverlapHelper::getContactManifoldG()const {
        return cm_g_;
    }

    inline ContactManifold& OverlapHelper::accContactManifoldG() {
        return cm_g_;
    }

    inline const ContactManifold& OverlapHelper::getContactManifoldA()const {
        return cm_l_[0];
    }

    inline ContactManifold& OverlapHelper::accContactManifoldA() {
        return cm_l_[0];
    }

    inline const ContactManifold& OverlapHelper::getContactManifoldB()const {
        return cm_l_[1];
    }

    inline ContactManifold& OverlapHelper::accContactManifoldB() {
        return cm_l_[1];
    }

    inline void OverlapHelper::changeRefShape_() {
        ref_shape_id_ = 1-ref_shape_id_;
        normal_mult_ *= -1;
    }

    template <typename RefShapeT, typename TargetShapeT, typename TargetShapeRsltT>
    inline bool OverlapHelper::overlapsInner_() {
        tr_target_shape_.set<TargetShapeRsltT>(shapes_[1-ref_shape_id_]->get<TargetShapeT>());
        target_to_ref_tr_ = (-shape_tr_[ref_shape_id_])*shape_tr_[1-ref_shape_id_];

        TargetShapeRsltT& ts = tr_target_shape_.acc<TargetShapeRsltT>();
        ts.transform(target_to_ref_tr_);
        const RefShapeT& rs = shapes_[ref_shape_id_]->get<RefShapeT>();
        return rs.overlaps(ts, otmp_);
    }

}

#undef TID