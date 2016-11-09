#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Vec2.h"
#include "Mat3.h"
#include "Angle.h"

namespace grynca {

    class Transform {
    public:
        Transform(const Vec2& position = {0,0}, const Angle& rotation = 0, const Vec2& scale = {1, 1});

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

        Mat3 calcMatrix()const;

        void move(const Vec2& m);
        void moveRelative(const Vec2& m);
        void rotate(const Angle& r);
        void scale(const Vec2& s);

    private:
        Vec2 position_;
        Vec2 scale_;
        Angle rotation_;
        f32 sin_r_, cos_r_;   // stored sin/cos
    };

}

#include "Transform.inl"
#endif //TRANSFORM_H
