/**
 * This file defines all helper functions. Those functions shouldn't depend on any thing in this project
 */
#pragma once
#ifndef _ICG_HELPER_H_
#define _ICG_HELPER_H_

#include "icg_common.h"

// error check helper from EPFL ICG class
static inline const char* ErrorString(GLenum error) {
	const char* msg;
	switch (error) {
#define Case(Token)  case Token: msg = #Token; break;
		Case(GL_INVALID_ENUM);
		Case(GL_INVALID_VALUE);
		Case(GL_INVALID_OPERATION);
		Case(GL_INVALID_FRAMEBUFFER_OPERATION);
		Case(GL_NO_ERROR);
		Case(GL_OUT_OF_MEMORY);
#undef Case
	}
	return msg;
}

static inline void _glCheckError(const char* file, int line, const char* comment) {
	GLenum error;
	while ((error = glGetError()) != GL_NO_ERROR) {
		fprintf(stderr, "ERROR: %s (file %s, line %i: %s).\n", comment, file, line, ErrorString(error));
	}
}

#ifndef NDEBUG
#	define check_error_gl(x) _glCheckError(__FILE__, __LINE__, x)
#else
#	define check_error_gl() ((void)0)
#endif

namespace icg {

	struct ImageData {
		std::vector<unsigned char> data;
		unsigned int width, height, channel;
		GLenum internalFormat, format, type;
	};

	cy::Matrix4f PerspectiveProj(float aspect, float x, float y,
		float n, float f, bool fixaspect = false) {
		if (!fixaspect) {
			if (aspect > 1) { x = y * aspect; } // scale x when Width > Height
			else { y = x / aspect; } // scale y when Height >= Width
		}
		cy::Matrix4f proj;
		proj.SetIdentity();
		proj(0, 0) = n / x;
		proj(1, 1) = n / y;
		proj(2, 2) = -(n + f) / (f - n);
		proj(2, 3) = -2 * f * n / (f - n);
		proj(3, 2) = -1;
		proj(3, 3) = 0;
		return proj;
	}

	cy::Matrix4f OrthographicProj(float aspect, float x, float y,
		float n, float f, bool fixaspect = false) {
		if (!fixaspect) {
			if (aspect > 1) { x = y * aspect; }
			else { y = x / aspect; }
		}
		cy::Matrix4f proj;
		proj = cy::Matrix4f::MatrixScale(2.0f / x, 2.0f / y, -2.0f / (f - n));
		proj(2, 3) = -(f + n) / (f - n);
		proj(3, 3) = 1;
		return proj;
	}

	cy::Matrix4f LookAt(cy::Point3f campos, cy::Point3f focus, cy::Point3f up)
	{
		cy::Matrix4f view;
		view.SetView(campos, focus, up);
		return view;
	}

	namespace helper {

		//
		// reference from http://stackoverflow.com/questions/20595340/loading-a-tga-bmp-file-in-c-opengl
		// usage
		//  info.HasAlphaChannel() 
		//  info.GetWidth()
		//  info.GetWidth()
		//  info.GetPixels().data()
		//
		class BMP
		{
		public:
			typedef union PixelInfo
			{
				std::uint32_t Colour;
				struct
				{
					unsigned char B, G, R, A;
				};
			} *PPixelInfo;
		private:
			std::uint32_t width, height;
			std::uint16_t BitsPerPixel;
			std::vector<unsigned char>& Pixels;

		public:
			BMP(std::vector<unsigned char>& pixels, const char* FilePath);
			std::vector<unsigned char> GetPixels() const { return this->Pixels; }
			std::uint32_t GetWidth() const { return this->width; }
			std::uint32_t GetHeight() const { return this->height; }
			bool HasAlphaChannel() { return BitsPerPixel == 32; }
			unsigned char NumberOfChannel() { return BitsPerPixel / 8; }
		};

		BMP::BMP(std::vector<unsigned char>& pixels, const char* FilePath)
			: Pixels(pixels)
		{
			std::fstream hFile(FilePath, std::ios::in | std::ios::binary);
			if (!hFile.is_open()) throw std::invalid_argument("Error: File Not Found.");

			hFile.seekg(0, std::ios::end);
			std::size_t Length = hFile.tellg();
			hFile.seekg(0, std::ios::beg);
			std::vector<unsigned char> FileInfo(Length);
			hFile.read(reinterpret_cast<char*>(FileInfo.data()), 54);

			if (FileInfo[0] != 'B' && FileInfo[1] != 'M')
			{
				hFile.close();
				throw std::invalid_argument("Error: Invalid File Format. Bitmap Required.");
			}

			if (FileInfo[28] != 24 && FileInfo[28] != 32)
			{
				hFile.close();
				throw std::invalid_argument("Error: Invalid File Format. 24 or 32 bit Image Required.");
			}

			BitsPerPixel = FileInfo[28];
			width = FileInfo[18] + (FileInfo[19] << 8);
			height = FileInfo[22] + (FileInfo[23] << 8);
			std::uint32_t PixelsOffset = FileInfo[10] + (FileInfo[11] << 8);
			std::uint32_t size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
			Pixels.resize(size);

			hFile.seekg(PixelsOffset, std::ios::beg);
			hFile.read(reinterpret_cast<char*>(Pixels.data()), size);
			hFile.close();
		}

