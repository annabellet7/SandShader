#ifndef TEXTURE_H
#define TEXTURE_H

#include "..\ew\external\glad.h"
#include "..\ew\external\stb_image.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Texture2D
{
public:
	Texture2D(const char* filePath, int filterModeMin, int FliterModMag, int wrapModeS, int wrapModeT, int alpha);
	~Texture2D();

	void bind(unsigned int slot = 0);

private:
	unsigned int mId;
	int mWidth, mHeight, mNrChannels;
};

#endif
