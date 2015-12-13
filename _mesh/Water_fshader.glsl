#version 330 core
out vec4 color;
in vec3 fpoint;
in vec3 fnormal_cam;
uniform float time;


in vec2 uv;

uniform mat4 MODEL;
uniform mat4 VIEW;

uniform sampler2D tex_water;

void main() {
//get normal for fragment 

	float dx_p = textureOffset(tex_water, uv, ivec2(+1,0))[0];
	float dx_m = textureOffset(tex_water, uv, ivec2(-1,0))[0];
	float dy_p = textureOffset(tex_water, uv, ivec2(0,+1))[0];
	float dy_m = textureOffset(tex_water, uv, ivec2(0,-1))[0];
	
	vec3 dx = normalize(vec3(1.0,dx_p - dx_m,0.0));
	vec3 dy = normalize(vec3(1.0,dy_p - dy_m,0.0));
	vec3 surfaceNorm = vec3(cross(dx,dy));
	
	vec3 L = vec3(500,10000,500); //light position
	vec3 H = normalize(L + fnormal_cam);
	float intensity = 15;
	float light = max(dot(surfaceNorm, normalize(L))*intensity, 0.0);

	color.rgb = vec3(0.25,0.25,0.5);
	color += vec3(1,1,1)*light;
	color += 1000*max(0, dot(surfaceNorm,H));
	color.a = 0.9;
}