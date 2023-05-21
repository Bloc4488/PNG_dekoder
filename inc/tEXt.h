#pragma once
#include <inc/chunk.h>

class tEXt_chunk : public Chunk
{
private:
	vector<char> _keyword;
	vector<char> _text;
public:
	tEXt_chunk(ifstream& stream, uint32_t length, char name[5]);
	void show_chunk() const override;
	bool assertChunk() const override;
	~tEXt_chunk() {}
};
