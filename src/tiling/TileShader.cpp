#include <tiling/TileShader.hpp>

#include <mstd/memory>
#include <mstd/misc>
#include <iostream>
#include <fstream>

using namespace mstd;

static GLuint createShader(const C8* filepath, GLenum type) {
	GLuint shader = glCreateShader(type);

	std::ifstream shaderFile = std::ifstream(filepath);
	Size length = shaderFile.seekg(0L, std::ios::end).tellg();
	shaderFile.seekg(0L, std::ios::beg);
	C8* source = mstd::alloc<C8>(length + 1);
	shaderFile.read(source, length);
	shaderFile.close();

	source[length] = '\0';

	glShaderSource(shader, 1, &source, nullptr);
	mstd::free(source);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		char shaderInfoLog[512];
		glGetShaderInfoLog(shader, 511, nullptr, shaderInfoLog);
		std::cerr << errorText << filepath << ":" << shaderInfoLog << std::endl;
		return -1;
	}

	return shader;
}

void TileShader::createShaderProgram(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
	GLuint vertexShader = createShader(vertexPath, GL_VERTEX_SHADER);
	GLuint fragmentShader = createShader(fragmentPath, GL_FRAGMENT_SHADER);
	GLuint geometryShader = createShader(geometryPath, GL_GEOMETRY_SHADER);
	shaderProgram = glCreateProgram();

	if (vertexShader == -1 || fragmentShader == -1 || geometryShader == -1) {
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteShader(geometryShader);
		return;
	}

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glAttachShader(shaderProgram, geometryShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(geometryShader);

	GLint status;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		char programInfoLog[512];
		glGetProgramInfoLog(shaderProgram, 511, nullptr, programInfoLog);
		std::cerr << errorText << "In linking shader program:" << programInfoLog << std::endl;
		return;
	}

	glUseProgram(shaderProgram);
}

TileShader::~TileShader() {
	if (shaderProgram) {
		glDeleteProgram(shaderProgram);
		shaderProgram = 0;
	}
}
