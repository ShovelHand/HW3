#version 330 core 
uniform float time;
in vec3 vpoint;
in vec3 vnormal;

out vec3 fpoint; ///< for debug
out vec3 fnormal_cam;
out vec2 uv;

///--- Uploaded by Eigen in C++
uniform mat4 CAMERA;
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;
uniform mat4 M;

uniform sampler2D tex_height;
float tex_at(vec2 uv){ return texture(tex_height,uv).r; }

void main() {          
    fpoint = vpoint + .5; ///< For coloring Debug [0,1]^3 
    fnormal_cam = inverse( transpose( mat3(VIEW * MODEL) )) * vnormal;
	uv = vec2(fpoint.x, fpoint.z + time/6); //make water texture appear to ripple by sliding (and repeating) on z axis 
//	float vheight =  tex_at(uv);
	gl_Position = PROJ * VIEW *  vec4(vpoint.x, -1, vpoint.z, 1.0);
	
   
}