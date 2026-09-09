#ifndef FLATLAKE_H
#define FLATLAKE_H

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace flatlake
{
    struct FlatLake
    {
        size_t dimension, num_vectors;
        std::vector<float> data;
        std::vector<uint64_t> ids;

        explicit FlatLake(size_t dim) : dimension(dim)
        {
            num_vectors = 0;
            data.reserve(dim * 100);
            ids.reserve(100);
        }

        void add(uint64_t id, const std::vector<float>& new_data)
        {
            if (new_data.size() != dimension) {
                throw std::invalid_argument("vector dimension does not match FlatLake");
            }

            ids.push_back(id);
            data.insert(data.end(), new_data.begin(), new_data.end());
            num_vectors++;
        }
    };
}

#endif // FLATLAKE_H
