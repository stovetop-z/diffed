#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include <vector>
#include <arm_neon.h>

namespace calculations
{
    inline float l2(const std::vector<float>& a, const std::vector<float>& b, size_t dimension)
    {
        float* fa = (float*)a.data();
        float* fb = (float*)b.data();

        alignas(32) float total = 0.0f;
        size_t i = 0;
        for(i; i + 4 <= dimension; i += 4)
        {
            float32x4_t f4a = vld1q_f32(fa + i);
            float32x4_t f4b = vld1q_f32(fb + i);

            float32x4_t f4c = vsubq_f32(f4a, f4b);
            total += vaddvq_f32(vmulq_f32(f4c, f4c));
        }
        for(i; i < dimension; i++)
        {
            total += pow(fa[i] - fb[i], 2); 
        }

        total = sqrt(total);
        return total;
    }

    inline float scalarDotProduct(const std::vector<float>& a, const std::vector<float>& b, size_t dim)
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

    inline float norm(const std::vector<float>& point, size_t dim)
    {
        float* fpoint = (float*)point.data();

        alignas(32) float result = 0.0f;
        size_t i = 0;
        for(i; i + 4 <= dim; i += 4)
        {
            float32x4_t fa = vld1q_f32(fpoint + i);
            result += vaddvq_f32(vmulq_f32(fa, fa));
        }
        for(i; i < dim; i++)
        {
            result += fpoint[i] * fpoint[i];
        }

        return sqrt(result);
    }

    inline float cosThetaDotProduct(const std::vector<float>& a, const std::vector<float>& b, size_t dim)
    {
        float scalar_dot = scalarDotProduct(a, b, dim);
        float norm_a = norm(a, dim);
        float norm_b = norm(b, dim);

        return scalar_dot / (norm_a * norm_b);
    }


    inline float thetaDotProduct(const std::vector<float>& a, const std::vector<float>& b, size_t dim)
    {
        float cos_theta_dot_prod = cosThetaDotProduct(a, b, dim);
        return acosf(cos_theta_dot_prod);
    }

    inline size_t minimumPos(const std::vector<float>& vec, size_t dim)
    {
        size_t min_pos = 0;
        float curr_min;
        for(size_t i = 0; i < dim; i++)
        {
            curr_min = vec[i];
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
        for(size_t i = 0; i < dim; i++)
        {
            std::vector<float> I(unit_matrix.begin() + i * dim, unit_matrix.begin() + i * dim + dim);
            distances.push_back(l2(I, point, dim));
        }

        return minimumPos(distances, dim);
    }

    inline size_t whichParsec(const std::vector<float>& point, size_t dim, size_t num_parsecs)
    {
        float length = norm(point, dim);

        for(size_t i = 0; i < num_parsecs; i++)
        {
            float parsec_i_lower_bound = i == 0 ? 0.0f : num_parsecs / ((float)i);
            float parsec_i_upper_bound = num_parsecs / ((float)i + 1);
            if(parsec_i_lower_bound <= length <= parsec_i_upper_bound)
            {
                return i;
            }
        }

        return num_parsecs - 1;
    }
}
#endif // CALCULATIONS_H