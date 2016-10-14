#ifndef ANGLE_H
#define ANGLE_H
#include <iostream>

namespace grynca {

    // fw
    class Vec2;

    class Angle {
        friend std::ostream& operator<<(std::ostream& os, const Angle& a);
        friend Angle operator+(const Angle& lhs, const Angle& rhs);
        friend Angle operator+(const Angle& lhs, float s);
        friend Angle operator-(const Angle& lhs, const Angle& rhs);
        friend Angle operator-(const Angle& lhs, float s);
        friend Angle operator*(const Angle& lhs, float s);
        friend Angle operator/(const Angle& lhs, float s);
        friend bool operator==(const Angle& lhs, const Angle& rhs);
        friend bool operator!=(const Angle& lhs, const Angle& rhs);
        friend bool operator<=(const Angle& lhs, const Angle& rhs);
        friend bool operator>=(const Angle& lhs, const Angle& rhs);
        friend bool operator<(const Angle& lhs, const Angle& rhs);
        friend bool operator>(const Angle& lhs, const Angle& rhs);
    public:
        static constexpr float Pi = 3.1415927410125732421875f;

        Angle(float rads =0.0f);

        float getRads()const;
        float getDegs()const;
        void setRads(float rads);
        void setDegs(float degs);

        float getSin()const;
        float getCos()const;
        void getSinCos(float& sin_out, float& cos_out);

        Vec2 getDirVec()const;

        Angle& operator+=(float s);
        Angle& operator+=(const Angle& v);
        Angle& operator-=(float s);
        Angle& operator-=(const Angle& v);
        Angle& operator*=(float s);
        Angle& operator/=(float s);
        Angle operator-()const;

        struct Internal {
            static float mod_Pi_(float x);      // wraps angle to (-Pi, Pi)

            // input must be in (-Pi, Pi) interval
            // approximation with parabolas
            //http://forum.devmaster.net/t/fast-and-accurate-sine-cosine/9648
            static float sin_fast_(float x);
            static float sin_fast2_(float x);
            static float cos_fast_(float x);
            static float cos_from_sin_(float sinr, float x);
        };
    private:
        float rads_;
    };

}


#include "Angle.inl"
#endif //ANGLE_H
