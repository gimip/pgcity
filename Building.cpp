#include "Building.hpp"

#include "City.hpp"
#include "graphicUtilities.hpp"
#include <stdlib.h>
#include "Shader.hpp"
#include "Camera.hpp"
#include <glm/gtc/type_ptr.hpp>

Building::Building()
{
  borders = new std::list<float*>();
  floors = (rand() % 2) + 1;
//  floors = 0;
}

Building::~Building()
{

}

void Building::generateModel(City* city)
{

  float heightOfBuilding2 = 0.25;//HEIGHT_OF_BUILDING;
  float heightOfBuilding = heightOfBuilding2 * (float)this->floors;
  int lengthOfRow = 9;

  this->modelSize = (this->borders->size() * 2 + this->borders->size() + 2 + 2) * lengthOfRow;
  this->model = createVerticesFromBuilding(this, city->getTerrain(), this->modelSize, lengthOfRow);

  this->indicesSize = this->borders->size() * 6 + 2 * 6 + 2 * 3;
  this->indices = createElementsFromBuilding(this, this->indicesSize);

  addNormalsForTriangles(this->model, this->modelSize, this->indices, this->indicesSize, 3, lengthOfRow);
  addTexCoordsToBuilding(this->model, this->modelSize, 6, 9);

//  GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo);
  glGenBuffers(1, &this->ebo);
  // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

  glBufferData(GL_ARRAY_BUFFER, this->modelSize * sizeof(GLfloat), this->model, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indicesSize * sizeof(GLuint), this->indices, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  // TexCoord attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
  glEnableVertexAttribArray(3);
}

extern glm::vec3 lightPos;
extern glm::vec3 bigger;
extern Camera camera;
extern GLuint screenWidth, screenHeight;
extern float maxFar;
extern glm::mat4 lightSpaceMatrix;

void Building::showModel(Shader* ourShader7, GLuint texture4, GLuint texture5)
{
//  ourShader7->Use();
//  glActiveTexture(GL_TEXTURE0);
//  glBindTexture(GL_TEXTURE_2D, texture4);
//  glUniform1i(glGetUniformLocation(ourShader7->Program, "ourTexture"), 0);
//
//  glActiveTexture(GL_TEXTURE1);
//  glBindTexture(GL_TEXTURE_2D, texture5);
//  glUniform1i(glGetUniformLocation(ourShader7->Program, "ourTextureRoof"), 1);
//
////  srand(37);
//
//  glBindVertexArray(this->vao);
//
//  glm::mat4 model;
//  model = glm::scale(model, bigger);
//  glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
//  glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, maxFar);
//
//  GLint modelLoc = glGetUniformLocation(ourShader7->Program, "model");
//  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//  glUniformMatrix4fv(glGetUniformLocation(ourShader7->Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
//  glUniformMatrix4fv(glGetUniformLocation(ourShader7->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//
//  GLint objectColorLoc = glGetUniformLocation(ourShader7->Program, "objectColor");
//  GLint lightPosLoc    = glGetUniformLocation(ourShader7->Program, "lightPos");
//  glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
//
//  GLint viewPosLoc     = glGetUniformLocation(ourShader7->Program, "viewPos");
//  glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
//  glUniformMatrix4fv(glGetUniformLocation(ourShader7->Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
//  glUniform3f(objectColorLoc, (float)(rand()/2)/(float)RAND_MAX, (float)(rand()/2)/(float)RAND_MAX, (float)(rand()/2)/(float)RAND_MAX);
//
//  glDrawElements(GL_TRIANGLES, this->indicesSize, GL_UNSIGNED_INT, 0);
}
