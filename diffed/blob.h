#ifndef STAGING_H
#define STAGING_H

#include <vector>
#include <string>
#include <string_view>
#include <filesystem>

struct Blob
{
    float time_modified;
    std::vector<float> embedding;
    std::vector<unsigned char> raw_data;
    std::filesystem::path p;
};
#endif