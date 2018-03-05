#include "Vec2.h"
#include "maths/common.h"
#include "Angle.h"
#include "Interval.h"
#include "Mat3.h"

namespace grynca {

    inline Vec2::Vec2()
     : v_(0.0f, 0.0f)
    {}

    inline Vec2::Vec2(f32 x, f32 y)
     : v_(x, y)
    {}

    inline Vec2::Vec2(const Vec2& v)
     : v_(v.v_)
    {}

    inline Vec2::Vec2(const glm::vec2& v)
     : v_(v)
    {}

    inline Vec2 Vec2::rotate(const Angle& a)const {
        return rotate(a.getDir());
    }

    inline Vec2 Vec2::rotate(const Dir2& rot_dir)const {
        return {v_.x*rot_dir.getX()-v_.y*rot_dir.getY(),
                v_.x*rot_dir.getY()+v_.y*rot_dir.getX()};
    }

    inline Vec2 Vec2::rotateInverse(const Dir2& rot_dir)const {
        // negate rotation sine
        return rotate(Vec2{rot_dir.getX(), -rot_dir.getY()});
    }

    inline Vec2 Vec2::rotateAround(const Vec2& rot_center, const Angle& a)const {
        return rotateAround(rot_center, a.getDir());
    }

    inline Vec2 Vec2::rotateAround(const Vec2& rot_center, const Dir2& rot_dir)const {
        glm::vec2 dv = v_-rot_center.v_;
        return {dv.x*rot_dir.getX()-dv.y*rot_dir.getY(),
                dv.x*rot_dir.getY()+dv.y*rot_dir.getX()};
    }

    inline Vec2 Vec2::rotateAroundInverse(const Vec2& rot_center, const Dir2& rot_dir)const {
        // negate rotation sine
        return rotateAround(rot_center, Vec2{rot_dir.getX(), -rot_dir.getY()});
    }

    inline Vec2 Vec2::perpL()const {
        return Vec2(v_.y, -v_.x);
    }

    inline Vec2 Vec2::perpR()const {
        return Vec2(-v_.y, v_.x);
    }

    inline Angle Vec2::getAngle()const {
        return Angle(atan2f(v_.y, v_.x));
    }

    inline f32 Vec2::getSqrLen()const {
        return glm::dot(v_, v_);
    }

    inline f32 Vec2::getLen()const {
        return glm::length(v_);
    }

    inline f32 Vec2::getX()const {
        return v_.x;
    }

    inline f32 Vec2::getY()const {
        return v_.y;
    }

    inline void Vec2::setX(f32 x) {
        v_.x = x;
    }

    inline void Vec2::setY(f32 y) {
        v_.y = y;
    }

    inline void Vec2::set(f32 x, f32 y) {
        v_.x = x;
        v_.y = y;
    }

    inline f32& Vec2::accX() {
        return v_.x;
    }

    inline f32& Vec2::accY() {
        return v_.y;
    }

    inline f32& Vec2::val(size_t i) {
        return v_[i];
    }

    inline const f32& Vec2::val(size_t i)const {
        return v_[i];
    }

    inline f32* Vec2::getDataPtr() {
        return &v_[0];
    }

    inline const f32* Vec2::getDataPtr() const {
        return &v_[0];
    }

    inline bool Vec2::isZero()const {
        return v_.x == 0 && v_.y == 0;
    }

    inline bool Vec2::isSame(const Vec2& v) {
        return fabsf((*this-v).getSqrLen()) < maths::EPS_SQRT;
    }

    inline f32 Vec2::calcRatio()const {
        return v_.x/v_.y;
    }

    inline Interval Vec2::projectPoints(const Vec2* points, size_t n_points)const {
        if (n_points == 0)
            return Interval();

        f32 min = glm::dot(v_, points[0].v_);
        f32 max = min;
        for (size_t i=1; i<n_points; ++i) {
            f32 p = glm::dot(v_, points[i].v_);
            if (p < min)
                min = p;
            else if (p > max)
                max = p;
        }
        return Interval(min, max);
    }

