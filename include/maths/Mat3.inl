#include "Mat3.h"
#include <ostream>

namespace grynca {
    inline Mat3::Mat3()
    {}

    inline Mat3:: Mat3(f32 v00, f32 v01, f32 v02,
                       f32 v10, f32 v11, f32 v12,
                       f32 v20, f32 v21, f32 v22)
     : m_(v00, v01, v02, v10, v11, v12, v20, v21, v22)
    {}

    inline Mat3::Mat3(const glm::mat3& m)
     : m_(m)
    {}


    inline Mat3 Mat3::createTransform(const Vec2& translation, const Angle& rotation, const Vec2& scale) {
    //static
        return createTransform(translation, rotation.getDir(), scale);
    }

    inline Mat3 Mat3::createTransform(const Vec2& translation, const Dir2& rot_dir, const Vec2& scale) {
    // static
        Vec2 sins = rot_dir.getY()*scale;
        Vec2 coss = rot_dir.getX()*scale;

        return {coss.getX(), sins.getX(), 0 ,
                -sins.getY(), coss.getY(), 0,
                translation.getX(), translation.getY(), 1};

    }

    inline Mat3 Mat3::createTranslationT(const Vec2& translation) {
    //static
        return {1, 0, 0,
                0, 1, 0,
                translation.getX(), translation.getY(), 1};
    }

    inline Mat3 Mat3::createRotationT(const Angle& rotation) {
    //static
        return createRotationT(rotation.getDir());
    }

    inline Mat3 Mat3::createRotationT(const Dir2& rot_dir) {
    //statict
        f32 cos_r = rot_dir.getX();
        f32 sin_r = rot_dir.getY();
        return {cos_r, sin_r, 0,
                -sin_r, cos_r, 0,
                0, 0, 1};
    }

    inline Mat3 Mat3::createScaleT(const Vec2& scale) {
    //static
        return {scale.getX(), 0, 0,
                0, scale.getY(), 0,
                0, 0, 1};
    }

    inline Mat3 Mat3::createProjection2D(const Vec2& viewport_size) {
    //static
        return createScaleT(Vec2(2.0f/viewport_size.getX(),     // 2 is because OpenGL transform to -1, 1 NDC
                                 -2.0f/viewport_size.getY()));  // invert y axis so +y is down
    }

    inline Mat3 Mat3::createProjectionIverse2D(const Vec2& viewport_size) {
    //static
        return createScaleT(Vec2(0.5f*viewport_size.getX(),
                                 -0.5f*viewport_size.getY()));
    }

    inline Mat3 Mat3::invert(const Mat3& m) {
    //static
        return Mat3(glm::inverse(m.getInternal()));
    }

    inline f32& Mat3::val(size_t col, size_t row) {
        return m_[col][row];
    }

    inline f32 Mat3::val(size_t col, size_t row)const {
        return m_[col][row];
    }

    inline Mat3& Mat3::operator*=(const Mat3& m) {
        m_*=m.m_;
        return *this;
    }

    inline glm::mat3& Mat3::getInternal() {
        return m_;
    }

    inline const glm::mat3& Mat3::getInternal()const {
        return m_;
    }

    inline Mat3 operator*(const Mat3& t, const Mat3& m) {
        return Mat3(t.m_*m.m_);
    }

    inline bool operator==(const Mat3& m1, const Mat3& m2) {
        return m1.m_ == m2.m_;
    }

    inline bool operator!=(const Mat3& m1, const Mat3& m2) {
        return m1.m_ != m2.m_;
    }

    inline std::ostream& operator <<(std::ostream& os, const Mat3& m) {
        os << "[" << m.val(0,0) << ", " << m.val(0,1) << ", " << m.val(0,2) <<"]" << std::endl;
        os << "[" << m.val(1,0) << ", " << m.val(1,1) << ", " << m.val(1,2) <<"]" << std::endl;
        os << "[" << m.val(2,0) << ", " << m.val(2,1) << ", " << m.val(2,2) <<"]" << std::endl;
        return os;
    }
}