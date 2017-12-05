#include "graphicUtilities.hpp"

#include "constants.hpp"
#include "utilities.hpp"
#include "Building.hpp"
#include "Node.hpp"
#include "Population.hpp"
#include "Road.hpp"
#include "Terrain.hpp"

struct CoordNorm
{
  glm::vec3 coordinates;
  glm::vec3 normal;
};

void addNormalToList(std::list<CoordNorm>* coordinatesNormals, glm::vec3 coords, glm::vec3 normal)
{
  bool found = false;
  CoordNorm coordNorm;
  std::list<CoordNorm>::iterator coordinatesNormalsIt;

  for (coordinatesNormalsIt = coordinatesNormals->begin(); coordinatesNormalsIt != coordinatesNormals->end(); coordinatesNormalsIt++)
  {
    coordNorm = *coordinatesNormalsIt;
    if (coordNorm.coordinates == coords)
    {
      found = true;
      break;
    }
  }

  if (found)
  {
    coordNorm.normal += normal;
  }
  else
  {
    CoordNorm newCoordNorm;
    newCoordNorm.coordinates = coords;
    newCoordNorm.normal = normal;
    coordinatesNormals->push_back(newCoordNorm);
  }
}

void addNormalsToMesh(GLfloat* vertices, int numberOfVertices, int firstNormalIndex, int lengthOfVertex)
{
  std::list<CoordNorm> coordinatesNormals; //vylepsit list->pole

  for (int i = 0; i < numberOfVertices; i += 3*lengthOfVertex)
  {
    glm::vec3 ij (vertices[i],vertices[i+1],vertices[i+2]);
    glm::vec3 i1j (vertices[i+lengthOfVertex],vertices[i+1+lengthOfVertex],vertices[i+2+lengthOfVertex]);
    glm::vec3 ij1 (vertices[i+2*lengthOfVertex],vertices[i+1+2*lengthOfVertex],vertices[i+2+2*lengthOfVertex]);

    glm::vec3 normal = glm::cross(i1j - ij,ij1 - ij);
    addNormalToList(&coordinatesNormals, ij, normal);
    addNormalToList(&coordinatesNormals, i1j, normal);
    addNormalToList(&coordinatesNormals, ij1, normal);
  }

  for (int i = 0; i < numberOfVertices; i += lengthOfVertex)
  {
    glm::vec3 coord (vertices[i],vertices[i+1],vertices[i+2]);
    for (std::list<CoordNorm>::iterator coordinatesNormalsIt = coordinatesNormals.begin(); coordinatesNormalsIt != coordinatesNormals.end(); coordinatesNormalsIt++)
    {
      CoordNorm coordNorm = *coordinatesNormalsIt;
      if (coordNorm.coordinates == coord)
      {
        coordNorm.normal = glm::normalize(coordNorm.normal);
        vertices[i+firstNormalIndex] = coordNorm.normal[0];
        vertices[i+firstNormalIndex+1] = coordNorm.normal[1];
        vertices[i+firstNormalIndex+2] = coordNorm.normal[2];
        break;
      }
    }
  }
}

void addNormalsForStripTriangles(GLfloat* vertices, int numberOfVertices, GLuint* elements, int numberOfElements, int firstNormalIndex, int lengthOfRow)
{
  int j = 0;
  for(int i = firstNormalIndex; i < numberOfVertices; i += lengthOfRow)
  {
    vertices[i] = 0.0;
    vertices[i+1] = 0.0;
    vertices[i+2] = 0.0;
  }
  for (int i = 0; i < numberOfElements - 1; i+=2) // (-1) <--- because the last element is PRIMITIVE_RESTART
  {
    GLuint a = elements[i];
    GLuint b = elements[i+1];
    GLuint c = elements[i+2];
    GLuint d = elements[i+3];

    glm::vec3 ij (vertices[a*lengthOfRow], vertices[a*lengthOfRow+1], vertices[a*lengthOfRow+2]);
    glm::vec3 i1j (vertices[b*lengthOfRow], vertices[b*lengthOfRow+1], vertices[b*lengthOfRow+2]);
    glm::vec3 ij1 (vertices[c*lengthOfRow], vertices[c*lengthOfRow+1], vertices[c*lengthOfRow+2]);
    glm::vec3 i1j1 (vertices[d*lengthOfRow], vertices[d*lengthOfRow+1], vertices[d*lengthOfRow+2]);

    glm::vec3 normal1 = -glm::cross(i1j - ij,ij1 - ij);
    glm::vec3 normal2 = glm::cross(i1j - i1j1,ij1 - i1j1);

    vertices[a*lengthOfRow+firstNormalIndex] += normal1[0];
    vertices[a*lengthOfRow+firstNormalIndex+1] += normal1[1];
    vertices[a*lengthOfRow+firstNormalIndex+2] += normal1[2];
    vertices[b*lengthOfRow+firstNormalIndex] += (normal1[0] + normal2[0]);
    vertices[b*lengthOfRow+firstNormalIndex+1] += (normal1[1] + normal2[0]);
    vertices[b*lengthOfRow+firstNormalIndex+2] += (normal1[2] + normal2[0]);
    vertices[c*lengthOfRow+firstNormalIndex] += (normal1[0] + normal2[0]);
    vertices[c*lengthOfRow+firstNormalIndex+1] += (normal1[1] + normal2[0]);
    vertices[c*lengthOfRow+firstNormalIndex+2] += (normal1[2] + normal2[0]);
    vertices[d*lengthOfRow+firstNormalIndex] += normal2[0];
    vertices[d*lengthOfRow+firstNormalIndex+1] += normal2[1];
    vertices[d*lengthOfRow+firstNormalIndex+2] += normal2[2];

    if (elements[i+4] == PRIMITIVE_RESTART)
    {
      i+=3;
    }
  }

  for(int i = firstNormalIndex; i < numberOfVertices; i += lengthOfRow)
  {
    glm::vec3 ij (vertices [i],vertices[i+1],vertices[i+2]);
    ij = glm::normalize(ij);

    vertices[i] = ij[0];
    vertices[i+1] = ij[1];
    vertices[i+2] = ij[2];
  }
}

/** \brief Add normals to vertices which are drawn by elements (indices). Elements have to be in clockwise order, otherwise normals will be in opposite direction.
 *
 * \param vertices GLfloat* Vertices for shader, adding normals to vertices
 * \param numberOfVertices int Count of vertices
 * \param elements GLuint* Indices for drawing vertices
 * \param numberOfElements int Count of indices
 * \param firstNormalIndex int Index where to add normals in vertices
 * \param lengthOfRow int Length of one vertex in vertices
 * \return void
 *
 */
void addNormalsForTriangles(GLfloat* vertices, int numberOfVertices, GLuint* elements, int numberOfElements, int firstNormalIndex, int lengthOfRow)
{
  for(int i = firstNormalIndex; i < numberOfVertices; i += lengthOfRow)
  {
    vertices[i] = 0.0;
    vertices[i+1] = 0.0;
    vertices[i+2] = 0.0;
  }

  for (int i = 0; i < numberOfElements; i+=3)
  {
    GLuint a = elements[i];
    GLuint b = elements[i+1];
    GLuint c = elements[i+2];

    glm::vec3 ij (vertices[a*lengthOfRow], vertices[a*lengthOfRow+1], vertices[a*lengthOfRow+2]);
    glm::vec3 i1j (vertices[b*lengthOfRow], vertices[b*lengthOfRow+1], vertices[b*lengthOfRow+2]);
    glm::vec3 ij1 (vertices[c*lengthOfRow], vertices[c*lengthOfRow+1], vertices[c*lengthOfRow+2]);

    glm::vec3 normal1 = glm::cross(ij1 - ij,i1j - ij);
    normal1 = glm::normalize(normal1);

    vertices[a*lengthOfRow+firstNormalIndex] += normal1[0];
    vertices[a*lengthOfRow+firstNormalIndex+1] += normal1[1];
    vertices[a*lengthOfRow+firstNormalIndex+2] += normal1[2];
    vertices[b*lengthOfRow+firstNormalIndex] += normal1[0];
    vertices[b*lengthOfRow+firstNormalIndex+1] += normal1[1];
    vertices[b*lengthOfRow+firstNormalIndex+2] += normal1[2];
    vertices[c*lengthOfRow+firstNormalIndex] += normal1[0];
    vertices[c*lengthOfRow+firstNormalIndex+1] += normal1[1];
    vertices[c*lengthOfRow+firstNormalIndex+2] += normal1[2];
  }

  for(int i = firstNormalIndex; i < numberOfVertices; i += lengthOfRow)
  {
    glm::vec3 ij (vertices [i],vertices[i+1],vertices[i+2]);
    ij = glm::normalize(ij);

    vertices[i] = ij[0];
    vertices[i+1] = ij[1];
    vertices[i+2] = ij[2];
  }
}

