#pragma once
#include<string>

class Image {
public:
	enum ImageType
	{
		texture, normal
	};

	int Width, Height, NRChannels;
	unsigned char *data;
	ImageType imageType;
	bool gammaCorrect = false;
	~Image();
};