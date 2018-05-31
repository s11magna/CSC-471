#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <memory>
#include "Image.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;

int main(int argc, char **argv)
{
	if(argc < 4) {
		cout << "Usage: Lab1 <out_image_name>.png width height" << endl;
		return 0;
	}
	// Output filename
	string filename(argv[1]);
	// Width of image
	int width = atoi(argv[2]);
	// Height of image
	int height = atoi(argv[3]);
	// Vertex 1 x-coord
	int x1 = atoi(argv[4]);
	// Vertex 1 y-coord
	int y1 = atoi(argv[5]);
	// Vertex 2 x-coord
	int x2 = atoi(argv[6]);
	// Vertex 2 y-coord
	int y2 = atoi(argv[7]);
    // Vertex 3 x-coord
	int x3 = atoi(argv[8]);
	//Vertex 4 y-coord
	int y3 = atoi(argv[9]);
	// Create the image. We're using a `shared_ptr`, a C++11 feature.
	auto image = make_shared<Image>(width, height);
	//Bounding Box Data Structure
	struct boundingBox {
		int xmin; 
		int xmax; 
		int ymin;
		int ymax;
	};

	boundingBox box1;	
	// Triangle Vertices
	
	// Bounding Box
	// X Max
	if ((x1 > x2) && (x1 > x3))
		box1.xmax = x1;
	else if (x2 > x3) {
		box1.xmax = x2;
	}
	else {
		box1.xmax = x3;
	}
	// X Min
	printf("X Max %d\n", box1.xmax);
	if ((x1 < x2) && (x1 < x3))
		box1.xmin = x1;
	else if (x2 < x3)
		box1.xmin = x2;
	else
		box1.xmin = x3;
	printf("X Min %d\n", box1.xmin);
	//Y Max 
	if ((y1 > y2) && (y1 > y3))
		box1.ymax = y1;
	else if (y2 > y3)
		box1.ymax = y2;
	else
		box1.ymax = y3;
	//Y Min
	printf("Y Max %d\n", box1.ymax);
	if ((y1 < y2) && (y1 < y3))
		box1.ymin = y1;
	else if (y2 < y3)
		box1.ymin = y2;
	else
		box1.ymin = y3;
	printf("Y Min %d\n", box1.ymin);



	for (int y = box1.ymin; y <= box1.ymax; ++y) {
		for (int x = box1.xmin; x <= box1.xmax; ++x) {
			if (x % 2 != 0) {
				image->setPixel(x, y, 0, 255, 0);
			}
			else {
				image->setPixel(x, y, 255, 0, 0);
			}
		}
	}
		image->setPixel(x1, y1, 0, 0, 255);
		image->setPixel(x2, y2, 0, 0, 255);
		image->setPixel(x3, y3, 0, 0, 255);

		// Write image to file
		image->writeToFile(filename);
		return 0;
}
