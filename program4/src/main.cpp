/*
Base code
Currently will make 2 FBOs and textures (only uses one in base code)
and writes out frame as a .png (Texture_output.png)

Winter 2017 - ZJW (Piddington texture write)
2017 integration with pitch and yaw camera lab (set up for texture mapping lab)
*/

#include <iostream>
#include <glad/glad.h>
#include "camera.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "WindowManager.h"
#include "GLTextureWriter.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

camera mycam;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> prog1;
	std::shared_ptr<Program> texProg;

	// Shape to be used (from obj file)
	shared_ptr<Shape> shape;
	shared_ptr<Shape> shape1;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	//geometry for texture render
	GLuint quad_VertexArrayID;
	GLuint quad_vertexbuffer;

	//Plane VertexArrayID
	GLuint VertexArrayID2;
	
	//Plane VertexBufferID
	GLuint VertexBufferID2;

	//Plane Indicies Array
	GLuint elementbuffer;

	// indicies of plane
	int parts = 0;
	//reference to texture FBO
	GLuint frameBuf[2];
	GLuint texBuf[2];
	GLuint depthBuf;
	vec2 posVec;

	bool FirstTime = true;
	bool Moving = false;
	int gMat = 0;
	float yaw, pitch;
	float cTheta = 0;
	bool mouseDown = false;
	double posX, posY;
	double prevX, prevY;
	float ypos ,foward;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			gMat = (gMat + 1) % 4;
		}
		else if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
	
			ypos += 0.925;
		
		}
		else if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			
			ypos -= 0.925;
		}
		else if (key == GLFW_KEY_W && action == GLFW_PRESS) {

			foward += 1.25;
		
		}
		else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			foward -= 1.25;
		
		}

	}



	void renderPlane(shared_ptr <MatrixStack> View)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		float aspect = width / (float)height;
		glfwGetCursorPos(windowManager->getHandle(), &posX, &posY);
		// Create the matrix stacks
		auto Projection = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();
		// Apply perspective projection.

		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);
		
	//	View->pushMatrix();
	
		prog1->bind();
		glUniformMatrix4fv(prog1->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog1->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		// cube object
		Model->pushMatrix(); {
			Model->translate(vec3(0, -1, -2.5));
			Model->rotate(radians(180.0f), vec3(1, 0, 0));
			Model->scale(vec3(100, .01, 100));
			glUniformMatrix4fv(prog1->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		}
		Model->popMatrix();
		
		shape1->draw(prog1);

		prog1->unbind();
		Projection->popMatrix();


		
	}



	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
	{
		cTheta += (float) deltaX;
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			mouseDown = true;
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX << " Pos Y " << posY << endl;
			Moving = true;
		}

		if (action == GLFW_RELEASE)
		{
			Moving = false;
			mouseDown = false;
		}
	}

	void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
		
		if (Moving)
		{
			float deltaX = xpos - prevX;
			float deltaY = ypos - prevY;	
			if (abs(deltaX) < 15) {
				yaw -= deltaX * 0.01;
			}
			if (abs(deltaX) < 15) {
				
					pitch += deltaY * 0.01;
				
			}
			posVec = vec2(deltaX, deltaY);
		}

	
		prevX = xpos;
		prevY = ypos;
	
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		cTheta = 0;
		// Set background color.
		glClearColor(0.5f, .5f, .5f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(
			resourceDirectory + "/simple_vert.glsl",
			resourceDirectory + "/simple_frag.glsl");
		if (! prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addUniform("parts");
		prog->addUniform("MatAmb");
		prog->addUniform("MatDif");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");



		// Initialize the GLSL program.
		prog1 = make_shared<Program>();
		prog1->setVerbose(true);
		prog1->setShaderNames(
			resourceDirectory + "/fancy_vert33.glsl",
			resourceDirectory + "/fancy_frag33.glsl");
		if (!prog1->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog1->addUniform("P");
		prog1->addUniform("V");
		prog1->addUniform("M");
		prog1->addAttribute("vertPos");
		prog1->addAttribute("vertNor");


		//create two frame buffer objects to toggle between
		glGenFramebuffers(2, frameBuf);
		glGenTextures(2, texBuf);
		glGenRenderbuffers(1, &depthBuf);
		createFBO(frameBuf[0], texBuf[0]);

		//set up depth necessary as rendering a mesh that needs depth test
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

		//more FBO set up
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);

		//create another FBO so we can swap back and forth
		createFBO(frameBuf[1], texBuf[1]);
		//this one doesn't need depth

		//set up the shaders to blur the FBO just a placeholder pass thru now
		//next lab modify and possibly add other shaders to complete blur
		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(
			resourceDirectory + "/pass_vert.glsl",
			resourceDirectory + "/tex_fragH.glsl");
		if (! texProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		texProg->addUniform("texBuf");
		texProg->addUniform("posVec");
		texProg->addAttribute("vertPos");
		texProg->addUniform("dir");
	 }

	void initGeom(const std::string& resourceDirectory)
	{
		// Initialize the obj mesh VBOs etc
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/Nefertiti-10K.obj");
		shape->resize();
		shape->init();

		shape1 = make_shared<Shape>();
		shape1->loadMesh(resourceDirectory + "/cube.obj");
		shape1->resize();
		shape1->init();

		//Initialize the geometry to render a quad to the screen
		//initQuad();
	
	}
	

	/**** geometry set up for a quad *****/
	void initQuad()
	{
		//now set up a simple quad for rendering FBO
		glGenVertexArrays(1, &quad_VertexArrayID);
		glBindVertexArray(quad_VertexArrayID);

		static const GLfloat g_quad_vertex_buffer_data[] =
		{
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,
		};

		glGenBuffers(1, &quad_vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
		glBindVertexArray(0);
	}

	/* Helper function to create the framebuffer object and
		associated texture to write to */

	void renderScene(shared_ptr <MatrixStack> View) {
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		if (Moving)
		{
			//set up to render to buffer
			//glBindFramebuffer(GL_FRAMEBUFFER, frameBuf[0]);
		}
		else
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Leave this code to just draw the meshes alone */
		float aspect = width / (float)height;
		glfwGetCursorPos(windowManager->getHandle(), &posX, &posY);
		// Create the matrix stacks
		auto Projection = make_shared<MatrixStack>();
		auto Model = make_shared<MatrixStack>();



		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);
		// Identity view - for now

		renderPlane(View);
		//Draw our scene - two meshes - right now to a texture
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));

		// globl transforms for 'camera' (you will fix this now!)
		Model->pushMatrix();
		Model->loadIdentity();

		float tx, tz, theta = 0;

		for (int i = 0; i < 10; i++)
		{
			parts = 0;
			tx = (7.5f) * sin(theta);
			tz = (4.8f) * cos(theta);
			/* draw left mesh */
			Model->pushMatrix();
			Model->translate(vec3(-tx, 0.f, -tz - (i + 3 % 2)));
			Model->rotate(6.28f + theta, vec3(0, 1, 0));
			Model->rotate(radians(-90.f), vec3(1, 0, 0));
			SetMaterial(i % 4);
			glUniform1i(prog->getUniform("parts"), parts);
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			shape->draw(prog);
			Model->popMatrix();
			theta += 6.28f / 10.f;
		}
		SetMaterial(3);
		robot(Model,2,0.46,-1.5);
		SetMaterial(2);
		robot(Model, 3, 0.46, -5.5);
		SetMaterial(0);
		robot(Model, -3, 0.46, -3.2);
		SetMaterial(1);
		robot(Model, -8, 0.46, -6.5);
		SetMaterial(2);
		robot(Model, -9, 0.46, -7.5);
		SetMaterial(0);
		robot(Model, 7, 0.46, -5.2);
		SetMaterial(3);
		robot(Model, -2, 0.46, - 9);
		SetMaterial(1);
		robot(Model, 8, 0.46, -4);
		SetMaterial(2);
		robot(Model, -9, 0.46, -3);
		SetMaterial(0);
		robot(Model, 7, 0.46, -2);

		prog->unbind();
	
		Projection->popMatrix();


	}
	void createFBO(GLuint& fb, GLuint& tex)
	{
		//initialize FBO
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

		//set up framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		//set up texture
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			cout << "Error setting up frame buffer - exiting" << endl;
			exit(0);
		}
	}

	// To complete image processing on the specificed texture
	// Right now just draws large quad to the screen that is texture mapped
	// with the prior scene image - next lab we will process
	void ProcessImage(GLuint inTex)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, inTex);

		// example applying of 'drawing' the FBO texture - change shaders
		texProg->bind();
		glUniform1i(texProg->getUniform("texBuf"), 0);
		glUniform2f(texProg->getUniform("posVec"), posVec.x,posVec.y);
		glUniform2f(texProg->getUniform("dir"), -1, 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);
		texProg->unbind();
	}
	void robot(shared_ptr <MatrixStack> Model,float x,float y,float z) {

		const static float shoulder_x = 1.0;
		const static float shoulder_disp = 1;
		auto extractSign = [](int a) { return a > 0 ? 1 : -1; };
		
		const static float left_shoulder_disp = -1.0;

		float upperarm_rot = 1.4*sin(glfwGetTime());
		if (abs(upperarm_rot) > 1)
		{
			upperarm_rot = extractSign(upperarm_rot);
		}

		float forearm_rot = 0.5*sin(glfwGetTime()) + 0.7;
		float hand_rot = 0.1*sin(glfwGetTime());

		Model->pushMatrix();
		{
			Model->loadIdentity();
		
			Model->translate(vec3(x, y, z));
			Model->scale(vec3(0.4, 0.4, 0.4));
			glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
			shape1->draw(prog);

			Model->pushMatrix();
			{
				Model->translate(vec3(0, -1.2, 0));

				Model->pushMatrix();
				{
					Model->translate(vec3(0.9, -1.5, 0));
					Model->rotate(1.8, vec3(0, 0, 1));
					Model->scale(vec3(0.95, 0.25, 0.25));
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					shape1->draw(prog);

				}
				Model->popMatrix();

				Model->pushMatrix();
				{
					Model->translate(vec3(-0.9, -1.5, 0));
					Model->rotate(1.4, vec3(0, 0, 1));
					Model->scale(vec3(0.95, 0.25, 0.25));
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					shape1->draw(prog);
				}
				Model->popMatrix();

				Model->scale(1.2);
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				shape1->draw(prog);
			}
			Model->popMatrix();


		
			Model->pushMatrix();
			{
		
				Model->translate(vec3(shoulder_disp - 0.1, shoulder_disp, 0));
				Model->rotate(upperarm_rot, vec3(0, 0, 1));
				Model->translate(vec3(shoulder_x, 0, 0));

				// Forearm
				Model->pushMatrix();
				{
					Model->translate(vec3(0.9, 0, 0));
					Model->rotate(forearm_rot, vec3(0, 0, 1));
					Model->translate(vec3(0.4, 0, 0));

					// Hand
					Model->pushMatrix();
					{
						Model->translate(vec3(0.8, 0.0, 0));
						Model->rotate(hand_rot, vec3(0, 0, 1));
						Model->scale(0.25);
						glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
						shape1->draw(prog);
					}
					Model->popMatrix();

					Model->scale(vec3(0.5, 0.25, 0.25));
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					shape1->draw(prog);
				}
				Model->popMatrix();

				// non-uniform scale
				Model->scale(vec3(0.95, 0.25, 0.25));
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				shape1->draw(prog);
			}
			Model->popMatrix();

			// Left arm
			Model->pushMatrix();
			{
				// place at shoulder
				Model->translate(vec3(left_shoulder_disp, -left_shoulder_disp + 0.1, 0));
				// rotate shoulder joint
				Model->rotate(1.15, vec3(0, 0, 1));
				// move to shoulder joint
				Model->translate(vec3(left_shoulder_disp, 0, 0));

				// Forearm
				Model->pushMatrix();
				{
					Model->translate(vec3(-0.9, 0, 0));
					Model->rotate(0.3, vec3(0, 0, 1));
					//Model->rotate(0.0, vec3(0, 0, 1));
					Model->translate(vec3(-0.4, 0, 0));

					// Hand
					Model->pushMatrix();
					{
						Model->translate(vec3(-0.8, 0.0, 0));
						Model->rotate(0.2, vec3(0, 0, 1));
						Model->scale(0.25);
						glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
						shape1->draw(prog);
					}
					Model->popMatrix();

					Model->scale(vec3(0.5, 0.25, 0.25));
					glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
					shape1->draw(prog);
				}
				Model->popMatrix();

				Model->scale(vec3(0.95, 0.25, 0.25));
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				shape1->draw(prog);
			}
			Model->popMatrix();

		
			Model->pushMatrix();
			{
				Model->loadIdentity();
				// play with these options
				Model->translate(vec3(x, 0.5 + y, z));
				Model->rotate(0.5, vec3(0, 10, 0));
				Model->scale(vec3(0.29, 0.29, 0.29));
				glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
				shape1->draw(prog);
			}
			Model->popMatrix();
		}
		Model->popMatrix();
		
	}
	void render()
	{
			
		auto View = make_shared<MatrixStack>();
		View->pushMatrix();
		View->translate(vec3(ypos, 0.5, foward));
		View->lookAt(vec3(0, 0, 0),
			vec3(cos(pitch)*cos(yaw), sin(pitch), cos(pitch)*sin(yaw)),// look at
			vec3(0, 1, 0) //view
		);
		
		renderScene(View);
		View->popMatrix();
	}
	
	// helper function to set materials for shading
	void SetMaterial(int i)
	{
		switch (i)
		{
		case 0: //shiny blue plastic
			glUniform3f(prog->getUniform("MatAmb"), 0.02f, 0.04f, 0.2f);
			glUniform3f(prog->getUniform("MatDif"), 0.0f, 0.16f, 0.9f);
			break;
		case 1: // flat grey
			glUniform3f(prog->getUniform("MatAmb"), 0.13f, 0.13f, 0.14f);
			glUniform3f(prog->getUniform("MatDif"), 0.3f, 0.3f, 0.4f);
			break;
		case 2: //brass
			glUniform3f(prog->getUniform("MatAmb"), 0.3294f, 0.2235f, 0.02745f);
			glUniform3f(prog->getUniform("MatDif"), 0.7804f, 0.5686f, 0.11373f);
			break;
		 case 3: //copper
			glUniform3f(prog->getUniform("MatAmb"), 0.1913f, 0.0735f, 0.0225f);
			glUniform3f(prog->getUniform("MatDif"), 0.7038f, 0.27048f, 0.0828f);
			break;
		}
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
	windowManager->init(512, 512);
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