void addTexCoordsToTerrain(Terrain* terrain, GLfloat* vertices, int numberOfVertices, int firstTextureIndex, int lengthOfRow)
{
  int k = firstTextureIndex;
  for (int i = 0; i < terrain->getWidth(); i++)
  {
    for (int j = 0; j < terrain->getHeight(); j++)
    {
      vertices[k] = ((float)i)/12.0; // Bottom Left
      vertices[k+1] = ((float)j)/12.0;
      k += lengthOfRow;

//        vertices[k] = 0.0; // Bottom Left
//        vertices[k+1] = 0.0;
//        k += lengthOfRow;
//        vertices[k] = 1.0; // Bottom Right
//        vertices[k+1] = 0.0;
//        k += lengthOfRow;
//        vertices[k] = 0.0; // Top Left
//        vertices[k+1] = 1.0;
//        k += lengthOfRow;
//        vertices[k] = 1.0; // Top Right
//        vertices[k+1] = 1.0;
//        k += lengthOfRow;
    }
  }
}

void addTexCoordsToRoad(GLfloat* roadVertices, int numberOfVertices, int firstTextureIndex, int lengthOfRow)
{
  float counter = 0.0;
  for (int i = firstTextureIndex; i < numberOfVertices;)
  {
    roadVertices[i] = 0.0; // Bottom Left
    roadVertices[i+1] = counter; //vylepsit na delku krivky
    i += lengthOfRow;
    roadVertices[i] = 0.04; // Upper Left
    roadVertices[i+1] = counter;
    i += lengthOfRow;
    roadVertices[i] = 0.96; // Upper Right
    roadVertices[i+1] = counter;
    i += lengthOfRow;
    roadVertices[i] = 1.0; // Bottom Right
    roadVertices[i+1] = counter;
    i += lengthOfRow;
    counter += 2.0;
  }
}

void addTexCoordsToNode(GLfloat* nodeVertices, int numberOfVertices, int firstTextureIndex, int lengthOfRow)
{
  int i = firstTextureIndex;
  for (; i < numberOfVertices - lengthOfRow;)
  {
    nodeVertices[i] = 0.0; // Upper Left
    nodeVertices[i+1] = 0.0;
    i += lengthOfRow;
    nodeVertices[i] = 0.0; // Bottom Left
    nodeVertices[i+1] = 0.0;
    i += lengthOfRow;
    nodeVertices[i] = 2.0; // Upper Right
    nodeVertices[i+1] = 0.0;
    i += lengthOfRow;
    nodeVertices[i] = 2.0; // Bottom Right
    nodeVertices[i+1] = 0.0;
    i += lengthOfRow;
  }
  nodeVertices[i] = 1.0;
  nodeVertices[i+1] = 2.0;
}

void addTexCoordsToBuilding(GLfloat* buildingVertices, int numberOfVertices, int firstTextureIndex, int lengthOfRow)
{
//  int tex = 6;
  int tex = firstTextureIndex;
  (void)(numberOfVertices);
  buildingVertices[tex] = 0.0;
  buildingVertices[tex+1] = 2.0;
  buildingVertices[tex+2] = 0.0;
  tex+=lengthOfRow;
  buildingVertices[tex] = 0.0;
  buildingVertices[tex+1] = 0.0;
  buildingVertices[tex+2] = 0.0;
  tex+=lengthOfRow;
  buildingVertices[tex] = 2.0;
  buildingVertices[tex+1] = 2.0;
  buildingVertices[tex+2] = 0.0;
  tex+=lengthOfRow;
  buildingVertices[tex] = 2.0;
  buildingVertices[tex+1] = 0.0;
  buildingVertices[tex+2] = 0.0;
  tex+=lengthOfRow;
  buildingVertices[tex] = 0.0;
  buildingVertices[tex+1] = 2.0;
  buildingVertices[tex+2] = 0.0;
  tex+=lengthOfRow;
  buildingVertices[tex] = 0.0;
  buildingVertices[tex+1] = 0.0;
  buildingVertices[tex+2] = 0.0;
  tex+=lengthOfRow;
  buildingVertices[tex] = 2.0;
  buildingVertices[tex+1] = 2.0;
  buildingVertices[tex+2] = 0.0;
  tex+=lengthOfRow;
  buildingVertices[tex] = 2.0;
  buildingVertices[tex+1] = 0.0;
  buildingVertices[tex+2] = 0.0;
  tex+=lengthOfRow;

  buildingVertices[tex] = 0.0;
  buildingVertices[tex+1] = 0.0;
  buildingVertices[tex+2] = 1.0;
  tex+=lengthOfRow;
  buildingVertices[tex] = 1.0;
  buildingVertices[tex+1] = 0.0;
  buildingVertices[tex+2] = 1.0;
  tex+=lengthOfRow;
  buildingVertices[tex] = 1.0;
  buildingVertices[tex+1] = 0.0;
  buildingVertices[tex+2] = 1.0;
  tex+=lengthOfRow;
  buildingVertices[tex] = 0.0;
  buildingVertices[tex+1] = 0.0;
  buildingVertices[tex+2] = 1.0;
  tex+=lengthOfRow;

  buildingVertices[tex] = 0.0;
  buildingVertices[tex+1] = 1.0;
  buildingVertices[tex+2] = 1.0;
  tex+=lengthOfRow;
  buildingVertices[tex] = 1.0;
  buildingVertices[tex+1] = 1.0;
  buildingVertices[tex+2] = 1.0;
  tex+=lengthOfRow;

  buildingVertices[tex] = 1.0;
  buildingVertices[tex+1] = 2.7;
  buildingVertices[tex+2] = 0.0;
  tex+=lengthOfRow;
  buildingVertices[tex] = 1.0;
  buildingVertices[tex+1] = 2.7;
  buildingVertices[tex+2] = 0.0;
  tex+=lengthOfRow;
}

/*deprecated, created triangle, better new version of element indexation
GLfloat* createVerticesFromTerrain(Terrain* terrain, int numberOfVertices, int lengthOfVertex)
{
  GLfloat* terrainVertices = new GLfloat[numberOfVertices];
  int j = 0;
  int k = 0;
  float** terr = terrain->getHeightMap();
  for (int i = 0; i < numberOfVertices; i+=6*lengthOfVertex, j++)
  {
    terrainVertices[i] = (float)j;
    terrainVertices[i+2] = (float)k;
    terrainVertices[i+1] = terr[j][k];

    terrainVertices[i+lengthOfVertex] = (float)(j+1);
    terrainVertices[i+2+lengthOfVertex] = (float)k;
    terrainVertices[i+1+lengthOfVertex] = terr[j+1][k];

    terrainVertices[i+2*lengthOfVertex] = (float)(j);
    terrainVertices[i+2+2*lengthOfVertex] = (float)(k+1);
    terrainVertices[i+1+2*lengthOfVertex] = terr[j][k+1];

    terrainVertices[i+3*lengthOfVertex] = (float)(j+1);
    terrainVertices[i+2+3*lengthOfVertex] = (float)(k+1);
    terrainVertices[i+1+3*lengthOfVertex] = terr[j+1][k+1];

    terrainVertices[i+4*lengthOfVertex] = (float)(j+1);
    terrainVertices[i+2+4*lengthOfVertex] = (float)(k);
    terrainVertices[i+1+4*lengthOfVertex] = terr[j+1][k];

    terrainVertices[i+5*lengthOfVertex] = (float)(j);
    terrainVertices[i+2+5*lengthOfVertex] = (float)(k+1);
    terrainVertices[i+1+5*lengthOfVertex] = terr[j][k+1];
    if (j == terrain->getHeight()-2)
    {
      j = -1;
      k++;
    }
  }
  return terrainVertices;
}
*/

GLfloat* createVerticesFromTerrain(Terrain* terrain, int numberOfVertices, int lengthOfVertex)
{
  GLfloat* terrainVertices = new GLfloat[numberOfVertices];
  float** terr = terrain->getHeightMap();
  int k = 0;
  for (int i = 0; i < terrain->getWidth(); i++)
  {
    for (int j = 0; j < terrain->getHeight(); j++)
    {
      terrainVertices[k] = (float)i;
      terrainVertices[k+2] = (float)j;
      terrainVertices[k+1] = terr[i][j];
      k += lengthOfVertex;
    }
  }
  return terrainVertices;
}

GLuint* createElementsFromTerrain(Terrain* terrain, int numberOfElements)
{
  GLuint* terrainElements = new GLuint[numberOfElements];
  int width = terrain->getWidth();
  GLuint i = 0;
  GLuint j = (GLuint) width;
  int e = 0;
  for (int k = 0 ; k < width-1; k++, e++)
  {
    for (; i < (GLuint)(k+1)*width; i++, j++, e++)
    {
      terrainElements[e] = i;
      terrainElements[++e] = j;
    }
    terrainElements[e] = PRIMITIVE_RESTART;
  }
  return terrainElements;
}

GLfloat* createVerticesFromPopulation(Population* population, int numberOfVertices, int lengthOfVertex)
{
  GLfloat* populationVertices = new GLfloat[numberOfVertices];
  float** pop = population->getDensity();
  int k = 0;
  for (int i = 0; i < population->width; i++)
  {
    for (int j = 0; j < population->height; j++)
    {
      populationVertices[k] = (float)i;
      populationVertices[k+2] = (float)j;
      populationVertices[k+1] = pop[i][j];
      k += lengthOfVertex;
    }
  }
  return populationVertices;
}

