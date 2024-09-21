#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "Image.h"
#include "../Utilities/FileUtil.hpp"

Image::~Image()
{
	stbi_image_free(data);
}