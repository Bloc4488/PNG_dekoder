#include <inc/IEND.h>

IEND_chunk::IEND_chunk(ifstream& stream, uint32_t length, char name[5])
{
	Length = length;
	strcpy(Name, name);
	if (Length != 0)
	{
		cerr << "Chunk IEND must be 0 length!";
	}
	stream.read(reinterpret_cast<char*>(&CRC), sizeof(CRC));
	CRC = reverse_uint32_t(CRC);
}

void IEND_chunk::show_chunk() const
{
	cout << "Chunk IEND contains nothing!" << endl;
}

void IEND_chunk::writeToFile(ofstream& out)
{
	uint32_t length = reverse_uint32_t(Length);
	out.write(reinterpret_cast<char*>(&length), sizeof(length));
	for (int i = 0; i < 4; i++)
	{
		out.write(reinterpret_cast<char*>(&Name[i]), sizeof(Name[i]));
	}
	uint32_t crc = reverse_uint32_t(CRC);
	out.write(reinterpret_cast<char*>(&crc), sizeof(crc));
}

bool IEND_chunk::assertChunk() const
{
	cout << "Asserting chunk IEND!" << endl;
	if (Length != 0) return false;
	return true;
}