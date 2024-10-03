#pragma once
#include<string>

class Image {
public:
	int Width, Height, NRChannels;
	unsigned char *data;
	bool gammaCorrect = false;
	~Image();
};