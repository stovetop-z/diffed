#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include <vector>
#include <cmath>
#include <limits>
#include <arm_neon.h>

namespace calculations
{
    inline float l2(const std::vector<float>& a, const std::vector<float>& b, size_t dimension)
    {
        const float* fa = a.data();
        const float* fb = b.data();
        float total = 0.0f;
        size_t i = 0;

        for(; i + 4 <= dimension; i += 4)
        {
            float32x4_t f4a = vld1q_f32(fa + i);
            float32x4_t f4b = vld1q_f32(fb + i);
            float32x4_t diff = vsubq_f32(f4a, f4b);
            total += vaddvq_f32(vmulq_f32(diff, diff));
        }
        for(; i < dimension; ++i)
        {
            float diff = fa[i] - fb[i];
            total += diff * diff;
        }
        return std::sqrt(total);
    }

    inline float scalarDotProduct(const std::vector<float>& a, const std::vector<float>& b, size_t dim)
    {
        const float* fa = a.data();
        const float* fb = b.data();
        float total = 0.0f;
        size_t i = 0;

        for(; i + 4 <= dim; i += 4)
        {
            float32x4_t f4a = vld1q_f32(fa + i);
            float32x4_t f4b = vld1q_f32(fb + i);
            total += vaddvq_f32(vmulq_f32(f4a, f4b));
        }
        for(; i < dim; ++i)
        {
            total += fa[i] * fb[i];
        }
        return total;
    }

    inline float norm(const std::vector<float>& point, size_t dim)
    {
        const float* fpoint = point.data();
        float result = 0.0f;
        size_t i = 0;

        for(; i + 4 <= dim; i += 4)
        {
            float32x4_t fa = vld1q_f32(fpoint + i);
            result += vaddvq_f32(vmulq_f32(fa, fa));
        }
        for(; i < dim; ++i)
        {
            result += fpoint[i] * fpoint[i];
        }
        return std::sqrt(result);
    }

    inline float cosThetaDotProduct(const std::vector<float>& a, const std::vector<float>& b, size_t dim)
    {
        float scalar_dot = scalarDotProduct(a, b, dim);
        float norm_a = norm(a, dim);
        float norm_b = norm(b, dim);
        if(norm_a == 0.0f || norm_b == 0.0f) return 0.0f;
        return scalar_dot / (norm_a * norm_b);
    }

    inline float thetaDotProduct(const std::vector<float>& a, const std::vector<float>& b, size_t dim)
    {
        float cos_theta = cosThetaDotProduct(a, b, dim);
        // Clamp to prevent NaN 
        cos_theta = std::fmax(-1.0f, std::fmin(1.0f, cos_theta));
        return std::acos(cos_theta);
    }

    inline size_t minimumPos(const std::vector<float>& vec, size_t dim)
    {
        if(dim == 0 || vec.empty()) return 0;
        size_t min_pos = 0;
        float curr_min = vec[0];

        for(size_t i = 1; i < dim; ++i)
        {
            if(vec[i] < curr_min)
            {
                min_pos = i;
                curr_min = vec[i];
            }
        }
        return min_pos;
    }

    inline size_t closestDimension(const std::vector<float>& unit_matrix, const std::vector<float>& point, size_t dim)
    {
        std::vector<float> distances;
        distances.reserve(dim);
        for(size_t i = 0; i < dim; ++i)
        {
            std::vector<float> I(unit_matrix.begin() + (i * dim), unit_matrix.begin() + ((i + 1) * dim));
            distances.push_back(l2(I, point, dim));
        }
        return minimumPos(distances, dim);
    }

    inline size_t whichParsec(const std::vector<float>& point, size_t dim, size_t num_parsecs, float max_expected_norm = 2.0f)
    {
        if(num_parsecs <= 1) return 0;

        float length = norm(point, dim);
        float step = max_expected_norm / static_cast<float>(num_parsecs);

        for(size_t i = 0; i < num_parsecs; ++i)
        {
            float lower = i * step;
            float upper = (i + 1) * step;
            if(length >= lower && length < upper)
            {
                return i;
            }
        }
        return num_parsecs - 1;
    }
}

#endif // CALCULATIONS_H