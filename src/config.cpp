#include "config.h"

#include <ranges>

#include <unordered_map>

#include <iostream>
#include <fstream>

namespace config {
    std::vector<std::string> split(std::string input, char delimiter) {
        std::vector<std::string> result;

        for(auto subrange : (input | std::views::split(delimiter))) {
            result.emplace_back(subrange.begin(), subrange.end());
        }

        return result;
    }

    void read(std::string filename, terrain::generator& generator) {
        std::ifstream file(filename);

        std::string line;
        int line_number = 0;

        std::vector<glm::vec3> colors;
        std::vector<float> levels;

        while(getline(file, line)) {
            line_number += 1;
            auto list = split(line, ' ');

            std::unordered_map<std::string, int> parameters = {
                { "shading", 1 },

                { "dimensions", 2 },
                { "vertex_distance", 1 },

                { "amplitude", 1 },
                { "frequency", 1 },
                { "persistance", 1 },
                { "lacunarity", 1 },
                { "minimum_height", 1 },

                { "level", 1 },
                { "color", 3 }
            };

            if(list.size()) {
                try {
                    std::string key = list[0];
                    if(parameters.contains(key) && (int)list.size() > parameters[key]) {

                        if(key == "shading") {
                            if(list[1] == "flat") {
                                generator.set_shading(terrain::shading::flat);
                            } else if(list[1] == "smooth") {
                                generator.set_shading(terrain::shading::smooth);
                            } else {
                                std::cerr << "Error while reading '" << filename << ":\n";
                                std::cerr << "- Expected 'smooth' or 'flat' keyword\n";
                                std::cerr << "- Line " << line_number << ": " << line << "\n";
                            }
                        } else if(key == "dimensions") {
                            generator.set_dimensions(std::stoi(list[1]), std::stoi(list[2]));
                        } else if(key == "vertex_distance") {
                            generator.set_displacement(std::stof(list[1]));
                        } else if(key == "amplitude") {
                            generator.set_amplitude(std::stof(list[1]));
                        } else if(key == "frequency") {
                            generator.set_frequency(std::stof(list[1]));
                        } else if(key == "persistance") {
                            generator.set_persistance(std::stof(list[1]));
                        } else if(key == "lacunarity") {
                            generator.set_lacunarity(std::stof(list[1]));
                        } else if(key == "minimum_height") {
                            generator.set_minimum_height(std::stof(list[1]));
                        } else if(key == "level") {
                            levels.push_back(std::stof(list[1]));
                        } else if(key == "color") {
                            colors.push_back(glm::vec3(
                                                 std::stof(list[1]) / 255,
                                                 std::stof(list[2]) / 255,
                                                 std::stof(list[3]) / 255));
                        }
                    } else if(!key.starts_with("#")) { // If not reading a comment
                        std::cerr << "Error while reading '" << filename << ":\n";
                        std::cerr << "- Invalid key or parameters\n";
                        std::cerr << "- Line " << line_number << ": " << line << "\n";
                    }
                } catch(std::exception& e) {
                    std::cerr << "Error while reading '" << filename << "':\n";
                    std::cerr << "- Line " << line_number << ": " << line << "\n";
                }
            }
        }

        generator.set_colors(colors);
        generator.set_height_percentages(levels);
    }
};
