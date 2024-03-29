#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Common.h"
#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"
#include "Primitives.h"

// covers range of ascii codes
bool keys[1024];
// Window dimensions
const GLint WIDTH = 1920, HEIGHT = 1080;
// Bogenma� (f�r Winkel)
const GLfloat toRadians{ 3.14159265f / 180.0f };

Window* mainWindow;
Camera* camera;
Primitives primitives;

std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

Texture* brickTexture;
Texture* dirtTexture;
Texture* metalTexture;
Texture* cloudTexture;
Texture* rustTexture;
Texture* goldTexture;
Texture* concreteTexture;
Texture* grayTexture;

// last argument is to alter light intesity
Light ambientLight = Light(1.0f, 1.0f, 1.0f, 1.0f);

std::string dirt = "Textures/dirt.png";
const char* d = dirt.c_str();

std::string brick = "Textures/brick.png";
const char* b = brick.c_str();

std::string metal = "Textures/metal.png";
const char* m = metal.c_str();

std::string cloud = "Textures/cloud.png";
const char* c = cloud.c_str();

std::string rust = "Textures/rust.png";
const char* r = rust.c_str();

std::string gold = "Textures/gold.png";
const char* g = gold.c_str();

std::string concrete = "Textures/concrete.png";
const char* con = concrete.c_str();

std::string gray = "Textures/gray.png";
const char* gr = gray.c_str();

GLfloat deltaTime{ 0.0f };
GLfloat lastFrame{ 0.0f };

// Variables for offset
GLboolean direction = true;
GLfloat offset = -3.5f;
GLfloat maxOffset = -2.8f;
GLfloat minOffset = -3.0f;
GLfloat increment = 0.008f;
//

// Creating Vertex Shader itself
/*static*/ const char* VertexShader = "Shaders/shader.vertex";

// Creating Fragment Shader itself
/*static*/ const char* FragmentShader = "Shaders/shader.fragment";

void CreateObjects()
{
	// Triangle
	Mesh* obj0 = new Mesh();
	obj0->CreateMesh(primitives.verticesTriangle, primitives.indicesTriangle, (4 * 5), (4 * 3));
	meshList.push_back(obj0);

	// Square
	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(primitives.verticesSquare, primitives.indicesSquare, (8 * 5), (12 * 3));
	meshList.push_back(obj1);

	// Pyramid
	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(primitives.verticesPyramid, primitives.indicesPyramid, (5 * 5), (6 * 3));
	meshList.push_back(obj2);

	// Floor
	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(primitives.verticesFloor, primitives.indicesFloor, (4 * 5), (2 * 3));
	meshList.push_back(obj3);

	// House
	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(primitives.verticesHouse, primitives.indicesHouse, (8 * 5), (12 * 3));
	meshList.push_back(obj4);

	// Walls
	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(primitives.verticesSquare, primitives.indicesSquare, (8 * 5), (12 * 3));
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(primitives.verticesSquare, primitives.indicesSquare, (8 * 5), (12 * 3));
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(primitives.verticesSquare, primitives.indicesSquare, (8 * 5), (12 * 3));
	meshList.push_back(obj7);

	Mesh* obj8 = new Mesh();
	obj8->CreateMesh(primitives.verticesSquare, primitives.indicesSquare, (8 * 5), (12 * 3));
	meshList.push_back(obj8);

	Mesh* obj9 = new Mesh();
	obj9->CreateMesh(primitives.verticesSquare, primitives.indicesSquare, (8 * 5), (12 * 3));
	meshList.push_back(obj9);

	Mesh* obj10 = new Mesh();
	obj10->CreateMesh(primitives.verticesSquare, primitives.indicesSquare, (8 * 5), (12 * 3));
	meshList.push_back(obj10);

	Mesh* obj11 = new Mesh();
	obj11->CreateMesh(primitives.verticesSquare, primitives.indicesSquare, (8 * 5), (12 * 3));
	meshList.push_back(obj11);
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateShaderCode(VertexShader, FragmentShader);
	shaderList.push_back(shader1);
}

