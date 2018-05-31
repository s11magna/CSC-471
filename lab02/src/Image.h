#pragma once
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>
#include <vector>

struct boundingBox {
	float xmin;
	float xmax;
	float ymin;
	float ymax;
};

struct points {
	float x;
	float y;
};

struct COLOR {
	int r;
	int g;
	int b;
};
class Image
{
public:
	Image(int width, int height);
	virtual ~Image();
	void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
	struct boundingBox boundBox(struct points v1, struct points v2, struct points v3);
	float totalArea(struct points v1, struct points v2, struct points v3);
	float AreaB(int X, int Y,struct points v1, struct points v2, struct points v3);
	float AreaC(int X, int Y, struct points v1, struct points v2, struct points v3);
	void writeToFile(const std::string &filename);
	int getWidth() const { return width; }
	int getHeight() const { return height; }

private:
	int width;
	int height;
	int comp;
	std::vector<unsigned char> pixels;
};

#endif
