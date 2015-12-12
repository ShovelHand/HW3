#version 330 core
out vec4 color;
in vec3 texCoords;
uniform samplerCube tex_sky;

void main() {
 //   color = vec3(1,1,1).rgb;
	color = texture(tex_sky, texCoords);
}


