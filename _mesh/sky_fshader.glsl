#version 330 core
out vec4 color;
in vec2 uv;
uniform sampler2D skytex;

void main() {
    color = texture(skytex,uv).rgba;
}


