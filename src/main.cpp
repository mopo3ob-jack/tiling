#include <SFML/Window.hpp>

#include <mstd/geometry>
#include <mstd/misc>

#include "../inc/TileRenderer.hpp"
#include "../inc/TileShader.hpp"
#include "../inc/stb_image.h"

#include <unistd.h>
#include <sys/fcntl.h>
#include <random>
#include <GL/glu.h>

#define printError() __printError(__FILE__, __LINE__)

void __printError(const char*, Size line);

using namespace mstd;

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

int main() {
	sf::ContextSettings settings;
	settings.depthBits = 0;
	settings.stencilBits = 0;
	settings.antialiasingLevel = 0;
	settings.majorVersion = 3;
	settings.minorVersion = 3;
	settings.attributeFlags = settings.Core;
	settings.sRgbCapable = true;

	sf::Window window(sf::VideoMode(640, 480), "Tile Renderer", sf::Style::Resize, settings);
	window.setActive(true);

	gladLoadGLLoader((GLADloadproc)sf::Context::getFunction);

	glViewport(0, 0, sf::VideoMode().width, sf::VideoMode().height);
	glClearColor(0.674509804f, 0.988235294f, 0.95686274509f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	fg.setScreenSize({16.0f, 12.0f});
	bg.setScreenSize({32.0f, 24.0f});

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

	Vector2f camera = {0};
	sf::Clock clock;
	F32 deltaTime;
	Bool running = true;
	while (running) {
		deltaTime = clock.getElapsedTime().asSeconds() * 10;
		clock.restart();

		sf::Event event;
		while (window.pollEvent(event)) {
			U32 width;
			switch (event.type) {
			case event.Closed:
				running = false;
				break;
			case event.Resized:
				width = event.size.height * 4 / 3;
				if (width < event.size.width) {
					glViewport((event.size.width - width) / 2, 0, width, event.size.height);
				} else {
					U32 height = event.size.width * 3 / 4;
					glViewport(0, (event.size.height - height) / 2, event.size.width, event.size.width * 3 / 4);
				}
				break;
			default:
				break;
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			camera.y += deltaTime;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			camera.y -= deltaTime;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			camera.x += deltaTime;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			camera.x -= deltaTime;
		}

		glClear(GL_COLOR_BUFFER_BIT);

		bg.render(camera);
		fg.render(camera);

		window.display();
	}

	return 0;
}

void __printError(const char* file, Size line) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << errorText << file << "(" << line << "): " << gluErrorString(error) << std::endl;
	}
}
