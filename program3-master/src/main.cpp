/* Lab 6 base code - transforms using matrix stack built on glm
* CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
*/

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "tiny_obj_loader.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;


vector<tinyobj::shape_t> shapes; // geometry
vector<tinyobj::material_t> objMaterials; // material
vector<unsigned int> triBuf;
vector<float> posBuf;
vector<float> norBuf;


class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> prog1;

	// Shape to be used (from obj file)


	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	GLuint VertexArrayID2;

	GLuint VertexBufferID2;

	GLuint normals;

	GLuint elementbuffer;

	int shaderU;
	int material;
	float lightDur;
	float rTheta;
	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
			
				if (shaderU == 3) {
					shaderU= 0;
				}
				else {
					shaderU++;
				}
			}
		else if (key == GLFW_KEY_Q ) {
				lightDur -= 1;
				
		}
		else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
			if (material == 4) {
				material = 0;
			}
			else {
				material++;
			}
		}
		else if (key == GLFW_KEY_E ) {
			lightDur += 1;
		}
		else if (key == GLFW_KEY_R & action == GLFW_PRESS) {

			rTheta += 0.1;
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX << " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();



		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);


		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("ks");
		prog->addUniform("ambient");
		prog->addUniform("kd");
		prog->addUniform("light");
		prog->addUniform("shader");
	
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addUniform("MatSpec");
		prog->addUniform("shine");
	
	}

	

	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		vec3 ks = vec3(0.6,0.6,0.6);
		vec3 ambient = vec3(0.3,0.3,0.3);
		vec3 kd = vec3(0.6,0.6,0.6);
		vec3 light = normalize(vec3((-2 + lightDur) ,2 ,2));
		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width / (float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		auto View = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

		// View is identity - for now
		View->pushMatrix();

		// Draw a stack of cubes with indiviudal transforms
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform1i(prog->getUniform("shader"), shaderU);
		glUniform3fv(prog->getUniform("ks"), 1, &ks[0]);
		glUniform3fv(prog->getUniform("ambient"), 1, &ambient[0]);
		glUniform3fv(prog->getUniform("kd"), 1, &kd[0]);
		glUniform3fv(prog->getUniform("light"), 1, &light[0]);
		SetMaterial(material);
		glBindVertexArray(VertexArrayID2);
		// draw the bottom cube with these 'global transforms'
		// draw top cube - aka head
		Model->pushMatrix(); {
			
		
			// play with these options
			Model->translate(vec3(-1, 0, -4));
			Model->rotate(rTheta, vec3(0, 1, 0));
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			glDrawElements(GL_TRIANGLES, triBuf.size() * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);

			/* After computing, do a final normalizing */
		
		}
		Model->popMatrix();

		Model->pushMatrix(); {

			// play with these options
		
			Model->translate(vec3(1, 0, -4));
			Model->rotate(rTheta, vec3(0, 1, 0));
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			glDrawElements(GL_TRIANGLES, triBuf.size() * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0);

			/* After computing, do a final normalizing */

		}
		Model->popMatrix();


		glBindVertexArray(0);
		prog->unbind();

		
		// Pop matrix stacks.
		Projection->popMatrix();
		View->popMatrix();

		// update shoulder angle - animate
	
		
	}
void initGeom()
	{

		glGenVertexArrays(1, &VertexArrayID2);
		glBindVertexArray(VertexArrayID2);

		 //bind this var to 1
		//generate vertex buffer to hand off to OGL		 		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID2);


		//set the current state to focus on our vertex buffer		
		//set the current state to focus on our vertex buffer

		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID2);
		glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);




		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, //  GLint index
			3, // size
			GL_FLOAT, //type
			GL_FALSE, //normalize
			0,  // stride
			(void*)0); // pointer

		glGenBuffers(1, &elementbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, triBuf.size() * sizeof(unsigned int), &triBuf[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);

		glGenBuffers(1, &normals);
		glBindBuffer(GL_ARRAY_BUFFER, normals);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float), &norBuf[0], GL_STATIC_DRAW);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(1, 3,GL_FLOAT, GL_FALSE, 0,  (void*)0); 

		glBindVertexArray(0);



	}

  
void createNormals(vector <float> &norBuf) {
	int indices1, indices2, indices3 = 0;
	vec3 v1, v2, v3, res1, res2, res3, norm;
	for (int i = 0; i < (posBuf.size()/ 3); i++)
	{
		norBuf.push_back(0);
		norBuf.push_back(0);
		norBuf.push_back(0);
	}
	for (int j = 0; j < (triBuf.size() / 3); j++)
	{
		// indicies that connect the triangle
		indices1 = triBuf[3 * j + 0];
		indices2 = triBuf[3 * j + 1];
		indices3 = triBuf[3 * j + 2];

		// vertices that compose of the triangle
		v1 = vec3((posBuf[3 * indices1 + 0]), (posBuf[3 * indices1 + 1]), (posBuf[3 * indices1 + 2]));
		v2 = vec3((posBuf[3 * indices2 + 0]), (posBuf[3 * indices2 + 1]), (posBuf[3 * indices2 + 2]));
		v3 = vec3((posBuf[3 * indices3 + 0]), (posBuf[3 * indices3 + 1]), (posBuf[3 * indices3 + 2]));

		res1 = v3 - v1;
		res2 = v2 - v1;

		res3 = cross(res2, res1);

		norm = normalize(res3);

		// new normal for each vertex

		norBuf[3 * indices1 + 0] += norm.x;
		norBuf[3 * indices1 + 1] += norm.y;
		norBuf[3 * indices1 + 2] += norm.z;

		norBuf[3 * indices2 + 0] += norm.x;
		norBuf[3 * indices2 + 1] += norm.y;
		norBuf[3 * indices2 + 2] += norm.z;

		norBuf[3 * indices3 + 0] += norm.x;
		norBuf[3 * indices3 + 1] += norm.y;
		norBuf[3 * indices3 + 2] += norm.z;

	}
	for (int i = 0; i < norBuf.size() / 3; ++i) {
		vec3 toNorm = glm::vec3(norBuf[3 * i + 0], norBuf[3 * i + 1], norBuf[3 * i + 2]);
		vec3 norm = glm::normalize(toNorm);
		norBuf[3 * i + 0] = norm.x;
		norBuf[3 * i + 1] = norm.y;
		norBuf[3 * i + 2] = norm.z;
	}
	
  }


