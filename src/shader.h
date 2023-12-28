#pragma once
#include <string>
#include <vector>

class uniform_accessor {
    private:
        const class shader& program;
        const std::string uniform;
    public:
        uniform_accessor(const shader& program, std::string uniform);
        void operator=(glm::mat4 mat) const;
        void operator=(glm::vec3 vec) const;
        void operator=(const std::vector<glm::vec3>& arr) const;
        void operator=(const std::vector<float>& arr) const;
        void operator=(int value) const;
};

class shader {
    private:
        unsigned int id;

        void check_errors(unsigned int shader, int status, std::string label) const;
    public:
        shader();
        shader(std::string vertex_path, std::string fragment_path);

        void load(std::string vertex_path, std::string fragment_path);

        void use() const;

        const uniform_accessor operator[](std::string variable) const;

        unsigned int get_id() const;
};
