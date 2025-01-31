#version 450

layout(triangles, invocations=2) in;
in vec4 fca[]; 
layout(triangle_strip, max_vertices=3) out;
out vec4 fcolor;
uniform mat4 mvp;

vec4 n4_test(vec4 p){
	vec2 offset = p.xy;
	return vec4(2 * normalize(offset), p.z, 1);
}

void main(void) {
	
	gl_Position = mvp * n4_test(gl_in[0].gl_Position);
	fcolor = fca[0];
	EmitVertex();

	gl_Position = mvp * n4_test(gl_in[1].gl_Position);
	fcolor = fca[1];
	EmitVertex();

	gl_Position = mvp * n4_test(gl_in[2].gl_Position);
	fcolor = fca[2];
	EmitVertex();

	EndPrimitive();
}
