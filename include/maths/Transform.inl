#include "Transform.h"
#include "common.h"
#include "glm/mat2x2.hpp"

namespace grynca {

    inline Transform::Transform()
    : position_(0, 0), scale_(1, 1), rotation_(0.0f), rot_dir_(1.0f, 0.0f)
    {}

    inline Transform::Transform(const Vec2& position, const Angle& rotation, const Vec2& scale)
     : position_(position), scale_(scale), rotation_(rotation), rot_dir_(rotation.getDir())
    {
    }

    inline Transform::Transform(const Mat3& m)
     : position_(m.val(2, 0), m.val(2, 1))
    {
        glm::mat2 rs_mat(m.val(0, 0), m.val(1, 0),
                         m.val(0, 1), m.val(1, 1));
        rotation_ = extractRotation_(rs_mat[0][0], rs_mat[1][0], rot_dir_);
        scale_ = extractScale_(rs_mat, rot_dir_);
    }

    inline Transform Transform::createTranslation(const Vec2& position) {
        // static
        return Transform(position, 0, {1,1});
    }

    inline Transform Transform::createRotation(const Angle& rotation) {
        // static
        return Transform({0, 0}, rotation, {1,1});
    }

    inline Transform Transform::createScale(const Vec2& scale) {
        // static
        return Transform({0, 0}, 0, scale);
    }

    Transform& Transform::set(const Transform& tr) {
        *this = tr;
        return *this;
    }

    inline Transform& Transform::setPosition(const Vec2& p) {
        position_ = p;
        return *this;
    }

    inline Transform& Transform::setScale(const Vec2& s) {
        scale_ = s;
        return *this;
    }

    inline Transform& Transform::setRotation(const Angle& r) {
        rotation_ = r;
        rot_dir_ = rotation_.getDir();
        return *this;
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

    inline Vec2& Transform::accPosition() {
        return position_;
    }

    inline Vec2& Transform::accScale() {
        return scale_;
    }

    inline const Dir2& Transform::getRightDir()const {
        return rot_dir_;
    }

    inline Dir2 Transform::getBottomDir()const {
        return rot_dir_.perpR();
    }

    inline Mat3 Transform::calcMatrix()const {
        return Mat3::createTransform(position_, rot_dir_, scale_);
    }

    inline Transform& Transform::move(const Vec2& m) {
        position_+=m;
        return *this;
    }

    inline Transform& Transform::moveRelative(const Vec2& m) {
        position_+= m.rotate(rot_dir_);
        return *this;
    }

    inline Transform& Transform::rotate(const Angle& r) {
        rotation_ += r;
        rot_dir_ = rotation_.getDir();
        return *this;
    }

    inline Transform& Transform::scale(const Vec2& s) {
        scale_ *= s;
        return *this;
    }

    inline const Dir2& Transform::getRotDir()const {
        return rot_dir_;
    }

    inline glm::mat2 Transform::calcRSMat()const {
        f32 cos_r = rot_dir_.getX();
        f32 sin_r = rot_dir_.getY();
        return glm::mat2 (cos_r*scale_.getX(), -sin_r*scale_.getY(),
                          sin_r*scale_.getX(), cos_r*scale_.getY());
    }

    inline bool Transform::isUnit()const {
        return position_.isZero() && rotation_.isZero() && scale_.getX() == 1 && scale_.getY() == 1;
    }

    inline Transform& Transform::operator*=(const Transform& t) {
        *this = *this*t;
        return *this;
    }

    inline Transform& Transform::operator/=(const Transform& t) {
        return operator*=(-t);
    }

    inline Transform Transform::operator-()const {
        Transform rslt;
        rslt.rotation_ = -rotation_;
        rslt.rot_dir_ = Angle::invertRotDir(rot_dir_);
        rslt.position_ = -position_.rotate(rslt.rot_dir_);
        rslt.scale_.set(1.0f/scale_.getX(), 1.0f/scale_.getY());
        return rslt;
    }

    inline Transform operator*(const Transform& t1, const Transform& t2) {
        Transform rslt;

        glm::mat2 m1 = t1.calcRSMat();
        glm::mat2 m2 = t2.calcRSMat();

        glm::vec2& t2_pos = *(glm::vec2*)t2.position_.getDataPtr();
        rslt.position_.accX() = glm::dot(t2_pos, m1[0]) + t1.position_.getX();
        rslt.position_.accY() = glm::dot(t2_pos, m1[1]) + t1.position_.getY();

        rslt.rotation_ = t2.rotation_ + t1.rotation_;
        rslt.rotation_.normalize();
        rslt.rot_dir_ = Angle::combineRotations(t1.rot_dir_, t2.rot_dir_);
        rslt.scale_ = Transform::extractScale_(m2*m1, rslt.rot_dir_);
        return rslt;
    }

    inline Transform operator/(const Transform& t1, const Transform& t2) {
        return operator*(t1, -t2);
    }

    inline Transform operator*(const Transform& t, f32 num) {
        return Transform(t.position_*num, t.rotation_*num, t.scale_*num);
    }

    inline Transform operator-(const Transform& t1, const Transform& t2) {
        return operator*(-t2, t1);
    }

    inline bool operator==(const Transform& t1, const Transform& t2) {
        return  t1.rot_dir_ == t2.rot_dir_
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

    inline Angle Transform::extractRotation_(f32 a, f32 b, Dir2& rot_dir_out) {
        Angle rslt(atan2f(b, a));
        rot_dir_out = rslt.getDir();
        return rslt;
    }

    inline Vec2 Transform::extractScale_(const glm::mat2& rs_mat, const Dir2& rot_dir) {
        glm::mat2 scale_m = rs_mat*glm::mat2(rot_dir.getX(), rot_dir.getY(), -rot_dir.getY(), rot_dir.getX());
        return Vec2(scale_m[0][0], scale_m[1][1]);
    }
}