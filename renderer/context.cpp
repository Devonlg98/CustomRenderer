#include "context.h"		// Header thats being implemented

// System Libraries
#include <iostream>			// cout
#include <cstdio>			// printf
#include <cassert>			// assert

// Project libraries
#include "glew/GL/glew.h"	// Always before glfw.
#include "glfw/glfw3.h"	

void APIENTRY errorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::cerr << message << std::endl;
	fprintf(stderr, message);
	fprintf(stderr, message);
}

bool context::init(int width, int height, const char *title)
{
	// Initialize glfw
	int glfwStatus = glfwInit();
	if (glfwStatus == GLFW_FALSE)
	{
		printf("An error occured with GLFW.");
		return false;
	}
	// assert based error checking (will crash program)
	assert(glfwStatus != GLFW_FALSE && "An error occured with GLFW.");

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	glfwMakeContextCurrent(window);

	// Initialize glew
	int glewStatus = glewInit();
	if (glewStatus != GLEW_OK)
	{
		printf("An error occured with GLEW.");
		return false;
	}

	// Print out diagonostics
	// GL_VERSION
	// GL_RENDERER
	// GL_VENDOR
	// GL_SHADING_LANGUAGE_VERSION

	printf("OpenGL Version: %s\n", (const char*)glGetString(GL_VERSION));
	printf("Renderer: %s\n", (const char*)glGetString(GL_RENDERER));
	printf("Vendor: %s\n", (const char*)glGetString(GL_VENDOR));
	printf("GLSL: %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	// add in error checking logic
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(errorCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, true);

	// Set up some good defaults
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	// Return true if everything's OK
	return true;
}

void context::tick()
{
	// Update glfw
	glfwPollEvents();				// Update events, inputs, etc.
	glfwSwapBuffers(window);		// Update window (the one in Windows)

}

void context::term()
{
	// clean up our window.
	glfwDestroyWindow(window);
	window = nullptr;

	// Clean up GLFW (which also terminates GLEW)
	glfwTerminate();
}

void context::clear()
{
	// Clear based on color?
	// Clears screen and replaces it w/ color
	// GIven by the glClearColor function call
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool context::shouldClose() const
{
	return glfwWindowShouldClose(window);
}

float context::time() const
{
	return (float)glfwGetTime();
}

void context::resetTime(float resetValue = 0.0)
{
	glfwSetTime(resetValue);
}
