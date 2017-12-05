#include "Node.hpp"

#include "City.hpp"
#include "graphicUtilities.hpp"

void Node::generateModel(City* city)
{
  int numRoads = this->roads->size();
  if (numRoads == 1)
  {
//    this->roads->push_back(this->roads->front()); //heh?
    numRoads++;
  }
  this->modelSize = (numRoads*4 + 1) * 8;
//  if (this->modelSize <= (4+1)*8)
//  {
//    continue; // 1 - blind valley, think how to solve it
//  }
  this->model = createVerticesFromNode(this, city->getTerrain(), this->modelSize, 8);

//          int numIndices = 6*numRoads + 3; //numRoads*4 + 2 + 1 + numRoads*2 + 1;
  this->indicesSize = numRoads*2*6 + numRoads*2*3; //numRoads*4 + 2 + 1 + numRoads*2 + 1;
  this->indices = createElementsFromNode(this, indicesSize);
//          addNormalsForStripTriangles(newModel,sizeOfNode,newIndices,numIndices,3,6);
  addNormalsForTriangles(this->model, this->modelSize, this->indices, this->indicesSize, 3, 8);
  addTexCoordsToNode(this->model, this->modelSize, 6, 8);

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
