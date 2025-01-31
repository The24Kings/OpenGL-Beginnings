version 460
layout(vertices=3) out;
in vec4 fca[];
out vec4 fct[];
uniform mat4 mvp;

void main(){
	vec4 distance = mvp * vec4(0, 0, 0, 1);
	int tesslv = 14 - (int(distance.z) / 2);

	gl_TessLevelOuter[0] = tesslv;
	gl_TessLevelOuter[1] = tesslv ;
	gl_TessLevelOuter[2] = tesslv ;
	gl_TessLevelInner[0] = tesslv ;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position; // Possible to use synchronization

	fct[gl_InvocationID] = fca[gl_InvocationID];
}