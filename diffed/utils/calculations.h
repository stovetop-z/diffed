#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include <vector>

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
        float total = 0.0f;
        for (size_t i = 0; i < dim; ++i) {
            total += a[i] * b[i];
        }
        return total;
    }
}
#endif // CALCULATIONS_H