		class TGA
		{
		public:
			typedef union PixelInfo
			{
				std::uint32_t Colour;
				struct
				{
					unsigned char R, G, B, A;
				};
			} *PPixelInfo;
		private:
			std::vector<unsigned char>& Pixels;
			bool ImageCompressed;
			std::uint32_t width, height, size, BitsPerPixel;

		public:
			TGA(std::vector<unsigned char>& pixels, const char* FilePath);
			std::vector<unsigned char> GetPixels() { return this->Pixels; }
			std::uint32_t GetWidth() const { return this->width; }
			std::uint32_t GetHeight() const { return this->height; }
			bool HasAlphaChannel() { return BitsPerPixel == 32; }
			unsigned char NumberOfChannel() { return BitsPerPixel / 8; }
		};

		TGA::TGA(std::vector<unsigned char>& pixels, const char* FilePath)
			: Pixels(pixels)
		{
			std::fstream hFile(FilePath, std::ios::in | std::ios::binary);
			if (!hFile.is_open()) { throw std::invalid_argument("File Not Found."); }

			unsigned char Header[18] = { 0 };
			std::vector<unsigned char> ImageData;
			static unsigned char DeCompressed[12] = { 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
			static unsigned char IsCompressed[12] = { 0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

			hFile.read(reinterpret_cast<char*>(&Header), sizeof(Header));

			if (!std::memcmp(DeCompressed, &Header, sizeof(DeCompressed)))
			{
				BitsPerPixel = Header[16];
				width = Header[13] * 256 + Header[12];
				height = Header[15] * 256 + Header[14];
				size = ((width * BitsPerPixel + 31) / 32) * 4 * height;

				if ((BitsPerPixel != 24) && (BitsPerPixel != 32))
				{
					hFile.close();
					throw std::invalid_argument("Invalid File Format. Required: 24 or 32 Bit Image.");
				}

				ImageData.resize(size);
				ImageCompressed = false;
				hFile.read(reinterpret_cast<char*>(ImageData.data()), size);
			}
			else if (!std::memcmp(IsCompressed, &Header, sizeof(IsCompressed)))
			{
				BitsPerPixel = Header[16];
				width = Header[13] * 256 + Header[12];
				height = Header[15] * 256 + Header[14];
				size = ((width * BitsPerPixel + 31) / 32) * 4 * height;

				if ((BitsPerPixel != 24) && (BitsPerPixel != 32))
				{
					hFile.close();
					throw std::invalid_argument("Invalid File Format. Required: 24 or 32 Bit Image.");
				}

				TGA::PixelInfo Pixel = { 0 };
				int CurrentByte = 0;
				std::size_t CurrentPixel = 0;
				ImageCompressed = true;
				unsigned char ChunkHeader = { 0 };
				int BytesPerPixel = (BitsPerPixel / 8);
				ImageData.resize(width * height * sizeof(TGA::PixelInfo));

				do
				{
					hFile.read(reinterpret_cast<char*>(&ChunkHeader), sizeof(ChunkHeader));

					if (ChunkHeader < 128)
					{
						++ChunkHeader;
						for (int I = 0; I < ChunkHeader; ++I, ++CurrentPixel)
						{
							hFile.read(reinterpret_cast<char*>(&Pixel), BytesPerPixel);

							ImageData[CurrentByte++] = Pixel.B;
							ImageData[CurrentByte++] = Pixel.G;
							ImageData[CurrentByte++] = Pixel.R;
							if (BitsPerPixel > 24) ImageData[CurrentByte++] = Pixel.A;
						}
					}
					else
					{
						ChunkHeader -= 127;
						hFile.read(reinterpret_cast<char*>(&Pixel), BytesPerPixel);

						for (int I = 0; I < ChunkHeader; ++I, ++CurrentPixel)
						{
							ImageData[CurrentByte++] = Pixel.B;
							ImageData[CurrentByte++] = Pixel.G;
							ImageData[CurrentByte++] = Pixel.R;
							if (BitsPerPixel > 24) ImageData[CurrentByte++] = Pixel.A;
						}
					}
				} while (CurrentPixel < (width * height));
			}
			else
			{
				hFile.close();
				throw std::invalid_argument("Invalid File Format. Required: 24 or 32 Bit TGA File.");
			}

			hFile.close();
			this->Pixels = ImageData;
		}

		//!
		//! loadfile Load file into a string
		inline std::string loadfile(const char *filename, std::ostream *outStream = &std::cout) {
			std::ifstream stream(filename, std::ios::in);
			if (!stream.is_open()) {
				if (outStream) *outStream << "ERROR: Cannot open file '" << filename << "'"<< std::endl;
				exit(EXIT_FAILURE);
			}
			std::string str = std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
			stream.close();
			return str;
		}

		/**
		 * @brief copychar: copy string data to char pointer
		 */
		inline void copychar(char * &str, const std::string& src, int start = 0)
		{
			if (str) { delete[] str; }
			if (src == "") { str = nullptr; }
			else {
				size_t len = strlen(src.c_str());
				str = new char[len + 1]; str[len] = '\0';
				strncpy(str, src.c_str(), len);
			}
		}

		/**
		 * @brief mouse2screen: convert mouse coordinate to [-1,1] * [-1,1]
		 */
		inline void mouse2screen(int x, int y, float width, float height, cy::Point2f& p)
		{
			p = cy::Point2f(2.0f * (float)x / width - 1.0f, 1.0f - 2.0f * (float)y / height);
		}

		/**
		 * @brief loadimg
		 */
		inline void loadimg(ImageData& image, const std::string filename, const std::string path = "")
		{
			if (!filename.empty()) {
				std::string fnamestr(path + filename);
				std::size_t found = fnamestr.find_last_of('.');
				if (found == std::string::npos) { std::cerr << "Error: unknown file format " << std::endl; return; }
				// check image format
				std::string ext = fnamestr.substr(found + 1);
				if (ext == "png" || ext == "PNG") {
					std::vector<unsigned char> buffer;
					lodepng::State state;
					lodepng::load_file(buffer, fnamestr); //load the image file with given filename
					unsigned int error = lodepng::decode(image.data, image.width, image.height, state, buffer);
					if (error) { //if there's an error, display it
						std::cout << "decoder error '" << fnamestr.c_str() << "' " << error << ": " << lodepng_error_text(error) << std::endl;
					}
					image.channel = lodepng_get_channels(&state.info_png.color);
					if (image.channel == 1) { 
						image.internalFormat = GL_R8;
						image.format = GL_RED;
					}
					else if (image.channel == 3) {
						image.internalFormat = GL_RGB8;
						image.format = GL_RGB;
					}
					else {
						image.internalFormat = GL_RGBA8;
						image.format = GL_RGBA;
					}
					image.type = GL_UNSIGNED_BYTE;
				}
				else if (ext == "tga" || ext == "TGA") {
					TGA tgainfo = TGA(image.data, fnamestr.c_str());
					image.width = tgainfo.GetWidth();
					image.height = tgainfo.GetHeight();
					image.channel = tgainfo.NumberOfChannel();
					if (image.channel == 1) {
						image.internalFormat = GL_R8;
						image.format = GL_RED;
					}
					else if (image.channel == 3) {
						image.internalFormat = GL_RGB8;
						image.format = GL_BGR;
					}
					else {
						image.internalFormat = GL_RGBA8;
						image.format = GL_BGRA;
					}
					image.type = GL_UNSIGNED_BYTE;
				}
				else if (ext == "bmp" || ext == "BMP") {
					std::cerr << "Error: BMP format reader is not implemented yet" << std::endl;
				}
				else {
					std::cerr << "Error: unknown file format " << ext << std::endl;
				}
			}
		}

		inline void saveimg(std::string filename, int count, std::vector<unsigned char>& image, unsigned int w, unsigned int h, bool compress = false) 
		{
			// encode as hard as possible with several filter types and window sizes
			filename += std::to_string(count) + ".png";
			std::vector<unsigned char> buffer;
			unsigned int error;
			lodepng::State state;
			if (compress) {
				state.encoder.filter_palette_zero = 0; //We try several filter types, including zero, allow trying them all on palette images too.
				state.encoder.add_id = false; //Don't add LodePNG version chunk to save more bytes
				state.encoder.text_compression = 1; //Not needed because we don't add text chunks, but this demonstrates another optimization setting
				state.encoder.zlibsettings.nicematch = 258; //Set this to the max possible, otherwise it can hurt compression
				state.encoder.zlibsettings.lazymatching = 1; //Definitely use lazy matching for better compression
				state.encoder.zlibsettings.windowsize = 32768; //Use maximum possible window size for best compression

				size_t bestsize = 0;
				bool inited = false;

				int beststrategy = 0;
				LodePNGFilterStrategy strategies[4] = { LFS_ZERO, LFS_MINSUM, LFS_ENTROPY, LFS_BRUTE_FORCE };
				std::string strategynames[4] = { "LFS_ZERO", "LFS_MINSUM", "LFS_ENTROPY", "LFS_BRUTE_FORCE" };

				// min match 3 allows all deflate lengths. min match 6 is similar to "Z_FILTERED" of zlib.
				int minmatches[2] = { 3, 6 };
				int bestminmatch = 0;

				int autoconverts[2] = { 0, 1 };
				std::string autoconvertnames[2] = { "0", "1" };
				int bestautoconvert = 0;

				int bestblocktype = 0;

				// Try out all combinations of everything
				for (int i = 0; i < 4; i++) //filter strategy
					for (int j = 0; j < 2; j++) //min match
						for (int k = 0; k < 2; k++) //block type (for small images only)
							for (int l = 0; l < 2; l++) //color convert strategy
							{
								if (bestsize > 3000 && (k > 0 || l > 0)) continue; /* these only make sense on small images */
								std::vector<unsigned char> temp;
								state.encoder.filter_strategy = strategies[i];
								state.encoder.zlibsettings.minmatch = minmatches[j];
								state.encoder.zlibsettings.btype = k == 0 ? 2 : 1;
								state.encoder.auto_convert = autoconverts[l];
								error = lodepng::encode(temp, image, w, h, state);

								if (error)
								{
									std::cout << "encoding error " << error << ": " << lodepng_error_text(error) << std::endl;
									return;
								}

								if (!inited || temp.size() < bestsize)
								{
									bestsize = temp.size();
									beststrategy = i;
									bestminmatch = state.encoder.zlibsettings.minmatch;
									bestautoconvert = l;
									bestblocktype = state.encoder.zlibsettings.btype;
									temp.swap(buffer);
									inited = true;
								}
							}

				std::cout << "Chosen filter strategy: " << strategynames[beststrategy] << std::endl;
				std::cout << "Chosen min match: " << bestminmatch << std::endl;
				std::cout << "Chosen block type: " << bestblocktype << std::endl;
				std::cout << "Chosen auto convert: " << autoconvertnames[bestautoconvert] << std::endl;

				lodepng::save_file(buffer, filename);
			} 
			else {
				error = lodepng::encode(buffer, image, w, h, state);
				if (error)
				{
					std::cout << "encoding error " << error << ": " << lodepng_error_text(error) << std::endl;
					return;
				}
				lodepng::save_file(buffer, filename);
			}
			std::cout << "New size: " << buffer.size() << " (" << (buffer.size() / 1024) << "K)" << std::endl;
		}


		inline void icgassert(bool result) { if (!result) { exit(EXIT_FAILURE); } }
		inline void debug(const cy::Matrix3f& m) {
#ifndef NDEBUG
			std::cout << std::endl;
			std::cout << "\t" << m(0, 0) << "\t" << m(0, 1) << "\t" << m(0, 2) << std::endl;
			std::cout << "\t" << m(1, 0) << "\t" << m(1, 1) << "\t" << m(1, 2) << std::endl;
			std::cout << "\t" << m(2, 0) << "\t" << m(2, 1) << "\t" << m(2, 2) << std::endl;
#endif
		}
		inline void debug(const cy::Matrix4f& m) {
#ifndef NDEBUG
			std::cout << std::endl;
			std::cout << "\t" << m(0, 0) << "\t" << m(0, 1) << "\t" << m(0, 2) << "\t" << m(0, 3) << std::endl;
			std::cout << "\t" << m(1, 0) << "\t" << m(1, 1) << "\t" << m(1, 2) << "\t" << m(1, 3) << std::endl;
			std::cout << "\t" << m(2, 0) << "\t" << m(2, 1) << "\t" << m(2, 2) << "\t" << m(2, 3) << std::endl;
			std::cout << "\t" << m(3, 0) << "\t" << m(3, 1) << "\t" << m(3, 2) << "\t" << m(3, 3) << std::endl;
#endif
		}
		inline void debug(const cy::Point2f& m) {
#ifndef NDEBUG
			std::cout << std::endl;
			std::cout << "\t" << m[0] << "\t" << m[1] << std::endl;
#endif
		}
		inline void debug(const cy::Point3f& m) {
#ifndef NDEBUG
			std::cout << std::endl;
			std::cout << "\t" << m[0] << "\t" << m[1] << "\t" << m[2] << std::endl;
#endif
		}
		inline void debug(const cy::Point4f& m) {
#ifndef NDEBUG
			std::cout << std::endl;
			std::cout << "\t" << m[0] << "\t" << m[1] << "\t" << m[2] << "\t" << m[3] << std::endl;
#endif
		}
	};

};

#endif//_ICG_HELPER_H_