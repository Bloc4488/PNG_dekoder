#include <inc/PLTE.h>

PLTE_chunk::PLTE_chunk(ifstream& stream, uint32_t length, char name[5])
{
	Length = length;
	strcpy(Name, name);
	for (unsigned int i = 0; i < Length; i += 3)
	{
		RGB_palette entry;
		stream.read(reinterpret_cast<char*>(&entry.red), sizeof(uint8_t));
		stream.read(reinterpret_cast<char*>(&entry.green), sizeof(uint8_t));
		stream.read(reinterpret_cast<char*>(&entry.blue), sizeof(uint8_t));
		_palette.push_back(entry);
	}
	stream.read(reinterpret_cast<char*>(&CRC), sizeof(CRC));
	CRC = reverse_uint32_t(CRC);
}


void PLTE_chunk::show_chunk() const
{
	cout << "Palette entries:" << endl;
	int i = 0;
	for (auto& a : _palette)
	{
		cout << "Entry " << i << " - " << "R " << unsigned(a.red) << " G " << unsigned(a.green) << " B " << unsigned(a.blue) << endl;
		i++;
	}
}

void PLTE_chunk::writeToFile(ofstream& out)
{
	uint32_t length = reverse_uint32_t(Length);
	out.write(reinterpret_cast<char*>(&length), sizeof(length));
	for (int i = 0; i < 4; i++)
	{
		out.write(reinterpret_cast<char*>(&Name[i]), sizeof(Name[i]));
	}
	for (auto& a : _palette)
	{
		out.write(reinterpret_cast<char*>(&a.red), sizeof(a.red));
		out.write(reinterpret_cast<char*>(&a.green), sizeof(a.green));
		out.write(reinterpret_cast<char*>(&a.blue), sizeof(a.blue));
	}
	uint32_t crc = reverse_uint32_t(CRC);
	out.write(reinterpret_cast<char*>(&crc), sizeof(crc));
}

bool PLTE_chunk::assertChunk() const
{
	cout << "Asserting chunk PLTE!" << endl;
	if (Length % 3 != 0) return false;
	//Check if color type 3 or 2(6)
	return true;
}