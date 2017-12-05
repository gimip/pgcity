#include "Terrain.hpp"

#include "utilities.hpp"
#include "terrainTools.hpp"
#include <math.h>
#include "constants.hpp"
#include "graphicUtilities.hpp"

Terrain::Terrain ()
{
  loadRawTerrain("debug/rawTerrain.txt");
//  loadSVGTerrain("svgTerrain.svg",10.0);
//  init(129,129,1);
//  init(1025,1025,1);
//  init(257,257,0.9);
//  init(257,257,1);
//  init(2049,2049,1);
  writeRawTerrain("debug/rawTerrain.txt");
if (debug)
{
  drawSVGterrain(5);
}
}

void choice1 (float** heightMap) // svah
{
  int b = 257;
  for (int i =0; i<b; i++)
  {
    for (int j=0; j<b; j++)
    {
      heightMap[i][j]= 250.0 - (((float) j )/7.0) ;
    }
  }
}

void choice2 (float** heightMap) //udoli
{
  int b = 129;
  for (int i =0; i<b; i++)
  {
    for (int j=0; j<(b+1)/2; j++)
    {
      heightMap[i][j]= 250.0 - ((float) j) - ((float) i) / 6.0;
      heightMap[i][b-j]= 250.0 - ((float) j) - ((float) i) / 6.0;
    }
  }
}

void choice3 (float** heightMap) //kopec
{
  int b = 257;
  for (int i=0; i<b; i++)
  {
    for (int j=0; j<b; j++)
    {
//      heightMap[i][j] = 230.0 - euclideanDistance(i,j,128.0,128.0) * 3.5;
      heightMap[i][j] = 230.0 - euclideanDistance(i,j,80.0,80.0) * 0.5;
      if (heightMap[i][j] < 0.0)
      {
        heightMap[i][j] = 0.0;
      }
    }
  }
}

void choice4 (float** heightMap) //vlnitá krajina
{
  int b = 257;
  for (int i=0; i<b; i++)
  {
    for (int j=0; j<b; j++)
    {
//      heightMap[i][j] = 110.0 - 60 * sin(double(i+j)*2.0*(M_PI/257.0)) * sin(double(i)*2.0*(M_PI/257.0));
      heightMap[i][j] = 110.0 - 15 * sin(double(i+j)*2.0*(M_PI/257.0)) * sin(double(i)*2.0*(M_PI/257.0));
      if (heightMap[i][j] < 0.0)
      {
        heightMap[i][j] = 0.0;
      }
    }
  }
}

Terrain::Terrain (int choice)
{
//  this->height = 129;
  this->height = 513;
//  this->width = 129;
  this->width = 513;
  this->ibu = -1.0;
  dMax = euclideanDistance(0,0,(float)height,(float)width);
  heightMap = new float* [height];
  for (int i=0; i<width; i++)
  {
    heightMap[i] = new float [width];
  }
  if (choice == 1)
  {
    choice1(heightMap);
  }
  if (choice == 2)
  {
    choice2(heightMap);
  }
  if (choice == 3)
  {
    choice3(heightMap);
  }
  if (choice == 4)
  {
    choice4(heightMap);
  }
  if (debug)
  {
    drawSVGterrain(5);
  }
}

Terrain::Terrain (int height, int width, float ibu)
{
  init(height,width,ibu);
}

