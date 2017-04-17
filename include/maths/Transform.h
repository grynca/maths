#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Vec2.h"
#include "Mat3.h"
#include "Angle.h"

namespace grynca {

    class Transform {

        friend Transform operator*(const Transform& t1, const Transform& t2);
        friend Transform operator/(const Transform& t1, const Transform& t2);
        friend Transform operator*(const Transform& t, f32 num);
        friend Transform operator-(const Transform& t1, const Transform& t2);   //calculates delta between transforms
        friend bool operator==(const Transform& t1, const Transform& t2);
        friend bool operator!=(const Transform& t1, const Transform& t2);
        friend std::ostream& operator<<(std::ostream& os, const Transform& t);
    public:
        Transform(const Vec2& position = {0,0}, const Angle& rotation = 0, const Vec2& scale = {1, 1});
        Transform(const Mat3& m);   // extract from matrix

        void setPosition(const Vec2& p);
        void setScale(const Vec2& s);
        void setRotation(const Angle& r);

        const Vec2& getPosition()const;
        const Vec2& getScale()const;
        const Angle& getRotation()const;
        f32 getRotSin()const;
        f32 getRotCos()const;

        Vec2& accPosition();
        Vec2& accScale();
        Angle& accRotation();

        Dir2 getRightDir()const;        // get local +X
        Dir2 getBottomDir()const;       // get local +Y

        Mat3 calcMatrix()const;

        void move(const Vec2& m);
        void moveRelative(const Vec2& m);
        void rotate(const Angle& r);
        void scale(const Vec2& s);

        Transform& operator*=(const Transform& t);
        Transform& operator/=(const Transform& t);
        Transform operator-()const;
    private:
        static Angle extractRotation_(f32 a, f32 b, f32& sin_out, f32& cos_out);
        static Vec2 extractScale_(f32 a, f32 b, f32 c, f32 d, f32 sin_r, f32 cos_r);

        Vec2 position_;
        Vec2 scale_;
        Angle rotation_;
        f32 sin_r_, cos_r_;   // stored sin/cos
    };

}

#include "Transform.inl"
#endif //TRANSFORM_H
