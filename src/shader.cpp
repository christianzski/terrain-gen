#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

#include <fstream>
#include <iostream>
#include <functional>

std::string read_file(std::string filename) {
    std::fstream file(filename);
    std::string body, line;
    while(getline(file, line)) {
        if(!line.empty()) line += "\n";
        body += line;
    }

    return body;
}

void shader::check_errors(unsigned int shader, int status, std::string label) const {
    int success;
    char log[512];

    std::function<void(GLuint, GLenum, GLint*)> check_error;
    std::function<void(GLuint, GLsizei, GLsizei*, GLchar*)> get_error;

    switch(status) {
        case GL_COMPILE_STATUS:
            check_error = glGetShaderiv;
            get_error = glGetShaderInfoLog;
            break;
        case GL_LINK_STATUS:
            check_error = glGetProgramiv;
            get_error = glGetProgramInfoLog;
            break;
        default: return;
    }

    check_error(shader, status, &success);

    if(!success) {
        get_error(shader, 512, NULL, log);
        std::cerr << "Shader compilation failed [" << label << "]\n";
        std::cerr << log << "\n";
    }

    assert(success);
}

shader::shader(): id(-1) {}

shader::shader(std::string vertex_path, std::string fragment_path) {
    load(vertex_path, fragment_path);
}

void shader::load(std::string vertex_path, std::string fragment_path) {
    id = glCreateProgram();

    // Load and compile vertex shader
    std::string vertex_file = read_file(vertex_path);
    const char* vertex_source = vertex_file.c_str();

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glCompileShader(vertex_shader);

    check_errors(vertex_shader, GL_COMPILE_STATUS, vertex_path);

    // Load and compile fragment shader
    std::string fragment_file = read_file(fragment_path);
    const char* fragment_source = fragment_file.c_str();

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(fragment_shader);

    check_errors(fragment_shader, GL_COMPILE_STATUS, fragment_path);

    // Link vertex and fragment shader into program
    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);
    glLinkProgram(id);

    check_errors(id, GL_LINK_STATUS, "Linking");

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void shader::use() const {
    glUseProgram(id);
}

const uniform_accessor shader::operator[](std::string variable) const {
    return uniform_accessor(*this, variable);
}

unsigned int shader::get_id() const {
    return id;
}

uniform_accessor::uniform_accessor(const shader& program, std::string uniform):
    program(program), uniform(uniform) {}

void uniform_accessor::operator=(glm::mat4 mat) const {
    glUniformMatrix4fv(glGetUniformLocation(program.get_id(), uniform.c_str()), 1,
                       GL_FALSE, glm::value_ptr(mat));
}

void uniform_accessor::operator=(glm::vec3 vec) const {
    glUniform3f(glGetUniformLocation(program.get_id(), uniform.c_str()),
                                     vec.x, vec.y, vec.z);
}

void uniform_accessor::operator=(int value) const {
    glUniform1i(glGetUniformLocation(program.get_id(), uniform.c_str()), value);
}

void uniform_accessor::operator=(const std::vector<glm::vec3>& arr) const {
    for(size_t i = 0; i < arr.size(); ++i) {
        std::string name = uniform + "[" + std::to_string(i) + "]";
        glUniform3fv(glGetUniformLocation(program.get_id(), name.c_str()),
                     1, glm::value_ptr(arr[i]));
    }
}

void uniform_accessor::operator=(const std::vector<float>& arr) const {
    for(size_t i = 0; i < arr.size(); ++i) {
        std::string name = uniform + "[" + std::to_string(i) + "]";
        glUniform1f(glGetUniformLocation(program.get_id(), name.c_str()), arr[i]);
    }
}
