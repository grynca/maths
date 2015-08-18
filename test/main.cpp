#include "base.h"
#include "maths.h"

#include <iostream>
#include <iomanip>

using namespace grynca;


int main() {
    Angle a;

    std::cout << "sin= ";
    for (uint32_t i=0; i<=10; ++i) {
        float rads = Angle::Pi/2 *((float)i/10);
        if (i!=0) std::cout << ", ";
        std::cout << std::setprecision(3) << Angle(rads).getSin();
    }
    std::cout << std::endl << "cos= ";
    for (uint32_t i=0; i<=10; ++i) {
        float rads = Angle::Pi/2 *((float)i/10);
        if (i!=0) std::cout << ", ";
        std::cout << std::setprecision(3) << Angle(rads).getCos();
    }

    std::cout << std::endl;

    Vec2 v1(10, 5);
    Vec2 v = v1;
    v *=3;
    v *=Vec2(2, 3);

    v /=Vec2(2, 3);
    v /=3;

    assert(v==v1);

    Mat3 t1 = Mat3::createTranslationT({10, 10})
              * Mat3::createRotationT({Angle::Pi/2})
              * Mat3::createScaleT({2, 2});

    Mat3 t2 =Mat3::createTransform({10, 10}, {Angle::Pi/2}, {2, 2});

    assert(t1 == t2);

    Shape s1;
    s1.set<Circle>(Vec2(10, 10), 20);

    Shape s2;
    s2.set<Rect>(Vec2(15, 15), Vec2(5, 10), Vec2(0, 0), Angle(Angle::Pi/2));

    OverlapInfo oi;
    if (s1.overlaps(s2, oi)) {
        std::cout << "overlap: " << std::endl;
        std::cout << "dirOut=" << oi.dirOut << ", depth= " << oi.depth << std::endl;
    }


    KEY_TO_CONTINUE();
    return 0;
}