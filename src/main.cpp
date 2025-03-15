#include <mstd/geometry>
#include <mstd/misc>

#include "../inc/TileRenderer.hpp"
#include "../inc/TileShader.hpp"
#include "../inc/stb_image.h"

#include <GLFW/glfw3.h>

#include <unistd.h>
#include <sys/fcntl.h>
#include <random>
#include <GL/glu.h>

#define printError() __printError(__FILE__, __LINE__)

void __printError(const char*, Size line);

using namespace mstd;

static void resize(GLFWwindow* window, int width, int height) {
	U32 newWidth = height * 4.0 / 3.0;
	if (newWidth < width) {
		glViewport((width - newWidth) / 2, 0, newWidth, height);
	} else {
		U32 newHeight = width * 3 / 4;
		glViewport(0, (height - newHeight) / 2, width, newHeight);
	}
}

int main() {
	glfwInit();
	
	glfwWindowHint(GLFW_SAMPLES, 16);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Tiling Renderer", monitor, nullptr);
	glfwMakeContextCurrent(window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	glfwSetFramebufferSizeCallback(window, resize);

	glViewport(0, 0, mode->width, mode->height);
	glClearColor(0.674509804f, 0.988235294f, 0.95686274509f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);

	TileShader shader;
	shader.createShaderProgram("shaders/vertex.glsl", "shaders/fragment.glsl", "shaders/geometry.glsl");

	TileRenderer fg(shader.getProgram());
	TileRenderer bg(shader.getProgram());
	fg.loadTextures({
		"textures/tl.png", "textures/tm.png", "textures/tr.png",
		"textures/ml.png", "textures/mm.png", "textures/mr.png",
		"textures/bl.png", "textures/bm.png", "textures/br.png",
	});
	bg.loadTextures({
		"textures/tl.png", "textures/tm.png", "textures/tr.png",
		"textures/ml.png", "textures/mm.png", "textures/mr.png",
		"textures/bl.png", "textures/bm.png", "textures/br.png",
	});

	fg.setScreenSize({16.0f, 12.0f}, {(F32)mode->width, (F32)mode->height});
	bg.setScreenSize({32.0f, 24.0f}, {(F32)mode->width, (F32)mode->height});

	U8 tiles[64 * 64];
	memset(tiles, 0, 64 * 64);

	for (U32 i = 0; i < 256; ++i) {
		std::default_random_engine engine(clock());
		std::uniform_int_distribution pos(0, 62);
		std::uniform_int_distribution size(1, 4);

		pos(engine); // Burn a predictable random value.
		U32 startX = pos(engine);
		U32 startY = pos(engine);
		U32 endX = startX + size(engine);
		U32 endY = startY + size(engine);
		if (endX > 63) endX = 63;
		if (endY > 63) endY = 63;

		tiles[startY * 64 + startX] = 7;
		for (U32 x = startX + 1; x < endX; ++x) {
			tiles[startY * 64 + x] = 8;
		}
		tiles[startY * 64 + endX] = 9;
		for (U32 y = startY + 1; y < endY; ++y) {
			tiles[y * 64 + startX] = 4;
			for (U32 x = startX + 1; x < endX; ++x) {
				tiles[y * 64 + x] = 5;
			}
			tiles[y * 64 + endX] = 6;
		}
		tiles[endY * 64 + startX] = 1;
		for (U32 x = startX + 1; x < endX; ++x) {
			tiles[endY * 64 + x] = 2;
		}
		tiles[endY * 64 + endX] = 3;
	}

	fg.tiles = tiles;
	fg.rebuildTiles({}, {1, 1});
	std::cout << "asdf\n";

	memset(tiles, 0, 64 * 64);
	for (U32 i = 0; i < 256; ++i) {
		std::default_random_engine engine(clock());
		std::uniform_int_distribution pos(0, 62);
		std::uniform_int_distribution size(1, 4);

		pos(engine); // Burn a predictable random value.
		U32 startX = pos(engine);
		U32 startY = pos(engine);
		U32 endX = startX + size(engine);
		U32 endY = startY + size(engine);
		if (endX > 63) endX = 63;
		if (endY > 63) endY = 63;

		tiles[startY * 64 + startX] = 7;
		for (U32 x = startX + 1; x < endX; ++x) {
			tiles[startY * 64 + x] = 8;
		}
		tiles[startY * 64 + endX] = 9;
		for (U32 y = startY + 1; y < endY; ++y) {
			tiles[y * 64 + startX] = 4;
			for (U32 x = startX + 1; x < endX; ++x) {
				tiles[y * 64 + x] = 5;
			}
			tiles[y * 64 + endX] = 6;
		}
		tiles[endY * 64 + startX] = 1;
		for (U32 x = startX + 1; x < endX; ++x) {
			tiles[endY * 64 + x] = 2;
		}
		tiles[endY * 64 + endX] = 3;
	}
	bg.tiles = tiles;
	bg.rebuildTiles({}, {1, 1});

	Vector2f cameraPosition(0);
	Vector2f cameraVelocity(0);
	F32 previousTime = glfwGetTime();
	F32 deltaTime;
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		deltaTime = glfwGetTime() - previousTime;
		previousTime = glfwGetTime();

		Vector2f cameraAcceleration(0);
		if (glfwGetKey(window, GLFW_KEY_UP)) {
			cameraAcceleration.y += 1.0;
		}

		if (glfwGetKey(window, GLFW_KEY_DOWN)) {
			cameraAcceleration.y -= 1.0;
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			cameraAcceleration.x += 1.0;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT)) {
			cameraAcceleration.x -= 1.0;
		}

		if (cameraAcceleration.magnitude() > 1.0f) {
			cameraAcceleration.normalize();
		}

		cameraAcceleration *= 4.0f; // pixels seconds^-2
		if (cameraVelocity.magnitude() > deltaTime) {
			Vector2f cameraFriction = cameraVelocity.normalized() * -0.3 * 9.81;
			cameraAcceleration += cameraFriction;
		} else {
			cameraVelocity = Vector2f(0.0);
		}

		cameraVelocity += cameraAcceleration * deltaTime;
		cameraPosition += 16.0f * cameraVelocity * deltaTime;

		glClear(GL_COLOR_BUFFER_BIT);

		bg.render(cameraPosition);
		fg.render(cameraPosition);

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void __printError(const char* file, Size line) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << errorText << file << "(" << line << "): " << gluErrorString(error) << std::endl;
	}
}
