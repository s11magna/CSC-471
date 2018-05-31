/* Release code for program 1 CPE 471 Fall 2016 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Image.h"

// This allows you to skip the `std::` in front of C++ standard library
// functions. You can also say `using std::cout` to be more selective.
// You should never do this in a header file.
using namespace std;
//Width and Height
 int g_width, g_height, mode;
//Vertex Points

points V[3] = { {0,0,0},{0,0,0},{0,0,0} };
float A, B, C;
float **arr;
float Beta, Alpha, Gamma, espn;
COLOR CLR[3] = { { 255,0,0 },{ 255,0,0 },{ 255,0,0 } };
boundingBox box1;
/*
   Helper function you will want all quarter
   Given a vector of shapes which has already been read from an obj file
   resize all vertices to the range [-1, 1]
 */
void resize_obj(std::vector<tinyobj::shape_t> &shapes){
   float minX, minY, minZ;
   float maxX, maxY, maxZ;
   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   double epsilon = 0.001;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;


   //Go through all vertices to determine min and max of each dimension
   for (size_t i = 0; i < shapes.size(); i++) {
      for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
         if(shapes[i].mesh.positions[3*v+0] < minX) minX = shapes[i].mesh.positions[3*v+0];
         if(shapes[i].mesh.positions[3*v+0] > maxX) maxX = shapes[i].mesh.positions[3*v+0];

         if(shapes[i].mesh.positions[3*v+1] < minY) minY = shapes[i].mesh.positions[3*v+1];
         if(shapes[i].mesh.positions[3*v+1] > maxY) maxY = shapes[i].mesh.positions[3*v+1];

         if(shapes[i].mesh.positions[3*v+2] < minZ) minZ = shapes[i].mesh.positions[3*v+2];
         if(shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = shapes[i].mesh.positions[3*v+2];
      }
   }

	//From min and max compute necessary scale and shift for each dimension
   float maxExtent, xExtent, yExtent, zExtent;
   xExtent = maxX-minX;
   yExtent = maxY-minY;
   zExtent = maxZ-minZ;
   if (xExtent >= yExtent && xExtent >= zExtent) {
      maxExtent = xExtent;
   }
   if (yExtent >= xExtent && yExtent >= zExtent) {
      maxExtent = yExtent;
   }
   if (zExtent >= xExtent && zExtent >= yExtent) {
      maxExtent = zExtent;
   }
   scaleX = 2.0 /maxExtent;
   shiftX = minX + (xExtent/ 2.0);
   scaleY = 2.0 / maxExtent;
   shiftY = minY + (yExtent / 2.0);
   scaleZ = 2.0/ maxExtent;
   shiftZ = minZ + (zExtent)/2.0;

   //Go through all verticies shift and scale them
   for (size_t i = 0; i < shapes.size(); i++) {
      for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
         shapes[i].mesh.positions[3*v+0] = (shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
         assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
         shapes[i].mesh.positions[3*v+1] = (shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
         assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
         shapes[i].mesh.positions[3*v+2] = (shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
         assert(shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
         assert(shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
      }
   }
}


int main(int argc, char **argv)
{
	if (argc < 5) { // has 5 arguments
		cout << "Usage: raster meshfile imagefile" << endl;
		return 0;
	}
	// OBJ filename
	string meshName(argv[1]); // Object.obj 
	string imgName(argv[2]); // Outfile.png

	//set g_width and g_height appropriately!
	g_width = atoi(argv[3]); // Width
	g_height = atoi(argv[4]); //Height
	mode = atoi(argv[5]); // mode

	if (mode != 1 && mode != 2) {
		fprintf(stderr, "ERROR: Incorrect Mode is Select. Choose either 1 or 2\n");
		exit(1);
	}
	//create an image
	auto image = make_shared<Image>(g_width, g_height);

	// triangle buffer
	vector<unsigned int> triBuf;
	// position buffer
	vector<float> posBuf;
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes; // geometry
	vector<tinyobj::material_t> objMaterials; // material
	string errStr;

	arr = makeArray(g_width, g_height);
	initArray(arr,g_width,g_height);

	bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
	/* error checking on read */
	if (!rc) {
		cerr << errStr << endl;
	}
	else {
		//keep this code to resize your object to be within -1 -> 1
		resize_obj(shapes);
		posBuf = shapes[0].mesh.positions; // vertex position coordinates
		triBuf = shapes[0].mesh.indices; //faces 
	}
	cout << "Number of vertices: " << posBuf.size() / 3 << endl; // vertices (XYZ)
	cout << "Number of triangles: " << triBuf.size() / 3 << endl; // positions of the vertices

	
	//TODO add code to iterate through each triangle and rasterize it 
	for (int i = 0; i < triBuf.size(); i += 3) { // Grab three Vertex Value ie.. 0,1,2
		// fix this if you have time ;(
		//v x's
		V[0].x = posBuf[triBuf[i] * 3];
		V[1].x = posBuf[triBuf[i + 1] * 3];
		V[2].x = posBuf[triBuf[i + 2] * 3];
		//v y's
		V[0].y = posBuf[(triBuf[i] * 3) + 1];
		V[1].y = posBuf[(triBuf[i + 1] * 3) + 1];
		V[2].y = posBuf[(triBuf[i + 2] * 3) + 1];
		//v z's
		V[0].z = posBuf[(triBuf[i] * 3) + 2];
		V[1].z = posBuf[(triBuf[i + 1] * 3) + 2];
		V[2].z = posBuf[(triBuf[i + 2] * 3) + 2];

		worldToPixel(g_height, g_width, V[0]);
		worldToPixel(g_height, g_width, V[1]);
		worldToPixel(g_height, g_width, V[2]);

		boundingBox box1 = boundBox(V[0], V[1], V[2]);
		// total Area calculation
		A = totalArea(V[0], V[1], V[2]);
		for (int y = box1.ymin; y <= box1.ymax; ++y) {
			for (int x = box1.xmin; x <= box1.xmax; ++x) {
				B = AreaB(x, y, V[0], V[1], V[2]);
				C = AreaC(x, y, V[0], V[1], V[2]);
				Beta = (B) / (A);
				Gamma = (C) / (A);
				Alpha = 1 - Beta - Gamma;
				if ((Alpha >= -espn && Alpha <= 1 + espn) && (Gamma >= -espn && Gamma <= 1 + espn) && (Beta >= -espn && Beta <= 1 + espn))
				{
					if (mode == 1) { // depth
						float Z = (Alpha * V[0].z) + (Beta * V[1].z) + (Gamma * V[2].z);
						float D = 127 * Z + 127;
						if (arr[x][y] < D) {
							arr[x][y] = D;
						}
					}
					else {  //interpolate two color red and blue
						
						    double r = (y /(double)(g_height));
							double g = 0;
						    double b = (1-r);
							image->setPixel(x,y,(r * 255),g,(b * 255));
					}
				}
			}
		}

	}
	// dpeth mode 1
	if (mode == 1) {
		for (int p = 0; p < g_width; p++) {
			for (int q = 0; q < g_height; q++) {
				image->setPixel(p, q, arr[p][q], 0, 0);
			}
		}
	}
	//write out the image
   image->writeToFile(imgName);*/
   
	return 0;
}
