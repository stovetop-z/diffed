#ifndef R_H
#define R_H

#include <unordered_set>
#include "utils/calculations.h"
#include "centroid.h"

struct QueryResult
{
    uint64_t id;
    float similarity;
};

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

    inline std::vector<size_t> topDimensions(const std::vector<float>& q_vec, size_t top_dims)
    {
        std::vector<std::pair<float, size_t>> dist_idx;
        dist_idx.reserve(dimensions);

        for(size_t i = 0; i < dimensions; ++i)
        {
            std::vector<float> I(identity_matrix.begin() + (i * dimensions), 
                                 identity_matrix.begin() + ((i + 1) * dimensions));
            dist_idx.push_back({calculations::l2(I, q_vec, dimensions), i});
        }

        std::sort(dist_idx.begin(), dist_idx.end());
        
        size_t take = std::min(top_dims, dimensions);
        std::vector<size_t> chosen;
        chosen.reserve(take);
        for(size_t i = 0; i < take; ++i)
        {
            chosen.push_back(dist_idx[i].second);
        }
        return chosen;
    }

    inline std::vector<QueryResult> query(const std::vector<float>& q_vec, size_t top_k, size_t probe_dims = 2)
    {
        size_t primary_parsec = calculations::whichParsec(q_vec, dimensions, num_parsecs);
        std::vector<size_t> dims = topDimensions(q_vec, probe_dims);

        // Collect candidate bucket indices (including adjacent parsec shells)
        std::vector<size_t> target_buckets;
        for(size_t dim_idx : dims)
        {
            target_buckets.push_back(dim_idx * num_parsecs + primary_parsec);

            // Probe adjacent shells if available
            if(primary_parsec > 0)
            {
                target_buckets.push_back(dim_idx * num_parsecs + (primary_parsec - 1));
            }
            if(primary_parsec + 1 < num_parsecs)
            {
                target_buckets.push_back(dim_idx * num_parsecs + (primary_parsec + 1));
            }
        }

        std::vector<QueryResult> results;
        std::unordered_set<uint64_t> seen_ids;

        for(size_t b_idx : target_buckets)
        {
            if(b_idx >= r.size()) continue;
            Centroid& target_bucket = r.at(b_idx);

            for(size_t i = 0; i < target_bucket.num_vectors; ++i)
            {
                uint64_t id = target_bucket.ids[i];
                if(seen_ids.find(id) != seen_ids.end()) continue;
                seen_ids.insert(id);

                std::vector<float> candidate(
                    target_bucket.data.begin() + (i * dimensions),
                    target_bucket.data.begin() + ((i + 1) * dimensions)
                );

                float sim = calculations::cosThetaDotProduct(q_vec, candidate, dimensions);
                results.push_back({id, sim});
            }
        }

        std::sort(results.begin(), results.end(), [](const QueryResult& a, const QueryResult& b) {
            return a.similarity > b.similarity;
        });

        if(results.size() > top_k)
        {
            results.resize(top_k);
        }
        return results;
    }

    inline std::string rCStr() const
    {
        std::string cstr = "";
        for(size_t i = 0; i < buckets; i++)
        {
            if(r.at(i).data.empty())
                continue;
                
            cstr += r.at(i).cStr();
        }

        return cstr;
    }
};
#endif // R_H