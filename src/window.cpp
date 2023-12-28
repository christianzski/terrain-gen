#include "window.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "shader.h"
#include "config.h"
#include "exporter.h"

void resize_callback(GLFWwindow* glfw, int w, int h) {
    auto win = static_cast<window*>(glfwGetWindowUserPointer(glfw));

    win->set_dimensions(w, h);
}

void key_callback(GLFWwindow* glfw, int key, int, int action, int) {
    auto win = static_cast<window*>(glfwGetWindowUserPointer(glfw));

    win->handle_key(key, action);
}

window::window(int width, int height) : width(width), height(height) {}

window::~window() {
    glfwDestroyWindow(glfw_window);
    glfwTerminate();
}

bool window::initialized() {
    if(!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfw_window = glfwCreateWindow(width, height, "Terrain Generator", NULL, NULL);

    if(!glfw_window) return false;

    glfwMakeContextCurrent(glfw_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetWindowUserPointer(glfw_window, this);
    glfwSetKeyCallback(glfw_window, key_callback);
    glfwSetWindowSizeCallback(glfw_window, resize_callback);

    terrain_shader.load("shaders/vertex.glsl", "shaders/fragment.glsl");

    config::read("config.txt", generator);

    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    terrain_shader.use();

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    return true;
}

bool window::should_close() const {
    return glfwWindowShouldClose(glfw_window);
}

void window::draw() {
    if(!rotating) time = glfwGetTime();

    if(generator.updated()) {
        generator.generate();
        generator.buffer(ibo, vbo);

        std::vector<float> heights = generator.get_heights();
        terrain_shader["colors"] = generator.get_colors();
        terrain_shader["heights"] = heights;
        terrain_shader["levels"] = heights.size();
    }

    glfwPollEvents();

    glClearColor(0.0f, 0.75f, 0.9f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vao);

    terrain_shader.use();

    glm::vec2 dimensions = generator.get_dimensions();

    float orbit_base = 500.0;
    glm::vec3 pos = glm::rotateY(
        glm::vec3(0.0,
                  generator.max_height(),
                  (-dimensions.x) / 2.0),
        (float)(time / std::fmax(1.0, ((float)dimensions.x / orbit_base))));

    terrain_shader["light_pos"] = pos;
    terrain_shader["camera_pos"] = pos;

    terrain_shader["model"] = glm::translate(glm::mat4(1.0),
                                             glm::vec3(-dimensions.x / 2.0,
                                                       0,
                                                       -dimensions.y / 2.0));

    terrain_shader["view"] = glm::lookAt(pos,
                                         glm::vec3(0, 0, 0),
                                         glm::vec3(0, 1, 0));

    terrain_shader["projection"] = glm::perspective(45.0f,
                                                    (float)width / height,
                                                    0.01f, dimensions.x);

    glDrawElements(GL_TRIANGLES, generator.indices_count(), GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(glfw_window);
}

void window::set_dimensions(int width, int height) {
    this->width = width;
    this->height = height;

    glViewport(0, 0, width, height);
}

void window::handle_key(int key, int action) {
    if(action == GLFW_RELEASE) {
        switch(key) {
            case GLFW_KEY_F1:
                wireframe = !wireframe;
                glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
                break;
            case GLFW_KEY_F2: {
                using terrain::shading::smooth;
                using terrain::shading::flat;
                generator.set_shading(generator.get_shading() == flat ? smooth : flat);
            } break;
            case GLFW_KEY_Z: rotating = !rotating; break;
            case GLFW_KEY_F5: config::read("config.txt", generator); break;
            case GLFW_KEY_F12: exporter::mesh(generator, "meshes/terrain.obj"); break;
        }
    }

    switch(key) {
        case GLFW_KEY_1: generator.update_settings(-0.1f, 0, 0, 0); break;
        case GLFW_KEY_2: generator.update_settings(+0.1f, 0, 0, 0); break;
        case GLFW_KEY_3: generator.update_settings(0, -0.1f, 0, 0); break;
        case GLFW_KEY_4: generator.update_settings(0, +0.1f, 0, 0); break;
        case GLFW_KEY_5: generator.update_settings(0, 0, -0.1f, 0); break;
        case GLFW_KEY_6: generator.update_settings(0, 0, +0.1f, 0); break;
        case GLFW_KEY_7: generator.update_settings(0, 0, 0, -0.1f); break;
        case GLFW_KEY_8: generator.update_settings(0, 0, 0, +0.1f); break;
        default: break;
    }
}
