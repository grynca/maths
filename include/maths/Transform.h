#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Vec2.h"
#include "Mat3.h"
#include "Angle.h"
#include "glm/mat2x2.hpp"

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
        Transform();
        Transform(const Vec2& position, const Angle& rotation, const Vec2& scale = {1, 1});
        Transform(const Mat3& m);   // extract from matrix

        static Transform createTranslation(const Vec2& position);
        static Transform createRotation(const Angle& rotation);
        static Transform createScale(const Vec2& scale);

        Transform& set(const Transform& tr);
        Transform& setPosition(const Vec2& p);
        Transform& setScale(const Vec2& s);
        Transform& setRotation(const Angle& r);

        const Vec2& getPosition()const;
        const Vec2& getScale()const;
        const Angle& getRotation()const;

        Vec2& accPosition();
        Vec2& accScale();

        const Dir2& getRightDir()const;        // get local +X
        Dir2 getBottomDir()const;       // get local +Y

        Mat3 calcMatrix()const;

        Transform& move(const Vec2& m);
        Transform& moveRelative(const Vec2& m);
        Transform& rotate(const Angle& r);
        Transform& scale(const Vec2& s);

        const Dir2& getRotDir()const;
        glm::mat2 calcRSMat()const;  // 2x2 rotation*scale matrix

        bool isUnit()const;

        Transform& operator*=(const Transform& t);
        Transform& operator/=(const Transform& t);
        Transform operator-()const;
    private:
        static Angle extractRotation_(f32 a, f32 b, Dir2& rot_dir_out);
        static Vec2 extractScale_(const glm::mat2& rs_mat, const Dir2& rot_dir);

        Vec2 position_;
        Vec2 scale_;
        Angle rotation_;
        Dir2 rot_dir_;      // (cos, sin)
    };

}

#include "Transform.inl"
#endif //TRANSFORM_H
