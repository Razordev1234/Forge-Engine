#version 460 core
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : enable 

in vec3 ourColor;
in vec2 TexCoord;
flat in uint64_t vTexHandle; 

out vec4 FragColor;

void main() {
    if (vTexHandle != 0ul) {
        sampler2D tex = sampler2D(vTexHandle);
        FragColor = texture(tex, TexCoord) * vec4(ourColor, 1.0);
    } else {
        FragColor = vec4(ourColor, 1.0);
    }
}