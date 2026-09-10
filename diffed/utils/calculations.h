#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include <vector>
#include <arm_neon.h>

namespace calculations
{
    inline float l2(const std::vector<float>& a, const std::vector<float>& b, size_t dimension)
    {
        float total = 0.0f;
        for(size_t i = 0; i < dimension; i++)
        {
            total += powf(b.at(i) - a.at(i), 2.0f);
        }
        total = sqrt(total);
        return total;
    }

    inline float dot_product(const std::vector<float>& a, const std::vector<float>& b, size_t dim)
    {
        float* fa = (float*)a.data();
        float* fb = (float*)b.data();

        alignas(32) float total = 0.0f;
        size_t i = 0;
        for (i; i + 4 <= dim; i += 4)
        {
            float32x4_t f4a = vld1q_f32(fa + i);
            float32x4_t f4b = vld1q_f32(fb + i);

            total += vaddvq_f32(vmulq_f32(f4a, f4b));
        }
        
        for (i; i < dim; ++i) 
        {
            total += a[i] * b[i];
        }
        
        return total;
    }
}
#endif // CALCULATIONS_H