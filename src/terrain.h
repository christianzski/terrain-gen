#pragma once
#include <optional>
#include <vector>
#include <limits>

#include <glm/glm.hpp>

#define DEFAULT_AMPLITUDE 1.7
#define DEFAULT_FREQUENCY 3.8
#define DEFAULT_LACUNARITY 2.6
#define DEFAULT_PERSISTANCE 1.5

#define DEFAULT_VERTICES 100
#define DEFAULT_DISPLACEMENT 5.0

namespace terrain {
    enum class shading {
        smooth, flat
    };

    class generator {
        private:
          float amplitude = DEFAULT_AMPLITUDE;
          float frequency = DEFAULT_FREQUENCY;
          float lacunarity = DEFAULT_LACUNARITY;
          float persistance = DEFAULT_PERSISTANCE;

          std::vector<float> vertices;
          std::vector<unsigned int> indices;

          shading shading_type = shading::flat;

          size_t vertex_width = DEFAULT_VERTICES, vertex_height = DEFAULT_VERTICES;
          float scale = DEFAULT_DISPLACEMENT;

          float minimum_height = -std::numeric_limits<float>::max();
          float maximum_height = std::numeric_limits<float>::min();

          // Whether there is a pending update in the terrain settings
          bool update = true;

          // Get a vec3 position from an index into indices
          const glm::vec3 get_vertex_position(int index) const;

          std::vector<float> height_levels;
          std::vector<float> height_percentages;
          std::vector<glm::vec3> color_levels;
        public:
          void generate();

          void set_amplitude(float);
          void set_frequency(float);
          void set_persistance(float);
          void set_lacunarity(float);

          void set_minimum_height(float);

          void set_colors(std::vector<glm::vec3>);
          void set_height_percentages(std::vector<float>);

          void set_dimensions(size_t width, size_t height);
          void set_displacement(float displacement);
          void set_shading(shading);

          void update_settings(float amplitude, float frequency,
                               float lacunarity, float persistance);

          void buffer(unsigned int ibo, unsigned int vbo);

          const std::vector<float>& get_heights() const;
          const std::vector<glm::vec3>& get_colors() const;

          shading get_shading() const;

          size_t indices_count() const;

          float max_height() const;

          bool updated() const;

          glm::vec2 get_dimensions() const;

          const std::vector<float>& get_vertices() const;
          const std::vector<unsigned int>& get_indices() const;

          const glm::vec3 face_normal(size_t index, int& sum) const;
    };
}
