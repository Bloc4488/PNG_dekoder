#include <inc/tIME.h>

tIME_chunk::tIME_chunk(ifstream& stream, uint32_t length, char name[5])
{
	Length = length;
	strcpy(Name, name);
	stream.read(reinterpret_cast<char*>(&_year), sizeof(_year));
	_year = ((_year & 0xFF) << 8) | ((_year >> 8) & 0xFF);
	stream.read(reinterpret_cast<char*>(&_month), sizeof(_month));
	stream.read(reinterpret_cast<char*>(&_day), sizeof(_day));
	stream.read(reinterpret_cast<char*>(&_hour), sizeof(_hour));
	stream.read(reinterpret_cast<char*>(&_minute), sizeof(_minute));
	stream.read(reinterpret_cast<char*>(&_second), sizeof(_second));
	stream.read(reinterpret_cast<char*>(&CRC), sizeof(CRC));
	CRC = reverse_uint32_t(CRC);
}

void tIME_chunk::show_chunk() const
{
	cout << "Last image modification: ";
	cout << unsigned(_day) << "." << unsigned(_month) << "." << _year << " " << unsigned(_hour) << ":" << unsigned(_minute) << ":" << unsigned(_second) << endl;
}

bool tIME_chunk::assertChunk() const
{
	cout << "Asserting chunk tIME!" << endl;
	if (unsigned(_month) < 1 || unsigned(_month) > 12) return false;
	if (unsigned(_day) < 1 || unsigned(_day) > 31) return false;
	if (unsigned(_hour) < 0 || unsigned(_hour) > 23) return false;
	if (unsigned(_minute) < 0 || unsigned(_minute) > 59) return false;
	if (unsigned(_second) < 0 || unsigned(_second) > 60) return false;
	return true;
}

void tIME_chunk::writeToFile(ofstream& out)
{
	uint32_t length = reverse_uint32_t(Length);
	out.write(reinterpret_cast<char*>(&length), sizeof(length));
	for (int i = 0; i < 4; i++)
	{
		out.write(reinterpret_cast<char*>(&Name[i]), sizeof(Name[i]));
	}
	uint16_t year = ((_year & 0xFF) << 8) | ((_year >> 8) & 0xFF);
	out.write(reinterpret_cast<char*>(&year), sizeof(year));
	out.write(reinterpret_cast<char*>(&_month), sizeof(_month));
	out.write(reinterpret_cast<char*>(&_day), sizeof(_day));
	out.write(reinterpret_cast<char*>(&_hour), sizeof(_hour));
	out.write(reinterpret_cast<char*>(&_minute), sizeof(_minute));
	out.write(reinterpret_cast<char*>(&_second), sizeof(_second));
	uint32_t crc = reverse_uint32_t(CRC);
	out.write(reinterpret_cast<char*>(&crc), sizeof(crc));
}