#pragma once
#include "terrain.h"

#include <string>
#include <vector>

namespace config {
    // Split a string by the delimiter, using C++20 ranges
    std::vector<std::string> split(std::string input, char delimiter);

    // Read a config file, updating the terrain generator settings
    void read(std::string filename, terrain::generator& generator);
}
