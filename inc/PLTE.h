#pragma once
#include <inc/chunk.h>

struct RGB_palette {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

class PLTE_chunk : public Chunk
{
private:
	vector<RGB_palette> _palette;
public:
	PLTE_chunk(ifstream& stream, uint32_t length, char name[5]);
	void show_chunk() const override;
	void writeToFile(ofstream& out) override;
	bool assertChunk() const override;
	~PLTE_chunk() {}
};
