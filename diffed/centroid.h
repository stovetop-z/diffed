#ifndef CENTROID_H
#define CENTROID_H

#include <vector>
#include "flatlake.h"
#include <string>
#include <cmath>

struct Centroid : flatlake::FlatLake
{
    size_t bucket;

    Centroid(size_t dim) : flatlake::FlatLake(dim)
    {
    }

    inline std::string cStr() const
    {
        std::string str = "";
        for(size_t i = 0; i < num_vectors; i++)
        {
            str += "id: " + std::to_string(ids.at(i)) + "\t[";
            for(size_t j = 0; j < dimensions; j++)
            {
                str += std::to_string(data.at(i * dimensions + j)) + ", ";
            }
            str.pop_back();
            str.pop_back();
            str += "]\n";
        }
        return str;
    }
};
#endif // CENTROID_H