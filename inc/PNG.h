#pragma once
#include <inc/chunk.h>
#include <inc/IHDR.h>
#include <inc/IDAT.h>
#include <inc/gAMA.h>
#include <inc/IEND.h>
#include <inc/PLTE.h>
#include <inc/tEXt.h>
#include <inc/tIME.h>
#include <inc/default_chunk.h>
#include <inc/RSA.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/utils/logger.hpp>


using namespace cv;


enum RSA_algorithm {ECB, CBC, Library};
enum SaveMode {Decrypt, Encrypt};

class PNG
{
private:
	vector<unique_ptr<Chunk>> _chunks;
	string _file_name;
	uint32_t _header[2];
	size_t _imageNumBytes = 0;
	vector<uint8_t> _data_IDAT;
	vector<uint8_t> _decompressed_data_IDAT;
	int _bytesPerPixel;
	rsa rsaCrypt;
public:
	PNG();
	PNG(string file);
	uint32_t reverse_uint32_t(uint32_t num) {
		return ((num & 0x000000ff) << 24) |
			((num & 0x0000ff00) << 8) |
			((num & 0x00ff0000) >> 8) |
			((num & 0xff000000) >> 24);
	}
	void readFile();
	void showInformationAllChunks();
	void showNamesAllChunks();
	void decompress_chunks_IDAT();
	vector<uint8_t> copmpress_data(const vector<uint8_t> data);
	void showFFT();
	void getDataIDAT();
	void anonymizePNG();
	void savePNG(vector<uint8_t> decrypted_data, RSA_algorithm option, SaveMode mode);
	void showImage();
	void rsaProcess(RSA_algorithm option);
	void rsaSetKeylength();
	bool assertPNG();
};