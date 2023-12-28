#include "terrain.h"
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/noise.hpp>

namespace terrain {
    void generator::generate() {
        vertices.clear();
        indices.clear();

        maximum_height = std::numeric_limits<float>::min();
        float minimum_height = std::numeric_limits<float>::max();

        for(size_t i = 0; i < vertex_height; ++i) {
            for(size_t j = 0; j < vertex_width; ++j) {
                int octaves = 3;
                float amplitude = this->amplitude;
                float frequency = this->frequency;
                float height = 0.0;
                for(int o = 0; o < octaves; ++o) {
                    height += amplitude * glm::perlin(glm::vec2((j * scale) / frequency,
                                                                (i * scale) / frequency));

                    frequency *= this->lacunarity;
                    amplitude *= this->persistance;
                }

                if(height < this->minimum_height) height = this->minimum_height;

                if(height < minimum_height) minimum_height = height;
                if(height > maximum_height) maximum_height = height;

                if(shading_type == shading::smooth) {
                    // Insert vertex x, y, z coordinates
                    vertices.insert(vertices.end(), {j * scale, height, i * scale});

                    // Leave space for normals; to be calculated later
                    vertices.insert(vertices.end(), {0, 0, 0});
                } else {
                    // Not the very efficient, but duplicate each vertex 6 times,
                    // Since no vertex can be connected to more than 6 triangles
                    for(int c = 0; c < 6; ++c) {
                        vertices.insert(vertices.end(), {j * scale, height, i * scale});
                        vertices.insert(vertices.end(), {0, 0, 0});
                    }
                }
            }
        }

        // Fill the index buffer with indices to vertices making up triangles
        for(size_t i = 0; i < vertex_height - 1; ++i) {
            for(size_t j = 0; j < vertex_width - 1; ++j) {
                if(shading_type == shading::smooth) {
                    indices.push_back(i * vertex_width + j);
                    indices.push_back(i * vertex_width + j + 1);
                    indices.push_back((i + 1) * vertex_width + j + 1);

                    indices.push_back(i * vertex_width + j);
                    indices.push_back((i + 1) * vertex_width + j + 1);
                    indices.push_back((i + 1) * vertex_width + j);
                } else {
                    indices.push_back((i * vertex_width + j) * 6);
                    indices.push_back((i * vertex_width + (j + 1)) * 6 + 2);
                    indices.push_back(((i + 1) * vertex_width + (j + 1)) * 6 + 3);

                    indices.push_back((i * vertex_width + j) * 6 + 1);
                    indices.push_back(((i + 1) * vertex_width + (j + 1)) * 6 + 4);
                    indices.push_back(((i + 1) * vertex_width + (j)) * 6 + 5);
                }
            }
        }

        const int cols = (vertex_width - 1);
        for(size_t i = 0; i < vertex_height; ++i) {
            for(size_t j = 0; j < vertex_width; ++j) {
                int sum = 0;
                if(shading_type == shading::smooth) {
                    glm::vec3 normal = glm::vec3(0.0);

                    // Compute the normals of all triangles connecting this vertex
                    normal += face_normal(((i - 1) * cols + (j - 1)) * 6, sum);
                    normal += face_normal(((i - 1) * cols + (j - 1)) * 6 + 3, sum);
                    normal += face_normal((i * cols + j) * 6, sum);
                    normal += face_normal((i * cols + j) * 6 + 3, sum);
                    normal += face_normal((i * cols + (j - 1)) * 6 + 3, sum);
                    normal += face_normal(((i - 1) * cols + j) * 6, sum);

                    // Compute vertex normal by averaging face normals
                    normal = glm::normalize(normal / glm::vec3(sum));

                    // Assign normal
                    vertices[(i * vertex_width + j) * 6 + 3] = normal.x;
                    vertices[(i * vertex_width + j) * 6 + 4] = normal.y;
                    vertices[(i * vertex_width + j) * 6 + 5] = normal.z;

                } else {
                    // Compute normals for each of the two triangles
                    // starting at this given vertex
                    for(int o = 0; o < 2; ++o) {
                        int index = (i * vertex_width + j) * 6 + 3 * o;
                        glm::vec3 normal = face_normal(index, sum);
                        if(sum) {
                            vertices[indices[index] * 6 + 3] = normal.x;
                            vertices[indices[index] * 6 + 4] = normal.y;
                            vertices[indices[index] * 6 + 5] = normal.z;
                        }
                    }
                }
            }
        }

        height_levels.clear();
        for(size_t i = 0; i < height_percentages.size(); ++i) {
            height_levels.push_back(height_percentages[i] *
                                    (maximum_height - minimum_height) + minimum_height);
        }

        update = true;
    }

    const glm::vec3 generator::get_vertex_position(int index) const {
        const int vertex_index = indices[index] * 6;
        return glm::vec3(vertices[vertex_index],
                         vertices[vertex_index + 1],
                         vertices[vertex_index + 2]);
    }

    const glm::vec3 generator::face_normal(size_t index, int& sum) const {
        if(index < indices.size() && index + 2 < indices.size()) {
            // Retrieve the positions of each triangle vertex
            glm::vec3 a = get_vertex_position(index);
            glm::vec3 b = get_vertex_position(index + 1);
            glm::vec3 c = get_vertex_position(index + 2);

            // Based on triangle winding, (a - b) x (c - b) should return the correct normal
            // Regardless of which triangles are being computed
            sum += 1;
            return glm::normalize(glm::cross((a - b), (c - b)));
        }

        return glm::vec3(0.0);
    }

    void generator::buffer(unsigned int ibo, unsigned int vbo) {
        update = false;
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     vertices.size() * sizeof(GLfloat), &vertices[0], GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);
    }

    bool generator::updated() const {
        return update;
    }

    size_t generator::indices_count() const {
        return indices.size();
    }

    const std::vector<float>& generator::get_vertices() const {
        return vertices;
    }

    const std::vector<unsigned int>& generator::get_indices() const {
        return indices;
    }

    void generator::set_amplitude(float amplitude) {
        this->amplitude = amplitude;
        update = true;
    }

    void generator::set_frequency(float frequency) {
        this->frequency = frequency;
        update = true;
    }

    void generator::set_persistance(float persistance) {
        this->persistance = persistance;
        update = true;
    }

    void generator::set_lacunarity(float lacunarity) {
        this->lacunarity = lacunarity;
        update = true;
    }

    void generator::set_minimum_height(float height) {
        this->minimum_height = height;
        update = true;
    }

    void generator::update_settings(float a, float f, float l, float p) {
        amplitude += a;
        frequency += f;
        lacunarity += l;
        persistance += p;

        update = true;
    }

    float generator::max_height() const {
        return maximum_height;
    }

    glm::vec2 generator::get_dimensions() const {
        return glm::vec2(vertex_width, vertex_height) * scale;
    }

    void generator::set_colors(std::vector<glm::vec3> list) {
        color_levels = list;
    }

    void generator::set_height_percentages(std::vector<float> list) {
        height_percentages = list;
    }

    void generator::set_dimensions(size_t width, size_t height) {
        vertex_width = width;
        vertex_height = height;

        update = true;
    }

    void generator::set_displacement(float displacement) {
        scale = displacement;
        update = true;
    }

    void generator::set_shading(shading type) {
        shading_type = type;
        update = true;
    }

    shading generator::get_shading() const {
        return shading_type;
    }

    const std::vector<float>& generator::get_heights() const {
        return height_levels;
    }

    const std::vector<glm::vec3>& generator::get_colors() const {
        return color_levels;
    }
}
