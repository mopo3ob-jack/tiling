#ifndef TILERENDERER_HPP
#define TILERENDERER_HPP

#include <glad/glad.h>

#include <vector>
#include <mstd/memory>
#include <mstd/geometry>

using namespace mstd;

class TileRenderer {
public:
	TileRenderer() {
		vertexArray = 0;
		texture = 0;
	}

	TileRenderer(GLuint shaderProgram) {
		vertexArray = 0;
		texture = 0;
		setShaderProgram(shaderProgram);
	}

	~TileRenderer();

	void setShaderProgram(GLuint shaderProgram);

	void loadTextures(std::vector<const char*> filepaths, U32 offset = 0);
	void rebuildTiles(Vector2<U32> chunk, Vector2<U32> loadedChunkSize);
	void setScreenSize(Vector2f screenSize, Vector2f framebufferSize);

	void render(Vector2f offset);

	/*
	 * "U16* tiles" is intended to be managed by an arena so that the chunks can be page aligned.
	 * Chunks are 64 x 32 tiles in size. This results in 4096 byte chunks that can be 
	 * pre-cached by the kernel at will.
	 *
	 * This is all assuming the page size is 4 KiB and not 1 MiB.
	 *
	 * The data is arranged in a z-ordered curve array of row-major chunks.
	 * https://en.wikipedia.org/wiki/Z-order_curve
	 * https://en.wikipedia.org/wiki/Row-_and_column-major_order
	 * 
	 * 4x4 chunk example:
	 *
	 * ----------------------
	 * |0   |1   |4    |5   |
	 * |    |    |     |    |
	 * |----+----+-----+----|
	 * |2   |3   |6    |7   |
	 * |    |    |     |    |
	 * |----+----+-----+----|
	 * |8   |9   |12   |13  |
	 * |    |    |     |    |
	 * |----+----+-----+----|
	 * |10  |11  |14   |15  |
	 * |    |    |     |    |
	 * ----------------------
	 *
	 *  With each individual chunk looking like:
	 *
	 *	0   2   4   8   16  ...   63
	 *	64  66  68  70  72  ...   127
	 *	128 .   
	 *	192     .  
	 *	256         .
	 *	.
	 *	.
	 *	.
	 *	4032 ...                  4095
	 */
	U16* tiles;

	static constexpr Size chunkWidth = 64;
	static constexpr Size chunkSizeBytes = 4096;

private:
	Vector2f tileDimensions;
	Vector2f offsetRound;
	Matrix4f projection;

	Vector2<U32> loadedTileSize;
	U32 loadedTileCount;

	GLuint shaderProgram;
	GLuint texture;
	GLuint vertexArray, vertexBuffer;
	GLuint offsetUniform, tileDimensionsUniform, xMaskUniform, yShiftUniform, projectionUniform;
};

#endif