    inline f32 Vec2::calcDirProjection(const Dir2& dir)const {
        return dot(*this, dir);
    }

    inline Vec2& Vec2::operator*=(f32 s) {
        v_*=s;
        return *this;
    }

    inline Vec2& Vec2::operator*=(const Vec2& v) {
        v_*=v.v_;
        return *this;
    }

    inline Vec2& Vec2::operator/=(f32 s) {
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

    inline Vec2 Vec2::operator-()const {
        return {-v_};
    }

    inline Vec2 operator*(f32 s, const Vec2& v) {
        return {v.v_*s};
    }

    inline Vec2 operator*(const Vec2& v, f32 s) {
        return {v.v_*s};
    }

    inline Vec2 operator*(const Mat3& t, const Vec2& v) {
        glm::vec3 vh(v.v_, 1.0f);
        vh = t.m_*vh;
        return {vh.x, vh.y};
    }

    inline Dir2 operator*(const Mat3& t, const Dir2& v) {
        glm::vec3 vh(v.v_, 0.0f);
        vh = t.m_*vh;
        return {vh.x, vh.y};
    }

    inline Vec2 operator*(const Vec2& v1, const Vec2& v2) {
        return {v1.v_*v2.v_};
    }

    inline Vec2 operator/(const Vec2& v, f32 s) {
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

    inline f32 cross(const Vec2& v1, const Vec2& v2) {
        return v1.getX()*v2.getY() - v1.getY()*v2.getX();
    }

    inline Vec2 cross(const Vec2& vec, f32 val) {
        return Vec2(val*vec.getY(), -val*vec.getX());
    }

    inline Vec2 cross(f32 val, const Vec2& vec) {
        return Vec2(-val*vec.getY(), val*vec.getX());
    }

    inline f32 dot(const Vec2& v1, const Vec2& v2) {
        return glm::dot(v1.v_, v2.v_);
    }

    inline bool isOnSegment(const Vec2& p, const Vec2& start, const Vec2& end) {
        if (!isOnLine(p, start, end))
            return false;

        f32 dx = end.getX()-start.getX();
        if (dx != 0.0f) {
            // TODO: permit on endpoints ?
            return (p.getX() > start.getX()) && (p.getX() < end.getX());
        }
        // else -> vertical line
        return (p.getY() > start.getY()) && (p.getY() < end.getY());
    }


    inline bool isOnLine(const Vec2& p, const Vec2& start, const Vec2& end) {
        return fabsf(cross(p-start, end-start)) < maths::EPS;
    }

    inline bool isRightFromLine(const Vec2& p, const Vec2& start, const Vec2& end) {
        f32 c = cross(p-start, end-start);
        return c<-maths::EPS;
    }

    inline bool isLeftFromLine(const Vec2& p, const Vec2& start, const Vec2& end) {
        f32 c = cross(p-start, end-start);
        return c>maths::EPS;
    }

    inline Vec2 normalize(const Vec2& v) {
        return Vec2(glm::normalize(v.v_));
    }

    inline bool overlapLineSegVSLine(const Vec2& s1, const Vec2& v1, const Vec2& s2, const Vec2& v2, f32& t1_out) {
        f32 dxd = cross(v1, v2);

        if (dxd == 0)
            // parallel or collinear
            return false;

        t1_out = cross(s2-s1, v2)/dxd;
        return true;
    }

    inline bool overlapLineSegVSLineSeg(const Vec2 &s1, const Vec2 &v1, const Vec2 &s2, const Vec2 &v2, f32 &t1_out, f32& t2_out) {
        f32 dxd = cross(v1, v2);

        if (dxd == 0)
            // parallel or collinear
            return false;

        Vec2 sd = s2-s1;
        t1_out = cross(sd, v2)/dxd;
        t2_out = cross(sd, v1)/dxd;
        return true;
    }
}