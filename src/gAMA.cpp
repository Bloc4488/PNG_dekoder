#include <inc/gAMA.h>

gAMA_chunk::gAMA_chunk(ifstream& stream, uint32_t length, char name[5])
{
	Length = length;
	strcpy(Name, name);
	stream.read(reinterpret_cast<char*>(&_gamma), sizeof(_gamma));
	_gamma = reverse_uint32_t(_gamma);
	stream.read(reinterpret_cast<char*>(&CRC), sizeof(CRC));
	CRC = reverse_uint32_t(CRC);
}

void gAMA_chunk::show_chunk() const
{
	cout << "Gamma: " << static_cast<float>(_gamma) / 100000.0f << endl;
}

bool gAMA_chunk::assertChunk() const
{
	cout << "Asserting chunk gAMA!" << endl;
	return true;
}