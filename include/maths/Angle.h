#ifndef ANGLE_H
#define ANGLE_H
#include <iostream>
#include "functions/defs.h"

namespace grynca {

    // fw
    class Vec2;
    class Dir2;

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
        friend f32 minAngleDiff(const Angle& a1, const Angle& a2);
    public:
        static constexpr f32 Pi = 3.1415927410125732421875f;
        static constexpr f32 Pi_2 = Pi*0.5f;
        static constexpr f32 Pi_4 = Pi*0.25f;

        constexpr Angle(f32 rads =0.0f);

        template<i32 FromDegs = -180, i32 ToDegs = 180, u32 DD = 10 /*degree divisibility*/>
        static Angle random();
        static constexpr Angle degrees(f32 degs);

        f32 getRads()const;
        f32 getDegs()const;
        void setRads(f32 rads);
        void setDegs(f32 degs);

        f32 getSin()const;
        f32 getCos()const;
        void getSinCos(f32& sin_out, f32& cos_out)const;

        Dir2 getDir()const;     // [cos, sin] , also first column of 2x2 rot_matrix

        bool isZero()const;

        static Dir2 invertRotDir(const Dir2& rot_dir);
        static Dir2 combineRotations(const Dir2& rot_dir1, const Dir2& rot_dir2);

        // normalize to -Pi..Pi
        Angle& normalize();
        Angle normalize()const;
        Angle& operator+=(f32 s);
        Angle& operator+=(const Angle& v);
        Angle& operator-=(f32 s);
        Angle& operator-=(const Angle& v);
        Angle& operator*=(f32 s);
        Angle& operator/=(f32 s);
        Angle operator-()const;
        operator f32()const;

    private:
        f32 cos_from_sin_(f32 sinr, f32 x)const;

        f32 rads_;
    };

}


#include "Angle.inl"
#endif //ANGLE_H
