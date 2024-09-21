#pragma once
#include<string>

class Image {
public:
	int Width, Height, NRChannels;
	unsigned char *data;
	~Image();
	static Image* LoadImageFromFile(std::string file,bool flip = false);
};