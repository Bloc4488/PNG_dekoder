#pragma once
#include <inc/chunk.h>
#include <zlib.h>

class IDAT_chunk : public Chunk
{
private:
	vector<uint8_t> _data;
public:
	IDAT_chunk(ifstream& stream, uint32_t length, char name[5]);
	void show_chunk() const override;
	vector<uint8_t> getChunkData();
	void writeToFile(ofstream& out);
	bool assertChunk() const override;
	~IDAT_chunk() {}
};
