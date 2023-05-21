#include <inc/PNG.h>

PNG::PNG(string file)
{
	file_name = file;
	readFile();
	if (!assertPNG())
	{
		cerr << "Assert mistake!" << endl << "Returning to default image dice.png!" << endl;
		chunks.clear();
		_data_IDAT.clear();
		header[0] = NULL;
		header[1] = NULL;
		file_name = "png_files/dice.png";
		readFile();
	}
}

PNG::PNG()
{
	file_name = "png_files/dice.png";
	readFile();
}

void PNG::readFile()
{
	ifstream file;
	file.open(file_name, ios::binary);
	file.read((char*)&header, sizeof(header));
	if (reverse_uint32_t(header[0]) != 0x89504e47 || reverse_uint32_t(header[1]) != 0x0d0a1a0a)
	{
		cerr << "Not a PNG file: " << file_name << endl;
	}
	while (file.good())
	{
		unique_ptr<Chunk> chunk;
		uint32_t Length;
		file.read(reinterpret_cast<char*>(&Length), sizeof(Length));
		Length = reverse_uint32_t(Length);
		char Name[5];
		file.read(Name, sizeof(Name) - 1);
		Name[4] = '\0';
		if (string(Name) == "IHDR")
		{
			chunk = make_unique<IHDR_chunk>(file, Length, Name);
		}
		else if (string(Name) == "gAMA")
		{
			chunk = make_unique<gAMA_chunk>(file, Length, Name);
		}
		else if (string(Name) == "IEND")
		{
			chunk = make_unique<IEND_chunk>(file, Length, Name);
		}
		else if (string(Name) == "IDAT")
		{
			chunk = make_unique<IDAT_chunk>(file, Length, Name);
		}
		else if (string(Name) == "PLTE")
		{
			chunk = make_unique<PLTE_chunk>(file, Length, Name);
		}
		else if (string(Name) == "tEXt")
		{
			chunk = make_unique<tEXt_chunk>(file, Length, Name);
		}
		else if (string(Name) == "tIME")
		{
			chunk = make_unique<tIME_chunk>(file, Length, Name);
		}
		else
		{
			chunk = make_unique<default_chunk>(file, Length, Name);
		}
		chunks.push_back(move(chunk));
		if (string(Name) == "IEND")
		{
			break;
		}
	}
	file.close();
}

void PNG::showInformationAllChunks()
{
	for (const auto& chunk : chunks)
	{
		cout << "Chunk name: " << chunk->Name << ", chunk length: " << chunk->Length << ", CRC: " << hex << chunk->CRC << dec << endl;
		chunk->show_chunk();
	}
}


void PNG::showNamesAllChunks()
{
	int i = 0;
	for (const auto& chunk : chunks)
	{
		i++;
		cout << i << " - " << chunk->Name << " ";
		cout << endl;
	}
}

void PNG::decompress_chunks_IDAT()
{
	for (int i = 0; i < chunks.size(); i++)
	{
		if (string(chunks[i]->Name) == "IDAT")
		{
			IDAT_chunk* chunk = static_cast<IDAT_chunk*>(chunks[i].get());
			auto chunkData = chunk->getChunkData();
			_data_IDAT.insert(_data_IDAT.end(), chunkData.begin(), chunkData.end());
		}
	}
	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = static_cast<uInt>(_data_IDAT.size());
	stream.next_in = _data_IDAT.data();
	if (inflateInit2(&stream, MAX_WBITS) != Z_OK) {
		std::cerr << "Failed to initialize zlib" << std::endl;
	}
	vector<uint8_t> decompressed_data;
	int k = 0;
	vector<uint8_t> data;
	while (true)
	{
		vector<uint8_t> buffer(1024);
		stream.avail_out = static_cast<uInt>(buffer.size());
		stream.next_out = buffer.data();
		int result = inflate(&stream, Z_NO_FLUSH);
		if (result == Z_STREAM_END)
		{
			decompressed_data.insert(decompressed_data.end(), buffer.begin(), buffer.begin() + buffer.size() - stream.avail_out);
			break;
		}
		else if (result != Z_OK)
		{
			cerr << "Failed to decompress data" << std::endl;
			inflateEnd(&stream);
		}
		decompressed_data.insert(decompressed_data.end(),
			buffer.begin(),
			buffer.begin() + buffer.size() - stream.avail_out);
	}
	_data_IDAT = decompressed_data;
	cout << "Decompressed data size: " << _data_IDAT.size() << endl;
	inflateEnd(&stream);	
}


