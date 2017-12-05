#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif // GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class City;

/** \brief Object is parent (probably virtual) class for objects that should be drawn with Opengl in 3D scene.
 *         It contains only getters and setter. Model for object is stored in model, it's size is in modelSize.
 *         CurrentLinks store number of currently linked positionSquares with the object.
 */
class Object
{
public:
  virtual ~Object();
  GLfloat* getModel() { return model; }
  void setModel(GLfloat* val) { model = val; } //?? sloucit setModel,VAO,VBO,ModelSize etc.
  GLuint* getIndices() { return indices; }
  void setIndices(GLuint* val) { indices = val; }
  GLuint getVAO() { return vao; }
  void setVAO(GLuint val) { vao = val; }
  GLuint getVBO() { return vbo; }
  void setVBO(GLuint val) { vbo = val; }
  GLuint getEBO() { return ebo; }
  void setEBO(GLuint val) { ebo = val; }
  int getModelSize() { return modelSize; }
  void setModelSize(int val) { modelSize = val; }
  int getIndicesSize() { return indicesSize; }
  void setIndicesSize(int val) { indicesSize = val; }
  int getCurrentLinks() { return currentLinks; }
  void setCurrentLinksToZero(void) { currentLinks = 0; }
  void setCurrentLinksPlusOne(void) { currentLinks++; }
  void deleteModel(void);
  virtual void generateModel(City* city) = 0;
protected:
  Object();
  GLfloat* model;
  GLuint* indices;
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
  int modelSize;
  int indicesSize;
  int currentLinks;
};

#endif // OBJECT_H_INCLUDED
