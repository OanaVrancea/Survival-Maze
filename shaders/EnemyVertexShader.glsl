#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_coordinate;


// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

//
uniform int isDestroyed;
uniform float time;

uniform vec3 object_color;

// Output value to fragment shader
out vec3 color;

float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

// Noise function https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float noise(vec2 p){
	vec2 ip = floor(p);
	vec2 u = fract(p);
	u = u*u*(3.0-2.0*u);
	
	float res = mix(
		mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
		mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
	return res*res;
}

void main()
{
    // TODO(student): Send output to fragment shader

    color = object_color;
    vec3 new_pos = v_position;

	float disp = noise(vec2(v_normal.x * time * 0.25, v_normal.y)) - 0.5;
	vec3 newPos = v_position;

    if(isDestroyed == 1){
        new_pos = v_position + v_normal * disp * cos(2.5f * time);
    }

     gl_Position = Projection * View * Model * vec4(new_pos, 1.0);

}
