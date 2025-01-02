#ifndef TILESHADER_HPP
#define TILESHADER_HPP

#include "glad/glad.h"

class TileShader {
public:
	TileShader() {
		shaderProgram = 0;
	}

	~TileShader();

	void createShaderProgram(const char* vertexPath, const char* fragmentPath, const char* geometryPath);

	void useProgram() const {
		glUseProgram(shaderProgram);
	}

	GLuint getProgram() const {
		return shaderProgram;
	}

	GLuint shaderProgram;
};

#endif
