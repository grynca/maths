#include "Frame.h"

namespace grynca {
    inline Frame::Frame(const Vec2& position, const Angle& rotation, const Vec2& scale)
     : position_(position), rotation_(rotation), scale_(scale)
    {}

    inline void Frame::setPosition(const Vec2& p) {
        position_ = p;
    }

    inline void Frame::setScale(const Vec2& s) {
        scale_ = s;
    }

    inline void Frame::setRotation(const Angle& r) {
        rotation_ = r;
    }

    inline const Vec2& Frame::getPosition()const {
        return position_;
    }

    inline const Vec2& Frame::getScale()const {
        return scale_;
    }

    inline const Angle& Frame::getRotation()const {
        return rotation_;
    }

    inline Mat3 Frame::getTransform()const {
        return Mat3::createTransform(position_, rotation_, scale_);
    }

    inline void Frame::move(const Vec2& m) {
        position_+=m;
    }

    inline void Frame::moveRelative(const Vec2& m) {
        position_+= m.rotate(rotation_);
    }

    inline void Frame::rotate(const Angle& r) {
        rotation_ += r;
    }

    inline void Frame::scale(const Vec2& s) {
        scale_ *= s;
    }
}