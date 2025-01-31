#version 460

in vec2 in_vertex;
in vec2 in_texcoord;
out vec4 gl_Position;
out vec2 frag_texcoord;

void main(void) {	
	gl_Position = vec4(1.8 * in_vertex.x - 0.9, 1.8 * in_vertex.y + 0.8, 0.0, 1.0);
	frag_texcoord = in_texcoord; 
}