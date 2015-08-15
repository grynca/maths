#ifndef MAT3_H
#define MAT3_H

#include <glm/glm.hpp>

namespace grynca {
    // fw
    class Angle;
    class Vec2;

    class Mat3 {
        friend Vec2 operator*(const Mat3& t, const Vec2& v);
        friend Mat3 operator*(const Mat3& t, const Mat3& m);
        friend bool operator==(const Mat3& m1, const Mat3& m2);
        friend bool operator!=(const Mat3& m1, const Mat3& m2);
        friend std::ostream& operator <<(std::ostream& os, const Mat3& m);
    public:
        Mat3();     // creates unit matrix
        Mat3(float v00, float v01, float v02,
             float v10, float v11, float v12,
             float v20, float v21, float v22);

        // translate*rotate*scale(first)
        static Mat3 createTransform(const Vec2& translation, const Angle& rotation, const Vec2& scale);
        static Mat3 createTransform(const Vec2& translation, float sin_r, float cos_r, const Vec2& scale);
        static Mat3 createTranslationT(const Vec2& translation);
        static Mat3 createRotationT(const Angle& rotation);
        static Mat3 createRotationT(float sin_r, float cos_r);
        static Mat3 createScaleT(const Vec2& scale);
        static Mat3 createProjection2D(const Vec2& viewport_size);
        static Mat3 createProjectionIverse2D(const Vec2& viewport_size);
        static Mat3 invert(const Mat3& m);

        float& val(size_t row, size_t col);
        float val(size_t row, size_t col)const;

        Mat3& operator*=(const Mat3& m);

        glm::mat3& getInternal();
        const glm::mat3& getInternal()const;
    private:
        Mat3(const glm::mat3& m);
        glm::mat3 m_;
    };

}

#include "Mat3.inl"
#endif //MAT3_H
