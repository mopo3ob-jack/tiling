#include <tiling/TileRenderer.hpp>
#include <tiling/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiling/stb_image_write.h>
#include <mstd/misc>
#include <random>

#include <iostream>

std::default_random_engine engine(clock());

U16 getRandom(U16 a, U16 b) {
	std::uniform_int_distribution distrib(0, 1);

	return distrib(engine) ? a : b;
}

TileRenderer::~TileRenderer() {
	if (texture) {
		glDeleteTextures(1, &texture);
		texture = 0;
	}

	if (vertexArray) {
		glDeleteVertexArrays(1, &vertexArray);
		glDeleteBuffers(1, &vertexBuffer);
		vertexArray = 0;
	}
}

void TileRenderer::setShaderProgram(GLuint shaderProgram) {
	this->shaderProgram = shaderProgram;
	offsetUniform = glGetUniformLocation(shaderProgram, "offset");
	tileDimensionsUniform = glGetUniformLocation(shaderProgram, "tileDimensions");
	projectionUniform = glGetUniformLocation(shaderProgram, "projection");
	xMaskUniform = glGetUniformLocation(shaderProgram, "xMask");
	yShiftUniform = glGetUniformLocation(shaderProgram, "yShift");
}

void TileRenderer::loadTextures(std::vector<const char*> filepaths, U32 offset) {
	if (texture) {
		glDeleteTextures(1, &texture);
		texture = 0;
	}

	stbi_set_flip_vertically_on_load(true);

	Size imageSizeBytes;
	void* data = nullptr;
	void* dest;
	for (Size i = 0; i < filepaths.size(); ++i) {
		U32 tileWidth;
		int channels;
		void* image = stbi_load(filepaths[i], (int*)&tileWidth, (int*)&tileWidth, &channels, 4);
		if (!image) {
			std::cerr << errorText << "Could not open texture file \"" << filepaths[i] << "\"\n";
			stbi_image_free(image);
			continue;
		} else {
			if (!data) {
				imageSizeBytes = tileWidth * tileWidth * 4;
				data = mstd::alloc<U8>(imageSizeBytes * (filepaths.size() + 1));
				dest = (char*)data + imageSizeBytes * (i + 1 + offset);

				std::memset(data, 0, (Size)dest - (Size)data);
			}

			std::memcpy(dest, image, imageSizeBytes);

			stbi_image_free(image);
			dest = (char*)dest + imageSizeBytes;
			glTexImage3D(
				GL_TEXTURE_2D_ARRAY, 0, GL_RGBA,
				tileWidth, tileWidth, filepaths.size() + 1,
				0, GL_RGBA, GL_UNSIGNED_BYTE, data
			);
		}
	}

	if (!data) {
		return;
	}
	
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void TileRenderer::rebuildTiles(Vector2<U32> chunk, Vector2<U32> loadedChunkSize) {
	glUseProgram(shaderProgram);

	if (!vertexArray) {
		glGenVertexArrays(1, &vertexArray);
		glGenBuffers(1, &vertexBuffer);
	}

	glBindVertexArray(vertexArray);

	loadedTileSize = loadedChunkSize * chunkWidth;
	loadedTileCount = loadedTileSize.w * loadedTileSize.h / 2;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, loadedTileCount * sizeof(U16), tiles, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_SHORT, sizeof(U16), (void*)0L);

}

void TileRenderer::setScreenSize(Vector2f screenSize, Vector2f framebufferSize) {
	tileDimensions = {2.0f / screenSize.w, 2.0f / screenSize.h};
	offsetRound = framebufferSize;
	offsetRound.x *= tileDimensions.x / 8.0f;
	offsetRound.y *= tileDimensions.y / 4.0f;

	projection = {
		{tileDimensions.x, 0.0, 0.0, 0.0},
		{0.0, tileDimensions.y, 0.0, 0.0},
		{0.0, 0.0, 1.0, 0.0},
		{0.0, 0.0, 0.0, 1.0}
	};
}

void TileRenderer::render(Vector2f offset) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

	glUseProgram(shaderProgram);
	glUniform2f(offsetUniform, offset.x, offset.y);
	glUniform2f(tileDimensionsUniform, tileDimensions.x, tileDimensions.y);
	projection.data[0][3] = -offset.x * tileDimensions.x;
	projection.data[1][3] = -offset.y * tileDimensions.y;
	glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, (GLfloat*)&projection.data);
	glUniform1i(xMaskUniform, loadedTileSize.w - 1);
	glUniform1ui(yShiftUniform, sizeof(loadedTileSize.w) * 8 - __builtin_clz(loadedTileSize.w - 1));

	glBindVertexArray(vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glDrawArrays(GL_POINTS, 0, loadedTileCount);
}
