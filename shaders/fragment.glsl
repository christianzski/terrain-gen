#version 330 core

in vec3 frag_normal;
in vec3 frag_pos;

out vec4 frag_color;

uniform vec3 light_pos;
uniform vec3 camera_pos;

#define MAX_LEVELS 5
uniform int levels;
uniform float heights[MAX_LEVELS];
uniform vec3 colors[MAX_LEVELS];

void main() {
    vec3 light_color = vec3(1.0);
    vec3 ambient = 0.5 * light_color;
    vec3 norm = normalize(frag_normal);
    vec3 light_dir = normalize(light_pos - frag_pos);

    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    vec3 color = vec3(0);
    vec3 specular = vec3(0.0);

    for(int i = 0; i < levels - 1; ++i) {
        if(frag_pos.y >= heights[i]) {
            if(frag_pos.y < heights[i + 1]) {
                float blending = (frag_pos.y - heights[i]) / ((heights[i + 1] - heights[i]) * 0.25);
                color = mix(colors[i], colors[i + 1], min(1.0, blending));
            } else color = colors[i + 1];
        }
    }

    frag_color = vec4(color * (ambient + diffuse + specular), 1.0);
}
