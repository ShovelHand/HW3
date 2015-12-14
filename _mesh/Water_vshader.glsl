#version 330 core 
uniform float time;
in vec3 vpoint;
in vec3 vnormal;
in vec2 TexCoord;

out vec3 fpoint; ///< for debug
out vec3 fnormal_cam;
out vec2 uv;
out vec2 uv1;

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
	uv = vec2(TexCoord.x + time/96, TexCoord.y + time/48); //make water texture appear to ripple by sliding (and repeating) on z axis 
	uv1 = vec2(TexCoord.x + time/48, TexCoord.y + time/96 ); //make water texture appear to ripple by sliding (and repeating) on z axis 
	gl_Position = PROJ * VIEW *  vec4(vpoint.x, -1, vpoint.z, 1.0);
	
   
}