#version 330 core
in vec2 TexCoord0;
out vec3 color;
in vec3 fpoint;
in vec3 fnormal_cam;
uniform float time;

in vec2 uv;

uniform sampler2D tex_grass;
uniform sampler2D tex_rock;
uniform sampler2D tex_debug;
uniform sampler2D tex_sand;
uniform sampler2D tex_snow;
uniform sampler2D tex_water;
uniform sampler2D tex_height;

void main() {
	vec3 L = vec3(1,2,0); //light position
	vec3 N = fpoint;
	float lamb = dot( L, N );
	lamb /= 10;
	vec3 grass = texture(tex_grass, uv).rgb;
    vec3 rock = texture(tex_rock, uv).rgb;
	vec3 debug = texture(tex_debug, uv).rgb;
	vec3 sand = texture(tex_sand, uv).rgb;
	
	/*
	if(fpoint.y > 3)
		color = texture(tex_snow, uv).rgb;
    if(fpoint.y > 0.8  && fpoint.y <3)
		color = mix(grass/2, rock, max(lamb,0));
	else if(fpoint.y > 0.2 && fpoint.y <=0.8)
		color = texture(tex_grass, uv).rgb;
	else if(fpoint.y <= 0.2 )
		color = texture(tex_sand, uv).rgb;
		*/
//	color = texture(tex_grass,vec2(uv)).rgb* lamb;
	color = texture(tex_height, TexCoord0.st).rgb* lamb;
	

}