void PNG::showFFT()
{
	utils::logging::setLogLevel(utils::logging::LogLevel::LOG_LEVEL_SILENT);
	Mat I = imread(file_name, IMREAD_GRAYSCALE);
	if (I.empty()) {
		cerr << "Error opening image" << endl;
	}
	resize(I, I, Size(400, 400), INTER_LINEAR);
	Mat padded;                            //expand input image to optimal size
	int m = getOptimalDFTSize(I.rows);
	int n = getOptimalDFTSize(I.cols); // on the border add zero values
	copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
	dft(complexI, complexI);            // this way the result may fit in the source matrix
	// compute the magnitude and switch to logarithmic scale
	// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
	Mat magI = planes[0];
	magI += Scalar::all(1);                    // switch to logarithmic scale
	log(magI, magI);
	// crop the spectrum, if it has an odd number of rows or columns
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
	// rearrange the quadrants of Fourier image  so that the origin is at the image center
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;
	Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right
	Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);
	normalize(magI, magI, 0, 1, NORM_MINMAX); // Transform the matrix with float values into a
	// viewable image form (float between values 0 and 1).
	imshow("Input Image", I);    // Show the result
	imshow("spectrum magnitude", magI);
	//IDFT
	Mat inverse;
	idft(complexI, inverse, DFT_SCALE | DFT_REAL_OUTPUT);
	Mat final;
	inverse.convertTo(final, CV_8U);
	imshow("Inverse", final);
	waitKey();
}

void PNG::anonymizePNG()
{
	string name = "png_files/clean.png";
	/*name.erase(file_name.end() - 4);
	name.append("_clean.png");*/
	ofstream out;
	out.open(name, ios::binary);
	if (!out.is_open())
	{
		cerr << "Can't open file to write in: " << name << endl;
	}
	out.write(reinterpret_cast<char*>(&header[0]), sizeof(header[0]));
	out.write(reinterpret_cast<char*>(&header[1]), sizeof(header[1]));
	for (auto& a : chunks)
	{
		if (string(a->Name) == "IHDR")
		{
			IHDR_chunk* chunk = static_cast<IHDR_chunk*>(a.get());
			chunk->writeToFile(out);
		}
		else if (string(a->Name) == "PLTE")
		{
			PLTE_chunk* chunk = static_cast<PLTE_chunk*>(a.get());
			chunk->writeToFile(out);
		}
		else if (string(a->Name) == "IDAT")
		{
			IDAT_chunk* chunk = static_cast<IDAT_chunk*>(a.get());
			chunk->writeToFile(out);
		}
		else if (string(a->Name) == "IEND")
		{
			IEND_chunk* chunk = static_cast<IEND_chunk*>(a.get());
			chunk->writeToFile(out);
		}
	}
	out.close();
	cout << "Image successfully saved: " << name << endl;
}

void PNG::showImage()
{
	utils::logging::setLogLevel(utils::logging::LogLevel::LOG_LEVEL_SILENT);
	Mat img = imread(file_name);
	resize(img, img, Size(400, 400), INTER_LINEAR);
	imshow("Image", img);
	waitKey();
}

bool PNG::assertPNG()
{
	cout << "Asserting image!" << endl;
	if (string(chunks[0]->Name) != "IHDR") return false;
	int id_PLTE = -1, id_IDAT1 = -1, id_gAMA = -1;
	for (int i = 0; i < chunks.size(); i++)
	{
		if (string(chunks[i]->Name) == "PLTE")
		{
			id_PLTE = i;
			continue;
		}
		if (string(chunks[i]->Name) == "IDAT")
		{
			id_IDAT1 = i;
			break;
		}
	}
	if (id_PLTE > id_IDAT1) return false;
	if (string(chunks[chunks.size() - 1]->Name) != "IEND") return false;
	for (int i = 0; i < chunks.size(); i++)
	{
		if (string(chunks[i]->Name) == "gAMA")
		{
			id_gAMA = i;
			break;
		}
	}
	if ((id_gAMA > id_PLTE && id_PLTE != -1) || id_gAMA > id_IDAT1) return false;
	for (int i = 0; i < chunks.size(); i++)
	{
		if (!chunks[i]->assertChunk()) return false;
	}
	cout << "Asserting completed, everything ok!" << endl;
	return true;
}