GLuint* createElementsFromPopulation(Population* population, int numberOfElements)
{
  GLuint* populationElements = new GLuint[numberOfElements];
  int width = population->width;
  GLuint i = 0;
  GLuint j = (GLuint) width;
  int e = 0;
  for (int k = 0 ; k < width-1; k++, e++)
  {
    for (; i < (GLuint)(k+1)*width; i++, j++, e++)
    {
      populationElements[e] = i;
      populationElements[++e] = j;
    }
    populationElements[e] = PRIMITIVE_RESTART;
  }
  return populationElements;
}

GLuint* createElementsFromRoad(Road* road, int numberOfElements) //9 + curve * 9
{
  GLuint* roadElements = new GLuint[numberOfElements];
  std::list<float*>* curvature = road->curvature;
  int curve = curvature->size();
  GLuint i = 0;
  GLuint j = 4;
//  for (int e = 0 ; e <  9 + curve * 9; e++)
  for (int e = 0 ; e <  numberOfElements; e++)
  {
    for (; e%9 != 8; i++, j++, e++)
//    for (int a = 0; a<8; i++, j++, e++, a++)
    {
      roadElements[e] = i;
      e++;
      roadElements[e] = j;
    }
//if (e > numberOfElements)
//{
//  printf("e moc %d, %d\n",e,numberOfElements);
//}
    roadElements[e] = PRIMITIVE_RESTART;
  }
  return roadElements;
}

GLfloat* createVerticesFromBuilding(Building* building, Terrain* terrain, int numberOfVertices, int lengthOfVertex)
{
  GLfloat* buildingVertices = new GLfloat[numberOfVertices];
  int i = 0;
  float maxP12zh = 0.0;
  float heightOfBuilding = (float) building->floors;
//      float averageX = 0.0;
//      float averageY = 0.0;
  for (std::list<float*>::iterator borderIt = building->borders->begin(); borderIt != building->borders->end() ;borderIt++)
  {
    float* p1 = *borderIt;
//        averageX += p1[0];
//        averageY += p1[1];
    float p1z = bilinearInterpolation(terrain->getHeightMap(),p1[0],p1[1],terrain->getHeight(),terrain->getWidth());
//        float p2z = bilinearInterpolation(terr,p2[0],p2[1],terrain->getHeight(),terrain->getWidth());
    float p12zh;

//        if (p1z > p2z)
//        {
      p12zh = p1z + heightOfBuilding;
//        }
//        else
//        {
//          p12zh = p2z + heightOfBuilding;
//        }

    if (maxP12zh < p12zh)
    {
      maxP12zh = p12zh;
    }

    buildingVertices[i] = p1[0];
    buildingVertices[i+2] = p1[1];
    buildingVertices[i+1] = p1z;

    buildingVertices[i+3] = 0.0;
    buildingVertices[i+5] = 0.0;
    buildingVertices[i+4] = 1.0;
    i += lengthOfVertex;

    buildingVertices[i] = p1[0];
    buildingVertices[i+2] = p1[1];
    buildingVertices[i+1] = p12zh;

    buildingVertices[i+3] = 0.0;
    buildingVertices[i+5] = 0.0;
    buildingVertices[i+4] = 1.0;
    i += lengthOfVertex;
  }

  i = 0;
  for (std::list<float*>::iterator borderIt = building->borders->begin(); borderIt != building->borders->end() ;borderIt++, i+=lengthOfVertex)
  {
    i += lengthOfVertex;
    buildingVertices[i+1] = maxP12zh;
  }

  float tmpX = buildingVertices[0] - buildingVertices[2*lengthOfVertex];
  float tmpY = buildingVertices[2] - buildingVertices[2*lengthOfVertex+2];

  float tmpX2 = buildingVertices[4*lengthOfVertex] - buildingVertices[6*lengthOfVertex];
  float tmpY2 = buildingVertices[4*lengthOfVertex+2] - buildingVertices[6*lengthOfVertex+2];
  i = 8*lengthOfVertex;
  buildingVertices[i] = buildingVertices[0] + 0.1 * tmpX;
  buildingVertices[i+2] = buildingVertices[2] + 0.1 * tmpY;
  buildingVertices[i+1] = maxP12zh - heightOfBuilding/15.0;

  buildingVertices[i+3] = 0.0;
  buildingVertices[i+5] = 0.0;
  buildingVertices[i+4] = 1.0;
  i += lengthOfVertex;
  buildingVertices[i] = buildingVertices[6*lengthOfVertex] - 0.1 * tmpX2;
  buildingVertices[i+2] = buildingVertices[6*lengthOfVertex+2] - 0.1 * tmpY2;
  buildingVertices[i+1] = maxP12zh - heightOfBuilding/15.0;

  buildingVertices[i+3] = 0.0;
  buildingVertices[i+5] = 0.0;
  buildingVertices[i+4] = 1.0;
  i += lengthOfVertex;

  buildingVertices[i] = buildingVertices[4*lengthOfVertex] + 0.1 * tmpX2;
  buildingVertices[i+2] = buildingVertices[4*lengthOfVertex+2] + 0.1 * tmpY2;
  buildingVertices[i+1] = maxP12zh - heightOfBuilding/15.0;

  buildingVertices[i+3] = 0.0;
  buildingVertices[i+5] = 0.0;
  buildingVertices[i+4] = 1.0;
  i += lengthOfVertex;
  buildingVertices[i] = buildingVertices[2*lengthOfVertex] - 0.1 * tmpX;
  buildingVertices[i+2] = buildingVertices[2*lengthOfVertex+2] - 0.1 * tmpY;
  buildingVertices[i+1] = maxP12zh - heightOfBuilding/15.0;

  buildingVertices[i+3] = 0.0;
  buildingVertices[i+5] = 0.0;
  buildingVertices[i+4] = 1.0;
  i += lengthOfVertex;

  buildingVertices[i] = (buildingVertices[0] + buildingVertices[2*lengthOfVertex])/2.0;
  buildingVertices[i+2] = (buildingVertices[2] + buildingVertices[2*lengthOfVertex+2])/2.0;
  buildingVertices[i+1] = maxP12zh + heightOfBuilding/3.0;

  buildingVertices[i+3] = 0.0;
  buildingVertices[i+5] = 0.0;
  buildingVertices[i+4] = 1.0;
  i += lengthOfVertex;
  buildingVertices[i] = (buildingVertices[4*lengthOfVertex] + buildingVertices[6*lengthOfVertex])/2.0;
  buildingVertices[i+2] = (buildingVertices[4*lengthOfVertex+2] + buildingVertices[6*lengthOfVertex+2])/2.0;
  buildingVertices[i+1] = maxP12zh + heightOfBuilding/3.0;

  buildingVertices[i+3] = 0.0;
  buildingVertices[i+5] = 0.0;
  buildingVertices[i+4] = 1.0;
  i += lengthOfVertex;

  buildingVertices[i] = (buildingVertices[0] + buildingVertices[2*lengthOfVertex])/2.0;
  buildingVertices[i+2] = (buildingVertices[2] + buildingVertices[2*lengthOfVertex+2])/2.0;
  buildingVertices[i+1] = maxP12zh + heightOfBuilding/3.0;

  buildingVertices[i+3] = 0.0;
  buildingVertices[i+5] = 0.0;
  buildingVertices[i+4] = 1.0;
  i += lengthOfVertex;
  buildingVertices[i] = (buildingVertices[4*lengthOfVertex] + buildingVertices[6*lengthOfVertex])/2.0;
  buildingVertices[i+2] = (buildingVertices[4*lengthOfVertex+2] + buildingVertices[6*lengthOfVertex+2])/2.0;
  buildingVertices[i+1] = maxP12zh + heightOfBuilding/3.0;

  buildingVertices[i+3] = 0.0;
  buildingVertices[i+5] = 0.0;
  buildingVertices[i+4] = 1.0;
  i += lengthOfVertex;

  return buildingVertices;
}

GLuint* createElementsFromBuilding(Building* building, int numberOfElements)
{
  GLuint* buildingElements = new GLuint[numberOfElements];
  int num = 0;
  int j = -1;
  for (std::list<float*>::iterator borderIt = building->borders->begin(); borderIt != (--building->borders->end()) ;borderIt++, num+=2)
  {
    buildingElements[++j] = num;
    buildingElements[++j] = num+2;
    buildingElements[++j] = num+1;
    buildingElements[++j] = num+2;
    buildingElements[++j] = num+3;
    buildingElements[++j] = num+1;
  }
  buildingElements[++j] = num;
  buildingElements[++j] = 0;
  buildingElements[++j] = num+1;
  buildingElements[++j] = 0;
  buildingElements[++j] = 1;
  buildingElements[++j] = num+1;

  buildingElements[++j] = 8;
  buildingElements[++j] = 12;
  buildingElements[++j] = 9;

  buildingElements[++j] = 12;
  buildingElements[++j] = 13;
  buildingElements[++j] = 9;

  buildingElements[++j] = 11;
  buildingElements[++j] = 10;
  buildingElements[++j] = 13;

  buildingElements[++j] = 13;
  buildingElements[++j] = 12;
  buildingElements[++j] = 11;

  buildingElements[++j] = 3;
  buildingElements[++j] = 14;
  buildingElements[++j] = 1;

  buildingElements[++j] = 7;
  buildingElements[++j] = 15;
  buildingElements[++j] = 5;
  return buildingElements;
}

