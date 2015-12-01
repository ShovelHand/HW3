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
/*
	float dx_p = textureOffset(tex_height, TexCoord0.st, ivec2(+1,0))[0];
	float dx_m = textureOffset(tex_height, TexCoord0.st, ivec2(-1,0))[0];
	float dy_p = textureOffset(tex_height, TexCoord0.st, ivec2(0,+1))[0];
	float dy_m = textureOffset(tex_height, TexCoord0.st, ivec2(0,-1))[0];
	
	vec3 dx = normalize(vec3(1.0,dx_p - dx_m,0.0));
	vec3 dy = normalize(vec3(1.0,dy_p - dy_m,0.0));
	vec3 surfaceNorm = vec3(cross(dx,dy));
	*/
	//rotate normal appropriately
	//surfaceNorm =  inverse( transpose( mat3(MODEL) ))*surfaceNorm;

//light
	vec3 L = vec3(0,5000,500); //light position
	float intensity = 10;
//	float light = max(dot(surfaceNorm, normalize(L))*intensity, 0.0);



	color.rgb = texture(tex_water, uv).rgb;
	color.a = 0.5;
}
