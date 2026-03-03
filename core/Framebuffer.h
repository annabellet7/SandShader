#pragma once

class Framebuffer
{
	unsigned int fbo;
	unsigned int rbo;
	unsigned int textureColorBuffer[2];

public:
	Framebuffer();

	void init(int width, int height, bool hdr, int num);
	void deleteBuffer();
	bool checkStatus();

	unsigned int getFbo();
	unsigned int getColorTexturebuffer(int i);
};