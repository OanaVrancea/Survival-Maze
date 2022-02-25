#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texcoord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform vec3 object_color;

// Output value to fragment shader
out vec3 color;
out vec3 normal;
out vec2 texcoord;

void main() {
	//Send color light output to fragment shader
	color = object_color;
	normal = v_normal;
	//texcoord = v_texcoord;
	gl_Position =  Model * vec4(v_position, 1.0);	
}
