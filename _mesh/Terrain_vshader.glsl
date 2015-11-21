#version 330 core 
uniform float time;
in vec3 vpoint;
in vec2 TexCoord;
out vec2 TexCoord0;
in vec3 vnormal;

out vec3 fpoint; ///< for debug
out vec3 fnormal_cam;
out float vheight; 
out vec2 uv;

///--- Uploaded by Eigen in C++
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;
uniform mat4 M;

uniform sampler2D tex_height;
float tex_at(vec2 uv){ return texture(tex_height,uv).r; }


void main() {          
    fpoint = vpoint + .5; ///< For coloring Debug [0,1]^3 
    fnormal_cam = inverse( transpose( mat3(VIEW * MODEL) )) * vnormal; 

	vheight =  tex_at(TexCoord);

    gl_Position = PROJ * VIEW * MODEL * M * vec4(vpoint.x, vheight, vpoint.z, 1.0);
	TexCoord0 = TexCoord;
    uv = vec2(fpoint.x, fpoint.z);
}