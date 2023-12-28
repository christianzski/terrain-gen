#include "exporter.h"

#include <filesystem>
#include <fstream>

namespace exporter {
    // Export a terrain mesh as an OBJ file at filename
    void mesh(terrain::generator& gen, std::string filename) {
        const std::vector<float>& vertices = gen.get_vertices();
        const std::vector<unsigned int>& indices = gen.get_indices();

        // Create meshes directory if it doesn't exist
        if(!std::filesystem::exists("meshes")) {
            std::filesystem::create_directory("meshes");
        }

        std::ofstream file(filename);
        file << "# Generated Terrain - @christianzski" << "\n";
        file << "o\n";

        file << "\n# normals\n";
        for(size_t i = 0; i < vertices.size(); i += 6) {
            file << "vn " << vertices[i + 3] << " " << vertices[i + 4] << " " << vertices[i + 5] << "\n";
        }

        file << "\n# vertices\n";
        for(size_t i = 0; i < vertices.size(); i += 6) {
            file << "v " << vertices[i] << " " << vertices[i + 1] << " " << vertices[i + 2] << "\n";
        }

        file << "\n# faces\n";
        for(size_t i = 0; i < indices.size(); i += 6) {
            // Write face vertices w/ normals - no uv's
            file << "f " << (indices[i] + 1) << "//" << (indices[i] + 1) << " " <<
                (indices[i + 1] + 1) << "//" << (indices[i + 1] + 1) << " " <<
                (indices[i + 2] + 1)  << "//" << (indices[i + 2] + 1) << "\n";

            file << "f " << (indices[i + 3] + 1) << "//" << (indices[i + 3] + 1) << " " <<
                (indices[i + 4] + 1) << "//" << (indices[i + 4] + 1) << " " <<
                (indices[i + 5] + 1) << "//" << (indices[i + 5] + 1) << "\n";
        }
    }
}
