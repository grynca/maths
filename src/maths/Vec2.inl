#include "Vec2.h"
#include "Angle.h"
#include "Interval.h"
#include "Mat3.h"

namespace grynca {

    inline Vec2::Vec2(float x, float y)
     : v_(x, y)
    {}

    inline Vec2::Vec2(const glm::vec2& v)
     : v_(v)
    {}

    inline Vec2 Vec2::rotate(const Angle& a)const {
        return rotate(a.getSin(), a.getCos());
    }

    inline Vec2 Vec2::rotate(float sin_r, float cos_r)const {
        return {v_.x*cos_r-v_.y*sin_r,
                v_.x*sin_r+v_.y*cos_r};
    }

    inline Vec2 Vec2::rotateAround(const Vec2& rot_center, const Angle& a)const {
        return rotateAround(rot_center, a.getSin(), a.getCos());
    }

    inline Vec2 Vec2::rotateAround(const Vec2& rot_center, float sin_r, float cos_r)const {
        glm::vec2 dv = v_-rot_center.v_;
        return {dv.x*cos_r-dv.y*sin_r,
                dv.x*sin_r+dv.y*cos_r};
    }

    inline Vec2 Vec2::perpL()const {
        return Vec2(-v_.y, v_.x);
    }

    inline Vec2 Vec2::perpR()const {
        return Vec2(v_.y, -v_.x);
    }

    inline Angle Vec2::getAngle()const {
        return Angle(atan2(-v_.y, v_.x));       // positive angle is clockwise
    }

    inline float Vec2::getSqrLen()const {
        return glm::dot(v_, v_);
    }

    inline float Vec2::getLen()const {
        return glm::length(v_);
    }

    inline float Vec2::getX()const {
        return v_.x;
    }

    inline float Vec2::getY()const {
        return v_.y;
    }

    inline void Vec2::setX(float x) {
        v_.x = x;
    }

    inline void Vec2::setY(float y) {
        v_.y = y;
    }

    inline float& Vec2::accX() {
        return v_.x;
    }

    inline float& Vec2::accY() {
        return v_.y;
    }

    inline float& Vec2::val(size_t i) {
        return v_[i];
    }

    inline float Vec2::val(size_t i)const {
        return v_[i];
    }

    inline bool Vec2::isZero()const {
        return v_.x == 0 && v_.y == 0;
    }

    inline Interval Vec2::projectPoints(Vec2* points, size_t n_points)const {
        if (n_points == 0)
            return Interval();

        float min = glm::dot(v_, points[0].v_);
        float max = min;
        for (size_t i=1; i<n_points; ++i) {
            float p = glm::dot(v_, points[i].v_);
            if (p < min)
                min = p;
            else if (p > max)
                max = p;
        }
        return Interval(min, max);
    }

    inline Vec2& Vec2::operator*=(float s) {
        v_*=s;
        return *this;
    }

    inline Vec2& Vec2::operator*=(const Vec2& v) {
        v_*=v.v_;
        return *this;
    }

    inline Vec2& Vec2::operator/=(float s) {
        v_/=s;
        return *this;
    }

    inline Vec2& Vec2::operator/=(const Vec2& v) {
        v_/=v.v_;
        return *this;
    }

    inline Vec2& Vec2::operator+=(const Vec2& v) {
        v_+=v.v_;
        return *this;
    }

    inline Vec2& Vec2::operator-=(const Vec2& v) {
        v_-=v.v_;
        return *this;
    }

    inline Vec2 Vec2::operator-() {
        return {-v_};
    }

    inline Vec2 operator*(float s, const Vec2& v) {
        return {v.v_*s};
    }

    inline Vec2 operator*(const Vec2& v, float s) {
        return {v.v_*s};
    }

    inline Vec2 operator*(const Mat3& t, const Vec2& v) {
        glm::vec3 vh(v.v_, 1.0f);
        vh = t.m_*vh;
        return {vh.x, vh.y};
    }

    inline Vec2 operator*(const Vec2& v1, const Vec2& v2) {
        return {v1.v_*v2.v_};
    }

    inline Vec2 operator/(const Vec2& v, float s) {
        return {v.v_/s};
    }

    inline Vec2 operator/(const Vec2& v1, const Vec2& v2) {
        return {v1.v_/v2.v_};
    }

    inline Vec2 operator+(const Vec2& v1, const Vec2& v2) {
        return {v1.v_+v2.v_};
    }

    inline Vec2 operator-(const Vec2& v1, const Vec2& v2) {
        return {v1.v_-v2.v_};
    }

    inline bool operator==(const Vec2& v1, const Vec2& v2) {
        return v1.v_ == v2.v_;
    }

    inline bool operator!=(const Vec2& v1, const Vec2& v2) {
        return v1.v_ != v2.v_;
    }

    inline std::ostream& operator <<(std::ostream& os, const Vec2& v) {
        os << "[" << v.getX() << ", " << v.getY() << "]";
        return os;
    }
}