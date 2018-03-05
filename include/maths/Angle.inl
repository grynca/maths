#include "Angle.h"
#include "Vec2.h"
#include <cmath>
#include <ostream>

namespace grynca {

    inline constexpr Angle::Angle(f32 rads)
     : rads_(rads)
    {
    }

    template<i32 FromDegs, i32 ToDegs, u32 DD>
    inline Angle Angle::random() {
        // static
        static constexpr u32 R = (ToDegs-FromDegs)*DD;
        static constexpr f32 F = FromDegs*Pi/180.f;
        static constexpr f32 N = Pi/(180.0f*DD);
        return f32(rand()%R)*N + F;
    }

    inline constexpr Angle Angle::degrees(f32 degs) {
        // static
        return Angle(degs *Angle::Pi/180.f);
    }

    inline f32 Angle::getRads()const {
        return rads_;
    }

    inline f32 Angle::getDegs()const {
        return getRads() *180.f/Pi;
    }

    inline void Angle::setRads(f32 rads) {
        rads_ = rads;
    }

    inline void Angle::setDegs(f32 degs) {
        setRads(degs *Pi/180.f);
    }

    inline f32 Angle::getSin()const {
        return sinf(rads_);
    }
    inline f32 Angle::getCos()const {
        return cosf(rads_);
    }

    inline void Angle::getSinCos(f32& sin_out, f32& cos_out)const {
#ifdef _WIN32
        sin_out = sinf(rads_);
        cos_out = cos_from_sin_(sin_out, rads_);
#else
        sincosf(rads_, &sin_out, &cos_out);
#endif
    }

    inline Dir2 Angle::getDir()const {
        f32 sin;
        f32 cos;
        getSinCos(sin, cos);
        return {cos, sin};
    }

    inline bool Angle::isZero()const {
        return rads_ == 0.0f;
    }

    inline Dir2 Angle::invertRotDir(const Dir2& rot_dir) {
    //static
        // negate sine
        return {rot_dir.getX(), -rot_dir.getY()};
    }

    inline Dir2 Angle::combineRotations(const Dir2& rot_dir1, const Dir2& rot_dir2) {
    // static
        f32 c1 = rot_dir1.getX();
        f32 s1 = rot_dir1.getY();
        f32 c2 = rot_dir2.getX();
        f32 s2 = rot_dir2.getY();
        return {c1*c2 - s1*s2, s1*c2 + c1*s2};
    }

    inline Angle& Angle::normalize() {
        f32 x1 = rads_ * (1.0f / Pi);
        int whole_Pis = (int)x1;
        rads_ = Pi * (x1 - whole_Pis - (whole_Pis%2));
        return *this;
    }

    inline Angle Angle::normalize()const {
        f32 x1 = rads_ * (1.0f / Pi);
        int whole_Pis = (int)x1;
        f32 rads = Pi * (x1 - whole_Pis - (whole_Pis%2));
        return Angle(rads);
    }

    inline Angle& Angle::operator+=(f32 s) {
        setRads(rads_+s);
        return *this;
    }

    inline Angle& Angle::operator+=(const Angle& v) {
        setRads(rads_+v.rads_);
        return *this;
    }

    inline Angle& Angle::operator-=(f32 s) {
        setRads(rads_ - s);
        return *this;
    }

    inline Angle& Angle::operator-=(const Angle& v) {
        setRads(rads_ - v.getRads());
        return *this;
    }

    inline Angle& Angle::operator*=(f32 s) {
        setRads(rads_*s);
        return *this;
    }

    inline Angle& Angle::operator/=(f32 s) {
        setRads(rads_/s);
        return *this;
    }

    inline Angle Angle::operator-()const {
        return Angle(-rads_);
    }

    inline Angle::operator f32()const {
        return rads_;
    }

    inline f32 Angle::cos_from_sin_(f32 sinr, f32 x)const {
    //static
        int q = abs((int)(x/Pi_2))%4;
        if (q == 1 || q == 2)
            return -sqrtf(1.0f - sinr*sinr);
        else
            return sqrtf(1.0f - sinr*sinr);
    }

    inline std::ostream& operator<<(std::ostream& os, const Angle& a) {
        os << a.rads_;
        return os;
    }

    inline Angle operator+(const Angle& lhs, const Angle& rhs) {
        return {lhs.rads_+rhs.rads_};
    }

    inline Angle operator+(const Angle& lhs, f32 s) {
        return {lhs.rads_+s};
    }

    inline Angle operator-(const Angle& lhs, const Angle& rhs) {
        return {lhs.getRads() - rhs.getRads()};
    }

    inline Angle operator*(const Angle& lhs, f32 s) {
        return {lhs.rads_*s};
    }

    inline Angle operator/(const Angle& lhs, f32 s) {
        return {lhs.rads_/s};
    }

    inline Angle operator-(const Angle& lhs, f32 s) {
        return {lhs.getRads() - s};
    }

    inline bool operator==(const Angle& lhs, const Angle& rhs) {
        return lhs.rads_ == rhs.rads_;
    }

    inline bool operator!=(const Angle& lhs, const Angle& rhs) {
        return lhs.rads_ != rhs.rads_;
    }

    inline bool operator<=(const Angle& lhs, const Angle& rhs) {
        return lhs.rads_ <= rhs.rads_;
    }

    inline bool operator>=(const Angle& lhs, const Angle& rhs) {
        return lhs.rads_ >= rhs.rads_;
    }

    inline bool operator<(const Angle& lhs, const Angle& rhs) {
        return lhs.rads_ < rhs.rads_;
    }

    inline bool operator>(const Angle& lhs, const Angle& rhs) {
        return lhs.rads_ > rhs.rads_;
    }

    inline f32 minAngleDiff(const Angle& a1, const Angle& a2) {
        return (a1-a2).normalize();
    }
}

#ifdef USE_FAST_SIN
#   undef sinf
#   undef cosf
#endif