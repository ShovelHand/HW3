#version 330 core
in vec3 vpointSky;
in vec2 vtexcoordSky;
out vec2 uv;

uniform mat4 M;

void main() {
    gl_Position = M * vec4(vpointSky, 1.0);

    uv = vtexcoordSky;
}
