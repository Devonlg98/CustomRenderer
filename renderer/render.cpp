#include "render.h"

#include <vector>   // std::vector
#include <cassert>  // assert
#include <cstddef>  // c-style function calls like fprintf
#include <string>   // std::string, std::getline
#include <fstream>  // fstream

#include "glm/ext.hpp"  // glm::value_ptr(v)
#include "tinyobjloader/tiny_obj_loader.h"
#include "stb/stb_image.h"


geometry loadGeometry(const char* filePath)
{
	tinyobj::attrib_t vertexAttributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string error;

	bool success = tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, &error, filePath);

	if (!success)
	{
		fprintf(stderr, error.c_str());
		return {}; 
	}
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;

	size_t offset = 0;
	for (size_t i = 0; i < shapes[0].mesh.num_face_vertices.size(); ++i)
	{
		unsigned char faceVertices = shapes[0].mesh.num_face_vertices[i];

		assert(faceVertices == 3 && "unsupported number of face vertices");

		// iterating over the vertex data to fit our own vertex format
		for (unsigned char j = 0; j < faceVertices; ++j)
		{
			tinyobj::index_t idx = shapes[0].mesh.indices[offset + j];

			// position
			tinyobj::real_t posX = vertexAttributes.vertices[3 * idx.vertex_index + 0];
			tinyobj::real_t posY = vertexAttributes.vertices[3 * idx.vertex_index + 1];
			tinyobj::real_t posZ = vertexAttributes.vertices[3 * idx.vertex_index + 2];

			// vertex colors (hard-coded for now) -- needs update to latest tObj
			tinyobj::real_t colR = 1.0f;
			tinyobj::real_t colG = 1.0f;
			tinyobj::real_t colB = 1.0f;


			// UVs
			tinyobj::real_t texU = vertexAttributes.texcoords[2 * idx.texcoord_index + 0];
			tinyobj::real_t texV = vertexAttributes.texcoords[2 * idx.texcoord_index + 1];

			// normal
			tinyobj::real_t norX = vertexAttributes.normals[3 * idx.normal_index + 0];
			tinyobj::real_t norY = vertexAttributes.normals[3 * idx.normal_index + 1];
			tinyobj::real_t norZ = vertexAttributes.normals[3 * idx.normal_index + 2];


			vertices.push_back(
				vertex
				{
					{posX, posY, posZ, 1.0f},   // vertex position
					{colR, colG, colB, 1.0f},   // vertex color
					{texU, texV},               // texture coordinates
					{norX, norY, norZ, 0.0f}    // vertex normal
				}
			);
			indices.push_back(faceVertices * i + j);
		}
		offset += faceVertices;
	}

	// return makeGeometry using the data from tinyobj
	return makeGeometry(vertices.data(), vertices.size(), indices.data(), indices.size());
}

geometry makeGeometry(vertex* verts, size_t vertCount, unsigned int* indices, size_t indxCount)
{
	geometry newGeo = {};
	newGeo.size = indxCount;

	glGenBuffers(1, &newGeo.vbo);       // VBO
	glGenBuffers(1, &newGeo.ibo);       // IBO
	glGenVertexArrays(1, &newGeo.vao);  // VAO

	glBindVertexArray(newGeo.vao);                      // bind VAO first
	glBindBuffer(GL_ARRAY_BUFFER, newGeo.vbo);          // bind VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newGeo.ibo);  // bind IBO
	glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(vertex), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	auto off = offsetof(vertex, pos);

	// Describe the data contained within the buffers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,
		4,             
		GL_FLOAT,      
		GL_FALSE,      
		sizeof(vertex),
		(void*)0);     

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,               
		4,             
		GL_FLOAT,      
		GL_FALSE,      
		sizeof(vertex),
		(void*)offsetof(vertex, col));     

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex),
		(void*)offsetof(vertex, uv));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex),
		(void*)offsetof(vertex, normal));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return newGeo;
}

void freeGeometry(geometry& geo)
{
	glDeleteBuffers(1, &geo.vbo);
	glDeleteBuffers(1, &geo.ibo);
	glDeleteVertexArrays(1, &geo.vao);

	// deleteeeeeeedeedededed
	geo = {};
}

