#ifndef VEC2_H
#define VEC2_H

#include "types/containers/fast_vector.h"
#include "glm_incl.h"

namespace grynca {
    //fw
    class Angle;
    class Mat3;
    class Interval;
    class Dir2;

    class Vec2 {
        friend Vec2 operator*(f32 n, const Vec2& v);
        friend Vec2 operator*(const Vec2& v, f32 s);
        friend Vec2 operator*(const Mat3& t, const Vec2& v);
        friend Dir2 operator*(const Mat3& t, const Dir2& v);
        friend Vec2 operator*(const Vec2& v1, const Vec2& v2);      // elementwise
        friend Vec2 operator/(const Vec2& v, f32 s);
        friend Vec2 operator/(const Vec2& v1, const Vec2& v2);      // elementwise
        friend Vec2 operator+(const Vec2& v1, const Vec2& v2);
        friend Vec2 operator-(const Vec2& v1, const Vec2& v2);
        friend bool operator==(const Vec2& v1, const Vec2& v2);
        friend bool operator!=(const Vec2& v1, const Vec2& v2);
        friend std::ostream& operator <<(std::ostream& os, const Vec2& v);
        friend f32 cross(const Vec2& v1, const Vec2& v2);     // 2D "cross", useful for telling on which side of first vector second vector is
        friend Vec2 cross(const Vec2& vec, f32 val);
        friend Vec2 cross(f32 val, const Vec2& vec);
        friend f32 dot(const Vec2& v1, const Vec2& v2);

        friend bool isOnSegment(const Vec2& p, const Vec2& start, const Vec2& end);
        friend bool isOnLine(const Vec2& p, const Vec2& start, const Vec2& end);
        friend bool isRightFromLine(const Vec2& p, const Vec2& start, const Vec2& end);
        friend bool isLeftFromLine(const Vec2& p, const Vec2& start, const Vec2& end);
        friend Vec2 normalize(const Vec2& v);

        // false only when parallel
        // v1/v2 is length vector (end-start)
        // t1_out/t2_out can be any number from -inf to inf
        friend bool overlapLineSegVSLine(const Vec2 &s1, const Vec2 &v1, const Vec2 &s2, const Vec2 &v2, f32 &t1_out);
        friend bool overlapLineSegVSLineSeg(const Vec2 &s1, const Vec2 &v1, const Vec2 &s2, const Vec2 &v2, f32 &t1_out, f32& t2_out);
    public:
        Vec2();
        Vec2(f32 x, f32 y);
        Vec2(const Vec2& v);

        // When in (+Ydown, +Xright coords. frame) positive rotation is clockwise
        Vec2 rotate(const Angle& a)const;
        Vec2 rotate(const Dir2& rot_dir)const;
        Vec2 rotateInverse(const Dir2& rot_dir)const;
        Vec2 rotateAround(const Vec2& rot_center, const Angle& a)const;
        Vec2 rotateAround(const Vec2& rot_center, const Dir2& rot_dir)const;
        Vec2 rotateAroundInverse(const Vec2& rot_center, const Dir2& rot_dir)const;

        Vec2 perpL()const;       // perpendicular left (rotated CCW)
        Vec2 perpR()const;       // perpendicular right (rotated CW)
        Angle getAngle()const;       // angle from +x axis

        f32 getSqrLen()const;
        f32 getLen()const;

        f32 getX()const;
        f32 getY()const;
        void setX(f32 x);
        void setY(f32 y);
        void set(f32 x, f32 y);
        f32& accX();
        f32& accY();
        f32& val(size_t i);
        const f32& val(size_t i)const;
        f32* getDataPtr();
        const f32* getDataPtr() const;

        bool isZero()const;
        bool isSame(const Vec2& v);
        f32 calcRatio()const;       // x/y

        Interval projectPoints(const Vec2* points, size_t n_points)const;

        // if dir id normalized it is also length
        f32 calcDirProjection(const Dir2& dir)const;


        Vec2& operator*=(f32 s);
        Vec2& operator*=(const Vec2& v);        // elementwise multiplication
        Vec2& operator/=(f32 s);
        Vec2& operator/=(const Vec2& v);        // elementwise division
        Vec2& operator+=(const Vec2& v);
        Vec2& operator-=(const Vec2& v);
        Vec2 operator-()const;
    protected:
        Vec2(const glm::vec2& v);

        glm::vec2 v_;
    };

    class Dir2 : public Vec2 {
    public:
        Dir2() : Vec2() {}
        Dir2(const Vec2& v) : Vec2(v) {}
        Dir2(f32 x, f32 y) : Vec2(x, y) {}

        Dir2 operator-()const { return {-v_.x, -v_.y}; }
    };
}

#include "Vec2.inl"
#endif //VEC2_H
