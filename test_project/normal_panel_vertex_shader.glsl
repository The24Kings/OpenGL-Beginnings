#version 460

layout(packed, binding=0) buffer model_list {
	mat4 models[];
};
layout(packed, binding=1) buffer scale_list {
	vec2 scales[];
};
layout(packed, binding=2) buffer normal_list {
	vec3 normals[];
};
layout(packed, binding=3) buffer up_list {
	vec3 ups[];
};
layout(packed, binding=4) buffer object_scale_list {
	vec2 object_scales[];
};
in vec2 in_vertex;
uniform mat4 vp;
out vec2 frag_texcoord;
out vec4 gl_Position;

void main(void) {	
	vec3 sideways = normalize(cross(ups[gl_InstanceID], normals[gl_InstanceID]));
	vec3 pos = in_vertex.x * object_scales[gl_InstanceID].x * ups[gl_InstanceID] 
			+ in_vertex.y * object_scales[gl_InstanceID].y * sideways;
	gl_Position = vp * models[gl_InstanceID] * vec4(pos, 1.0);
	frag_texcoord = (vec2(in_vertex.y, -in_vertex.x) * 0.5 + vec2(0.5, 0.5)) * scales[gl_InstanceID];
}