void Terrain::init (int height, int width, float ibu)
{
  this->height = height;
  this->width = width;
  this->ibu = ibu;
  heightMap = skelet(3, height, width);

  dMax = euclideanDistance(0,0,(float)height,(float)width);
  mDBU(heightMap, height, width, ibu, dMax);
  midD(heightMap, height, width);

  float** oldhmp = heightMap;
  heightMap = new float* [height];
  for (int i=0; i<width; i++)
  {
    heightMap[i] = new float [width];
  }
  for (int i=0; i<width; i++)
  {
   for (int j=0; j<width; j++)
    {
      heightMap[i][j] = oldhmp[i][j];
    }
  }
//  int con2 = 3;
//  float tmp;
//  for (int i=0+con2; i<height-con2; i++)
//  {
//    for (int j=0+con2; j<height-con2; j++)
//    {
//      tmp = 0.0;
//      for (int k=i-con2; k<i+con2; k++)
//      {
//        for (int l=j-con2; l<j+con2; l++)
//        {
//          tmp += oldhmp[k][l];
//        }
//      }
//      heightMap[i][j] = tmp/(float)((2*con2)*(2*con2));
//      density[i][j] = olddensity[i][j];
//    }
//  }

  int filterSize = 11; // need to be odd number
//  heightMap = convolution(createOnesFilter(filterSize),filterSize,oldhmp,height);
//  heightMap = convolution(createGaussianFilter(filterSize,1.0),filterSize,heightMap,height);

 float** heightMap1 = convolutionRestrictedEmpty(createOnesFilter(3),3,oldhmp,height,230.0,300.0);
 float** heightMap2 = convolutionRestrictedEmpty(createOnesFilter(7),7,oldhmp,height,210.0,230.0);
 float** heightMap3 = convolutionRestrictedEmpty(createOnesFilter(11),11,oldhmp,height,0.0,210.0);
// heightMap3 = convolutionRestrictedEmpty(createGaussianFilter(11,1.0),11,heightMap3,height,0.0,210.0);
for (int i=0; i<width; i++)
{
 for (int j=0; j<width; j++)
  {
    heightMap[i][j] = heightMap1[i][j] + heightMap2[i][j] + heightMap3[i][j];
  }
}
  heightMap = convolutionRestrictedFilled(createOnesFilter(5),5,heightMap,height,227.0,233.0);
  heightMap = convolutionRestrictedFilled(createOnesFilter(9),9,heightMap,height,207.0,213.0);

// heightMap = convolutionRestrictedFilled(createOnesFilter(1),1,oldhmp,height,230.0,300.0);
// heightMap = convolutionRestrictedFilled(createOnesFilter(7),7,heightMap,height,210.0,230.0);
// heightMap = convolutionRestrictedFilled(createOnesFilter(11),11,heightMap,height,0.0,210.0);
// heightMap = convolutionRestrictedFilled(createGaussianFilter(11,1.0),11,heightMap,height,0.0,210.0);

}

Terrain::~Terrain ()
{
  for(int i = 0; i < width; i++)
    {
      delete [] heightMap[i];
    }
  delete [] heightMap;
}

float** Terrain::getHeightMap ()
{
  return heightMap;
}

int Terrain::getHeight ()
{
  return height;
}

int Terrain::getWidth ()
{
  return width;
}

void Terrain::drawSVGterrain(int magnify)
{
  FILE* file = fopen("debug/terrain.svg","w+");
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);
  drawSVGterrain(magnify,file);
  fprintf(file,"</svg>");
  fclose(file);
}

void Terrain::drawSVGterrain(int magnify, FILE* file)
{
  float multiply = (float) magnify;

  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      fprintf(file,"\t<rect x=\"%d\" y=\"%d\" width=\"%f\" height=\"%f\" style=\"fill:rgb(%d,%d,%d)\"/>\n",j*magnify,i*magnify,multiply,multiply,(int)heightMap[i][j],(int)heightMap[i][j],(int)heightMap[i][j]);
    }
  }
}

