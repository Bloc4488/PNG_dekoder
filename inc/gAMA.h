#pragma once
#include <inc/chunk.h>

class gAMA_chunk : public Chunk
{
private:
	uint32_t _gamma;
public:
	~gAMA_chunk() {}
	gAMA_chunk(ifstream& stream, uint32_t length, char name[5]);
	void show_chunk() const override;
	bool assertChunk() const override;
};