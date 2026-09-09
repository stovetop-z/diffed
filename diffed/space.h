#ifndef SPACE_H
#define SPACE_H

#include <vector>
#include "utils/calculations.h"
#include "centroid.h"

struct Space
{
    std::vector<float> reference_frame;
    std::vector<Centroid> space;
    float grain;
    size_t dimension;

    Space(size_t dim, float grain) : dimension(dim), grain(grain)
    {
        reference_frame = std::vector<float>(dim, 0.0f);
    }

    void setReference(const std::vector<float>& new_ref, size_t dim)
    {
        dimension = dim;
        reference_frame.reserve(new_ref.size());
        reference_frame.insert(reference_frame.begin(), new_ref.data(), new_ref.data() + new_ref.size());
    }
};
#endif // SPACE_H