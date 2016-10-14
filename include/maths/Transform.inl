#include "Transform.h"

namespace grynca {
    inline Transform::Transform(const Vec2& position, const Angle& rotation, const Vec2& scale)
     : position_(position), rotation_(rotation), scale_(scale)
    {
        rotation_.getSinCos(sin_r_, cos_r_);
    }

    inline void Transform::setPosition(const Vec2& p) {
        position_ = p;
    }

    inline void Transform::setScale(const Vec2& s) {
        scale_ = s;
    }

    inline void Transform::setRotation(const Angle& r) {
        rotation_ = r;
        rotation_.getSinCos(sin_r_, cos_r_);
    }

    inline const Vec2& Transform::getPosition()const {
        return position_;
    }

    inline const Vec2& Transform::getScale()const {
        return scale_;
    }

    inline const Angle& Transform::getRotation()const {
        return rotation_;
    }

    inline float Transform::getRotSin()const {
        return sin_r_;
    }

    inline float Transform::getRotCos()const {
        return cos_r_;
    }

    inline Vec2& Transform::accPosition() {
        return position_;
    }

    inline Vec2& Transform::accScale() {
        return scale_;
    }

    inline Angle& Transform::accRotation() {
        return rotation_;
    }

    inline Mat3 Transform::calcMatrix()const {
        return Mat3::createTransform(position_, sin_r_, cos_r_, scale_);
    }

    inline void Transform::move(const Vec2& m) {
        position_+=m;
    }

    inline void Transform::moveRelative(const Vec2& m) {
        position_+= m.rotate(sin_r_, cos_r_);
    }

    inline void Transform::rotate(const Angle& r) {
        rotation_ += r;
        rotation_.getSinCos(sin_r_, cos_r_);
    }

    inline void Transform::scale(const Vec2& s) {
        scale_ *= s;
    }
}