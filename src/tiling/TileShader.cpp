#include <tiling/TileShader.hpp>

#include <mstd/memory>
#include <mstd/misc>
#include <sys/fcntl.h>
#include <unistd.h>
#include <iostream>

using namespace mstd;

#define printError() __printError(__FILE__, __LINE__)

void __printError(const char*, Size line);

static GLuint createShader(const char* filepath, GLenum type) {
	GLuint shader = glCreateShader(type);

	int shaderFile = open(filepath, O_RDONLY, 0666);
	Size length = lseek(shaderFile, 0L, SEEK_END);
	lseek(shaderFile, 0L, SEEK_SET);
	char* source = mstd::alloc<char>(length + 1);
	read(shaderFile, source, length);
	close(shaderFile);

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
