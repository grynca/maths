#include "Transform.h"
#include "common.h"

namespace grynca {
    inline Transform::Transform(const Vec2& position, const Angle& rotation, const Vec2& scale)
     : position_(position), scale_(scale), rotation_(rotation)
    {
        rotation_.getSinCos(sin_r_, cos_r_);
    }

    inline Transform::Transform(const Mat3& m)
     : position_(m.val(2, 0), m.val(2, 1))
    {
        f32 a = m.val(0, 0);
        f32 b = m.val(0, 1);
        f32 c = m.val(1, 0);
        f32 d = m.val(1, 1);
        rotation_ = extractRotation_(a, b, sin_r_, cos_r_);
        scale_ = extractScale_(a, b, c, d, sin_r_, cos_r_);
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

    inline f32 Transform::getRotSin()const {
        return sin_r_;
    }

    inline f32 Transform::getRotCos()const {
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

    inline Dir2 Transform::getRightDir()const {
        return Dir2(cos_r_, sin_r_);
    }

    inline Dir2 Transform::getBottomDir()const {
        return Dir2(-sin_r_, cos_r_);
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


    inline Transform& Transform::operator*=(const Transform& t) {
        f32 a1 = cos_r_*scale_.getX();
        f32 b1 = sin_r_*scale_.getX();
        f32 c1 = -sin_r_*scale_.getY();
        f32 d1 = cos_r_*scale_.getY();

        f32 a2 = t.cos_r_*t.scale_.getX();
        f32 b2 = t.sin_r_*t.scale_.getX();
        f32 c2 = -t.sin_r_*t.scale_.getY();
        f32 d2 = t.cos_r_*t.scale_.getY();

        position_.accX() = t.position_.getX()*a1 + t.position_.getY()*c1 + position_.getX();
        position_.accY() = t.position_.getX()*b1 + t.position_.getY()*d1 + position_.getY();

        rotation_ = t.rotation_ + rotation_;
        rotation_.normalize();
        sin_r_ = t.sin_r_*cos_r_ + t.cos_r_*sin_r_;
        cos_r_ = t.cos_r_*cos_r_ - t.sin_r_*sin_r_;

        scale_ = extractScale_(a2*a1 + b2*c1,
                               a2*b1 + b2*d1,
                               c2*a1 + d2*c1,
                               d2*d1 + c2*b1,
                               sin_r_, cos_r_);
        return *this;
    }

    inline Transform& Transform::operator/=(const Transform& t) {
        return operator*=(-t);
    }

    inline Transform Transform::operator-()const {
        Transform rslt;
        rslt.rotation_ = -rotation_;
        rslt.position_ = -position_;
        rslt.scale_.set(1.0f/scale_.getX(), 1.0f/scale_.getY());
        rslt.sin_r_ = -sin_r_;
        rslt.cos_r_ = cos_r_;
        return rslt;
    }

    inline Transform operator*(const Transform& t1, const Transform& t2) {
        Transform rslt;

        f32 a1 = t1.cos_r_*t1.scale_.getX();
        f32 b1 = t1.sin_r_*t1.scale_.getX();
        f32 c1 = -t1.sin_r_*t1.scale_.getY();
        f32 d1 = t1.cos_r_*t1.scale_.getY();

        f32 a2 = t2.cos_r_*t2.scale_.getX();
        f32 b2 = t2.sin_r_*t2.scale_.getX();
        f32 c2 = -t2.sin_r_*t2.scale_.getY();
        f32 d2 = t2.cos_r_*t2.scale_.getY();

        rslt.position_.accX() = t2.position_.getX()*a1 + t2.position_.getY()*c1 + t1.position_.getX();
        rslt.position_.accY() = t2.position_.getX()*b1 + t2.position_.getY()*d1 + t1.position_.getY();

        rslt.rotation_ = t2.rotation_ + t1.rotation_;
        rslt.rotation_.normalize();
        rslt.sin_r_ = t2.sin_r_*t1.cos_r_ + t2.cos_r_*t1.sin_r_;
        rslt.cos_r_ = t2.cos_r_*t1.cos_r_ - t2.sin_r_*t1.sin_r_;

        rslt.scale_ = Transform::extractScale_(a2*a1 + b2*c1,
                                               a2*b1 + b2*d1,
                                               c2*a1 + d2*c1,
                                               d2*d1 + c2*b1,
                                               rslt.sin_r_, rslt.cos_r_);
        return rslt;
    }

    inline Transform operator/(const Transform& t1, const Transform& t2) {
        return operator*(t1, -t2);
    }

    inline Transform operator*(const Transform& t, f32 num) {
        return Transform(t.position_*num, t.rotation_*num, t.scale_*num);
    }

    inline Transform operator-(const Transform& t1, const Transform& t2) {
        return Transform(t1.position_ - t2.position_, t1.rotation_ - t2.rotation_, t1.scale_ - t2.scale_);
    }

    inline bool operator==(const Transform& t1, const Transform& t2) {
        return  t1.cos_r_ == t2.cos_r_
                && t1.sin_r_ == t2.sin_r_
                && t1.position_ == t2.position_
                && t1.scale_ == t2.scale_;
    }

    inline bool operator!=(const Transform& t1, const Transform& t2) {
        return !operator==(t1, t2);
    }

    inline std::ostream& operator<<(std::ostream& os, const Transform& t) {
        os << "P: " << t.position_ << ", R: " << t.rotation_ << ", S:" << t.scale_;
        return os;
    }

    inline Angle Transform::extractRotation_(f32 a, f32 b, f32& sin_out, f32& cos_out) {
        Angle rslt(atan2f(b, a));
        rslt.getSinCos(sin_out, cos_out);
        return rslt;
    }

    inline Vec2 Transform::extractScale_(f32 a, f32 b, f32 c, f32 d, f32 sin_r, f32 cos_r) {
        Vec2 rslt;
        if (fabs(sin_r) < maths::EPS) {
            rslt.setX(a/cos_r);
            rslt.setY(d/cos_r);
        }
        else {
            rslt.setX(b/sin_r);
            rslt.setY(c/sin_r);
        }
        return rslt;
    }
}