void SetMaterial(int i) {
	switch (i) {
	case 0:
		glUniform3f(prog->getUniform("MatAmb"), 1, 1,1);
		glUniform3f(prog->getUniform("MatDif"), 1, 1, 1);
		glUniform3f(prog->getUniform("MatSpec"), 1, 1, 1);
		glUniform1f(prog->getUniform("shine"), 60);
		break;
	case 1: // shiny blue plastic
		glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
		glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
		glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
		glUniform1f(prog->getUniform("shine"), 120.0);
		break;
	case 2: // flat grey
		glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
		glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
		glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
		glUniform1f(prog->getUniform("shine"), 4.0);
		break;
	case 3: // brass
		glUniform3f(prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
		glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
		glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
		glUniform1f(prog->getUniform("shine"), 27.9);
		break;
	case 4: // emerald
		glUniform3f(prog->getUniform("MatAmb"), 0.0215, 0.1745, 0.0215);
		glUniform3f(prog->getUniform("MatDif"), 0.07568, 0.61424, 0.07568);
		glUniform3f(prog->getUniform("MatSpec"), 0.633, 0.727811, 0.633);
		glUniform1f(prog->getUniform("shine"), 80);
		break;

	}
 }

};


void resize_obj(std::vector<tinyobj::shape_t> &shapes) {
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
			if (shapes[i].mesh.positions[3 * v + 0] < minX) minX = shapes[i].mesh.positions[3 * v + 0];
			if (shapes[i].mesh.positions[3 * v + 0] > maxX) maxX = shapes[i].mesh.positions[3 * v + 0];

			if (shapes[i].mesh.positions[3 * v + 1] < minY) minY = shapes[i].mesh.positions[3 * v + 1];
			if (shapes[i].mesh.positions[3 * v + 1] > maxY) maxY = shapes[i].mesh.positions[3 * v + 1];

			if (shapes[i].mesh.positions[3 * v + 2] < minZ) minZ = shapes[i].mesh.positions[3 * v + 2];
			if (shapes[i].mesh.positions[3 * v + 2] > maxZ) maxZ = shapes[i].mesh.positions[3 * v + 2];
		}
	}

	//From min and max compute necessary scale and shift for each dimension
	float maxExtent, xExtent, yExtent, zExtent;
	xExtent = maxX - minX;
	yExtent = maxY - minY;
	zExtent = maxZ - minZ;
	if (xExtent >= yExtent && xExtent >= zExtent) {
		maxExtent = xExtent;
	}
	if (yExtent >= xExtent && yExtent >= zExtent) {
		maxExtent = yExtent;
	}
	if (zExtent >= xExtent && zExtent >= yExtent) {
		maxExtent = zExtent;
	}
	scaleX = 2.0 / maxExtent;
	shiftX = minX + (xExtent / 2.0);
	scaleY = 2.0 / maxExtent;
	shiftY = minY + (yExtent / 2.0);
	scaleZ = 2.0 / maxExtent;
	shiftZ = minZ + (zExtent) / 2.0;

	//Go through all verticies shift and scale them
	for (size_t i = 0; i < shapes.size(); i++) {
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			shapes[i].mesh.positions[3 * v + 0] = (shapes[i].mesh.positions[3 * v + 0] - shiftX) * scaleX;
			assert(shapes[i].mesh.positions[3 * v + 0] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 0] <= 1.0 + epsilon);
			shapes[i].mesh.positions[3 * v + 1] = (shapes[i].mesh.positions[3 * v + 1] - shiftY) * scaleY;
			assert(shapes[i].mesh.positions[3 * v + 1] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 1] <= 1.0 + epsilon);
			shapes[i].mesh.positions[3 * v + 2] = (shapes[i].mesh.positions[3 * v + 2] - shiftZ) * scaleZ;
			assert(shapes[i].mesh.positions[3 * v + 2] >= -1.0 - epsilon);
			assert(shapes[i].mesh.positions[3 * v + 2] <= 1.0 + epsilon);
		}
	}
}

void loadShape(string meshName) {
	string errStr;
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
}



int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources"; // Where the resources are loaded from

	
	// OBJ filename
	 // Object.obj 
	

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;
	string shape = argv[1];
	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state
	loadShape("../resources/" + shape);

	application->init(resourceDir);
	application->createNormals(norBuf);
	application->initGeom();

	

	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();
		
		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
