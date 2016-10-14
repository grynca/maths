#ifndef MATHS_TEST_GON_H
#define MATHS_TEST_GON_H

#include <stdint.h>
#include "maths/Angle.h"
#include "types/containers/fast_vector.h"
#include <cmath>

using namespace grynca;

float err(float gt, float rslt) {
    if ( fabsf(gt-rslt) < 0.00001f )
        return 0;
    float e = fabsf(gt-rslt)/gt;
    return float(int(e*100000))/1000;
}

static inline void testGon() {
    uint32_t n = 1e6;

    fast_vector<float> results_sin(n);
    fast_vector<float> results_sin_fast(n);

    fast_vector<float> results_cos(n);
    fast_vector<float> results_cos_fast(n);

    float start = -2*Angle::Pi;
    float inc = fabsf(2*start)/n;
    {
        BlockMeasure m("sin ");
        float a = start;
        for (uint32_t i=0; i<n; ++i) {
            results_sin[i] = sinf(a);
            a += inc;
        }
    }

    {
        BlockMeasure m("sin_fast_ ");
        float a = start;
        for (uint32_t i=0; i<n; ++i) {
            results_sin_fast[i] = Angle::Internal::sin_fast2_(Angle::Internal::mod_Pi_(a));
            a += inc;
        }
    }

    {
        BlockMeasure m("cos ");
        float a = start;
        for (uint32_t i=0; i<n; ++i) {
            results_cos[i] = cosf(a);
            a += inc;
        }
    }

    {
        BlockMeasure m("cos from sin ");
        float a = start;
        for (uint32_t i=0; i<n; ++i) {
            int q = abs((int)(a/M_PI_2))%4;
            if (q == 1 || q == 2)
                results_cos_fast[i] = -sqrtf(1.0f - results_sin_fast[i]*results_sin_fast[i]);
            else
                results_cos_fast[i] = sqrtf(1.0f - results_sin_fast[i]*results_sin_fast[i]);
            a += inc;
        }
    }

    std::cout << "sin err (%): ";
    for (uint32_t i=0; i<n; i+=n/10) {
        std::cout << err(results_sin[i], results_sin_fast[i]) << " ";
    }
    std::cout << std::endl;

    std::cout << "cos err (%): ";
    for (uint32_t i=0; i<n; i+=n/10) {
        std::cout << err(results_cos[i], results_cos_fast[i]) << " ";
    }
    std::cout << std::endl;
}

#endif //MATHS_TEST_GON_H
