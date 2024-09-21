#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "Image.h"
#include "../Utilities/FileUtil.hpp"

Image::~Image()
{
	stbi_image_free(data);
}

Image* Image::LoadImageFromFile(std::string file,bool flip)
{
	Image* image = new Image();
	stbi_set_flip_vertically_on_load(flip);
	image->data = stbi_load(GetImagePath(file).c_str(), &image->Width, &image->Height, &image->NRChannels, 0);
	if (!image->data)
		std::cout << "Failed to load Image" << std::endl;
	return image;
}