#version 460
layout(triangles, equal_spacing, ccw) in;
uniform mat4 mvp;
in vec4 fct[];
out vec4 fca;
//in vec4 cntr[];

vec4 n4_test(vec4 p){
	vec3 offset = p.xyz;
	return vec4(2 * normalize(offset), 1);
}

void main(){
	vec4 p = 
			gl_TessCoord.r * gl_in[0].gl_Position +
			gl_TessCoord.g * gl_in[1].gl_Position +
			gl_TessCoord.b * gl_in[2].gl_Position;

	gl_Position = p;

	fca = 
			gl_TessCoord.x * fct[0] +
			gl_TessCoord.y * fct[1] +
			gl_TessCoord.z * fct[2];
}
