#include "context.h"	// Context
#include "render.h"		// Vertex, geometry, shader

#include "glm/ext.hpp"

#include <limits>

int main()
{
	context game;
	game.init(512, 512, "Zavala");

	// Vertices
	vertex triVerts[] =
	{
		{ { -.5f, -.5f, 0, 1 }, { 0.0f, 0.0f, 1.0f, 1.0f }, {0,   0}, { 0.0f, 0.0f, -1.0f, 0.0f } }, // bottom-left
		{ {  .5f, -.5f, 0, 1 }, { 0.0f, 1.0f, 0.0f, 1.0f }, {1,   0}, { 0.0f, 0.0f, -1.0f, 0.0f } }, // bottom-right
		{ {   0.f, .5f, 0, 1 }, { 1.0f, 0.0f, 0.0f, 1.0f }, {0.5, 1}, { 0.0f, 0.0f, -1.0f, 0.0f } }  // top-center

	};

	vertex quadVerts[] =
	{
		{ { -1.f, -1.f, 0, 1 }, { 1.0f, 1.0f, 1.0f, 1.0f }, {0, 0}, { 0.0f, 0.0f, -1.0f, 0.0f } }, // bottom-left
		{ {   1.f,-1.f, 0, 1 }, { 1.0f, 1.0f, 1.0f, 1.0f }, {1, 0}, { 0.0f, 0.0f, -1.0f, 0.0f } }, // bottom-right
		{ {  -1.f, 1.f, 0, 1 }, { 1.0f, 1.0f, 1.0f, 1.0f }, {0, 1}, { 0.0f, 0.0f, -1.0f, 0.0f } }, // top-left
		{ {   1.f, 1.f, 0, 1 }, { 1.0f, 1.0f, 1.0f, 1.0f }, {1, 1}, { 0.0f, 0.0f, -1.0f, 0.0f } }  // top-right

	};

	// Indices
	unsigned int triIndices[] = { 2,0,1 };
	unsigned int quadIndices[] = { 3, 2, 0, 0, 1, 3 };

	// Make the geometry
	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);
	geometry quad = makeGeometry(quadVerts, 4, quadIndices, 6);
	geometry triObj = loadGeometry("res\\tri.obj");
	geometry cubeObj = loadGeometry("res\\cube.obj");
	geometry vaseObj = loadGeometry("res\\SimpleVase.obj");


	// Load the textures
	texture zavala = loadTexture("res\\zavala.png");
	//texture zavala = loadTexture("res\\terry.png");

	// Make the shaders
	shader basicShader = loadShader("res\\basic.vert", "res\\basic.frag");
	shader mvpShader = loadShader("res\\mvp.vert", "res\\basic.frag");
	shader lightShader = loadShader("res\\light.vert", "res\\light.frag");

	//lighting
	light sun = { {-1, 0, 0}, {1,1,1} };

	//const char* basicVertShader =
	//	"#version 410\n"
	//	"layout (location = 0) in vec4 position;\n"
	//	"out vec4 vColor;\n"
	//	"void main()\n"
	//	"{\n"
	//	"	gl_Position = position;\n" 
	//	"	vColor = position;\n"
	//	"}";

	//// Source for fragment shader
	//const char* basicFragShader =
	//	"#version 410\n"
	//	"in vec4 vColor\n;"
	//	"out vec4 outputColor;\n"
	//	"void main() { outputColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); }";

	//const char* mvpVertShader =
	//	"#version 410\n"
	//	"layout (location = 0) in vec4 position;\n"
	//	"layout (location = 0) uniform mat4 proj;\n"
	//	"layout (location = 1) uniform mat4 view;\n"
	//	"layout (location = 2) uniform mat4 model;\n"
	//	"out vec4 vColor;\n"
	//	"void main()\n"
	//	"{\n"
	//	"	gl_Position = proj * view * model * position;\n"
	//	"	vcolor = position;\n"
	//	"};";

	//// Make the shader
	//shader basicShader = makeShader(basicVertShader, basicFragShader);
	//shader mvpShader = makeShader(mvpVertShader, basicFragShader);

	// set up matrices
	glm::mat4 camProj = glm::perspective(glm::radians(80.0f), 512.0f / 512.0f, 0.1f, 100.0f);
	glm::mat4 camView = glm::lookAt(glm::vec3(1, 1, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 triModel = glm::identity<glm::mat4>();

	setUniform(lightShader, 0, camProj);
	setUniform(lightShader, 1, camView);
	setUniform(lightShader, 2, triModel);

	setUniform(lightShader, 3, zavala, 0);

	setUniform(lightShader, 5, {0.1f, 0.1f, 0.1f });
	setUniform(lightShader, 6, sun.color);
	setUniform(lightShader, 7, sun.direction);

	while (!game.shouldClose())
	{
		game.tick();
		// Implement game logic here

		triModel = glm::rotate(triModel, glm::radians(1.0f), glm::vec3(0, 1, 0));

		game.clear();

		// Implement render logic here
		setUniform(lightShader, 2, triModel);
		setUniform(lightShader, 4, game.time());

		draw(lightShader, cubeObj);
		//draw(mvpShader, triangle);
		//draw(basicShader, triangle);

	}

	freeGeometry(triangle);
	freeGeometry(quad);
	freeGeometry(triObj);
	freeGeometry(cubeObj);

	freeShader(basicShader);
	freeShader(mvpShader);



	game.term();
	return 0;
}