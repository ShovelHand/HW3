#version 330 core 
uniform float time;
in vec3 vpoint;
in vec3 vnormal;

out vec3 fpoint; ///< for debug
out vec3 fnormal_cam;
out float vheight; ///< for debug coloring

out vec2 uv;

///--- Uploaded by Eigen in C++
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;
uniform mat4 M;

uniform sampler2D _tex_heightMap;
float tex_at(vec2 uv){ return texture(_tex_heightMap,uv).rgb; }


void main() {          
    fpoint = vpoint + .5; ///< For coloring Debug [0,1]^3 
    fnormal_cam = inverse( transpose( mat3(VIEW * MODEL) )) * vnormal; 

	vheight =  tex_at(vec2(vpoint.x, vpoint.y));
//	vheight = 0.5 * sin(10*vpoint.x + time);
    gl_Position = PROJ * VIEW * MODEL * M * vec4(vpoint.x, vheight, vpoint.z, 1.0);
    uv = vec2(fpoint.x, fpoint.z);


}