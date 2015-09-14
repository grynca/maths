#include "Angle.h"
#include <cmath>
#include <ostream>

namespace grynca {

    inline Angle::Angle(float rads)
     : rads_(mod_Pi_(rads))
    {}

    inline float Angle::getRads()const {
        return rads_;
    }

    inline float Angle::getDegs()const {
        return rads_ *180.f/Pi;
    }

    inline void Angle::setRads(float rads) {
        rads_ = mod_Pi_(rads);
    }

    inline void Angle::setDegs(float degs) {
        rads_ = mod_Pi_(degs *Pi/180.f);
    }

    inline float Angle::getSin()const {
        return sin_fast_(rads_);
    }
    inline float Angle::getCos()const {
        return cos_fast_(rads_);
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

    inline float Angle::mod_Pi_(float x) {
    //static
        float x1 = x * (1.0f / Pi);
        int whole_Pis = (int)x1;
        return Pi * (x1 - whole_Pis - (whole_Pis%2));
    }

    inline float Angle::sin_fast_(float x) {
    //static
        static const float B = 4.0f/Pi;
        static const float C = -4.0f/(Pi*Pi);
        static const float P = 0.225f;

        float y = B * x + C * x * fabs(x);

        y = P * (y * fabs(y) - y) + y;
        return y;
    }

    inline float Angle::cos_fast_(float x) {
    //static
        x += Pi/2;
        // because of this branch it is 2x slower as sin ;/
        if (x>Pi)
            x-= 2*Pi;
        return sin_fast_(x);
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