//GLfloat* createVerticesFromNode(Node* node, Terrain* terrain, int numberOfVertices, int lengthOfVertex)
//{
//  GLfloat* nodeVertices = new GLfloat[numberOfVertices];
//
//  const float widthOfRoad = 0.1;
//  const float heightOfRoad = 0.05;
//  const float sizeOfNode = 0.1;
//  float** terr = terrain->getHeightMap();
//  std::list<Road*> nodeRoads = node->road;
//  int numRoads = noderoads->size();
//  if (numRoads < 2) // 1 - blind valley, 0 - should not happen
//  {
//    delete [] nodeVertices;
//    return NULL; // workaround for now
////    return nodeVertices;
//  }
//  std::list<Road*>::iterator nodeRoadsIt = noderoads->begin();
//  Road* clockwiseRoad = *nodeRoadsIt;
//  std::list<Road*>* tmpRoadList = new std::list<Road*>();
//  tmpRoadList->push_back(clockwiseRoad);
//  Road* counterClockwiseRoad = rightRoad2(node,tmpRoadList,360.0);
//
//  return nodeVertices;
//}

GLfloat* createVerticesFromNode(Node* node, Terrain* terrain, int numberOfVertices, int lengthOfVertex)
{
  GLfloat* nodeVertices = new GLfloat[numberOfVertices];
  float** terr = terrain->getHeightMap();
  std::list<Road*>* nodeRoads = node->roads;
  int numRoads = nodeRoads->size();
//  if (numRoads == 1) // 1 - blind valley, 0 - should not happen
//  {
//    nodeRoads->push_back(nodeRoads->front());
//    delete [] nodeVertices;
//    return NULL; // workaround for now
//    return nodeVertices;
//  }

  std::list<Road*>::iterator nodeRoadsIt = nodeRoads->begin();
  Road* clockwiseRoad = *nodeRoadsIt;
  std::list<Road*>* tmpRoadList = new std::list<Road*>();
  float* here = new float [2];
  here[0] = node->x;
  here[1] = node->y;
  float* otherPoint = new float [2];

  int i = 0;
  while (clockwiseRoad != NULL)
  {
    std::list<float*>* curvature = clockwiseRoad->curvature;
    int curve = curvature->size();
    if (curve == 0)
    {
      Node* otherEndOfRoad = getOtherEndOfRoad(node, clockwiseRoad);
      otherPoint[0] = otherEndOfRoad->x;
      otherPoint[1] = otherEndOfRoad->y;
    }
    else
    {
      if (clockwiseRoad->from == node)
      {
        float* tmp = (*curvature->begin());
        otherPoint[0] = tmp[0];
        otherPoint[1] = tmp[1];
      }
      else
      {
        float* tmp = (*curvature->rbegin());
        otherPoint[0] = tmp[0];
        otherPoint[1] = tmp[1];
      }
    }
    float* normalVector = getLeftNormalVector(here,otherPoint);
    normalizeVectorN(normalVector,2);

    float* vectP1P2 = new float [2];
    vectP1P2[0] = otherPoint[0] - here[0];
    vectP1P2[1] = otherPoint[1] - here[1];
    normalizeVectorN(vectP1P2,2);

    float p1x = here[0] - normalVector[0] * WIDTH_OF_ROAD + vectP1P2[0] * SIZE_OF_NODE;
    float p1y = here[1] - normalVector[1] * WIDTH_OF_ROAD + vectP1P2[1] * SIZE_OF_NODE;
    float p1z = bilinearInterpolation(terr,p1x,p1y,terrain->getWidth(),terrain->getHeight());
    float p2x = here[0] + normalVector[0] * WIDTH_OF_ROAD + vectP1P2[0] * SIZE_OF_NODE;
    float p2y = here[1] + normalVector[1] * WIDTH_OF_ROAD + vectP1P2[1] * SIZE_OF_NODE;
    float p2z = bilinearInterpolation(terr,p2x,p2y,terrain->getWidth(),terrain->getHeight());
    float p12zh;
    if (p1z > p2z)
    {
      p12zh = p1z + HEIGHT_OF_ROAD;
    }
    else
    {
      p12zh = p2z + HEIGHT_OF_ROAD;
    }

    nodeVertices[i] = p1x;
    nodeVertices[i+2] = p1y;
    nodeVertices[i+1] = p12zh;
    i += lengthOfVertex;

    nodeVertices[i] = p1x;
    nodeVertices[i+2] = p1y;
    nodeVertices[i+1] = p1z;
    i += lengthOfVertex;

    nodeVertices[i] = p2x;
    nodeVertices[i+2] = p2y;
    nodeVertices[i+1] = p12zh;
    i += lengthOfVertex;

    nodeVertices[i] = p2x;
    nodeVertices[i+2] = p2y;
    nodeVertices[i+1] = p2z;
    i += lengthOfVertex;

    tmpRoadList->push_back(clockwiseRoad);
    float angle = 360.0;
    clockwiseRoad = leftRoad2(node, tmpRoadList, &angle);
    if (numRoads == 1)
    {
      float tmp1x = here[0] + normalVector[0] * WIDTH_OF_ROAD - vectP1P2[0] * SIZE_OF_NODE;
      float tmp1y = here[1] + normalVector[1] * WIDTH_OF_ROAD - vectP1P2[1] * SIZE_OF_NODE;
      float tmp1z = bilinearInterpolation(terr,tmp1x,tmp1y,terrain->getWidth(),terrain->getHeight());
      float tmp2x = here[0] - normalVector[0] * WIDTH_OF_ROAD - vectP1P2[0] * SIZE_OF_NODE;
      float tmp2y = here[1] - normalVector[1] * WIDTH_OF_ROAD - vectP1P2[1] * SIZE_OF_NODE;
      float tmp2z = bilinearInterpolation(terr,tmp2x,tmp2y,terrain->getWidth(),terrain->getHeight());
      float tmp12zh;
      if (p1z > p2z)
      {
        tmp12zh = tmp1z + HEIGHT_OF_ROAD;
      }
      else
      {
        tmp12zh = tmp2z + HEIGHT_OF_ROAD;
      }

      nodeVertices[i] = tmp1x;
      nodeVertices[i+2] = tmp1y;
      nodeVertices[i+1] = tmp12zh;
      i += lengthOfVertex;

      nodeVertices[i] = tmp1x;
      nodeVertices[i+2] = tmp1y;
      nodeVertices[i+1] = tmp1z;
      i += lengthOfVertex;

      nodeVertices[i] = tmp2x;
      nodeVertices[i+2] = tmp2y;
      nodeVertices[i+1] = tmp12zh;
      i += lengthOfVertex;

      nodeVertices[i] = tmp2x;
      nodeVertices[i+2] = tmp2y;
      nodeVertices[i+1] = tmp2z;
      i += lengthOfVertex;
    }
  }

  nodeVertices[i] = node->x;
  nodeVertices[i+2] = node->y;
  nodeVertices[i+1] = bilinearInterpolation(terr,node->x,node->y,terrain->getWidth(),terrain->getHeight()) + HEIGHT_OF_ROAD;
  return nodeVertices;
}

GLuint* createElementsFromNode(Node* node, int numberOfElements)
{
  GLuint* nodeElements = new GLuint[numberOfElements];
  int i = 0;
  std::list<Road*>* tmpList = node->roads;
  int numRoads = tmpList->size();
  if (numRoads == 1)
  {
    numRoads++;
  }
  GLuint j = 0;
  for (; i < (numRoads*2-1)*6; i+=6, j+=2)
  {
    nodeElements[i] = j;
    nodeElements[i+1] = j+1;
    nodeElements[i+2] = j+2;
    nodeElements[i+3] = j+1;
    nodeElements[i+4] = j+3;
    nodeElements[i+5] = j+2;
  }
  nodeElements[i] = j;
  nodeElements[i+1] = j+1;
  nodeElements[i+2] = 0;
  nodeElements[i+3] = j+1;
  nodeElements[i+4] = 1;
  nodeElements[i+5] = 0;
  i += 6;

  GLuint middlePoint = numRoads*4;

  GLuint m = 0;
  for (int k = 0; k < 2*numRoads-1; k++, m += 2)
  {
    nodeElements[i++] = m;
    nodeElements[i++] = middlePoint;
    nodeElements[i++] = m+2;
  }
  nodeElements[i++] = m;
  nodeElements[i++] = middlePoint;
  nodeElements[i++] = 0;
//  nodeElements[i++] = 0;
//  nodeElements[i++] = middlePoint;
//  nodeElements[i++] = 2;
//
//  nodeElements[i++] = 2;
//  nodeElements[i++] = 4;
//  nodeElements[i++] = 0;
//  nodeElements[i++] = 6;
//
//  GLuint small = 8;
//  GLuint big = numRoads*4-2;
//  while (big > small)
//  {
//    nodeElements[i++] = big;
//    nodeElements[i++] = small;
//    big -= 2;
//    small += 2;
//  }
//  nodeElements[i++] = PRIMITIVE_RESTART;
  return nodeElements;
}

