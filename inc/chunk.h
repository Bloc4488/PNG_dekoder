#pragma once
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <vector>
#include <memory>
using namespace std;

class Chunk
{
public:
	uint32_t Length = 0;
	char Name[5] = { 0 };
	uint32_t CRC = 0;
	Chunk() {}
	Chunk(uint32_t Len, char name[5], uint32_t crc)
	{
		Length = Len;
		strcpy(Name, name);
		CRC = crc;
	}
	virtual ~Chunk() {}
	virtual void show_chunk() const = 0;
	virtual bool assertChunk() const = 0;
	uint32_t reverse_uint32_t(uint32_t num) {
		return ((num & 0x000000ff) << 24) |
			((num & 0x0000ff00) << 8) |
			((num & 0x00ff0000) >> 8) |
			((num & 0xff000000) >> 24);
	}
	virtual void writeToFile(ofstream& out) = 0;
};
