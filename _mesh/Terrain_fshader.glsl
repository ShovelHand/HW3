#version 330 core
in vec2 TexCoord0;
out vec3 color;
in vec3 fpoint;
in vec3 fnormal_cam;
uniform float time;
in float vheight;

in vec2 uv;

uniform mat4 MODEL;
uniform mat4 VIEW;

uniform sampler2D tex_grass;
uniform sampler2D tex_rock;
uniform sampler2D tex_debug;
uniform sampler2D tex_sand;
uniform sampler2D tex_snow;
uniform sampler2D tex_water;
uniform sampler2D tex_height;
uniform sampler2D tex_sky;

void main() {
//get normal for fragment 
	float dx_p = textureOffset(tex_height, TexCoord0.st, ivec2(+1,0))[0];
	float dx_m = textureOffset(tex_height, TexCoord0.st, ivec2(-1,0))[0];
	float dy_p = textureOffset(tex_height, TexCoord0.st, ivec2(0,+1))[0];
	float dy_m = textureOffset(tex_height, TexCoord0.st, ivec2(0,-1))[0];
	
	vec3 dx = normalize(vec3(1.0,dx_p - dx_m,0.0));
	vec3 dy = normalize(vec3(1.0,dy_p - dy_m,0.0));
	vec3 surfaceNorm = vec3(cross(dx,dy));

	//rotate normal appropriately
	//surfaceNorm =  inverse( transpose( mat3(MODEL) ))*surfaceNorm;

//light
	vec3 L = vec3(500,10000,500); //light position
	float intensity = 15;
	float light = max(dot(surfaceNorm, normalize(L))*intensity, 0.0);
	vec3 H = normalize(L + fnormal_cam);

	//calculate texture blend based on height and slope
	vec3 grass = texture(tex_grass, uv).rgb;
    vec3 rock = texture(tex_rock, uv).rgb;
	vec3 debug = texture(tex_debug, uv).rgb;
	vec3 sand = texture(tex_sand, uv).rgb;
	vec3 snow = texture(tex_snow, uv).rgb;

	//texture blending
	if(vheight < 0.5)
		color = mix(sand, grass, vheight*1.5);
	else if(vheight >=0.5 && vheight < 2)
		color = mix(grass, rock, vheight);
	else if(vheight >= 2)
	{
		color = mix(rock,snow,vheight/3.0);
		color += 10*max(0, pow(dot(surfaceNorm,H),1.25));
	}

	if (light < 0.3) light = 0.3; 
			color *= light;		 
//	color = texture(tex_height, TexCoord0.st).rgb;
	
}
