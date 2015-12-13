#version 330 core
layout (location = 0) in vec3 vpointSky;
in vec2 vtexcoordSky;
out vec3 texCoords;

//uniform mat4 M;
///--- Uploaded by Eigen in C++
//uniform mat4 CAMERA;
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;


void main() {


	gl_Position =PROJ * vec4(vpointSky.x, vpointSky.y, vpointSky.z, 1.0);
	texCoords = vpointSky;
}