#include <string>
#include <fstream>
#include <stdlib.h>
bool Terrain::loadSVGTerrain(const char filename [], int coefficient) //coefficient is for correct square loading (division)
{
  //for string to float/double = cstdlib/strtod
  std::string line, subString;
  std::ifstream svgFile (filename);
  std::size_t found1, found2;
  int x, y;
  float currentHeight;

  if (svgFile.is_open())
  {
    if (svgFile.good())
    {
      getline (svgFile,line);
      found1 = line.find("height=\"");
      found2 = line.find("\"",found1+8);
      if (found1 != std::string::npos || found2 != std::string::npos)
      {
        subString = line.substr(found1+8,found2-3);
        height = atoi(subString.data()) / coefficient;
      }
      else
      {
        printf("Could not find Height.");
        return false;
      }

      found1 = line.find("width=\"");
      found2 = line.find("\"",found1+7);
      if (found1 != std::string::npos || found2 != std::string::npos)
      {
        subString = line.substr(found1+7,found2-3);
        width = atoi(subString.data()) / coefficient;
      }
      else
      {
        printf("Could not find Width.");
        return false;
      }
    }
    else
    {
      printf("Unable to open file.");
        return false;
    }

    heightMap = new float* [height];
    for (int i=0; i<width; i++)
    {
      heightMap[i] = new float [width];
    }

    while (svgFile.good())
    {
      getline (svgFile,line);
      found1 = line.find("x=\"");
      found2 = line.find("\"",found1+3);
      if (found1 != std::string::npos || found2 != std::string::npos)
      {
        subString = line.substr(found1+3,found2-1);
        x = atoi(subString.data()) / coefficient;
      }
      else
      {
        found1 = line.find("</svg>");
        if (found1 != std::string::npos)
        {
          return true;
        }
        printf("Could not find x.");
        continue;
      }

      found1 = line.find("y=\"");
      found2 = line.find("\"",found1+3);
      if (found1 != std::string::npos || found2 != std::string::npos)
      {
        subString = line.substr(found1+3,found2-1);
        y = atoi(subString.data()) / coefficient;
      }
      else
      {
        printf("Could not find y.");
        continue;
      }

      found1 = line.find("rgb(");
      found2 = line.find(",",found1+4);
      if (found1 != std::string::npos || found2 != std::string::npos)
      {
        subString = line.substr(found1+4,found2-1);
        currentHeight = (float) atoi(subString.data());
      }
      else
      {
        printf("Could not find height.");
        continue;
      }

      heightMap[y][x] = currentHeight;
    }
    svgFile.close();
  }
  else
  {
    printf("Unable to open file.");
    return false;
  }

  return true;
//  this->ibu = ibu;
}

void Terrain::writeRawTerrain(const char filename [])
{
  FILE* file = fopen(filename,"w+");
  fprintf(file,"%d %d %f //width height ibu, below x y height\n", width, height, ibu);
  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      fprintf(file,"%d %d %f \n", i, j, heightMap[i][j]);
    }
  }
  fclose(file);
}

bool Terrain::loadRawTerrain(const char filename [])
{
  //for string to float/double = cstdlib/strtod
  std::string line, subString;
  std::ifstream rawFile (filename);
  std::size_t found1, found2;
  int x, y;
  float terrainHeight;

  if (rawFile.is_open())
  {
    if (rawFile.good())
    {
      getline (rawFile,line);
      found1 = line.find(" ");
      if (found1 != std::string::npos)
      {
        subString = line.substr(0,found1);
        width = atoi(subString.data());
      }
      else
      {
        printf("Could not find Width.");
        return false;
      }

      found2 = line.find(" ",found1+1);
      if (found1 != std::string::npos || found2 != std::string::npos)
      {
        subString = line.substr(found1,found2);
        height = atoi(subString.data());
      }
      else
      {
        printf("Could not find Height.");
        return false;
      }

      found1 = line.find(" ",found2+1);
      if (found1 != std::string::npos || found2 != std::string::npos)
      {
        subString = line.substr(found2,found1);
        ibu = strtod(subString.data(), NULL);
      }
      else
      {
        printf("Could not find ibu.");
        return false;
      }
    }
    else
    {
      printf("Unable to open file.");
        return false;
    }

    heightMap = new float* [height];
    for (int i=0; i<width; i++)
    {
      heightMap[i] = new float [width];
    }

    while (rawFile.good())
    {
      getline (rawFile,line);
      found1 = line.find(" ");
      if (found1 != std::string::npos)
      {
        subString = line.substr(0,found1);
        x = atoi(subString.data());
      }
      else
      {
        if (rawFile.eof())
        {
          break;
        }
        printf("Could not find x");
        continue;
      }

      found2 = line.find(" ",found1+1);
      if (found1 != std::string::npos || found2 != std::string::npos)
      {
        subString = line.substr(found1,found2);
        y = atoi(subString.data());
      }
      else
      {
        printf("Could not find y.");
        continue;
      }

      found1 = line.find(" ",found2+1);
      if (found1 != std::string::npos || found2 != std::string::npos)
      {
        subString = line.substr(found2,found1);
        terrainHeight = strtod(subString.data(), NULL);
      }
      else
      {
        printf("Could not find height.");
        continue;
      }
      heightMap[x][y] = terrainHeight;
    }
    rawFile.close();
  }
  else
  {
    printf("Unable to open file.");
    return false;
  }

  return true;
//  this->ibu = ibu;
}

