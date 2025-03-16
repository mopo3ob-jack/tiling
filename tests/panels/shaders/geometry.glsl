#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out; 

in uint g_textureIndex[];

flat out uint f_textureIndex;
out vec2 uv;

uniform vec2 tileDimensions;
uniform mat4 projection;

const float pad = 0.0 / 256.0;
const float B = 1.0 / 256.0;
const float S = 0.0 / 16.0;

void main() {
	f_textureIndex = g_textureIndex[0];
	gl_Position = projection * gl_in[0].gl_Position;
	uv = vec2(B, B);
	EmitVertex();
	gl_Position = projection * (gl_in[0].gl_Position + vec4(1.0, 0.0, 0.0, 0.0));
	uv = vec2(1.0 + S - B, 0.0 + B);
	EmitVertex();
	gl_Position = projection * (gl_in[0].gl_Position + vec4(0.0, 1.0, 0.0, 0.0));
	uv = vec2(0.0 + B, 1.0 + S - B);
	EmitVertex();
	gl_Position = projection * (gl_in[0].gl_Position + vec4(1.0, 1.0, 0.0, 0.0));
	uv = vec2(1.0 + S - B, 1.0 + S - B);
	EmitVertex();
	EndPrimitive();
}