void CalculateOffset()
{
	if (direction)
		offset += increment;
	else
		offset -= increment;

	if (offset >= maxOffset)
		direction = !direction;
	else if (offset <= minOffset)
		direction = true;
}

int main()
{
	mainWindow = new Window(1060, 600);
	mainWindow->InitialiseWindow();
	// Camera speed, Mouse sensitivity
	camera = new Camera(12.0f, 1.0f);

	dirtTexture = new Texture(d);
	dirtTexture->LoadTextureA();
	brickTexture = new Texture(b);
	brickTexture->LoadTextureA();
	metalTexture = new Texture(m);
	metalTexture->LoadTextureA();
	cloudTexture = new Texture(c);
	cloudTexture->LoadTextureA();
	rustTexture = new Texture(r);
	rustTexture->LoadTextureA();
	goldTexture = new Texture(g);
	goldTexture->LoadTextureA();
	concreteTexture = new Texture(con);
	concreteTexture->LoadTextureA();
	grayTexture = new Texture(gr);
	grayTexture->LoadTextureA();

	CreateObjects();
	CreateShaders();

	GLfloat angle{ 0.0f };
	GLfloat itemAngle{ 0.0f };
	GLuint uniformProjection{ 0 };
	GLuint uniformModel{ 0 };
	GLuint uniformView{ 0 };	
	GLuint uniformAmbientIntensity{ 0 };
	GLuint uniformAmbientColor{ 0 };

	// Projection Matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<GLfloat>(mainWindow->GetBufferWidth()) / static_cast<GLfloat>(mainWindow->GetBufferHeight()), 0.1f, 100.0f);

	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow->mainWindow))
	{
		// Delta Time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		camera->ProcessKeyboardInput(mainWindow->GetKeys(), deltaTime);
		camera->UpdateMouse(mainWindow->PreventXoffsetMoving(), mainWindow->PreventYoffsetMoving(), deltaTime);

		// Clear window
		glClearColor(0.8f, 1.0f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int i{ 0 }; i < shaderList.size(); ++i)
		{
			shaderList[0]->UseShaderProgram();
			uniformModel = shaderList[0]->GetUniformModel();
			uniformProjection = shaderList[0]->GetUniformProjection();
			uniformView = shaderList[0]->GetUniformView();
			uniformAmbientColor = shaderList[0]->GetAmbientColorLocation();
			uniformAmbientIntensity = shaderList[0]->GetAmbientIntensityLocation();
		}
		ambientLight.CreateLight(uniformAmbientIntensity, uniformAmbientColor);

		// initialised 4 x 4 Model Matrix for each object
		glm::mat4 triangle{ 1.0f };
		glm::mat4 square{ 1.0f };
		glm::mat4 pyramid{ 1.0f };
		glm::mat4 pyramidStable{ 1.0f };
		glm::mat4 plane{ 1.0f };
		glm::mat4 house{ 1.0f };
		glm::mat4 wall1{ 1.0f };
		glm::mat4 wall2{ 1.0f };
		glm::mat4 wall3{ 1.0f };
		glm::mat4 wall4{ 1.0f };
		glm::mat4 wall5{ 1.0f };
		glm::mat4 top{ 1.0f };
		glm::mat4 item{ 1.0f };

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera->ViewMatrix()));

		CalculateOffset();
		angle += 0.3f;
		itemAngle += 1.8f;

		// Triangle
		{
			triangle = glm::translate(triangle, glm::vec3(-2.5f, 0.0f, -4.0f));
			triangle = glm::rotate(triangle, 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
			triangle = glm::scale(triangle, glm::vec3(0.3f, 0.3f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(triangle));
			metalTexture->UseTexture();
			meshList[0]->RenderMesh();
		}

		// Square
		{
			square = glm::translate(square, glm::vec3(2.5f, 0.0f, -4.0f));
			square = glm::rotate(square, angle * toRadians, glm::vec3(-1.0f, -1.0f, 1.0f));
			square = glm::scale(square, glm::vec3(0.3f, 0.3f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(square));
			brickTexture->UseTexture();
			meshList[1]->RenderMesh();
		}

		// Pyramid
		{
			pyramid = glm::translate(pyramid, glm::vec3(0.0f, 2.0f, -4.0f));
			pyramid = glm::rotate(pyramid, angle * toRadians, glm::vec3(-1.0f, -1.0f, -1.0f));
			pyramid = glm::scale(pyramid, glm::vec3(0.5f, 0.5f, 1.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(pyramid));
			grayTexture->UseTexture();
			meshList[2]->RenderMesh();
		}

		// PyramidStable
		{
			pyramidStable = glm::translate(pyramidStable, glm::vec3(0.0f, -1.0f, -22.0f));
			pyramidStable = glm::rotate(pyramidStable, 180.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			pyramidStable = glm::scale(pyramidStable, glm::vec3(3.5f, 3.5f, 6.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(pyramidStable));
			concreteTexture->UseTexture();
			meshList[2]->RenderMesh();
		}

		// Plane
		{
			plane = glm::translate(plane, glm::vec3(0.0f, -3.5f, -10.0f));
			plane = glm::scale(plane, glm::vec3(3.0f, 0.05f, 3.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(plane));
			dirtTexture->UseTexture();
			meshList[3]->RenderMesh();
		}

		// House
		{
			house = glm::translate(house, glm::vec3(11.0f, -2.5f, -13.0f));
			house = glm::scale(house, glm::vec3(2.5f, 1.2f, 4.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(house));
			rustTexture->UseTexture();
			meshList[4]->RenderMesh();
		}

		// Wall 1
		{
			wall1 = glm::translate(wall1, glm::vec3(-15.0f, -2.5f, -15.0f));
			wall1 = glm::rotate(wall1, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			wall1 = glm::scale(wall1, glm::vec3(2.0f, 1.0f, 0.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wall1));
			metalTexture->UseTexture();
			meshList[5]->RenderMesh();
		}

		// Wall 2
		{
			wall2 = glm::translate(wall2, glm::vec3(-13.0f, -2.5f, -13.0f));
			wall2 = glm::rotate(wall2, 0.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			wall2 = glm::scale(wall2, glm::vec3(2.0f, 1.0f, 0.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wall2));
			metalTexture->UseTexture();
			meshList[6]->RenderMesh();
		}

		// Wall 3
		{
			wall3 = glm::translate(wall3, glm::vec3(-11.5f, -2.5f, -17.0f));
			wall3 = glm::rotate(wall3, 0.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			wall3 = glm::scale(wall3, glm::vec3(3.5f, 1.0f, 0.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wall3));
			metalTexture->UseTexture();
			meshList[6]->RenderMesh();
		}

		// Wall 4
		{
			wall4 = glm::translate(wall4, glm::vec3(-11.0f, -2.5f, -9.5f));
			wall4 = glm::rotate(wall4, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			wall4 = glm::scale(wall4, glm::vec3(3.5f, 1.0f, 0.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wall4));
			metalTexture->UseTexture();
			meshList[7]->RenderMesh();
		}

		// Wall 5
		{
			wall5 = glm::translate(wall5, glm::vec3(-8.0f, -2.5f, -11.5f));
			wall5 = glm::rotate(wall5, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			wall5 = glm::scale(wall5, glm::vec3(5.5f, 1.0f, 0.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(wall5));
			metalTexture->UseTexture();
			meshList[8]->RenderMesh();
		}

		// Wall 6
		{
			top = glm::translate(top, glm::vec3(-11.1f, -1.5f, -11.5f));
			top = glm::rotate(top, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			top = glm::scale(top, glm::vec3(6.0f, 0.1f, 3.5f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(top));
			metalTexture->UseTexture();
			meshList[9]->RenderMesh();
		}

		// Gold Item
		{
			item = glm::translate(item, glm::vec3(-13.0f, offset, -15.0f));
			item = glm::rotate(item, itemAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			item = glm::scale(item, glm::vec3(0.1f, 0.2f, 0.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(item));
			goldTexture->UseTexture();
			meshList[10]->RenderMesh();
		}

		glUseProgram(0);
		// Swap front and back buffers
		glfwSwapBuffers(mainWindow->mainWindow);
		// poll events (keys pressed and / or released | mouse moved or not)
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}