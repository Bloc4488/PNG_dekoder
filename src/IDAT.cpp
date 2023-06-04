#include <inc/IDAT.h>

IDAT_chunk::IDAT_chunk(ifstream& stream, uint32_t length, char name[5])
{
	Length = length;
	strcpy(Name, name);
	_data.resize(Length);
	stream.read(reinterpret_cast<char*>(_data.data()), Length);
	stream.read(reinterpret_cast<char*>(&CRC), sizeof(CRC));
	CRC = reverse_uint32_t(CRC);
}

void IDAT_chunk::show_chunk() const
{
	cout << "Compressed data size: " << _data.size() << endl;
}

vector<uint8_t> IDAT_chunk::getChunkData()
{
	return _data;
}

void IDAT_chunk::writeToFile(ofstream& out)
{
	uint32_t length = reverse_uint32_t(Length);
	out.write(reinterpret_cast<char*>(&length), sizeof(length));
	for (int i = 0; i < 4; i++)
	{
		out.write(reinterpret_cast<char*>(&Name[i]), sizeof(Name[i]));
	}
	for (auto& a : _data)
	{
		out.write(reinterpret_cast<char*>(&a), sizeof(a));
	}
	uint32_t crc = reverse_uint32_t(CRC);
	out.write(reinterpret_cast<char*>(&crc), sizeof(crc));
}

void IDAT_chunk::writeToFileDecrypted(ofstream& out, vector<uint8_t> data)
{
	uint32_t length = reverse_uint32_t(Length);
	out.write(reinterpret_cast<char*>(&length), sizeof(length));
	for (int i = 0; i < 4; i++)
	{
		out.write(reinterpret_cast<char*>(&Name[i]), sizeof(Name[i]));
	}
	for (auto& a : data)
	{
		out.write(reinterpret_cast<char*>(&a), sizeof(a));
	}
	uint32_t crc = reverse_uint32_t(CRC);
	out.write(reinterpret_cast<char*>(&crc), sizeof(crc));
}

bool IDAT_chunk::assertChunk() const
{
	cout << "Asserting chunk IDAT!" << endl;
	return true;
}