#include "Angle.h"
#include "Vec2.h"
#include <cmath>
#include <ostream>

namespace grynca {

    inline Angle::Angle(float rads)
     : rads_(rads)
    {
    }

    inline float Angle::getRads()const {
        return rads_;
    }

    inline float Angle::getDegs()const {
        return getRads() *180.f/Pi;
    }

    inline void Angle::setRads(float rads) {
        rads_ = rads;
    }

    inline void Angle::setDegs(float degs) {
        setRads(degs *Pi/180.f);
    }

    inline float Angle::getSin()const {
        return Internal::sin_fast2_(Internal::mod_Pi_(rads_));
    }
    inline float Angle::getCos()const {
        return Internal::cos_fast_(Internal::mod_Pi_(rads_));
    }

    inline void Angle::getSinCos(float& sin_out, float& cos_out) {
        sin_out = getSin();
        cos_out = Internal::cos_from_sin_(sin_out, rads_);
    }

    inline Vec2 Angle::getDirVec()const {
        float sin = getSin();
        float cos = Internal::cos_from_sin_(sin, rads_);
        return {cos, sin};
    }

    inline Angle& Angle::operator+=(float s) {
        setRads(rads_+s);
        return *this;
    }

    inline Angle& Angle::operator+=(const Angle& v) {
        setRads(rads_+v.rads_);
        return *this;
    }

    inline Angle& Angle::operator-=(float s) {
        setRads(rads_-s);
        return *this;
    }

    inline Angle& Angle::operator-=(const Angle& v) {
        setRads(rads_-v.rads_);
        return *this;
    }

    inline Angle& Angle::operator*=(float s) {
        setRads(rads_*s);
        return *this;
    }

    inline Angle& Angle::operator/=(float s) {
        setRads(rads_/s);
        return *this;
    }

    inline Angle Angle::operator-()const {
        return Angle(-rads_);
    }

    inline float Angle::Internal::mod_Pi_(float x) {
    //static
        float x1 = x * (1.0f / Pi);
        int whole_Pis = (int)x1;
        return Pi * (x1 - whole_Pis - (whole_Pis%2));
    }

    inline float Angle::Internal::sin_fast_(float x) {
    //static
        static const float B = 4.0f/Pi;
        static const float C = -4.0f/(Pi*Pi);
        static const float P = 0.225f;

        float y = B * x + C * x * fabsf(x);
        y = P * (y * fabsf(y) - y) + y;
        return y;
    }

    inline float Angle::Internal::sin_fast2_(float x) {
    // static
        float x2 = x * x;
        return (((((-2.05342856289746600727e-08f*x2 + 2.70405218307799040084e-06f)*x2
                   - 1.98125763417806681909e-04f)*x2 + 8.33255814755188010464e-03f)*x2
                 - 1.66665772196961623983e-01f)*x2 + 9.99999707044156546685e-01f)*x;
    }

    inline float Angle::Internal::cos_fast_(float x) {
    //static
        x += Pi/2;
        // because of this branch it is 2x slower as sin ;/
        if (x>Pi)
            x-= 2*Pi;
        return sin_fast2_(x);
    }

    inline float Angle::Internal::cos_from_sin_(float sinr, float x) {
    //static
        int q = abs((int)(x/M_PI_2))%4;
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

    inline Angle operator+(const Angle& lhs, float s) {
        return {lhs.rads_+s};
    }

    inline Angle operator-(const Angle& lhs, const Angle& rhs) {
        return {lhs.rads_-rhs.rads_};
    }

    inline Angle operator*(const Angle& lhs, float s) {
        return {lhs.rads_*s};
    }

    inline Angle operator/(const Angle& lhs, float s) {
        return {lhs.rads_/s};
    }

    inline Angle operator-(const Angle& lhs, float s) {
        return {lhs.rads_-s};
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
}