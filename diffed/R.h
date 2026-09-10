#ifndef R_H
#define R_H

#include <vector>
#include "utils/calculations.h"
#include "centroid.h"

struct R
{
    std::vector<float> reference_frame;
    std::vector<float> identity_matrix;
    std::vector<float> parsecs;
    std::vector<Centroid> r;
    size_t num_parsecs; // This is the number of hyperplanes to make in space
    size_t dimensions;
    size_t buckets; // Buckets is going to be the number of hyperplanes that intersect with a dimensions (dimensions * grain)

    R(size_t dim, size_t num_parsecs) : dimensions(dim), num_parsecs(num_parsecs)
    {
        reference_frame = std::vector<float>(dim, 0.0f);
        setUnitMatrix();
        buckets = dim * num_parsecs; // e.g. dim = 10, grain = 4; buckets = 40
        setR();
    }

    inline std::vector<float> setIdentityVector(size_t loc)
    {
        std::vector<float> I;
        for(size_t i = 0; i < dimensions; i++)
        {
            if(i != loc)
            {
                I.push_back(0.0f);
                continue;
            }

            I.push_back(1.0f);
        }
        return I;
    }

    inline void setUnitMatrix()
    {
        identity_matrix.reserve(dimensions * dimensions);
        for(size_t i = 0; i < dimensions; i++)
        {
            std::vector<float> I_i = setIdentityVector(i * dimensions);
            identity_matrix.insert(identity_matrix.begin(), I_i.begin(), I_i.end());
        }
    }

    // In case the centroids have a better reference point for normalization
    inline void setReference(const std::vector<float>& new_ref, size_t dim)
    {
        dimensions = dim;
        reference_frame.reserve(new_ref.size());
        reference_frame.insert(reference_frame.begin(), new_ref.data(), new_ref.data() + new_ref.size());
    }

    inline void setR()
    {
        r.resize(buckets, Centroid(dimensions));
    }

    inline void add(uint64_t id, const std::vector<float>& a, size_t dim)
    {
        size_t parsec = calculations::whichParsec(a, dim, num_parsecs);
        size_t dimension = calculations::closestDimension(identity_matrix, a, dim);
        size_t bucket = dimension * num_parsecs + parsec; 

        Centroid& c = r.at(bucket);

        c.add(id, a);
        c.bucket = bucket;
    }

    inline const char* rCStr()
    {
        std::string cstr = "";
        for(size_t i = 0; i < buckets; i++)
        {
            if(r.at(i).data.empty())
                continue;
                
            cstr += r.at(i).cStr();
        }

        return cstr.c_str();
    }
};
#endif // R_H