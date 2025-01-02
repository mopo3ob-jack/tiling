#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out; 

in uint g_textureIndex[];

flat out uint f_textureIndex;
out vec2 uv;

uniform vec2 tileDimensions;

void main() {
	f_textureIndex = g_textureIndex[0];
	gl_Position = gl_in[0].gl_Position;
	uv = vec2(0.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(tileDimensions.x, 0.0, 0.0, 0.0);
	uv = vec2(1.0, 0.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(0.0, tileDimensions.y, 0.0, 0.0);
	uv = vec2(0.0, 1.0);
	EmitVertex();
	gl_Position = gl_in[0].gl_Position + vec4(tileDimensions, 0.0, 0.0);
	uv = vec2(1.0, 1.0);
	EmitVertex();
	EndPrimitive();
}
