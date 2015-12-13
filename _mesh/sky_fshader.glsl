#version 330 core
out vec3 color;
in vec3 texCoords;
uniform samplerCube tex_sky;

void main() {
    color = vec3(0.75,0.75,1).rgb;
//	color = texture(tex_sky, texCoords);
}


