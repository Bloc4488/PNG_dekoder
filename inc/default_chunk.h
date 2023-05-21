#pragma once
#include <inc/chunk.h>

class default_chunk : public Chunk
{
private:
	vector<uint8_t> _data;
public:
	~default_chunk() {}
	default_chunk(ifstream& stream, uint32_t length, char name[5]);
	void show_chunk() const override;
	bool assertChunk() const override;
};