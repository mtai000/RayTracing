#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cstdlib"
#include <iostream>

class rtw_image {
public:
	rtw_image() {}
	rtw_image(const char* filename) {

		if (Load(filename)) return;

		std::cerr << "ERROR: Could not laod image file: " << filename << std::endl;
	}
	~rtw_image() {
		delete[] bdata;
		delete[] fdata;

	}

	bool Load(const char* filename)
	{
		auto n = bytesPerPixel;
		fdata = stbi_loadf(filename, &mWidth, &mHeight, &n, bytesPerPixel);

		if (fdata == nullptr) return false;
		bytesPerScanline = mWidth * bytesPerPixel;
		CovertToBytes();
		return true;
	}

	const unsigned char* pixelData(int x, int y) const {
		static unsigned char magenta[] = { 255,0,255 };
		if (bdata == nullptr) return magenta;

		x = clamp(x, 0, mWidth);
		y = clamp(y, 0, mHeight);

		auto c = bdata + y * bytesPerScanline + x * bytesPerPixel;
		if (c > &bdata[totalBytes - 1])
			std::cerr << "out of buffer index" << std::endl;
		return c;
	}

	int GetWidth() const { return  mWidth; }
	int GetHeight() const { return mHeight; }
private:
	const int bytesPerPixel = 3;
	int bytesPerScanline = 0;
	int mWidth = 0;
	int mHeight = 0;
	int totalBytes = 0;
	float* fdata = nullptr;
	unsigned char* bdata = nullptr;

	void CovertToBytes() {
		totalBytes = mHeight * mWidth * bytesPerPixel;
		bdata = new unsigned char[totalBytes];

		auto* bptr = bdata;
		auto* fptr = fdata;
		for (auto i = 0; i < totalBytes; i++, fptr++, bptr++) {
			*bptr = floatToByte(*fptr);
		}

	}

	unsigned char floatToByte(float value) {
		if (value <= 0.0) return 0;
		if (value >= 1.0) return 255;
		return (unsigned int)(256.0 * value);
	}

};
