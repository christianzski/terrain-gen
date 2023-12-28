#pragma once

#include <string>

#include "terrain.h"

namespace exporter {
    // Export a terrain mesh as an OBJ file at filename
    void mesh(terrain::generator& gen, std::string filename);
}
