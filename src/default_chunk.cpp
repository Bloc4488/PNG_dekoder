#include <inc/default_chunk.h>

default_chunk::default_chunk(ifstream& stream, uint32_t length, char name[5])
{
	Length = length;
	strcpy(Name, name);
	_data.resize(Length);
	stream.read(reinterpret_cast<char*>(_data.data()), Length);
	stream.read(reinterpret_cast<char*>(&CRC), sizeof(CRC));
	CRC = reverse_uint32_t(CRC);
}

void default_chunk::show_chunk() const
{
	cout << "Data length : " << _data.size() << endl;
}

bool default_chunk::assertChunk() const
{
	cout << "Asserting unknown chunk!" << endl;
	return true;
}