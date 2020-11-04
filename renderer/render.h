#pragma once

#include <vector>

#include "glew/GL/glew.h"	
#include "glm/glm.hpp"		


// Defining our vertex structure
struct vertex
{
	glm::vec4 pos;	// Vertex Pos
	glm::vec4 col;
	glm::vec2 uv;
	glm::vec4 normal;
};

// An object to represent our mesh
struct geometry
{
	GLuint vao, vbo, ibo;		// Buffers
	GLuint size;				// Index count
};

// An object to represent our shader
struct shader
{
	GLuint program;
};

struct texture
{
	GLuint handle;
	unsigned int width, height, channels;
};

struct light
{
	glm::vec3 direction;	// Should be normalized-
	glm::vec3 color;
};

// Functions to make and unmake the above types
geometry loadGeometry(const char * filePath);
geometry makeGeometry(vertex *verts, size_t vertCount,
	unsigned int *indices, size_t indxCount);
void freeGeometry(geometry &geo);

texture loadTexture(const char *filePath);
texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char *pixels);
void freeTexture(texture &tex);

shader loadShader(const char *vertPath, const char *fragPath);
shader makeShader(const char *vertSource, const char *fragSource);
void freeShader(shader &shad);

void draw(const shader &shad, const geometry &geo);

void setUniform(const shader& shad, GLuint location, const glm::mat4 &value);
void setUniform(const shader& shad, GLuint location, const texture &tex, int textureSlot);
void setUniform(const shader& shad, GLuint location, float value);
void setUniform(const shader& shad, GLuint location, const glm::vec3 &value);