void Terrain::generateModel(City* city)
{
  (void)(city);

//  int number = terrain->getHeight() * terrain->getWidth() * 8;
  this->modelSize = height * width * 8;
//  GLfloat* terrainVertices = createVerticesFromTerrain(terrain, number, 8);
  this->model = createVerticesFromTerrain(this, this->modelSize, 8);

//  int elementNumber = 2 * terrain->getWidth() * (terrain->getHeight()-1) + terrain->getHeight();
  this->indicesSize = 2 * width * (height - 1) + height;
//  GLuint* terrainElements = createElementsFromTerrain(terrain, elementNumber);
  this->indices = createElementsFromTerrain(this, this->indicesSize);
  addNormalsForStripTriangles(model, modelSize, indices, indicesSize, 3, 8);
  addTexCoordsToTerrain(this, model,modelSize, 6, 8);

//  GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo);
  glGenBuffers(1, &this->ebo);
  // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
  glBindVertexArray(this->vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, modelSize * sizeof(GLfloat), model, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(GLuint), indices, GL_STATIC_DRAW);
  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  // TexCoord attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0); // Unbind VAO
}

//#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
//extern Shader ourShader6;
//extern Camera camera;
extern GLuint screenWidth, screenHeight;
extern float maxFar;
extern GLuint texture1;
extern glm::vec3 lightPos;
extern glm::mat4 lightSpaceMatrix;
extern glm::vec3 bigger;

void Terrain::showModel()
{
//    ourShader6.Use();
//    GLint modelLoc = glGetUniformLocation(ourShader6.Program, "model");
//    glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "view"),1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(camera.GetViewMatrix()))));
//    glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "projection"),1, GL_FALSE, glm::value_ptr(glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, maxFar)));
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, texture1);
////      glUniform1i(glGetUniformLocation(ourShader6.Program, "ourTexture1"), 0);
//
//    GLint objectColorLoc = glGetUniformLocation(ourShader6.Program, "objectColor");
//    GLint lightPosLoc    = glGetUniformLocation(ourShader6.Program, "lightPos");
//    glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
////      GLint lightDirPos = glGetUniformLocation(ourShader6.Program, "lightDirection");
//    GLint viewPosLoc     = glGetUniformLocation(ourShader6.Program, "viewPos");
//    glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);
//    glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
//    glUniform3f(objectColorLoc, 0.8f, 0.5f, 0.25f);
//    // Pass the matrices to the shader
//
//    glBindVertexArray(this->vao);
////      for(GLuint i = 0; i < 1/*0*/; i++)
////      {
//        // Calculate the model matrix for each object and pass it to shader before drawing
//        glm::mat4 modell;
////          model = glm::translate(model, cubePositions[i]);
////            GLfloat angle = 20.0f * i;
////            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
//    modell = glm::scale(modell, bigger);
//        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modell));
//
////            glDrawArrays(GL_TRIANGLES, 0, 36);
////          glDrawArrays(GL_TRIANGLES, 0, number);
//        glDrawElements(GL_TRIANGLE_STRIP, this->indicesSize, GL_UNSIGNED_INT, 0);
//        glBindTexture(GL_TEXTURE_2D, 1);
////            glDrawArrays(GL_LINES, 0, 36);
////      }
}