GLfloat* createVerticesFromRoad(Road* road, Terrain* terrain, int numberOfVertices, int lengthOfVertex)
{
  GLfloat* roadVertices = new GLfloat[numberOfVertices];
  //roads
  // correct settings?
//  const float widthOfRoad = 0.006;
//  const float heightOfRoad = 0.001;
  // /correct settings?
//  const float widthOfRoad = 0.08;
//  const float heightOfRoad = 0.005;
//
//  const float sizeOfNode = 0.08;
  float** terr = terrain->getHeightMap();
  std::list<float*>* curvature = road->curvature;
  int curve = curvature->size();

  if (curve == 0) // numberOfVertices == 288
  {
    float* point1 = new float [2];//smaznout ???prejmenovat point1/2 na from/to???
    point1[0] = road->from->x;
    point1[1] = road->from->y;
    float* point2 = new float[2];
    point2[0] = road->to->x;
    point2[1] = road->to->y;

    float* normalVector = getLeftNormalVector(point1,point2);
    float normOfNormalVector = sizeOfVectorN (normalVector,2); //create function getNormalizedVector
    normalVector[0] /= normOfNormalVector;
    normalVector[1] /= normOfNormalVector;

    float* vectP1P2 = new float [2];
    vectP1P2[0] = point2[0] - point1[0];
    vectP1P2[1] = point2[1] - point1[1];
//    float v12 = sizeOfVectorN (vectP1P2,2); //create function getNormalizedVector
//    vectP1P2[0] /= v12;
//    vectP1P2[1] /= v12;
    normalizeVectorN(vectP1P2,2);

    float p1x = point1[0] + normalVector[0]*WIDTH_OF_ROAD + vectP1P2[0] * SIZE_OF_NODE;
    float p1y = point1[1] + normalVector[1]*WIDTH_OF_ROAD + vectP1P2[1] * SIZE_OF_NODE;
    float p1z = bilinearInterpolation(terr,p1x,p1y,terrain->getWidth(),terrain->getHeight());
    float p2x = point1[0] - normalVector[0]*WIDTH_OF_ROAD + vectP1P2[0] * SIZE_OF_NODE;
    float p2y = point1[1] - normalVector[1]*WIDTH_OF_ROAD + vectP1P2[1] * SIZE_OF_NODE;
    float p2z = bilinearInterpolation(terr,p2x,p2y,terrain->getWidth(),terrain->getHeight());
    float p12zh;
    if (p1z > p2z)
    {
//      p2z = p1z;
      p12zh = p1z + HEIGHT_OF_ROAD;
    }
    else
    {
//      p1z = p2z;
      p12zh = p2z + HEIGHT_OF_ROAD;
    }
//    roadVertices[0] = p1x;
//    roadVertices[2] = p1y;
//    roadVertices[1] = p1z;
//
//    roadVertices[lengthOfVertex] = p2x;
//    roadVertices[lengthOfVertex+2] = p2y;
//    roadVertices[lengthOfVertex+1] = p2z;
//
//    roadVertices[2*lengthOfVertex] = p1x;
//    roadVertices[2*lengthOfVertex+2] = p1y;
////    roadVertices[2*lengthOfVertex+1] = p1z + heightOfRoad;
//    roadVertices[2*lengthOfVertex+1] = p12zh;
//
//    roadVertices[3*lengthOfVertex] = p2x;
//    roadVertices[3*lengthOfVertex+2] = p2y;
//    roadVertices[3*lengthOfVertex+1] = p2z;
//
//    roadVertices[4*lengthOfVertex] = p1x;
//    roadVertices[4*lengthOfVertex+2] = p1y;
////    roadVertices[4*lengthOfVertex+1] = p1z + heightOfRoad;
//    roadVertices[4*lengthOfVertex+1] = p12zh;
//
//    roadVertices[5*lengthOfVertex] = p2x;
//    roadVertices[5*lengthOfVertex+2] = p2y;
////    roadVertices[5*lengthOfVertex+1] = p2z + heightOfRoad;
//    roadVertices[5*lengthOfVertex+1] = p12zh;
    int i = 0;//6*lengthOfVertex;

    float p3x = point2[0] + normalVector[0]*WIDTH_OF_ROAD - vectP1P2[0] * SIZE_OF_NODE;
    float p3y = point2[1] + normalVector[1]*WIDTH_OF_ROAD - vectP1P2[1] * SIZE_OF_NODE;
    float p3z = bilinearInterpolation(terr,p3x,p3y,terrain->getWidth(),terrain->getHeight());
    float p4x = point2[0] - normalVector[0]*WIDTH_OF_ROAD - vectP1P2[0] * SIZE_OF_NODE;
    float p4y = point2[1] - normalVector[1]*WIDTH_OF_ROAD - vectP1P2[1] * SIZE_OF_NODE;
    float p4z = bilinearInterpolation(terr,p4x,p4y,terrain->getWidth(),terrain->getHeight());
    float p34zh;
    if (p3z > p4z)
    {
//      p4z = p3z;
      p34zh = p3z + HEIGHT_OF_ROAD;
    }
    else
    {
//      p3z = p4z;
      p34zh = p4z + HEIGHT_OF_ROAD;
    }

    //left
    roadVertices[i] = p1x;
    roadVertices[i+2] = p1y;
    roadVertices[i+1] = p1z;
    i += lengthOfVertex;

    roadVertices[i] = p1x;
    roadVertices[i+2] = p1y;
//    roadVertices[i+7] = p1z + heightOfRoad;
    roadVertices[i+1] = p12zh;
    i += lengthOfVertex;

    roadVertices[i] = p2x;
    roadVertices[i+2] = p2y;
//    roadVertices[i+43] = p3z + heightOfRoad;
    roadVertices[i+1] = p12zh;
    i += lengthOfVertex;

    roadVertices[i] = p2x;
    roadVertices[i+2] = p2y;
//    roadVertices[i+85] = p4z + heightOfRoad;
    roadVertices[i+1] = p2z;
    i += lengthOfVertex;

//    roadVertices[i+12] = p3x;
//    roadVertices[i+14] = p3y;
//    roadVertices[i+13] = p3z;
//
//    roadVertices[i+18] = p1x;
//    roadVertices[i+20] = p1y;
////    roadVertices[i+19] = p1z + heightOfRoad;
//    roadVertices[i+19] = p12zh;

    roadVertices[i] = p3x;
    roadVertices[i+2] = p3y;
    roadVertices[i+1] = p3z;
    i += lengthOfVertex;
//
    roadVertices[i] = p3x;
    roadVertices[i+2] = p3y;
//    roadVertices[i+31] = p3z + heightOfRoad;
    roadVertices[i+1] = p34zh;
    i += lengthOfVertex;

//  //middle
    roadVertices[i] = p4x;
    roadVertices[i+2] = p4y;
//    roadVertices[i+37] = p1z + heightOfRoad;
    roadVertices[i+1] = p34zh;
    i += lengthOfVertex;
//
    roadVertices[i] = p4x;
    roadVertices[i+2] = p4y;
//    roadVertices[i+49] = p2z + heightOfRoad;
    roadVertices[i+1] = p4z;
    i += lengthOfVertex;
//
//    roadVertices[i+30] = p3x;
//    roadVertices[i+32] = p3y;
////    roadVertices[i+31] = p3z + heightOfRoad;
//    roadVertices[i+31] = p34zh;

//    roadVertices[i+30] = p3x;
//    roadVertices[i+32] = p3y;
//    roadVertices[i+31] = p3z;
//



//    roadVertices[i+54] = p3x;
//    roadVertices[i+56] = p3y;
////    roadVertices[i+49] = p2z + heightOfRoad;
//    roadVertices[i+55] = p34zh;
//
//    roadVertices[i+60] = p2x;
//    roadVertices[i+62] = p2y;
////    roadVertices[i+43] = p3z + heightOfRoad;
//    roadVertices[i+61] = p12zh;
//
//    roadVertices[i+66] = p1x;
//    roadVertices[i+68] = p1y;
////    roadVertices[i+67] = p4z + heightOfRoad;
//    roadVertices[i+67] = p12zh;
//
//
//  //right
//
//    roadVertices[i+72] = p4x;
//    roadVertices[i+74] = p4y;
////    roadVertices[i+49] = p2z + heightOfRoad;
//    roadVertices[i+73] = p4z;
//
//    roadVertices[i+78] = p4x;
//    roadVertices[i+80] = p4y;
//    roadVertices[i+79] = p34zh;

//    roadVertices[i+90] = p4x;
//    roadVertices[i+92] = p4y;
//    roadVertices[i+91] = p34zh;
//
//    roadVertices[i+96] = p2x;
//    roadVertices[i+98] = p2y;
////    roadVertices[i+97] = p4z + heightOfRoad;
//    roadVertices[i+97] = p2z;
//
//    roadVertices[i+102] = p2x;
//    roadVertices[i+104] = p2y;
//    roadVertices[i+103] = p12zh;
//    i+=108;

//    roadVertices[i] = p3x;
//    roadVertices[i+2] = p3y;
//    roadVertices[i+1] = p3z;
//
//    roadVertices[i+6] = p3x;
//    roadVertices[i+8] = p3y;
////    roadVertices[i+7] = p3z + heightOfRoad;
//    roadVertices[i+7] = p34zh;
//
//    roadVertices[i+12] = p4x;
//    roadVertices[i+14] = p4y;
//    roadVertices[i+13] = p4z;
//
//    roadVertices[i+18] = p4x;
//    roadVertices[i+20] = p4y;
//    roadVertices[i+19] = p4z;
//
//    roadVertices[i+24] = p3x;
//    roadVertices[i+26] = p3y;
////    roadVertices[i+25] = p3z + heightOfRoad;
//    roadVertices[i+25] = p34zh;
//
//    roadVertices[i+30] = p4x;
//    roadVertices[i+32] = p4y;
////    roadVertices[i+31] = p4z + heightOfRoad;
//    roadVertices[i+31] = p34zh;

    return roadVertices;
  }
//  if (curve == 1)
//  {
//    return NULL;
//  }
  std::list<float*>::iterator curvatureIt = curvature->begin();
  //pridat podminku na nulovou curvature

  float* point1 = new float [2];//smaznout ???prejmenovat point1/2 na from/to???
  point1[0] = road->from->x;
  point1[1] = road->from->y;
//  float* point2 = new float[2];
//  point2[0] = road->to->x;
//  point2[1] = road->to->y;
  float* point2 = *curvatureIt;

  float* normalVector = getLeftNormalVector(point1,point2);
  normalizeVectorN(normalVector,2);
//  float normOfNormalVector = sizeOfVectorN (normalVector,2); //create function getNormalizedVector
//  normalVector[0] /= normOfNormalVector;
//  normalVector[1] /= normOfNormalVector;

  float* vectP1P2 = new float [2];
  vectP1P2[0] = point2[0] - point1[0];
  vectP1P2[1] = point2[1] - point1[1];
  normalizeVectorN(vectP1P2,2);
//  float v12 = sizeOfVectorN (vectP1P2,2); //create function getNormalizedVector
//  vectP1P2[0] /= v12;
//  vectP1P2[1] /= v12;

  float p1x = point1[0] + normalVector[0]*WIDTH_OF_ROAD + vectP1P2[0] * SIZE_OF_NODE;
  float p1y = point1[1] + normalVector[1]*WIDTH_OF_ROAD + vectP1P2[1] * SIZE_OF_NODE;
  float p1z = bilinearInterpolation(terr,p1x,p1y,terrain->getWidth(),terrain->getHeight());
  float p2x = point1[0] - normalVector[0]*WIDTH_OF_ROAD + vectP1P2[0] * SIZE_OF_NODE;
  float p2y = point1[1] - normalVector[1]*WIDTH_OF_ROAD + vectP1P2[1] * SIZE_OF_NODE;
  float p2z = bilinearInterpolation(terr,p2x,p2y,terrain->getWidth(),terrain->getHeight());
  float p12zh;
  if (p1z > p2z)
  {
//      p2z = p1z;
    p12zh = p1z + HEIGHT_OF_ROAD;
  }
  else
  {
//      p1z = p2z;
    p12zh = p2z + HEIGHT_OF_ROAD;
  }

  int i = 0;
  roadVertices[0] = p1x;
  roadVertices[2] = p1y;
  roadVertices[1] = p1z;

//  roadVertices[lengthOfVertex] = p2x;
//  roadVertices[lengthOfVertex+2] = p2y;
//  roadVertices[lengthOfVertex+1] = p2z;

  roadVertices[1*lengthOfVertex] = p1x;
  roadVertices[1*lengthOfVertex+2] = p1y;
  roadVertices[1*lengthOfVertex+1] = p12zh;

  roadVertices[2*lengthOfVertex] = p2x;
  roadVertices[2*lengthOfVertex+2] = p2y;
  roadVertices[2*lengthOfVertex+1] = p12zh;

  roadVertices[3*lengthOfVertex] = p2x;
  roadVertices[3*lengthOfVertex+2] = p2y;
  roadVertices[3*lengthOfVertex+1] = p2z;

//  roadVertices[5*lengthOfVertex] = p2x;
//  roadVertices[5*lengthOfVertex+2] = p2y;
//  roadVertices[5*lengthOfVertex+1] = p2z + heightOfRoad;

//  int i = 0;//6*lengthOfVertex;
//  float p3x = 0.0, p3y = 0.0, p4x = 0.0, p4y = 0.0, p34zh=0.0;
/*one
  for (int numCurve = 0; numCurve < curve+1; numCurve++, i+=108)
  {
    float p3x = point2[0] + normalVector[0]*widthOfRoad;
    float p3y = point2[1] + normalVector[1]*widthOfRoad;
    float p3z = bilinearInterpolation(terr,p3x,p3y,terrain->getWidth(),terrain->getHeight());
    float p4x = point2[0] - normalVector[0]*widthOfRoad;
    float p4y = point2[1] - normalVector[1]*widthOfRoad;
    float p4z = bilinearInterpolation(terr,p4x,p4y,terrain->getWidth(),terrain->getHeight());
    float p34zh;
    if (p3z > p4z)
    {
//      p4z = p3z;
      p34zh = p3z + heightOfRoad;
    }
    else
    {
//      p3z = p4z;
      p34zh = p4z + heightOfRoad;
    }
  //left
    roadVertices[i] = p1x;
    roadVertices[i+2] = p1y;
    roadVertices[i+1] = p1z;

    roadVertices[i+6] = p1x;
    roadVertices[i+8] = p1y;
//    roadVertices[i+7] = p1z + heightOfRoad;
    roadVertices[i+7] = p12zh;

    roadVertices[i+12] = p3x;
    roadVertices[i+14] = p3y;
    roadVertices[i+13] = p3z;

    roadVertices[i+18] = p1x;
    roadVertices[i+20] = p1y;
//    roadVertices[i+19] = p1z + heightOfRoad;
    roadVertices[i+19] = p12zh;

    roadVertices[i+24] = p3x;
    roadVertices[i+26] = p3y;
    roadVertices[i+25] = p3z;

    roadVertices[i+30] = p3x;
    roadVertices[i+32] = p3y;
//    roadVertices[i+31] = p3z + heightOfRoad;
    roadVertices[i+31] = p34zh;
  //middle
    roadVertices[i+36] = p1x;
    roadVertices[i+38] = p1y;
//    roadVertices[i+37] = p1z + heightOfRoad;
    roadVertices[i+37] = p12zh;

    roadVertices[i+42] = p3x;
    roadVertices[i+44] = p3y;
//    roadVertices[i+43] = p3z + heightOfRoad;
    roadVertices[i+43] = p34zh;

    roadVertices[i+48] = p2x;
    roadVertices[i+50] = p2y;
//    roadVertices[i+49] = p2z + heightOfRoad;
    roadVertices[i+49] = p12zh;

    roadVertices[i+54] = p2x;
    roadVertices[i+56] = p2y;
//    roadVertices[i+49] = p2z + heightOfRoad;
    roadVertices[i+55] = p12zh;

    roadVertices[i+60] = p3x;
    roadVertices[i+62] = p3y;
//    roadVertices[i+43] = p3z + heightOfRoad;
    roadVertices[i+61] = p34zh;

    roadVertices[i+66] = p4x;
    roadVertices[i+68] = p4y;
//    roadVertices[i+67] = p4z + heightOfRoad;
    roadVertices[i+67] = p34zh;
  //right
    roadVertices[i+72] = p2x;
    roadVertices[i+74] = p2y;
    roadVertices[i+73] = p2z;

    roadVertices[i+78] = p2x;
    roadVertices[i+80] = p2y;
//    roadVertices[i+49] = p2z + heightOfRoad;
    roadVertices[i+79] = p12zh;

    roadVertices[i+84] = p4x;
    roadVertices[i+86] = p4y;
//    roadVertices[i+85] = p4z + heightOfRoad;
    roadVertices[i+85] = p34zh;

    roadVertices[i+90] = p2x;
    roadVertices[i+92] = p2y;
    roadVertices[i+91] = p2z;

    roadVertices[i+96] = p4x;
    roadVertices[i+98] = p4y;
//    roadVertices[i+97] = p4z + heightOfRoad;
    roadVertices[i+97] = p34zh;

    roadVertices[i+102] = p4x;
    roadVertices[i+104] = p4y;
    roadVertices[i+103] = p4z;

    curvatureIt++;
    point1 = point2;
    if (curvatureIt == curvature->end())
    {
      point2 = new float[2];
      point2[0] = road->to->x;
      point2[1] = road->to->y;
      i+=108;
      break;
    }
    else
    {
      point2 = *curvatureIt;
    }
    normalVector = getLeftNormalVector(point1,point2);
    normOfNormalVector = sizeOfVectorN (normalVector,2); //create function getNormovanyvector
    normalVector[0] /= normOfNormalVector;
    normalVector[1] /= normOfNormalVector;
    p1x = point1[0] + normalVector[0]*widthOfRoad;
    p1y = point1[1] + normalVector[1]*widthOfRoad;
    p1z = bilinearInterpolation(terr,p1x,p1y,terrain->getWidth(),terrain->getHeight());
    p2x = point1[0] - normalVector[0]*widthOfRoad;
    p2y = point1[1] - normalVector[1]*widthOfRoad;
    p2z = bilinearInterpolation(terr,p2x,p2y,terrain->getWidth(),terrain->getHeight());
    if (p1z > p2z)
    {
  //      p2z = p1z;
      p12zh = p1z + heightOfRoad;
    }
    else
    {
  //      p1z = p2z;
      p12zh = p2z + heightOfRoad;
    }
  }
one*/
//  /*two

  i = 4 * lengthOfVertex;
  float* previousNormalVector = normalVector;

  for (int curveSize = 0; curveSize < curvature->size(); curveSize++/*, i+=24*/)
  {
    curvatureIt++;
    float* point3;
    if (curvatureIt == curvature->end())
    {
      point3 = new float[2];
      point3[0] = road->to->x;
      point3[1] = road->to->y;
//      i+=24;
//      break;
    }
    else
    {
      point3 = *curvatureIt;
    }
    float* newNormalVector = getLeftNormalVector(point2,point3);
    float normOfNormalVector = sizeOfVectorN (newNormalVector,2); //create function getNormalizedVector
    newNormalVector[0] /= normOfNormalVector;
    newNormalVector[1] /= normOfNormalVector;

    float margin0 = (previousNormalVector[0]*WIDTH_OF_ROAD + newNormalVector[0]*WIDTH_OF_ROAD) / 2.0;
    float margin1 = (previousNormalVector[1]*WIDTH_OF_ROAD + newNormalVector[1]*WIDTH_OF_ROAD) / 2.0;
    float p3x = point2[0] + margin0;
    float p3y = point2[1] + margin1;
    float p3z = bilinearInterpolation(terr,p3x,p3y,terrain->getWidth(),terrain->getHeight());
    float p4x = point2[0] - margin0;
    float p4y = point2[1] - margin1;
    float p4z = bilinearInterpolation(terr,p4x,p4y,terrain->getWidth(),terrain->getHeight());
    float p34zh;
    if (p3z > p4z)
    {
//      p4z = p3z;
      p34zh = p3z + HEIGHT_OF_ROAD;
    }
    else
    {
//      p3z = p4z;
      p34zh = p4z + HEIGHT_OF_ROAD;
    }

    //left
//    roadVertices[i] = p1x;
//    roadVertices[i+2] = p1y;
//    roadVertices[i+1] = p1z;
//
//    roadVertices[i+6] = p1x;
//    roadVertices[i+8] = p1y;
////    roadVertices[i+7] = p1z + heightOfRoad;
//    roadVertices[i+7] = p12zh;

    roadVertices[i] = p3x;
    roadVertices[i+2] = p3y;
    roadVertices[i+1] = p3z;
    i += lengthOfVertex;
//    roadVertices[i+6] = p1x;
//    roadVertices[i+8] = p1y;
////    roadVertices[i+19] = p1z + heightOfRoad;
//    roadVertices[i+7] = p12zh;

//    roadVertices[i+24] = p3x;
//    roadVertices[i+26] = p3y;
//    roadVertices[i+25] = p3z;
//
//    roadVertices[i+30] = p3x;
//    roadVertices[i+32] = p3y;
////    roadVertices[i+31] = p3z + heightOfRoad;
//    roadVertices[i+31] = p34zh;

//    roadVertices[i+30] = p3x;
//    roadVertices[i+32] = p3y;
//    roadVertices[i+31] = p3z;

    roadVertices[i] = p3x;
    roadVertices[i+2] = p3y;
//    roadVertices[i+31] = p3z + heightOfRoad;
    roadVertices[i+1] = p34zh;
    i += lengthOfVertex;

  //middle
    roadVertices[i] = p4x;
    roadVertices[i+2] = p4y;
//    roadVertices[i+37] = p1z + heightOfRoad;
    roadVertices[i+1] = p34zh;
    i += lengthOfVertex;

//    roadVertices[i+42] = p3x;
//    roadVertices[i+44] = p3y;
////    roadVertices[i+49] = p2z + heightOfRoad;
//    roadVertices[i+43] = p34zh;
//
//    roadVertices[i+48] = p2x;
//    roadVertices[i+50] = p2y;
////    roadVertices[i+43] = p3z + heightOfRoad;
//    roadVertices[i+49] = p12zh;



//    roadVertices[i+54] = p3x;
//    roadVertices[i+56] = p3y;
////    roadVertices[i+49] = p2z + heightOfRoad;
//    roadVertices[i+55] = p34zh;
//
//    roadVertices[i+60] = p2x;
//    roadVertices[i+62] = p2y;
////    roadVertices[i+43] = p3z + heightOfRoad;
//    roadVertices[i+61] = p12zh;
//
//    roadVertices[i+66] = p1x;
//    roadVertices[i+68] = p1y;
////    roadVertices[i+67] = p4z + heightOfRoad;
//    roadVertices[i+67] = p12zh;


  //right

    roadVertices[i] = p4x;
    roadVertices[i+2] = p4y;
//    roadVertices[i+49] = p2z + heightOfRoad;
    roadVertices[i+1] = p4z;
    i += lengthOfVertex;

//    roadVertices[i+78] = p4x;
//    roadVertices[i+80] = p4y;
//    roadVertices[i+79] = p34zh;
//
//    roadVertices[i+84] = p2x;
//    roadVertices[i+86] = p2y;
////    roadVertices[i+85] = p4z + heightOfRoad;
//    roadVertices[i+85] = p2z;
//
//    roadVertices[i+90] = p4x;
//    roadVertices[i+92] = p4y;
//    roadVertices[i+91] = p34zh;
//
//    roadVertices[i+96] = p2x;
//    roadVertices[i+98] = p2y;
////    roadVertices[i+97] = p4z + heightOfRoad;
//    roadVertices[i+97] = p2z;
//
//    roadVertices[i+102] = p2x;
//    roadVertices[i+104] = p2y;
//    roadVertices[i+103] = p12zh;

    previousNormalVector = newNormalVector;
    //
    point1 = point2;
    //
    point2 = point3;
    p1x = p3x;
    p1y = p3y;
    p1z = p3z;
    p2x = p4x;
    p2y = p4y;
    p2z = p4z;
    p12zh = p34zh;
  }
//two*/
//  point1 = point2;
//  point2 = new float[2];
//  point2[0] = road->to->x;
//  point2[1] = road->to->y;
//  i+=24;
//  float* vectP1P2 = new float [2];

//  point1 = *curvature->rbegin();
//  point2[0] = road->to->x;
//  point2[1] = road->to->y;
  normalVector = previousNormalVector;
  vectP1P2[0] = point2[0] - point1[0];
  vectP1P2[1] = point2[1] - point1[1];
  normalizeVectorN(vectP1P2,2);
//  v12 = sizeOfVectorN (vectP1P2,2); //create function getNormalizedVector
//  vectP1P2[0] /= v12;
//  vectP1P2[1] /= v12;

  float p3x = point2[0] + normalVector[0]*WIDTH_OF_ROAD - vectP1P2[0] * SIZE_OF_NODE;
  float p3y = point2[1] + normalVector[1]*WIDTH_OF_ROAD - vectP1P2[1] * SIZE_OF_NODE;
  float p3z = bilinearInterpolation(terr,p3x,p3y,terrain->getWidth(),terrain->getHeight());
  float p4x = point2[0] - normalVector[0]*WIDTH_OF_ROAD - vectP1P2[0] * SIZE_OF_NODE;
  float p4y = point2[1] - normalVector[1]*WIDTH_OF_ROAD - vectP1P2[1] * SIZE_OF_NODE;
  float p4z = bilinearInterpolation(terr,p4x,p4y,terrain->getWidth(),terrain->getHeight());
  float p34zh;
  if (p3z > p4z)
  {
//      p4z = p3z;
    p34zh = p3z + HEIGHT_OF_ROAD;
  }
  else
  {
//      p3z = p4z;
    p34zh = p4z + HEIGHT_OF_ROAD;
  }

    //left
//    roadVertices[i] = p1x;
//    roadVertices[i+2] = p1y;
//    roadVertices[i+1] = p1z;
//
//    roadVertices[i+6] = p1x;
//    roadVertices[i+8] = p1y;
////    roadVertices[i+7] = p1z + heightOfRoad;
//    roadVertices[i+7] = p12zh;

    roadVertices[i] = p3x;
    roadVertices[i+2] = p3y;
    roadVertices[i+1] = p3z;
    i += lengthOfVertex;

//    roadVertices[i+18] = p1x;
//    roadVertices[i+20] = p1y;
////    roadVertices[i+19] = p1z + heightOfRoad;
//    roadVertices[i+19] = p12zh;

//    roadVertices[i+24] = p3x;
//    roadVertices[i+26] = p3y;
//    roadVertices[i+25] = p3z;
//
//    roadVertices[i+30] = p3x;
//    roadVertices[i+32] = p3y;
////    roadVertices[i+31] = p3z + heightOfRoad;
//    roadVertices[i+31] = p34zh;

//    roadVertices[i+30] = p3x;
//    roadVertices[i+32] = p3y;
//    roadVertices[i+31] = p3z;

    roadVertices[i] = p3x;
    roadVertices[i+2] = p3y;
//    roadVertices[i+31] = p3z + heightOfRoad;
    roadVertices[i+1] = p34zh;
    i += lengthOfVertex;

  //middle
    roadVertices[i] = p4x;
    roadVertices[i+2] = p4y;
//    roadVertices[i+37] = p1z + heightOfRoad;
    roadVertices[i+1] = p34zh;
    i += lengthOfVertex;

//    roadVertices[i+42] = p3x;
//    roadVertices[i+44] = p3y;
////    roadVertices[i+49] = p2z + heightOfRoad;
//    roadVertices[i+43] = p34zh;
//
//    roadVertices[i+48] = p2x;
//    roadVertices[i+50] = p2y;
////    roadVertices[i+43] = p3z + heightOfRoad;
//    roadVertices[i+49] = p12zh;



//    roadVertices[i+54] = p3x;
//    roadVertices[i+56] = p3y;
////    roadVertices[i+49] = p2z + heightOfRoad;
//    roadVertices[i+55] = p34zh;
//
//    roadVertices[i+60] = p2x;
//    roadVertices[i+62] = p2y;
////    roadVertices[i+43] = p3z + heightOfRoad;
//    roadVertices[i+61] = p12zh;
//
//    roadVertices[i+66] = p1x;
//    roadVertices[i+68] = p1y;
////    roadVertices[i+67] = p4z + heightOfRoad;
//    roadVertices[i+67] = p12zh;


  //right

    roadVertices[i] = p4x;
    roadVertices[i+2] = p4y;
//    roadVertices[i+49] = p2z + heightOfRoad;
    roadVertices[i+1] = p4z;
    i += lengthOfVertex;

//    roadVertices[i+78] = p4x;
//    roadVertices[i+80] = p4y;
//    roadVertices[i+79] = p34zh;
//
//    roadVertices[i+84] = p2x;
//    roadVertices[i+86] = p2y;
////    roadVertices[i+85] = p4z + heightOfRoad;
//    roadVertices[i+85] = p2z;
//
//    roadVertices[i+90] = p4x;
//    roadVertices[i+92] = p4y;
//    roadVertices[i+91] = p34zh;
//
//    roadVertices[i+96] = p2x;
//    roadVertices[i+98] = p2y;
////    roadVertices[i+97] = p4z + heightOfRoad;
//    roadVertices[i+97] = p2z;
//
//    roadVertices[i+102] = p2x;
//    roadVertices[i+104] = p2y;
//    roadVertices[i+103] = p12zh;
//  i+=108;

//  roadVertices[i] = p3x;
//  roadVertices[i+2] = p3y;
//  roadVertices[i+1] = p3z;
//
//  roadVertices[i+6] = p3x;
//  roadVertices[i+8] = p3y;
////  roadVertices[i+7] = p3z + heightOfRoad;
//  roadVertices[i+7] = p34zh;
//
//  roadVertices[i+12] = p4x;
//  roadVertices[i+14] = p4y;
//  roadVertices[i+13] = p4z;
//
//  roadVertices[i+18] = p4x;
//  roadVertices[i+20] = p4y;
//  roadVertices[i+19] = p4z;
//
//  roadVertices[i+24] = p3x;
//  roadVertices[i+26] = p3y;
////  roadVertices[i+25] = p3z + heightOfRoad;
//  roadVertices[i+25] = p34zh;
//
//  roadVertices[i+30] = p4x;
//  roadVertices[i+32] = p4y;
////  roadVertices[i+31] = p4z + heightOfRoad;
//  roadVertices[i+31] = p34zh;

//if (i > numberOfVertices)
//{
//  printf("moc %d, %d\n",i,numberOfVertices);
//}

//for (int q=0; q<numberOfVertices; q++)
//{
//  printf("moc %f, %d\n",roadVertices[q]);
//}

  return roadVertices;
}
/* Deprecated
deprecated

Node* findNodeNearestCamera(std::list<Node*> nodes, Camera camera)
{
  float camX = camera.Position.x;
  float camY = camera.Position.z;
  Node* nearestNode = *nodes->begin();
  float nearestDistance = euclideanDistance(camX,camY,nearestNode->x,nearestNode->y);
  Node* node;
  float distance;
  for(std::list<Node*>::iterator nodesIt = nodes->begin(); nodesIt != nodes->end(); nodesIt++)
  {
    node = *nodesIt;
    distance = euclideanDistance(camX,camY,node->x,node->y);
    if (distance < nearestDistance)
    {
      nearestDistance = distance;
      nearestNode = node;
    }
  }
  return nearestNode;
}

void addNewRoadsFromNodes(std::list<Road*>* roads, std::list<Node*>* nodes, std::list<Node*>* borderNodes, float restriction, Camera camera)
{
  float camX = camera.Position.x;
  float camY = camera.Position.z;
  std::list<Node*>* newNodes;
  float nodeDistance;
  for (std::list<Node*>::iterator nodesIt = nodes->begin(); nodesIt != nodes->end(); nodesIt++)
  {
    Node* node = *nodesIt;
    newNodes = addNewRoadsFromNode(roads,node);
    for (std::list<Node*>::iterator newNodesIt = newNodes->begin(); newNodesIt != newNodes->end(); newNodesIt++)
    {
      Node* newNode = *newNodesIt;
      nodeDistance = euclideanDistance(camX,camY,node->x,node->y);
      if (nodeDistance > restriction)
      {
        borderNodes->push_back(*newNodesIt);
      }
      else
      {
        nodes->push_back(*newNodesIt);
      }
    }
  }
}

std::list<Node*>* addNewRoadsFromNode(std::list<Road*>* roads, Node* node) //returns end Nodes
{
  std::list<Node*>* newNodes = new std::list<Node*>();
  for (std::list<Road*>::iterator nodeRoadsIt = node->road.begin(); nodeRoadsIt != node->road.end(); nodeRoadsIt++)
  {
    Road* nodeRoad = *nodeRoadsIt;
    bool found = false;
    for (std::list<Road*>::iterator roadsIt = roads->begin(); roadsIt != roads->end(); roadsIt++)
    {
      Road* road = *roadsIt;
      if (nodeRoad == road)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      roads->push_back(nodeRoad);
      if (nodeRoad->from == node)
      {
        newNodes->push_back(nodeRoad->to);
      }
      else
      {
        newNodes->push_back(nodeRoad->from);
      }
    }
  }
  return newNodes;
}

void addNewBlocksFromRoads(std::list<Block*>* blocks, std::list<Road*>* roads)
{
  for (std::list<Road*>::iterator roadsIt = roads->begin(); roadsIt != roads->end(); roadsIt++)
  {
    Road* road = *roadsIt;
    Block* newBlock = road->firstBlock;
    bool found = false;
    for (std::list<Block*>::iterator blocksIt = blocks->begin(); blocksIt != blocks->end(); blocks++)
    {
      Block* block = *blocksIt;
      if (block == newBlock)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      blocks->push_back(newBlock);
    }

    newBlock = road->secondBlock;
    found = false;
    for (std::list<Block*>::iterator blocksIt = blocks->begin(); blocksIt != blocks->end(); blocks++)
    {
      Block* block = *blocksIt;
      if (block == newBlock)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      blocks->push_back(newBlock);
    }
  }
}

void updateNewRoadsFromNodes(std::list<Road*>* roads, std::list<Node*>* nodes, std::list<Node*>* borderNodes, float restriction, Camera camera)
{
  float camX = camera.Position.x;
  float camY = camera.Position.z;
  Node* node;
  float nodeDistance;
  for (std::list<Node*>::iterator nodesIt = nodes->begin(); nodesIt != nodes->end(); nodesIt++)
  {
    node = *nodesIt;
    nodeDistance = euclideanDistance(camX,camY,node->x,node->y);
    if (nodeDistance > restriction)
    {
      changeNodeToBorderNode(roads,borderNodes,node);//smaze borderNodes, ktery jsou na nej napojeny, odstrani ulice, ktery vednou do borderu a udela z node border
    }
  }
  for (std::list<Node*>::iterator borderNodesIt = borderNodes->begin(); borderNodesIt != borderNodes->end(); borderNodesIt++)
  {
    node = *borderNodesIt;
    nodeDistance = euclideanDistance(camX,camY,node->x,node->y);
    if (nodeDistance > restriction)
    {
      std::list<Node*>* newBorderNodes = addNewRoadsFromNode(roads,node);
      for (std::list<Node*>::iterator newBorderNodesIt = newBorderNodes->begin(); newBorderNodesIt != newBorderNodes->end(); newBorderNodesIt++)
      {
        borderNodes->push_back(*newBorderNodesIt);
        //clear and delete newBorderNodes;
      }
    }
  }
}

void changeNodeToBorderNode(std::list<Road*>* roads, std::list<Node*>* borderNodes, Node* node)
{
  std::list<Road*> nodeRoads = node->road;
  Node* endNode;
  Node* borderNode;
  for (std::list<Road*>::iterator nodeRoadsIt = noderoads->begin(); nodeRoadsIt != noderoads->end(); nodeRoadsIt++)
  {
    Road* nodeRoad = *nodeRoadsIt;
    endNode = nodeRoad->from;
    if (endNode == node)
    {
      endNode = nodeRoad->to;
    }
    for (std::list<Node*>::iterator borderNodeIt = borderNodes->begin(); borderNodeIt != borderNodes->end(); borderNodeIt++)
    {
      borderNode = *borderNodeIt;
      if (endNode == borderNode)
      {
        borderNodes->erase(borderNodeIt);
        for (std::list<Road*>::iterator roadsIt = roads->begin(); roadsIt != roads->end(); roadsIt++)
        {
          if (nodeRoad == (*roadsIt))
          {
            roads->erase(roadsIt);
            break;
          }
        }
        break;
      }
    }
  }
  borderNodes->push_back(node);
}

deprecated
*/

//test
