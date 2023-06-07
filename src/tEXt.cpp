#include <inc/tEXt.h>

tEXt_chunk::tEXt_chunk(ifstream& stream, uint32_t length, char name[5])
{
	Length = length;
	strcpy(Name, name);
	vector<uint8_t> data;
	data.resize(Length);
	int nullSeparator = -1;
	stream.read(reinterpret_cast<char*>(data.data()), Length);
	for (size_t i = 0; i < data.size(); i++)
	{
		if (data[i] == 0x00)
		{
			nullSeparator = static_cast<int>(i);
			break;
		}
	}
	for (size_t i = 0; i < data.size(); i++)
	{
		if (nullSeparator == -1 || i == nullSeparator)
			continue;
		else if (i < static_cast<size_t>(nullSeparator))
			_keyword.push_back(data[i]);
		else
			_text.push_back(data[i]);
	}
	stream.read(reinterpret_cast<char*>(&CRC), sizeof(CRC));
	CRC = reverse_uint32_t(CRC);
}

void tEXt_chunk::show_chunk() const
{
	for (unsigned int i = 0; i < _keyword.size(); i++)
	{
		cout << static_cast<char>(_keyword[i]);
	}
	cout << " ";
	for (unsigned int i = 0; i < _text.size(); i++)
	{
		cout << static_cast<char>(_text[i]);
	}
	cout << endl;
}

bool tEXt_chunk::assertChunk() const
{
	cout << "Asserting chunk tEXt!" << endl;
	if (_keyword.size() < 1 || _keyword.size() > 79) return false;
	for (int i = 0; i < _keyword.size(); i++)
	{
		if (_keyword[i] == 0) return false;
	}
	for (int i = 0; i < _text.size(); i++)
	{
		if (_text[i] == 0) return false;
	}
	return true;
}

void tEXt_chunk::writeToFile(ofstream& out)
{
	uint32_t length = reverse_uint32_t(Length);
	out.write(reinterpret_cast<char*>(&length), sizeof(length));
	for (int i = 0; i < 4; i++)
	{
		out.write(reinterpret_cast<char*>(&Name[i]), sizeof(Name[i]));
	}
	for (auto& a : _keyword)
	{
		out.write(reinterpret_cast<char*>(&a), sizeof(a));
	}
	uint8_t Null = 0x00;
	out.write(reinterpret_cast<char*>(&Null), sizeof(Null));
	for (auto& a : _text)
	{
		out.write(reinterpret_cast<char*>(&a), sizeof(a));
	}
	uint32_t crc = reverse_uint32_t(CRC);
	out.write(reinterpret_cast<char*>(&crc), sizeof(crc));
}