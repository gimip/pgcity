#ifndef GRAPHICUTILITIES_H_INCLUDED
#define GRAPHICUTILITIES_H_INCLUDED

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif // GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <list>
#include <glm/glm.hpp>

class Building;
class Node;
class Population;
class Road;
class Terrain;

GLfloat* createVerticesFromTerrain(Terrain* terrain, int numberOfVertices, int lengthOfVertex);
GLuint* createElementsFromTerrain(Terrain* terrain, int numberOfElements);
GLfloat* createVerticesFromPopulation(Population* population, int numberOfVertices, int lengthOfVertex);
GLuint* createElementsFromPopulation(Population* population, int numberOfElements);
GLfloat* createVerticesFromNode(Node* node, Terrain* terrain, int numberOfVertices, int lengthOfVertex);
GLuint* createElementsFromNode(Node* node, int numberOfElements);
GLfloat* createVerticesFromRoad(Road* road, Terrain* terrain, int numberOfVertices, int lengthOfVertex);
GLuint* createElementsFromRoad(Road* road, int numberOfElements);
GLfloat* createVerticesFromBuilding(Building* building, Terrain* terrain, int numberOfVertices, int lengthOfVertex);
GLuint* createElementsFromBuilding(Building* building, int numberOfElements);
void addNormalsToMesh(GLfloat* vertices, int numberOfVertices, int firstNormalIndex, int lengthOfRow);
void addNormalsForStripTriangles(GLfloat* vertices, int numberOfVertices, GLuint* elements, int numberOfElements, int firstNormalIndex, int lengthOfRow);
void addNormalsForTriangles(GLfloat* vertices, int numberOfVertices, GLuint* elements, int numberOfElements, int firstNormalIndex, int lengthOfRow);
void addTexCoordsToTerrain(Terrain* terrain, GLfloat* vertices, int numberOfVertices, int firstTextureIndex, int lengthOfRow);
void addTexCoordsToRoad(GLfloat* roadVertices, int numberOfVertices, int firstTextureIndex, int lengthOfRow);
void addTexCoordsToNode(GLfloat* nodeVertices, int numberOfVertices, int firstTextureIndex, int lengthOfRow);
void addTexCoordsToBuilding(GLfloat* buildingVertices, int numberOfVertices, int firstTextureIndex, int lengthOfRow);
/* Deprecated
Node* findNodeNearestCamera(std::list<Node*> nodes, Camera camera);
void addNewRoadsFromNodes(std::list<Road*>* roads, std::list<Node*>* nodes, std::list<Node*>* borderNodes, float restriction, Camera camera);
std::list<Node*>* addNewRoadsFromNode(std::list<Road*>* roads, Node* node);
void addNewBlocksFromRoads(std::list<Block*>* blocks, std::list<Road*>* roads);
void updateNewRoadsFromNodes(std::list<Road*>* roads, std::list<Node*>* nodes, std::list<Node*>* borderNodes, float restriction, Camera camera);
void changeNodeToBorderNode(std::list<Road*>* roads, std::list<Node*>* borderNodes, Node* node);
*/

#endif // GRAPHICUTILITIES_H_INCLUDED
