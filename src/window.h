#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "terrain.h"
#include "shader.h"
class window {
    private:
      int width, height;
      GLFWwindow* glfw_window = nullptr;
      bool rotating = false, wireframe = false;

      shader terrain_shader;
      terrain::generator generator;

      unsigned int vao, vbo, ibo;
      double time;
    public:
      window(int width, int height);

      ~window(); // Destructor

      window(const window& w) = delete; // Copy constructor
      window& operator=(const window&) = delete; // Copy assignment
      window(window&& w) = delete; // Move constructor
      window& operator=(window&&) = delete; // Move assignment

      bool initialized();

      bool should_close() const;

      void draw();

      void set_dimensions(int w, int h);

      void handle_key(int key, int action);
};
