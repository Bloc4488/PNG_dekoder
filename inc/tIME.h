#pragma once
#include <inc/chunk.h>

class tIME_chunk : public Chunk
{
private:
	uint16_t _year;
	uint8_t _month;
	uint8_t _day;
	uint8_t _hour;
	uint8_t _minute;
	uint8_t _second;
public:
	tIME_chunk(ifstream& stream, uint32_t length, char name[5]);
	void show_chunk() const override;
	bool assertChunk() const override;
	~tIME_chunk() {}
};
