#version 330 core
out vec3 color;
in vec2 uv;
uniform sampler2D tex_sky;

void main() {
    color = vec3(1,1,1).rgb;
	color = texture(tex_sky, uv).rgb;
}


