#version 330 core

layout (location = 0) in uint v_textureIndex;

uniform vec2 offset;
uniform vec2 tileDimensions;
uniform int xMask;
uniform uint yShift;

out uint g_textureIndex;

void main() {
	vec2 position = vec2(-1.0);

	position += tileDimensions * vec2(gl_VertexID & xMask, gl_VertexID >> yShift);
	position -= offset * tileDimensions;

	gl_Position = vec4(position, 0.0, 1.0);

	g_textureIndex = v_textureIndex;
}
