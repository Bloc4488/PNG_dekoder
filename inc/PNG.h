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
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/utils/logger.hpp>


using namespace cv;


class PNG
{
private:
	vector<unique_ptr<Chunk>> chunks;
	string file_name;
	uint32_t header[2];
	vector<uint8_t> _data_IDAT;
	int _bytesPerPixel;
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
	void showFFT();
	void anonymizePNG();
	void showImage();
	bool assertPNG();
};