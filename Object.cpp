#include "Object.hpp"

Object::Object()
{
  model = NULL;
  indices = NULL;
  vao = 0;
  vbo = 0;
  ebo = 0;
  modelSize = 0;
  indicesSize = 0;
  currentLinks = 0;
}

Object::~Object()
{
  //dtor
}

void Object::deleteModel()
{
  // smazat vse!!!!!
  delete[] model;
  if (indices != NULL)
  {
    delete[] indices;
  }
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  if (ebo != 0)
  {
    glDeleteBuffers(1, &ebo);
  }
  model = NULL;
  indices = NULL;
  vao = 0;
  vbo = 0;
  ebo = 0;
  modelSize = 0;
  indicesSize = 0;
  currentLinks = 0;
}
