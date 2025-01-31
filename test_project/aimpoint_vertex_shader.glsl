#version 460

in vec3 in_vertex;
out vec4 gl_Position;

void main(void) {	
	gl_Position = vec4(in_vertex, 1.0);
}