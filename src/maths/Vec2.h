#ifndef VEC2_H
#define VEC2_H

#include "types/containers/fast_vector.h"
#include "glm/glm.hpp"

namespace grynca {
    //fw
    class Angle;
    class Mat3;
    class Interval;

    class Vec2 {
        friend Vec2 operator*(float n, const Vec2& v);
        friend Vec2 operator*(const Vec2& v, float s);
        friend Vec2 operator*(const Mat3& t, const Vec2& v);
        friend Vec2 operator*(const Vec2& v1, const Vec2& v2);      // elementwise
        friend Vec2 operator/(const Vec2& v, float s);
        friend Vec2 operator/(const Vec2& v1, const Vec2& v2);      // elementwise
        friend Vec2 operator+(const Vec2& v1, const Vec2& v2);
        friend Vec2 operator-(const Vec2& v1, const Vec2& v2);
        friend bool operator==(const Vec2& v1, const Vec2& v2);
        friend bool operator!=(const Vec2& v1, const Vec2& v2);
        friend std::ostream& operator <<(std::ostream& os, const Vec2& v);
        friend float cross(const Vec2& v1, const Vec2& v2);
        friend Vec2 normalize(const Vec2& v);
    public:
        Vec2();
        Vec2(float x, float y);

        // When in (+Ydown, +Xright coords. frame) positive rotation is clockwise
        Vec2 rotate(const Angle& a)const;
        Vec2 rotate(float sin_r, float cos_r)const;    // with precalculated sin, cos
        Vec2 rotateAround(const Vec2& rot_center, const Angle& a)const;
        Vec2 rotateAround(const Vec2& rot_center, float sin_r, float cos_r)const;    // with precalculated sin, cos

        Vec2 perpL()const;       // perpendicular left (rotated CCW)
        Vec2 perpR()const;       // perpendicular right (rotated CW)
        Angle getAngle()const;       // angle from +x axis

        float getSqrLen()const;
        float getLen()const;

        float getX()const;
        float getY()const;
        void setX(float x);
        void setY(float y);
        float& accX();
        float& accY();
        float& val(size_t i);
        float val(size_t i)const;

        bool isZero()const;

        Interval projectPoints(Vec2* points, size_t n_points)const;


        Vec2& operator*=(float s);
        Vec2& operator*=(const Vec2& v);        // elementwise multiplication
        Vec2& operator/=(float s);
        Vec2& operator/=(const Vec2& v);        // elementwise division
        Vec2& operator+=(const Vec2& v);
        Vec2& operator-=(const Vec2& v);
        Vec2 operator-()const;
    private:
        Vec2(const glm::vec2& v);

        glm::vec2 v_;
    };

}

#include "Vec2.inl"
#endif //VEC2_H
