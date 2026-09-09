#ifndef CENTROID_H
#define CENTROID_H

#include <vector>
#include "flatlake.h"
#include <cmath>

struct Centroid : flatlake::FlatLake
{
    size_t bucket;

    Centroid(size_t dim, size_t bucket) : flatlake::FlatLake(dim), bucket(bucket)
    {
    }
};
#endif // CENTROID_H