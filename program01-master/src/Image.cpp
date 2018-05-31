#include <iostream>
#include <cassert>
#include "Image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

Image::Image(int w, int h) :
	width(w),
	height(h),
	comp(3),
	pixels(width*height*comp, 0)
{
}

Image::~Image()
{
}

void Image::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	// The pixel data is laid out row by row. Each row consists of 'width'
	// columns, and each column consists of 3 unsigned chars.

	// First check for bounds
	if(y < 0 || y >= height) {
		cout << "Row " << y << " is out of bounds" << endl;
		return;
	}
	if(x < 0 || x >= width) {
		cout << "Col " << x << " is out of bounds" << endl;
		return;
	}

	// Since the origin (0, 0) of the image is the upper left corner, we need
	// to flip the row to make the origin be the lower left corner.
	y = height - y - 1;
	// index corresponding to row and col, (assuming single component image)
	int index = y*width + x;
	// Multiply by 3 to get the index for the rgb components.
	assert(index >= 0);
	assert(3*index + 2 < (int)pixels.size());
	pixels[3*index + 0] = r;
	pixels[3*index + 1] = g;
	pixels[3*index + 2] = b;
}

void Image::writeToFile(const string &filename)
{
	// The distance in bytes from the first byte of a row of pixels to the
	// first byte of the next row of pixels
	int stride_in_bytes = width*comp*sizeof(unsigned char);
	int rc = stbi_write_png(filename.c_str(), width, height, comp, &pixels[0], stride_in_bytes);
	if(rc) {
		cout << "Wrote to " << filename << endl;
	} else {
		cout << "Couldn't write to " << filename << endl;
	}
}
// Compute Bounding Box
struct boundingBox boundBox(struct points v1, struct points v2, struct points v3)
{
	struct boundingBox box1;
	// Bounding Box
	// X Max
	if ((v1.x > v2.x) && (v1.x > v3.x))
		box1.xmax = v1.x;
	else if (v2.x > v3.x) {
		box1.xmax = v2.x;
	}
	else {
		box1.xmax = v3.x;
	}
	// X Min
	if ((v1.x < v2.x) && (v1.x < v3.x))
		box1.xmin = v1.x;
	else if (v2.x < v3.x)
		box1.xmin = v2.x;
	else
		box1.xmin = v3.x;
	//Y Max 
	if ((v1.y > v2.y) && (v1.y > v3.y))
		box1.ymax = v1.y;
	else if (v2.y > v3.y)
		box1.ymax = v2.y;
	else
		box1.ymax = v3.y;
	//Y Min
	if ((v1.y < v2.y) && (v1.y < v3.y))
		box1.ymin = v1.y;
	else if (v2.y < v3.y)
		box1.ymin = v2.y;
	else
		box1.ymin = v3.y;

	return box1;
}
 // BaryCentric Coordinates
float totalArea(struct points v1, struct points v2, struct points v3) {
	float A = 0.5*(((v2.x - v1.x)*(v3.y - v1.y)) - ((v3.x - v1.x)*(v2.y - v1.y)));
	return A;
}
float AreaB(int X, int Y, struct points v1, struct points v2, struct points v3) {
	float B = 0.5 *(((v1.x - v3.x)*(Y - v3.y)) - ((X - v3.x)*(v1.y - v3.y)));
	return B;
}
float AreaC(int X, int Y, struct points v1, struct points v2, struct points v3) {
	float C = 0.5 *(((v2.x - v1.x)*(Y - v1.y)) - ((X - v1.x)*(v2.y - v1.y)));
	return C;
}

// World Space to Pixel Space
void worldToPixel(int h, int w, points &V1)
{
   // One possible source of error
	double l = -w / (double)h;
	double R = w / (double)h;
	double b = -1.0;
	double t = 1.0;


	double e = -(h - 1) / (b - t);
	double f = (h - 1) - (e*t);

	double c = -(w - 1) / (l - R);
	double d = (w - 1) - (c*R);

	V1.x = (float)(c * V1.x + d);
	V1.y = (float)(e * V1.y + f);												
}

float **makeArray(int width, int height) {
	float **arr = new float *[width];
	for (int i = 0; i < width; ++i) {
		arr[i] = new float[height];
	}
	return arr;
}

void initArray(float **arr,int width ,int height) {
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			arr[x][y] = 0;
		}
	}
}

