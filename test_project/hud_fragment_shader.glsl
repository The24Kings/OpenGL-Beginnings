#version 460

in vec2 frag_texcoord;
out vec4 outcolor;
uniform sampler2D tex;

void main(void) {
	outcolor = texture(tex, vec2(frag_texcoord.x, -frag_texcoord.y));
	//if(outcolor.r < 0.1)
		//discard;
}