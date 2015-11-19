#version 330 core
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

vec3 R = vec3(1,0,0);
vec3 G = vec3(0,1,0);
vec3 B = vec3(0,0,1);
vec3 K = vec3(0,0,0);

void main() {
    //debug code!!
 vec3 N = normalize(fnormal_cam);
    vec3 L = vec3(-1,1,-1);
    L = normalize( vec3(cos(time),0,sin(time)) );
    float lamb = dot( N, L );

    vec3 p = normalize(fpoint-.5);

	vec3 grass = texture(tex_grass, uv).rgb;
    vec3 rock = texture(tex_rock, uv).rgb;
	vec3 debug = texture(tex_debug, uv).rgb;
	vec3 sand = texture(tex_sand, uv).rgb;
    
    color = texture(tex_debug, uv).rgb;
//	color = mix(grass, rock, max(lamb,0));
	//-------
/*
    vec3 N = normalize(fnormal_cam);
    vec3 L = vec3(0,0,-1);
    L = normalize( vec3(cos(time),0,sin(time)) );
    float lamb = dot( N, L );
    //color = vec3(1,0,0) * lamb;
    vec3 p = normalize(fpoint-.5);
    float _phi = (6.28 - atan(p.z,p.x)) / 6.28;
    float _theta = (3.14 - acos(p.y)) / 3.14;
    vec3 grass = texture(tex_grass, vec2(_phi,_theta)).rgb;
    vec3 rock = texture(tex_rock, vec2(_phi,_theta)).rgb;
	vec3 debug = texture(tex_debug, vec2(_phi,_theta)).rgb;
    
	color = mix(debug, rock, max(lamb,0));
    
    
    if(lamb>0){
        vec3 V = vec3(0,0,-1);
        vec3 R = reflect(V,N);
        float glossy = pow( max( dot(-R,L), 0 ), 100);
        color += vec3(.5) * glossy; 
    }
  */      

}
