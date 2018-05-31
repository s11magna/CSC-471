#pragma once
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>
#include <vector>

struct boundingBox {
	int xmin;
	int xmax;
	int ymin;
	int ymax;
};

struct points {
	float x;
	float y;
	float z;
};

struct COLOR {
	int r;
	int g;
	int b;
};

void worldToPixel(int h, int w, points &V1);
float AreaB(int X, int Y, struct points v1, struct points v2, struct points v3);
float AreaC(int X, int Y, struct points v1, struct points v2, struct points v3);
struct boundingBox boundBox(struct points v1, struct points v2, struct points v3);
float totalArea(struct points v1, struct points v2, struct points v3);
float **makeArray(int width, int height);
void initArray(float **arr,int width,int height);
class Image
{
public:
	Image(int width, int height);
	virtual ~Image();
	void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
	void writeToFile(const std::string &filename);
	

	// add function to compute color

	int getWidth() const { return width; }
	int getHeight() const { return height; }

private:
	int width;
	int height;
	int comp;
	std::vector<unsigned char> pixels;
};

#endif
