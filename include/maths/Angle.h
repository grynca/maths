#ifndef ANGLE_H
#define ANGLE_H
#include <iostream>
#include "functions/defs.h"

namespace grynca {

    // fw
    class Vec2;

    class Angle {
        friend std::ostream& operator<<(std::ostream& os, const Angle& a);
        friend Angle operator+(const Angle& lhs, const Angle& rhs);
        friend Angle operator+(const Angle& lhs, f32 s);
        friend Angle operator-(const Angle& lhs, const Angle& rhs);
        friend Angle operator-(const Angle& lhs, f32 s);
        friend Angle operator*(const Angle& lhs, f32 s);
        friend Angle operator/(const Angle& lhs, f32 s);
        friend bool operator==(const Angle& lhs, const Angle& rhs);
        friend bool operator!=(const Angle& lhs, const Angle& rhs);
        friend bool operator<=(const Angle& lhs, const Angle& rhs);
        friend bool operator>=(const Angle& lhs, const Angle& rhs);
        friend bool operator<(const Angle& lhs, const Angle& rhs);
        friend bool operator>(const Angle& lhs, const Angle& rhs);
    public:
        static constexpr f32 Pi = 3.1415927410125732421875f;
        static constexpr f32 Pi_2 = Pi*0.5f;

        Angle(f32 rads =0.0f);

        f32 getRads()const;
        f32 getDegs()const;
        void setRads(f32 rads);
        void setDegs(f32 degs);

        f32 getSin()const;
        f32 getCos()const;
        void getSinCos(f32& sin_out, f32& cos_out);

        Vec2 getDirVec()const;

        Angle& operator+=(f32 s);
        Angle& operator+=(const Angle& v);
        Angle& operator-=(f32 s);
        Angle& operator-=(const Angle& v);
        Angle& operator*=(f32 s);
        Angle& operator/=(f32 s);
        Angle operator-()const;

        struct Internal {
            static f32 mod_Pi_(f32 x);      // wraps angle to (-Pi, Pi)

            // input must be in (-Pi, Pi) interval
            // approximation with parabolas
            //http://forum.devmaster.net/t/fast-and-accurate-sine-cosine/9648
            static f32 sin_fast_(f32 x);
            static f32 sin_fast2_(f32 x);
            static f32 cos_fast_(f32 x);
            static f32 cos_from_sin_(f32 sinr, f32 x);
        };
    private:
        f32 rads_;
    };

}


#include "Angle.inl"
#endif //ANGLE_H
