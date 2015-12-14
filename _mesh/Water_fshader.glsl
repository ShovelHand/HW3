#version 330 core
out vec4 color;
in vec3 fpoint;
in vec3 fnormal_cam;
uniform float time;


in vec2 uv;
in vec2 uv1;

uniform mat4 MODEL;
uniform mat4 VIEW;

uniform sampler2D tex_water;
uniform sampler2D tex_water1;

void main() {
//get normal for fragment 

	float dx_p = textureOffset(tex_water, uv, ivec2(+1,0))[0];
	float dx_m = textureOffset(tex_water, uv, ivec2(-1,0))[0];
	float dy_p = textureOffset(tex_water, uv, ivec2(0,+1))[0];
	float dy_m = textureOffset(tex_water, uv, ivec2(0,-1))[0];
	
	vec3 dx = normalize(vec3(1.0,dx_p - dx_m,0.0));
	vec3 dy = normalize(vec3(1.0,dy_p - dy_m,0.0));
	vec3 surfaceNorm = vec3(cross(dx*2*max(abs(cos(time/4.0)),0.5),dy*2*max(abs(sin(time/4.0)),0.5)));

	 dx_p = textureOffset(tex_water1, uv1, ivec2(+1,0))[0];
	 dx_m = textureOffset(tex_water1, uv1, ivec2(-1,0))[0];
	 dy_p = textureOffset(tex_water1, uv1, ivec2(0,+1))[0];
	 dy_m = textureOffset(tex_water1, uv1, ivec2(0,-1))[0];
	
	 dx = normalize(vec3(1.0,dx_p - dx_m,0.0));
	 dy = normalize(vec3(1.0,dy_p - dy_m,0.0));
	 vec3 surfaceNorm1 = vec3(cross(dx*2*max(abs(sin(time/4.0)),0.5),dy*2*max(abs(cos(time/4.0)),0.5)));
	
	vec3 L = vec3(500,10000,500); //light position
	vec3 H = normalize(L + fnormal_cam);
	float intensity = 15;
	float light = max(dot(surfaceNorm, normalize(L))*intensity, 0.0);

	color.rgb = vec3(0.125,0.125,0.25);
	color.rgb += vec3(1,1,1)*light;
	color.rgb += 1000*max(0, pow(dot(surfaceNorm,H),1.7));
	color.rgb += 1000*max(0, pow(dot(surfaceNorm1,H),1.7));
	color.a = 0.9;
}