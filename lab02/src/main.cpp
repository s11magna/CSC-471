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
	float x1 = atoi(argv[4]);
	// Vertex 1 y-coord
	float y1 = atoi(argv[5]);
	// Vertex 2 x-coord
	// RGB Value Vertex 1
	float x2 = atoi(argv[6]);
	// Vertex 2 y-coord
	float y2 = atoi(argv[7]);
    // Vertex 3 x-coord
	float x3 = atoi(argv[8]);
	//Vertex 4 y-coord
	float y3 = atoi(argv[9]);
	// Create the image. We're using a `shared_ptr`, a C++11 feature.
	auto image = make_shared<Image>(width, height);
	//Bounding Box Data Structure
	boundingBox box1;
	//Intialized Vertices
	points v1 = { x1,y1 };
	points v2 = { x2,y2 };
	points v3 = { x3,y3 };
	COLOR v1a = { 255,0,0};
	COLOR v2a = { 0,255,0 };
	COLOR v3a = { 0,0,255};
	// Triangle Vertices Bounds

	box1 = image->boundBox(v1, v2, v3);

	float A = image->totalArea(v1, v2, v3);
    float B, C;
	float Beta, Alpha, Gamma;
	float esp = 0.00001;
	for (int y = box1.ymin; y <= box1.ymax; ++y) {
		for (int x = box1.xmin; x <= box1.xmax; ++x) {
			B = image->AreaB(x, y, v1, v2, v3);
			C = image->AreaC(x, y, v1, v2, v3);
			Beta = (B) / (A);
			Gamma = (C) / (A);
			Alpha = 1 - Beta - Gamma;
			if ((Alpha >= -esp && Alpha <= 1 + esp) && (Gamma >= -esp && Gamma <= 1 + esp) && (Beta >= -esp && Beta <= 1 + esp))
			{
				float R = (Alpha * v1a.r) + (Beta * v2a.r) + (Gamma * v3a.r);
				float G = (Alpha * v1a.g) + (Beta * v2a.g) + (Gamma * v3a.g);
				float B = (Alpha * v1a.b) + (Beta * v2a.b) + (Gamma * v3a.b);
				image->setPixel(x, y, R, G, B);
			}
		}
	}

		// Write image to file
		image->writeToFile(filename);
		return 0;
}
