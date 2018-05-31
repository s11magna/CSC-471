/* Lab 5 base code - transforms using local matrix functions
	to be written by students -
	CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "WindowManager.h"

// used for helper in perspective
#include "glm/glm.hpp"

using namespace std;
using namespace glm;

class Matrix
{

public:

	static void printMat(float *A, const char *name = 0)
	{
		// OpenGL uses col-major ordering:
		// [ 0  4  8 12]
		// [ 1  5  9 13]
		// [ 2  6 10 14]
		// [ 3  7 11 15]
		// The (i, j)th element is A[i+4*j].

		if (name)
		{
			printf("%s=[\n", name); 
		}

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				printf("%- 5.2f ", A[i + 4*j]);
			}
			printf("\n");
		}

		if (name)
		{
			printf("];");
		}
		printf("\n");
	}

	static void createIdentityMat(float *M)
	{
		// set all values to zero
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				M[i + 4*j] = 0;
			}
		}

		// overwrite diagonal with 1s
		M[0] = M[5] = M[10] = M[15] = 1;
	}

	static void createTranslateMat(float *T, float x, float y, float z)
	{
		T[0] = 1; // first column
		T[1] = 0;
		T[2] = 0;
		T[3] = 0;
		          // second column
		T[4] = 0;
		T[5] = 1;
		T[6] = 0;
		T[7] = 0;

				// third column
		T[8] = 0;
		T[9] = 0;
		T[10] = 1;
		T[11] = 0;

				// fourth column
		T[12] = x;
		T[13] = y;
		T[14] = z;
		T[15] = 1;
 	}

	static void createScaleMat(float *S, float x, float y, float z)
	{
		S[0] = x; // first column
		S[1] = 0;
		S[2] = 0;
		S[3] = 0;
		// second column
		S[4] = 0;
		S[5] = y;
		S[6] = 0;
		S[7] = 0;

		// third column
		S[8] = 0;
		S[9] = 0;
		S[10] = z;
		S[11] = 0;

		// fourth column
		S[12] = 0;
		S[13] = 0;
		S[14] = 0;
		S[15] = 1;
	}

	static void createRotateMatX(float *R, float radians)
	{
		R[0] = 1; // first column
		R[1] = 0;
		R[2] = 0;
		R[3] = 0;
		// second column
		R[4] = 0;
		R[5] = cos(radians);
		R[6] = -sin(radians);
		R[7] = 0;

		// third column
		R[8] = 0;
		R[9] = sin(radians);
		R[10] = cos(radians);
		R[11] = 0;

		// fourth column
		R[12] = 0;
		R[13] = 0;
		R[14] = 0;
		R[15] = 1;
	}

	static void createRotateMatY(float *R, float radians)
	{
		R[0] = cos(radians); // first column
		R[1] = 0;
		R[2] = sin(radians);
		R[3] = 0;
		// second column
		R[4] = 0;
		R[5] = 1;
		R[6] = 0;
		R[7] = 0;

		// third column
		R[8] = -sin(radians);
		R[9] = 0;
		R[10] = cos(radians);
		R[11] = 0;

		// fourth column
		R[12] = 0;
		R[13] = 0;
		R[14] = 0;
		R[15] = 1;
	}

	static void createRotateMatZ(float *R, float radians)
	{
		R[0] = cos(radians); // first column
		R[1] = -sin(radians);
		R[2] = 0;
		R[3] = 0;
		// second column
		R[4] = sin(radians);
		R[5] = cos(radians);
		R[6] = 0;
		R[7] = 0;

		// third column
		R[8] = 0;
		R[9] = 0;
		R[10] = 1;
		R[11] = 0;

		// fourth column
		R[12] = 0;
		R[13] = 0;
		R[14] = 0;
		R[15] = 1;
	}

	static void multMat(float *C, const float *A, const float *B)
	{
		float c = 0;

		for (int k = 0; k < 4; ++k)
		{
			// Process kth column of C
			for (int i = 0; i < 4; ++i)
			{
				// Process ith row of C.
				// The (i,k)th element of C is the dot product
				// of the ith row of A and kth col of B.
				c = 0;

				// vector dot product
				for (int j = 0; j < 4; ++j)
				{
					c = c + (A[i + 4 * j]) * (B[j + 4 * k]);
				}
				C[i + 4 * k] = c;
			}
		}
	}

	static void createPerspectiveMat(float *m, float fovy, float aspect, float zNear, float zFar)
	{
		// http://www-01.ibm.com/support/knowledgecenter/ssw_aix_61/com.ibm.aix.opengl/doc/openglrf/gluPerspective.htm%23b5c8872587rree
		float f = 1.0f / glm::tan(0.5f * fovy);

		m[ 0] = f / aspect;
		m[ 1] = 0.0f;
		m[ 2] = 0.0f;
		m[ 3] = 0.0f;
		m[ 4] = 0;

		m[ 5] = f;
		m[ 6] = 0.0f;
		m[ 7] = 0.0f;
		m[ 8] = 0.0f;

		m[ 9] = 0.0f;
		m[10] = (zFar + zNear) / (zNear - zFar);
		m[11] = -1.0f;
		m[12] = 0.0f;

		m[13] = 0.0f;
		m[14] = 2.0f * zFar * zNear / (zNear - zFar);
		m[15] = 0.0f;
	}

};

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> prog1;
	std::shared_ptr<Program> prog2;
	std::shared_ptr<Program> prog3;

	// Shape to be used (from obj file)
	shared_ptr<Shape> shape;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
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
		glClearColor(0.12f, 0.34f, 0.56f, 1.0f);

		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		if (! prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->init();
		prog->addUniform("P");
		prog->addUniform("MV");
		prog->addUniform("trans");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");


		// Initialize the GLSL program.
		prog1 = make_shared<Program>();
		prog1->setVerbose(true);
		prog1->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		if (!prog1->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog1->init();
		prog1->addUniform("P");
		prog1->addUniform("MV");
		prog1->addUniform("trans");
		prog1->addAttribute("vertPos");
		prog1->addAttribute("vertNor");

		// Third Block
		prog2 = make_shared<Program>();
		prog2->setVerbose(true);
		prog2->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		if (!prog2->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog2->init();
		prog2->addUniform("P");
		prog2->addUniform("MV");
		prog2->addUniform("trans");
		prog2->addAttribute("vertPos");
		prog2->addAttribute("vertNor");

		// last Block
		prog3 = make_shared<Program>();
		prog3->setVerbose(true);
		prog3->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/simple_frag.glsl");
		if (!prog3->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog3->init();
		prog3->addUniform("P");
		prog3->addUniform("MV");
		prog3->addUniform("trans");
		prog3->addAttribute("vertPos");
		prog3->addAttribute("vertNor");


	}

	void initGeom(const std::string& resourceDirectory)
	{
		// Initialize mesh.
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/cube.obj");
		shape->resize();
		shape->init();
	}

	void render()
	{
		// Local modelview matrix use this for lab 5
		float MV[16] = {0};
		float P[16] = {0};
		float trans[16] = {0};
		float A[16],C[16];
		float D[16], E[16], F[16], temp[16];

		float X[16], Y[16], Z[16];
		float temp1[16], temp2[16], temp3[16];
		float X1[16], Y1[16], Z1[16];
		float rotate[16],temp5[16];
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use the local matrices for lab 5
		float aspect = width/(float)height;
		Matrix::createPerspectiveMat(P, 70.0f, aspect, 0.1f, 100.0f);
		Matrix::createIdentityMat(MV);

		// model view matrix change
		Matrix::createRotateMatY(rotate,0.36);
		
		// First L
		Matrix::createTranslateMat(D, -5.0, 0.0, -7); //x y z
		Matrix::createScaleMat(E, 0.3, 2.5, 0.5);
		Matrix::multMat(trans, D, E);

		prog1->bind();
		glUniformMatrix4fv(prog1->getUniform("P"), 1, GL_FALSE, P);
		glUniformMatrix4fv(prog1->getUniform("MV"), 1, GL_FALSE, rotate);
		glUniformMatrix4fv(prog1->getUniform("trans"), 1, GL_FALSE, trans);
		shape->draw(prog1);
		prog1->unbind();


		//The side ways bloc
		Matrix::createTranslateMat(A, -4.2, 0.5, -7);
		Matrix::createScaleMat(F, 1.4, 0.3, 0.5);
		Matrix::createRotateMatZ(C, 0.585398);

		Matrix::multMat(temp, A,C);
		Matrix::multMat(trans, temp, F);
	    

		 //CAF -- scale then translate then rotate
		// first you multiple scale, translate and rotate

		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, P);
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, rotate);
		glUniformMatrix4fv(prog->getUniform("trans"), 1, GL_FALSE, trans);
		shape->draw(prog);
		prog->unbind();


	

		// Second L
		Matrix::createTranslateMat(X, -3.5, 0, -7);
		Matrix::createScaleMat(Y, 0.3, 2.5, 0.5);
		Matrix::multMat(trans, X, Y);

		prog2->bind();
		glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, P);
		glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, rotate);
		glUniformMatrix4fv(prog2->getUniform("trans"), 1, GL_FALSE, trans);
		shape->draw(prog2);
		prog2->unbind();




		// The Last L

		Matrix::createTranslateMat(X1, -2.0, 0, -7);
		Matrix::createScaleMat(Y1, 0.3, 2.5, 0.5);
		Matrix::multMat(trans, X1, Y1);
		

		prog3->bind();
		glUniformMatrix4fv(prog3->getUniform("P"), 1, GL_FALSE, P);
		glUniformMatrix4fv(prog3->getUniform("MV"), 1, GL_FALSE, rotate);
		glUniformMatrix4fv(prog3->getUniform("trans"), 1, GL_FALSE, trans);

		shape->draw(prog3);
		prog3->unbind();


	}
};

int main(int argc, char **argv)
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
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
