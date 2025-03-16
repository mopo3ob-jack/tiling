#version 330 core

flat in uint f_textureIndex;
in vec2 uv;

out vec4 fragColor;

uniform sampler2DArray textures;

void main() {
	fragColor = texture(textures, vec3(uv, f_textureIndex));
}
