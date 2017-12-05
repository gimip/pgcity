#include "Road.hpp"

#include "City.hpp"
#include "graphicUtilities.hpp"

Road::Road()
{
  from = NULL;
  to = NULL;
  curvature = new std::list<float*>();
  firstBlock = NULL;
  secondBlock = NULL;
}

void Road::generateModel(City* city)
{
  int curve = this->curvature->size();
//          int sizeOfRoad = /*144+*/(curve+1)*108/*+144*/; // zmenit na (curve+1)*108 ???? zahodit ctverce na zacatku/konci ????
  this->modelSize = (4 + curve*4 + 4) * 8;
  this->model = createVerticesFromRoad(this, city->getTerrain(), this->modelSize, 8);
//          addNormalsToMesh(newModel,sizeOfRoad,3,6);

  this->indicesSize = 9 + curve*9;
  this->indices = createElementsFromRoad(this, this->indicesSize);
  addNormalsForStripTriangles(this->model, this->modelSize, this->indices, this->indicesSize, 3, 8);
  addTexCoordsToRoad(this->model, this->modelSize, 6, 8);

  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo);
  glGenBuffers(1, &this->ebo);
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
  glBufferData(GL_ARRAY_BUFFER, this->modelSize * sizeof(GLfloat), this->model, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indicesSize * sizeof(GLuint), this->indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);
}
