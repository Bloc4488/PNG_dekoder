#pragma once
#include <inc/chunk.h>
#include <fstream>


class IHDR_chunk : public Chunk
{
private:
	uint32_t _width;
	uint32_t _height;
	uint8_t _bit_depth;
	uint8_t _color_type;
	uint8_t _compression_method;
	uint8_t _filter_method;
	uint8_t _interlace_method;
public:
	~IHDR_chunk() {}
	IHDR_chunk(ifstream& stream, uint32_t length, char name[5]);
	void show_chunk() const override;
	uint32_t getWidth() { return _width; }
	uint32_t getHeight() { return _height; }
	uint8_t getColorType() { return _color_type; }
	bool assertChunk() const override;
	void writeToFile(ofstream& out) override;
};

