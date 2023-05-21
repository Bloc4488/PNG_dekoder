#include <inc/IHDR.h>

void IHDR_chunk::show_chunk() const
{
	cout << "Width: " << _width << " pixels" << endl;
	cout << "Height: " << _height << " pixels" << endl;
	cout << "Bit depth " << unsigned(_bit_depth) << " bits per channel" << endl;
	string color;
	switch (_color_type)
	{
	case 0:
		color = "Grayscale";
		break;
	case 2:
		color = "RGB";
		break;
	case 3:
		color = "Palette";
		break;
	case 4:
		color = "Grayscale + alpha";
		break;
	case 6:
		color = "RGBA";
		break;
	}
	cout << "Color type: " << color << " (" << unsigned(_color_type) << ")" << endl;
	cout << "Compression method: " << unsigned(_compression_method) << endl;
	cout << "Filter method: " << unsigned(_filter_method) << endl;
	cout << "Intelace method: " << unsigned(_interlace_method) << endl;
}

IHDR_chunk::IHDR_chunk(ifstream& stream, uint32_t length, char name[5])
{
	Length = length;
	strcpy(Name, name);
	stream.read(reinterpret_cast<char*>(&_width), sizeof(_width));
	_width = reverse_uint32_t(_width);
	stream.read(reinterpret_cast<char*>(&_height), sizeof(_height));
	_height = reverse_uint32_t(_height);
	stream.read(reinterpret_cast<char*>(&_bit_depth), sizeof(_bit_depth));
	stream.read(reinterpret_cast<char*>(&_color_type), sizeof(_color_type));
	stream.read(reinterpret_cast<char*>(&_compression_method), sizeof(_compression_method));
	stream.read(reinterpret_cast<char*>(&_filter_method), sizeof(_filter_method));
	stream.read(reinterpret_cast<char*>(&_interlace_method), sizeof(_interlace_method));
	stream.read(reinterpret_cast<char*>(&CRC), sizeof(CRC));
	CRC = reverse_uint32_t(CRC);
}

void IHDR_chunk::writeToFile(ofstream& out)
{
	uint32_t length = reverse_uint32_t(Length);
	out.write(reinterpret_cast<char*>(&length), sizeof(length));
	for (int i = 0; i < 4; i++)
	{
		out.write(reinterpret_cast<char*>(&Name[i]), sizeof(Name[i]));
	}
	uint32_t width = reverse_uint32_t(_width);
	out.write(reinterpret_cast<char*>(&width), sizeof(width));
	uint32_t height = reverse_uint32_t(_height);
	out.write(reinterpret_cast<char*>(&height), sizeof(height));
	out.write(reinterpret_cast<char*>(&_bit_depth), sizeof(_bit_depth));
	out.write(reinterpret_cast<char*>(&_color_type), sizeof(_color_type));
	out.write(reinterpret_cast<char*>(&_compression_method), sizeof(_compression_method));
	out.write(reinterpret_cast<char*>(&_filter_method), sizeof(_filter_method));
	out.write(reinterpret_cast<char*>(&_interlace_method), sizeof(_interlace_method));
	uint32_t crc = reverse_uint32_t(CRC);
	out.write(reinterpret_cast<char*>(&crc), sizeof(crc));
}

bool IHDR_chunk::assertChunk() const
{
	cout << "Asserting chunk IHDR!" << endl;
	if (unsigned(_width) <= 0 || unsigned(_height) <= 0) return false;
	if (_bit_depth != 1 && _bit_depth != 2 && _bit_depth != 4 && _bit_depth != 8 && _bit_depth != 16) return false;
	if (_color_type != 0 && _color_type != 2 && _color_type != 3 && _color_type != 4 && _color_type != 6) return false;
	if (_color_type == 0 && (_bit_depth != 1 && _bit_depth != 2 && _bit_depth != 4 && _bit_depth != 8 && _bit_depth != 16)) return false;
	if (_color_type == 2 && (_bit_depth != 8 && _bit_depth != 16)) return false;
	if (_color_type == 3 && (_bit_depth != 1 && _bit_depth != 2 && _bit_depth != 4 && _bit_depth != 8)) return false;
	if (_color_type == 4 && (_bit_depth != 8 && _bit_depth != 16)) return false;
	if (_color_type == 6 && (_bit_depth != 8 && _bit_depth != 16)) return false;
	if (_compression_method != 0) return false;
	if (_filter_method != 0) return false;
	if (_interlace_method != 0 && _interlace_method != 1) return false;
	return true;
}