texture loadTexture(const char* filePath)
{
	assert(filePath != nullptr && "File path was invalid.");
	int imageWidth, imageHeight, imageFormat;
	unsigned char* rawPixelData = nullptr;
	stbi_set_flip_vertically_on_load(true);
	rawPixelData = stbi_load(filePath,
		&imageWidth,
		&imageHeight,
		&imageFormat,
		STBI_default);
	assert(rawPixelData != nullptr && "Image failed to load.");
	texture newTexture = makeTexture(imageWidth, imageHeight, imageFormat, rawPixelData);
	assert(newTexture.handle != 0 && "Failed to create texture.");

	stbi_image_free(rawPixelData);
	return newTexture;
}

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char* pixels)
{
	// Figure out the OGL texture format to use
	// Only support 1-4 channels
	assert(channels > 0 && channels < 5);
	GLenum oglFormat = GL_RED;
	switch (channels)
	{
	case 1:
		oglFormat = GL_RED;     // RED
		break;
	case 2:
		oglFormat = GL_RG;      // RED GREEN
		break;
	case 3:
		oglFormat = GL_RGB;     // RED GREEN BLUE
		break;
	case 4:
		oglFormat = GL_RGBA;    // RED GREEN BLUE ALPHA
		break;
	default:
		fprintf(stderr, "Invalid format specified! Must be RGBA.");
		break;
	}

	// generate a texture
	texture retVal = { 0, width, height, channels };
	glGenTextures(1, &retVal.handle);

	// bind and buffer data to it
	glBindTexture(GL_TEXTURE_2D, retVal.handle);
	// Texture to buffer data to was previously bound
	glTexImage2D(GL_TEXTURE_2D,
		0,               
		oglFormat,      
		width,           
		height,          
		0,               
		oglFormat,       
		GL_UNSIGNED_BYTE,
		pixels);         

	// Set some filtering settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // filtering applied when texel density is greater than display (interpolating existing data)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // filtering applied when texel density is less than display (extrapolating data)

	// Return the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return retVal;
}

void freeTexture(texture& tex)
{
	glDeleteTextures(1, &tex.handle);
	tex = {};   // default initialize
}

bool checkShader(GLuint target, const char* humanReadableName = "")
{
	assert(glIsShader(target) && "target is not a shader -- cannot check");

	GLint shaderCompileStatus = 0;
	glGetShaderiv(target, GL_COMPILE_STATUS, &shaderCompileStatus);
	if (shaderCompileStatus != GL_TRUE)
	{
		GLsizei logLength = 0;
		GLchar message[1024];
		glGetShaderInfoLog(target, 1024, &logLength, message);
		fprintf(stderr, "\n[ERROR] %s Shader \n %s", humanReadableName, message);

		// return an empty shader if it fails
		return false;
	}

	return true;
}

shader loadShader(const char* vertPath, const char* fragPath)
{
	// extract the text for vertex shader from the file
	std::fstream vertFile(vertPath,         // path to the file
		std::ios::in);    // the mode of operation (in == reading)

	assert(vertFile.is_open() && "Failed to open vertex shader file.");
	std::string vertSource;
	for (std::string line; std::getline(vertFile, line);)
	{
		vertSource += line + "\n";
	}

	vertFile.close();

	// extract the text for vertex shader from the file
	std::fstream fragFile(fragPath,         // path to the file
		std::ios::in);    // the mode of operation (in == reading)

	assert(fragFile.is_open() && "Failed to open fragment shader file.");
	std::string fragSource;
	for (std::string line; std::getline(fragFile, line);)
	{
		fragSource += line + "\n";
	}

	fragFile.close();

	return makeShader(vertSource.c_str(), fragSource.c_str());

	// extract the text for fragment shader from the file
	// pass it along to makeShader and return the final return value
}

shader makeShader(const char* vertSource, const char* fragSource)
{
	// make a shader program
	shader newShad = {};
	newShad.program = glCreateProgram(); // no parameters

	// create the shaders (not the same thing as a shader program)
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);     // this shader is a vertex shader
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);   // this shader is a fragment shader

	// compile the shaders
	glShaderSource(vert, 1, &vertSource, 0);    // send source code for specified shader
	glShaderSource(frag, 1, &fragSource, 0);
	glCompileShader(vert);                      // actually compile the shader
	glCompileShader(frag);

	// validate the shaders
	bool shaderOK = checkShader(vert, "Vertex");
	assert(shaderOK && "Vertex shader failed to compile.");
	shaderOK = checkShader(frag, "Fragment");
	assert(shaderOK && "Fragment shader failed to compile.");

	// attach the shaders
	glAttachShader(newShad.program, vert); // associate the vertex shader w/ the shader program
	glAttachShader(newShad.program, frag); // associate the fragment shader w/ the shader program

	// link the shader program
	glLinkProgram(newShad.program); // link the shader program

	// delete the shaders
	glDeleteShader(vert);   // delete the vertex shader
	glDeleteShader(frag);   // delete the fragment shader

	// return the shader
	return newShad;
}

void freeShader(shader& shad)
{
	glDeleteProgram(shad.program);
	shad = {};
}

void draw(const shader& shad, const geometry& geo)
{
	// specify which shader
	glUseProgram(shad.program);
	// specify which VAO
	glBindVertexArray(geo.vao);

	// draw
	glDrawElements(GL_TRIANGLES,    // primitive type
		geo.size,        // indices
		GL_UNSIGNED_INT, // index type
		0);
}

void setUniform(const shader& shad, GLuint location, const glm::mat4& value)
{

	glProgramUniformMatrix4fv(shad.program, location, 1, GL_FALSE, glm::value_ptr(value));
}

void setUniform(const shader& shad, GLuint location, const texture& tex, int textureSlot)
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, tex.handle);

	glProgramUniform1i(shad.program, location, textureSlot);
}

void setUniform(const shader& shad, GLuint location, float value)
{
	glProgramUniform1f(shad.program, location, value);
}

void setUniform(const shader& shad, GLuint location, const glm::vec3& value)
{
	glProgramUniform3fv(shad.program, location, 1, glm::value_ptr(value));
}