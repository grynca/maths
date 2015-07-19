#ifndef FRAME_H
#define FRAME_H

#include "Vec2.h"
#include "Mat3.h"
#include "Angle.h"

namespace grynca {

    class Frame {
    public:
        Frame(const Vec2& position = {0,0}, const Angle& rotation = 0, const Vec2& scale = {0,0});

        void setPosition(const Vec2& p);
        void setScale(const Vec2& s);
        void setRotation(const Angle& r);

        const Vec2& getPosition()const;
        const Vec2& getScale()const;
        const Angle& getRotation()const;

        Mat3 getTransform()const;

        void move(const Vec2& m);
        void moveRelative(const Vec2& m);
        void rotate(const Angle& r);
        void scale(const Vec2& s);

    private:
        Vec2 position_;
        Vec2 scale_;
        Angle rotation_;
    };

}

#include "Frame.inl"
#endif //FRAME_H
