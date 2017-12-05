#ifndef TEXTUREUTILITIES_H_INCLUDED
#define TEXTUREUTILITIES_H_INCLUDED

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif // GLEW_STATIC

#include <GL/glew.h>
#include <vector>

GLuint loadTextureJPG(const char* imagePath);
GLuint loadCubemap(std::vector<const GLchar*> faces);

#endif // TEXTUREUTILITIES_H_INCLUDED
