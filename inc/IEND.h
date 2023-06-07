#pragma once
#include <inc/chunk.h>

class IEND_chunk : public Chunk
{
public:
	IEND_chunk(ifstream& stream, uint32_t length, char name[5]);
	void show_chunk() const override;
	void writeToFile(ofstream& out) override;
	bool assertChunk() const override;
	~IEND_chunk() {}
};
