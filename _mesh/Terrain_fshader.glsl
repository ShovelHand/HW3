#version 330 core
out vec3 color;
in vec3 fpoint;
in vec3 fnormal_cam;
uniform float time;

uniform sampler2D tex_day;
uniform sampler2D tex_night;

in float vheight; ///< for debug coloring


vec3 Y = vec3(1,1,0);
vec3 G = vec3(0,1,0);

void main() {
    color = vec3(1.0/vheight, 1.0*vheight, 0.7 - vheight);

}