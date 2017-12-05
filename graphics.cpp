#include "graphics.hpp"

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif // GLEW_STATIC

//#include "glew/include/GL/glew.h"
#include <GL/glew.h>

//#include "glfw/include/GLFW/glfw3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <list>
#include <vector> // nahradit

#include "Camera.hpp"
#include "Shader.hpp"
#include "Block.hpp"
#include "Building.hpp"
#include "City.hpp"
#include "Land.hpp"
#include "Node.hpp"
#include "Population.hpp"
#include "Road.hpp"
#include "Roadmap.hpp"
#include "Terrain.hpp"
#include "PositionMatrix.hpp"
#include "PositionSquare.hpp"
#include "constants.hpp"
#include "graphicUtilities.hpp"
#include "textureUtilities.hpp"
#include "utilities.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Std. Includes
#include <string>

#include <SOIL/SOIL.h>
//// GLEW
//#define GLEW_STATIC
//#include <GL/glew.h>
//
//// GLFW
//#include <GLFW/glfw3.h>
//
//// GL includes
//#include "Shader.h"
//#include "Camera.h"

// GLM Mathemtics
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

// Other Libs
//#include <SOIL.h>

// Properties
//GLuint screenWidth = 1500, screenHeight = 800;
GLuint screenWidth = 1920, screenHeight = 1080;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();

// Camera
Camera camera(glm::vec3(10.0f, 600.0f, 150.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

glm::vec3 lightPos(10.0f, 650.0f, 100.0f);
glm::vec3 cubePos(-50.0f, 50.0f, 0.0f);
GLfloat lastClick[1024];

const float biggy = 60.0f; // PRI ZMENE BIGGY - hledat // PRI ZMENE BIGGER
glm::vec3 bigger (biggy);  // PRI ZMENE BIGGER
bool showRoads = true;
bool showTerrain = true;
bool showPopulation = false;
bool showNodes = true;
bool showBorders = true;
bool showBlocks = false;
bool showLands = false;
bool updateView = false;
bool showSkybox = true;
//bool full = false;
bool full = false;
bool fixed = false;
std::list<PositionSquare*>* getFullView(PositionMatrix* positionMatrix);
std::list<PositionSquare*>* getView(PositionMatrix* positionMatrix, int squaresAround, float angle, float distance);
void emptyCurrentLinks(std::list<PositionSquare*>* viewList);
void generateModels(std::list<PositionSquare*>* viewList, City* city);
GLuint loadCubemap(std::vector<const GLchar*> faces);

// The MAIN function, from here we start our application and run our Game loop
int graphicsLoop(City* city)
{
  for (int i = 0; i<1024; i++)
  {
    lastClick[i] = 0.0;
  }

  Terrain* terrain = city->getTerrain();
  Population* population = city->getPopulation();
  RoadMap* roadMap = city->getRoadMap();
  Blocks* blocks = city->getBlocks();
//  Lands* lands = city->getLands();
//  std::list<Land*>* pozz = lands->buildingLands;
//  Lands* lands = city->getLands();
//  std::list<Land*>* pozz = lands->vegetationLand;

//  Terrain* terrain = new Terrain();
//  Population* population = new Population (terrain, 50.0, 50.0, 200.0, 5.0);
//  RoadMap* roadMap = new RoadMap (terrain, population);

//  Lands* lands = new Lands (blocks,roadMap->getRoads(),population);
//  lands->drawLands(10,roadMap->getRoads());
//  std::list<Land*>* buildings = lands.buildingLands;

  camera.Position.x = (terrain->getHeight()/2)*biggy;
  camera.Position.y = ((terrain->getHeightMap())[terrain->getHeight()/2][terrain->getHeight()/2] + 30.0)*biggy;
  camera.Position.z = (terrain->getHeight()/2)*biggy;
  lightPos.x = camera.Position.x;
  lightPos.y = camera.Position.y * 5;
  lightPos.z = camera.Position.z;
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

//    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "FPS: ?, One Frame(+-ms): ?", glfwGetPrimaryMonitor(), NULL); // Fullscreen
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "FPS: ?, One Frame(+-ms): ?", NULL, NULL); // Windowed
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW to setup the OpenGL Function pointers
    glewExperimental = GL_TRUE;
    glewInit();

  terrain->generateModel(city);
  population->generateModel(city);


    // Define the viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);

    // Setup some OpenGL options
    //glEnable(GL_MULTISAMPLE); // Enabled by default on some drivers, but not all so always enable to make sure
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); //??? nutne?
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(PRIMITIVE_RESTART);

    // Setup and compile our shaders
    Shader ourShader("shaders/shader1.vs", "shaders/shader1.fs");
//    Shader ourShader2("shader2.vs", "shader2.fs");
    Shader ourShader2("shaders/shader4.vs", "shaders/shader4.fs");
    Shader ourShader5("shaders/shader5.vs", "shaders/shader5.fs");
    Shader ourShader6("shaders/shader6.vs", "shaders/shader6.fs");
    Shader ourShader7("shaders/shader7.vs", "shaders/shader7.fs");

GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,-0.5f,  0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,-0.5f,  0.5f, -0.5f,

        -0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,

         0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,

        -0.5f, -0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,-0.5f,  0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,-0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f,-0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,-0.5f,  0.5f, -0.5f
    };

    GLfloat skyboxVertices[] = {
        // Positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(-60.0f, -250.0f, -160.0f),
        glm::vec3(-60.0f+bigger.x*terrain->getHeight(), -250.0f, -160.0f),
        glm::vec3(-60.0f+bigger.x*terrain->getHeight(), -250.0f, -160.0f+bigger.x*terrain->getHeight()),
    };

//light
    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    GLuint lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, 6*6*6*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    // Set the vertex attributes (only position data for the lamp))
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the normal vectors
//    glEnableVertexAttribArray(0);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // TexCoord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
//light

//skybox
    glBindVertexArray(0);
    Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.fs");
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    // Load textures

    std::vector<const GLchar*> faces;
    faces.push_back("textures/right.jpg");
    faces.push_back("textures/left.jpg");
    faces.push_back("textures/top.jpg");
    faces.push_back("textures/bottom.jpg");
    faces.push_back("textures/back.jpg");
    faces.push_back("textures/front.jpg");
    GLuint cubemapTexture = loadCubemap(faces);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glBindVertexArray(0);

    // Cubemap (Skybox)
//skybox

//texture
GLuint texture1 = loadTextureJPG("textures/terrainTexture2.jpg");
GLuint texture2 = loadTextureJPG("textures/roadTexture2.jpg");
GLuint texture3 = loadTextureJPG("textures/nodeTexture4.jpg");
GLuint texture4 = loadTextureJPG("textures/nodeTexture3.2.jpg");
GLuint texture5 = loadTextureJPG("textures/roofTexture.jpg");

//preProcessing
int squaresAround = 2;
float angle = 60.0f;
float distance = 1000.0;
float sizeOfPositionSquare = 20.0;
PositionMatrix* positionMatrix = new PositionMatrix(city, sizeOfPositionSquare);
//bool full = false;
std::list<PositionSquare*>* viewList;
if (full)
{
viewList = getFullView(positionMatrix);//pohlidat hranice matice uvnitr fce!
}
else
{
viewList = getView(positionMatrix, squaresAround, angle, distance);//pohlidat hranice matice uvnitr fce!
}
std::list<PositionSquare*>* garbage = new std::list<PositionSquare*> ();
//preProcessing


    // Game loop
    double lastTime = glfwGetTime();
    double lastGurbageDump = glfwGetTime();
    int nbFrames = 0;
    char title[50] = "FPS: ?, One Frame(+-ms): ?";
    while(!glfwWindowShouldClose(window))
    {
        // Set frame time
      double currentTime = glfwGetTime();
      nbFrames++;
      if ( currentTime - lastTime >= 1.0 )
      { // If last prinf() was more than 1 sec ago
           // printf and reset timer
//             printf("%f ms/frame\n", 1000.0/double(nbFrames));
           int s = (int)(1000.0/double(nbFrames));
           sprintf(title,"FPS: %d, One Frame(+-ms): %d",nbFrames,s);
           nbFrames = 0;
           lastTime += 1.0;
      }
      glfwSetWindowTitle(window, title);

      GLfloat currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;

      // Check and call events
      glfwPollEvents();
      Do_Movement();


      emptyCurrentLinks(viewList);
      if (updateView)
      {
//        emptyCurrentLinks(viewList);

if (fixed)
{
//std::list<PositionSquare*>* viewList = getFullView(positionMatrix);//pohlidat hranice matice uvnitr fce!
//generateModels(viewList,terrain);
generateModels(viewList, city);
}
else
{
        std::list<PositionSquare*>* newStack;
        if (full)
        {
          newStack = getFullView(positionMatrix);//pohlidat hranice matice uvnitr fce!
        }
        else
        {
          newStack = getView(positionMatrix, squaresAround, angle, distance);//pohlidat hranice matice uvnitr fce!
        }
        for (std::list<PositionSquare*>::iterator newStackIt = newStack->begin(); newStackIt != newStack->end(); newStackIt++)
        {
          PositionSquare* newPS = *newStackIt;
          if (newPS->getTimeToBeDeleted() != 0.0)
          {
            garbage->remove(newPS);
            newPS->setTimeToBeDeleted(0.0);
          }
          else
          {
            bool isInList = false;
            for (std::list<PositionSquare*>::iterator viewListIt = viewList->begin(); viewListIt != viewList->end(); viewListIt++)
            {
              if (newPS == *viewListIt)
              {
                isInList = true;
                break;
              }
            }
            if (!isInList)
            {
              std::list<PositionSquare*>* newStackList = new std::list<PositionSquare*> (1, newPS);
              generateModels(newStackList, city);
            }
          }
        }

        for (std::list<PositionSquare*>::iterator viewListIt = viewList->begin(); viewListIt != viewList->end(); viewListIt++)
        {
          PositionSquare* oldPS = *viewListIt;
          bool isInList = false;
          for (std::list<PositionSquare*>::iterator newStackIt = newStack->begin(); newStackIt != newStack->end(); newStackIt++)
          {
            if (oldPS == *newStackIt)
            {
              isInList = true;
              break;
            }
          }
          if (!isInList)
          {
//            viewListIt = viewList->erase(viewListIt);
            garbage->push_front(oldPS);
            //-> garbage spocte cas vyhozeni
//            oldPS->setTimeToBeDeleted(1.0);
            oldPS->setTimeToBeDeleted(glfwGetTime()+1.0);
            //-> garbage spocte cas vyhozeni
          }
        }

        if (currentTime - lastGurbageDump >= 3.0 )
        {
          for (std::list<PositionSquare*>::iterator garbageIt = garbage->begin(); garbageIt != garbage->end();)
          {
            PositionSquare* psGarbage = *garbageIt;
            if (currentTime > psGarbage->getTimeToBeDeleted())
            {
              psGarbage->deleteModels();
FILE* file = fopen("konstruktDestrukt.txt","a+");
fprintf(file,"\n%f: Odstranuju PositionSquare z garbage", glfwGetTime());
fclose(file);
              garbageIt = garbage->erase(garbageIt);
            }
            else
            {
              garbageIt++;
            }
          }
        }

        viewList->clear(); // problem of newStack pointer?? vyzkouset
        delete viewList; // problem of newStack pointer?? vyzkouset   -- jestli utika pamet?
        viewList = newStack; // problem of newStack pointer?? vyzkouset
}
        updateView = false;
      }
      // Clear the colorbuffer
//        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


//updateNewRoadsFromNodes(roads,nodes,borderNodes,restriction,camera);

/////

        glDepthMask(GL_FALSE);// Remember to turn depth writing off
        skyboxShader.Use();
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
//        glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
        float maxFar = (float) (terrain->getHeight() * terrain->getWidth());
//        glm::mat4 projection = glm::perspective(degreesToRadians(camera.Zoom), (float)screenWidth/(float)screenHeight, 0.1f, maxFar);
        glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, maxFar);
        if (showSkybox)
        {
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
//        glUniform1i(glGetUniformLocation(shader.Program, "skybox"), 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);

/////
      glm::mat4 lightProjection, lightView;
      glm::mat4 lightSpaceMatrix;
      GLfloat near_plane = 1.0f, far_plane = 7.5f;
      lightProjection = glm::ortho(-20.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
      //lightProjection = glm::perspective(45.0f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // Note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene.
      lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
      lightSpaceMatrix = lightProjection * lightView;

      // Use cooresponding shader when setting uniforms/drawing objects
      ourShader2.Use();
      glUniform1i(glGetUniformLocation(ourShader2.Program, "diffuseTexture"), 0);
  glUniform1i(glGetUniformLocation(ourShader2.Program, "shadowMap"), 1);

      GLint objectColorLoc = glGetUniformLocation(ourShader2.Program, "objectColor");
      GLint lightColorLoc  = glGetUniformLocation(ourShader2.Program, "lightColor");
      GLint lightPosLoc    = glGetUniformLocation(ourShader2.Program, "lightPos");
      GLint lightDirPos = glGetUniformLocation(ourShader2.Program, "lightDirection");
      glUniformMatrix4fv(glGetUniformLocation(ourShader2.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

      glUniform3f(lightDirPos, -0.2f, -1.0f, -0.3f);
      GLint viewPosLoc     = glGetUniformLocation(ourShader2.Program, "viewPos");
//        glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
      glUniform3f(objectColorLoc, 0.0f, 1.0f, 0.0f);
      glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f);
      glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
      glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);


      // Draw our first triangle
//        ourShader.Use();

      // Bind Textures using texture units
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, texture1);
//        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture1"), 0);
//        glActiveTexture(GL_TEXTURE1);
//        glBindTexture(GL_TEXTURE_2D, texture2);
//        glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture2"), 1);

      // Create camera transformation
//      glm::mat4 view;
      view = camera.GetViewMatrix();

//      glm::mat4 projection;
//       projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 100.0f);
//      projection = glm::perspective(degreesToRadians(camera.Zoom), (float)screenWidth/(float)screenHeight, 0.1f, 10000.0f);
      // Get the uniform locations
      GLint modelLoc = glGetUniformLocation(ourShader2.Program, "model");
      GLint viewLoc = glGetUniformLocation(ourShader2.Program, "view");
      GLint projLoc = glGetUniformLocation(ourShader2.Program, "projection");
      // Pass the matrices to the shader
      glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
//terrain
if(showTerrain)
{
//  terrain->showModel();



  ourShader6.Use();
        GLint modelLoc = glGetUniformLocation(ourShader6.Program, "model");
      glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "view"),1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "projection"),1, GL_FALSE, glm::value_ptr(projection));
//      glActiveTexture(GL_TEXTURE0);
//      glBindTexture(GL_TEXTURE_2D, texture1);
////      glUniform1i(glGetUniformLocation(ourShader6.Program, "ourTexture1"), 0);
//
//      GLint objectColorLoc = glGetUniformLocation(ourShader6.Program, "objectColor");
//      GLint lightPosLoc    = glGetUniformLocation(ourShader6.Program, "lightPos");
//      glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
////      GLint lightDirPos = glGetUniformLocation(ourShader6.Program, "lightDirection");
//      GLint viewPosLoc     = glGetUniformLocation(ourShader6.Program, "viewPos");
//      glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);
//      glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
//      glUniform3f(objectColorLoc, 0.8f, 0.5f, 0.25f);
//      // Pass the matrices to the shader
//
//      glBindVertexArray(VAO);
////      for(GLuint i = 0; i < 1/*0*/; i++)
////      {
//          // Calculate the model matrix for each object and pass it to shader before drawing
//          glm::mat4 model;
////          model = glm::translate(model, cubePositions[i]);
////            GLfloat angle = 20.0f * i;
////            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
//      model = glm::scale(model, bigger);
//          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//
////            glDrawArrays(GL_TRIANGLES, 0, 36);
////          glDrawArrays(GL_TRIANGLES, 0, number);
//          glDrawElements(GL_TRIANGLE_STRIP, elementNumber, GL_UNSIGNED_INT, 0);
//          glBindTexture(GL_TEXTURE_2D, 1);
////            glDrawArrays(GL_LINES, 0, 36);
////      }

/////////////////////////////////////////////// docasne pod timto smazat
//    ourShader6.Use();
//    GLint modelLoc = glGetUniformLocation(ourShader6.Program, "model");
//    glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "view"),1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(camera.GetViewMatrix()))));
//    glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "projection"),1, GL_FALSE, glm::value_ptr(glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, maxFar)));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
//      glUniform1i(glGetUniformLocation(ourShader6.Program, "ourTexture1"), 0);

    GLint objectColorLoc = glGetUniformLocation(ourShader6.Program, "objectColor");
    GLint lightPosLoc    = glGetUniformLocation(ourShader6.Program, "lightPos");
    glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
//      GLint lightDirPos = glGetUniformLocation(ourShader6.Program, "lightDirection");
    GLint viewPosLoc     = glGetUniformLocation(ourShader6.Program, "viewPos");
    glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);
    glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glUniform3f(objectColorLoc, 0.8f, 0.5f, 0.25f);
    // Pass the matrices to the shader

//    glBindVertexArray(this->vao);
    glBindVertexArray(terrain->getVAO());
//      for(GLuint i = 0; i < 1/*0*/; i++)
//      {
        // Calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 modell;
//          model = glm::translate(model, cubePositions[i]);
//            GLfloat angle = 20.0f * i;
//            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
    modell = glm::scale(modell, bigger);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modell));

//            glDrawArrays(GL_TRIANGLES, 0, 36);
//          glDrawArrays(GL_TRIANGLES, 0, number);
//        glDrawElements(GL_TRIANGLE_STRIP, this->indicesSize, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_TRIANGLE_STRIP, terrain->getIndicesSize(), GL_UNSIGNED_INT, 0);
        glBindTexture(GL_TEXTURE_2D, 1);
//            glDrawArrays(GL_LINES, 0, 36);
//      }
}
//terrain

//population
ourShader2.Use();
if(showPopulation)
{
//  population->showModel();



//      glBindVertexArray(popVAO);
//      glUniform3f(objectColorLoc, 1.0f, 1.0f, 0.0f);
////      for(GLuint i = 0; i < 1/*0*/; i++)
////      {
//          // Calculate the model matrix for each object and pass it to shader before drawing
//          glm::mat4 model;
////          model = glm::translate(model, cubePositions[i]);
////            GLfloat angle = 20.0f * i;
////            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
//      model = glm::scale(model, bigger);
//          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//
////            glDrawArrays(GL_TRIANGLES, 0, 36);
////          glDrawArrays(GL_TRIANGLES, 0, number);
//          glDrawElements(GL_TRIANGLE_STRIP, elementPopNumber, GL_UNSIGNED_INT, 0);
////            glDrawArrays(GL_LINES, 0, 36);
////      }

///////// pod timto docasne
    ourShader2.Use();
//    glBindVertexArray(this->vao);
    glBindVertexArray(population->getVAO());
    glUniform3f(glGetUniformLocation(ourShader2.Program, "objectColor"), 1.0f, 1.0f, 0.0f);
//      for(GLuint i = 0; i < 1/*0*/; i++)
//      {
        // Calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model;
//          model = glm::translate(model, cubePositions[i]);
//            GLfloat angle = 20.0f * i;
//            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
      model = glm::scale(model, bigger);
      glUniformMatrix4fv(glGetUniformLocation(ourShader2.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

//            glDrawArrays(GL_TRIANGLES, 0, 36);
//          glDrawArrays(GL_TRIANGLES, 0, number);
        glDrawElements(GL_TRIANGLE_STRIP, population->getIndicesSize(), GL_UNSIGNED_INT, 0);
//            glDrawArrays(GL_LINES, 0, 36);
//      }
}
//population

////ROADS
//if(showRoads)
//{
//  std::list<int>::iterator sizeOfRoadsIt = sizeOfRoads.begin();
//  for(std::list<GLuint>::iterator roadVAOsIt = roadVAOs.begin(); roadVAOsIt != roadVAOs.end(); roadVAOsIt++,sizeOfRoadsIt++)
//  {
//    GLuint roadVAO = *roadVAOsIt;
//    glBindVertexArray(roadVAO);
//    int sizeOfRoad = *sizeOfRoadsIt;
//    glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
//    for(GLuint i = 0; i < 1/*0*/; i++)
//    {
//        // Calculate the model matrix for each object and pass it to shader before drawing
//        glm::mat4 model;
//        model = glm::translate(model, cubePositions[i]);
////            GLfloat angle = 20.0f * i;
////            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
////        model = glm::scale(model, glm::vec3(10.0f)); // Make it a smaller cube
//        model = glm::scale(model, bigger);
//        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//
////            glDrawArrays(GL_TRIANGLES, 0, 36);
//        glDrawArrays(GL_TRIANGLES, 0, sizeOfRoad);
////            glDrawArrays(GL_LINES, 0, 36);
//    }
//  }
//}
if(showRoads)
{
ourShader6.Use();
for(std::list<PositionSquare*>::iterator psListIt = viewList->begin(); psListIt != viewList->end(); psListIt++)
{
  PositionSquare* positionSquare = *psListIt;
  std::list<Object*>* objects = positionSquare->getObjects();
  for (std::list<Object*>::iterator objectsIt = objects->begin(); objectsIt != objects->end(); objectsIt++)
  {
    Object* object = *objectsIt;
    if (object->getModel() != NULL && object->getCurrentLinks() == 0) // pozdeji odstranit != NULL
    {
      glBindVertexArray(object->getVAO());
//      glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
      glm::mat4 model;
//      model = glm::translate(model, cubePositions[0]);
      model = glm::scale(model, bigger);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        GLint modelLoc = glGetUniformLocation(ourShader6.Program, "model");
      glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "view"),1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "projection"),1, GL_FALSE, glm::value_ptr(projection));
      glActiveTexture(GL_TEXTURE0);
//      glBindTexture(GL_TEXTURE_2D, texture2);
//      glUniform1i(glGetUniformLocation(ourShader6.Program, "ourTexture1"), 0);

      GLint objectColorLoc = glGetUniformLocation(ourShader6.Program, "objectColor");
      GLint lightPosLoc    = glGetUniformLocation(ourShader6.Program, "lightPos");
      glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
//      GLint lightDirPos = glGetUniformLocation(ourShader6.Program, "lightDirection");
      GLint viewPosLoc     = glGetUniformLocation(ourShader6.Program, "viewPos");
      glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);
      glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
      glUniform3f(objectColorLoc, 0.66f, 0.66f, 0.66f);

      if (Node* node = dynamic_cast<Node*>(object))
      {
//        ourShader2.Use();

        glBindTexture(GL_TEXTURE_2D, texture3);
        glDrawElements(GL_TRIANGLES, object->getIndicesSize(), GL_UNSIGNED_INT, 0);
      }
      if (Road* road = dynamic_cast<Road*>(object))
      {
//          ourShader6.Use();
//        GLint modelLoc = glGetUniformLocation(ourShader6.Program, "model");
//      glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "view"),1, GL_FALSE, glm::value_ptr(view));
//      glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "projection"),1, GL_FALSE, glm::value_ptr(projection));
////      glActiveTexture(GL_TEXTURE0);
//      glBindTexture(GL_TEXTURE_2D, texture2);
////      glUniform1i(glGetUniformLocation(ourShader6.Program, "ourTexture1"), 0);
//
//      GLint objectColorLoc = glGetUniformLocation(ourShader6.Program, "objectColor");
//      GLint lightPosLoc    = glGetUniformLocation(ourShader6.Program, "lightPos");
//      glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
////      GLint lightDirPos = glGetUniformLocation(ourShader6.Program, "lightDirection");
//      GLint viewPosLoc     = glGetUniformLocation(ourShader6.Program, "viewPos");
//      glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);
//      glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
//      glUniform3f(objectColorLoc, 0.66f, 0.66f, 0.66f);

        glBindTexture(GL_TEXTURE_2D, texture2);
        glDrawElements(GL_TRIANGLE_STRIP, object->getIndicesSize(), GL_UNSIGNED_INT, 0);
      }
//      if (object->getIndicesSize() != 0)
//      {
//        glDrawElements(GL_TRIANGLE_STRIP, object->getIndicesSize(), GL_UNSIGNED_INT, 0);
//      }
//      else
//      {
//        glDrawArrays(GL_TRIANGLES, 0, object->getModelSize());
//      }
      if(Building* building = dynamic_cast<Building*>(object))
      {
        srand(37);
          ourShader7.Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture4);
  glUniform1i(glGetUniformLocation(ourShader7.Program, "ourTexture"), 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture5);
  glUniform1i(glGetUniformLocation(ourShader7.Program, "ourTextureRoof"), 1);
//
//  srand(37);
//
//  glm::mat4 model;
//  model = glm::scale(model, bigger);
//  glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
//  glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, maxFar);

  GLint modelLoc = glGetUniformLocation(ourShader7.Program, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(glGetUniformLocation(ourShader7.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(glGetUniformLocation(ourShader7.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

  GLint objectColorLoc = glGetUniformLocation(ourShader7.Program, "objectColor");
  GLint lightPosLoc    = glGetUniformLocation(ourShader7.Program, "lightPos");
  glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);

  GLint viewPosLoc     = glGetUniformLocation(ourShader7.Program, "viewPos");
  glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
  glUniformMatrix4fv(glGetUniformLocation(ourShader7.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
  glUniform3f(objectColorLoc, (float)(rand()/2)/(float)RAND_MAX, (float)(rand()/2)/(float)RAND_MAX, (float)(rand()/2)/(float)RAND_MAX);

  glDrawElements(GL_TRIANGLES, building->getIndicesSize(), GL_UNSIGNED_INT, 0);
      }
    }
//    if(Building* building = dynamic_cast<Building*>(object))
//    {
//
//    }
//    else
//    {
    object->setCurrentLinksPlusOne();
//    }
//    object->setCurrentLinksPlusOne();
  }
}
}
ourShader2.Use();
////ROADS

// Buildings
//if(showRoads)
//{
//ourShader7.Use();
//for(std::list<PositionSquare*>::iterator psListIt = viewList->begin(); psListIt != viewList->end(); psListIt++)
//{
//  PositionSquare* positionSquare = *psListIt;
//  std::list<Object*>* objects = positionSquare->getObjects();
//  for (std::list<Object*>::iterator objectsIt = objects->begin(); objectsIt != objects->end(); objectsIt++)
//  {
//    Object* object = *objectsIt;
//    if (object->getModel() != NULL && object->getCurrentLinks() == 0) // pozdeji odstranit != NULL
//    {
//      glBindVertexArray(object->getVAO());
////      glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
//      glm::mat4 model;
////      model = glm::translate(model, cubePositions[0]);
//      model = glm::scale(model, bigger);
//      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//
//        GLint modelLoc = glGetUniformLocation(ourShader7.Program, "model");
//      glUniformMatrix4fv(glGetUniformLocation(ourShader7.Program, "view"),1, GL_FALSE, glm::value_ptr(view));
//      glUniformMatrix4fv(glGetUniformLocation(ourShader7.Program, "projection"),1, GL_FALSE, glm::value_ptr(projection));
////      glActiveTexture(GL_TEXTURE0);
////      glBindTexture(GL_TEXTURE_2D, texture2);
////      glUniform1i(glGetUniformLocation(ourShader6.Program, "ourTexture1"), 0);
//
//      GLint objectColorLoc = glGetUniformLocation(ourShader7.Program, "objectColor");
//      GLint lightPosLoc    = glGetUniformLocation(ourShader7.Program, "lightPos");
//      glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
////      GLint lightDirPos = glGetUniformLocation(ourShader6.Program, "lightDirection");
//      GLint viewPosLoc     = glGetUniformLocation(ourShader7.Program, "viewPos");
//      glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);
//      glUniformMatrix4fv(glGetUniformLocation(ourShader6.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
//      glUniform3f(objectColorLoc, 0.66f, 0.66f, 0.66f);
//      if(Building* building = dynamic_cast<Building*>(object))
//      {
//  glActiveTexture(GL_TEXTURE0);
//  glBindTexture(GL_TEXTURE_2D, texture4);
//  glUniform1i(glGetUniformLocation(ourShader7.Program, "ourTexture"), 0);
//
//  glActiveTexture(GL_TEXTURE1);
//  glBindTexture(GL_TEXTURE_2D, texture5);
//  glUniform1i(glGetUniformLocation(ourShader7.Program, "ourTextureRoof"), 1);
////
//  srand(37);
////
//  glm::mat4 model;
//  model = glm::scale(model, bigger);
//  glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
//  glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, maxFar);
//
//  GLint modelLoc = glGetUniformLocation(ourShader7.Program, "model");
//  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//  glUniformMatrix4fv(glGetUniformLocation(ourShader7.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
//  glUniformMatrix4fv(glGetUniformLocation(ourShader7.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//
//  GLint objectColorLoc = glGetUniformLocation(ourShader7.Program, "objectColor");
//  GLint lightPosLoc    = glGetUniformLocation(ourShader7.Program, "lightPos");
//  glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
//
//  GLint viewPosLoc     = glGetUniformLocation(ourShader7.Program, "viewPos");
//  glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
//  glUniformMatrix4fv(glGetUniformLocation(ourShader7.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
//  glUniform3f(objectColorLoc, (float)(rand()/2)/(float)RAND_MAX, (float)(rand()/2)/(float)RAND_MAX, (float)(rand()/2)/(float)RAND_MAX);
//
//  glDrawElements(GL_TRIANGLES, building->getIndicesSize(), GL_UNSIGNED_INT, 0);
//      }
//    }
//    object->setCurrentLinksPlusOne();
//  }
//}
//}
// Buildings
//ourShader2.Use();

//BLOCKS
//if(showBlocks && blocks!=NULL)
//{
//  srand(37);
//  std::list<int>::iterator blocksNumberIt = blocksNumber->begin();
//  for (std::list<GLuint>::iterator blocksVAOIt = blocksVAO.begin(); blocksVAOIt != blocksVAO.end(); blocksVAOIt++, blocksNumberIt++)
//  {
//    GLfloat blockVAO = *blocksVAOIt;
//    int blockNumber = *blocksNumberIt;
//    glBindVertexArray(blockVAO);
//    glUniform3f(objectColorLoc, (float)(rand()/2)/(float)RAND_MAX, (float)(rand()/2)/(float)RAND_MAX, (float)(rand()/2)/(float)RAND_MAX);
//    glm::mat4 model;
//    model = glm::scale(model, bigger);
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//    glDrawArrays(GL_TRIANGLES, 0, blockNumber);
//  }

//}
//BLOCKS

//LANDS
//ourShader7.Use();
//if(showLands && lands != NULL)
//{
//  glActiveTexture(GL_TEXTURE0);
//  glBindTexture(GL_TEXTURE_2D, texture4);
//      glUniform1i(glGetUniformLocation(ourShader7.Program, "ourTexture"), 0);
//  glActiveTexture(GL_TEXTURE1);
//  glBindTexture(GL_TEXTURE_2D, texture5);
//      glUniform1i(glGetUniformLocation(ourShader7.Program, "ourTextureRoof"), 1);
//  srand(37);
//  std::list<int>::iterator landsNumberIt = landsNumber->begin();
//  std::list<int>::iterator landsElementNumberIt = landsElementNumber->begin();
//  for (std::list<GLuint>::iterator landsVAOIt = landsVAO.begin(); landsVAOIt != landsVAO.end(); landsVAOIt++, landsNumberIt++, landsElementNumberIt++)
//  {
//    GLfloat landVAO = *landsVAOIt;
//    int landNumber = *landsNumberIt;
//    int elementNumber = *landsElementNumberIt;
//    glBindVertexArray(landVAO);
////    glUniform3f(objectColorLoc, (float)(rand()/2)/(float)RAND_MAX, (float)(rand()/2)/(float)RAND_MAX, (float)(rand()/2)/(float)RAND_MAX);
//    glm::mat4 model;
//    model = glm::scale(model, bigger);
////    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//
//      GLint modelLoc = glGetUniformLocation(ourShader7.Program, "model");
//    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//      glUniformMatrix4fv(glGetUniformLocation(ourShader7.Program, "view"),1, GL_FALSE, glm::value_ptr(view));
//      glUniformMatrix4fv(glGetUniformLocation(ourShader7.Program, "projection"),1, GL_FALSE, glm::value_ptr(projection));
////      glActiveTexture(GL_TEXTURE0);
////      glBindTexture(GL_TEXTURE_2D, texture2);
////      glUniform1i(glGetUniformLocation(ourShader7.Program, "ourTexture"), 0);
////      glUniform1i(glGetUniformLocation(ourShader7.Program, "ourTextureRoof"), 1);
//
//      GLint objectColorLoc = glGetUniformLocation(ourShader7.Program, "objectColor");
//      GLint lightPosLoc    = glGetUniformLocation(ourShader7.Program, "lightPos");
//      glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
////      GLint lightDirPos = glGetUniformLocation(ourShader6.Program, "lightDirection");
//      GLint viewPosLoc     = glGetUniformLocation(ourShader7.Program, "viewPos");
//      glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);
//      glUniformMatrix4fv(glGetUniformLocation(ourShader7.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
//    glUniform3f(objectColorLoc, (float)(rand()/2)/(float)RAND_MAX, (float)(rand()/2)/(float)RAND_MAX, (float)(rand()/2)/(float)RAND_MAX);
//
////    glDrawArrays(GL_TRIANGLES, 0, landNumber);
//    glDrawElements(GL_TRIANGLES, elementNumber, GL_UNSIGNED_INT, 0);
//  }
//}
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, 0);
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, 0);
glActiveTexture(GL_TEXTURE0);
ourShader2.Use();
//LANDS

///////BORDERS
//if (showBorders)
//{
//      glBindVertexArray(VAO2);
//      glUniform3f(objectColorLoc, 0.0f, 0.0f, 1.0f);
//      for(GLuint i = 0; i < 1/*0*/; i++)
//      {
//          // Calculate the model matrix for each object and pass it to shader before drawing
//          glm::mat4 model;
////          model = glm::translate(model, cubePositions[i]);
////            GLfloat angle = 20.0f * i;
////            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
//          model = glm::scale(model, glm::vec3(2.0f)); // Make it a smaller cube
//          model = glm::scale(model, bigger);
//          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//
////            glDrawArrays(GL_TRIANGLES, 0, 36);
//          glDrawArrays(GL_TRIANGLES, 0, number);
////            glDrawArrays(GL_LINES, 0, 36);
//      }
//      glBindVertexArray(VAO3);
//      glUniform3f(objectColorLoc, 0.0f, 0.5f, 0.5f);
//      for(GLuint i = 0; i < 1/*0*/; i++)
//      {
//          // Calculate the model matrix for each object and pass it to shader before drawing
//          glm::mat4 model;
////          model = glm::translate(model, cubePositions[i]);
////            GLfloat angle = 20.0f * i;
////            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
//          model = glm::scale(model, glm::vec3(2.0f)); // Make it a smaller cube
//      model = glm::scale(model, bigger);
//          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//
////            glDrawArrays(GL_TRIANGLES, 0, 36);
//          glDrawArrays(GL_TRIANGLES, 0, number);
////            glDrawArrays(GL_LINES, 0, 36);
//      }
//      glBindVertexArray(VAO3);
//      glUniform3f(objectColorLoc, 0.7f, 0.5f, 0.3f);
////      for(GLuint i = 0; i < 1/*0*/; i++)
////      {
//          // Calculate the model matrix for each object and pass it to shader before drawing
//          glm::mat4 model;
////          model = glm::translate(model, cubePositions[1]);
////            GLfloat angle = 20.0f * i;
////            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
//          model = glm::scale(model, glm::vec3(2.0f)); // Make it a smaller cube
//      model = glm::scale(model, bigger);
//      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//
////            glDrawArrays(GL_TRIANGLES, 0, 36);
//          glDrawArrays(GL_TRIANGLES, 0, number);
////            glDrawArrays(GL_LINES, 0, 36);
////      }
//      glBindVertexArray(VAO3);
//      glUniform3f(objectColorLoc, 0.25f, 0.5f, 0.25f);
//      for(GLuint i = 0; i < 1/*0*/; i++)
//      {
//          // Calculate the model matrix for each object and pass it to shader before drawing
//          glm::mat4 model;
////          model = glm::translate(model, cubePositions[1]);
////            GLfloat angle = 20.0f * i;
////            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
//          model = glm::scale(model, glm::vec3(2.0f)); // Make it a smaller cube
//      model = glm::scale(model, bigger);
//      model = glm::rotate(model, (float)(M_PI*1.5), glm::vec3(0.0f,1.0f,0.0f));
//          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//
////            glDrawArrays(GL_TRIANGLES, 0, 36);
//          glDrawArrays(GL_TRIANGLES, 0, number);
////            glDrawArrays(GL_LINES, 0, 36);
//      }
//      glUniform3f(objectColorLoc, 0.8f, 0.8f, 0.25f);
//      for(GLuint i = 0; i < 1/*0*/; i++)
//      {
//          // Calculate the model matrix for each object and pass it to shader before drawing
//          glm::mat4 model;
////          model = glm::translate(model, cubePositions[2]);
////            GLfloat angle = 20.0f * i;
////            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
//          model = glm::scale(model, glm::vec3(2.0f)); // Make it a smaller cube
//      model = glm::scale(model, bigger);
//      model = glm::rotate(model, (float)(M_PI*1.5), glm::vec3(0.0f,1.0f,0.0f));
//          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
//
////            glDrawArrays(GL_TRIANGLES, 0, 36);
//          glDrawArrays(GL_TRIANGLES, 0, number);
////            glDrawArrays(GL_LINES, 0, 36);
//      }
//
//}
///////BORDERS

      glBindVertexArray(lightVAO);
      glUniform3f(objectColorLoc, 0.3f, 0.6f, 0.25f);
      for(GLuint i = 0; i < 1/*0*/; i++)
      {
          // Calculate the model matrix for each object and pass it to shader before drawing
          glm::mat4 model;
          model = glm::translate(model, cubePos);
//            GLfloat angle = 20.0f * i;
//            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
          model = glm::scale(model, glm::vec3(10.0f)); // Make it a smaller cube
      model = glm::scale(model, bigger);
          glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

          glDrawArrays(GL_TRIANGLES, 0, 36);
      }


      ourShader.Use();
      glBindVertexArray(lightVAO);
      // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
      // Set matrices
      glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
      modelLoc = glGetUniformLocation(ourShader.Program, "model");
      viewLoc  = glGetUniformLocation(ourShader.Program, "view");
      projLoc  = glGetUniformLocation(ourShader.Program, "projection");
//        glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
      glm::mat4 model;
      model = glm::mat4();
      model = glm::translate(model, lightPos);
      model = glm::scale(model, bigger);
      model = glm::scale(model, glm::vec3(4.0f)); // Make it a smaller cube
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
      // Draw the light object (using light's vertex attributes)

      glDrawArrays(GL_TRIANGLES, 0, 36);

  ourShader5.Use();
//              projection = glm::perspective(camera.Zoom, (float)screenWidth/(float)screenHeight, 0.1f, 1000.0f);
      // Get the uniform locations        glUniform1i(glGetUniformLocation(ourShader2.Program, "diffuseTexture"), 0);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
      glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
      glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
      modelLoc = glGetUniformLocation(ourShader5.Program, "model");
      viewLoc = glGetUniformLocation(ourShader5.Program, "view");
      projLoc = glGetUniformLocation(ourShader5.Program, "projection");

//      glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
//      skyboxShader.Use();
//      view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
//      glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
//      glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//      // skybox cube
//      glBindVertexArray(skyboxVAO);
////      glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
//      glDrawArrays(GL_TRIANGLES, 0, 36);
//      glBindVertexArray(0);
//      glDepthFunc(GL_LESS); // Set depth function back to default

      glBindVertexArray(0);
      // Swap the buffers
      glfwSwapBuffers(window);
  }

  // Properly de-allocate all resources once they've outlived their purpose
//  glDeleteVertexArrays(1, &VAO);
//  glDeleteBuffers(1, &VBO);
  glfwTerminate();
  return 0;
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (keys[GLFW_KEY_Q])
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (keys[GLFW_KEY_E])
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (keys[GLFW_KEY_L])
        lightPos = camera.Position;
    if (keys[GLFW_KEY_K])
        cubePos = camera.Position;
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_S] || keys[GLFW_KEY_A] || keys[GLFW_KEY_D])
    {
      updateView = true;
      std::cout << camera.Position.x << " ";
      std::cout << camera.Position.y << " ";
      std::cout << camera.Position.z << std::endl;
    }
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
//    std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }

    if (mode == GLFW_MOD_SHIFT)
    {
      camera.MovementSpeed = 600.0f;
    }
    else if (mode == GLFW_MOD_CONTROL)
    {
      camera.MovementSpeed = 3000.0f;
    }
    else if (mode == GLFW_MOD_ALT)
    {
      camera.MovementSpeed = 6.0f;
    }
    else
    {
      camera.MovementSpeed = 60.0f;
    }

    GLfloat timer = glfwGetTime();
    if(keys[GLFW_KEY_R] && timer - lastClick[GLFW_KEY_R] > 0.2)
    {
      if (showRoads)
      {
        showRoads = false;
      }
      else
      {
        showRoads = true;
      }
      lastClick[GLFW_KEY_R] = timer;
    }
    if(keys[GLFW_KEY_T] && timer - lastClick[GLFW_KEY_T] > 0.2)
    {
      if (showTerrain)
      {
        showTerrain = false;
      }
      else
      {
        showTerrain = true;
      }
      lastClick[GLFW_KEY_T] = timer;
    }

    if(keys[GLFW_KEY_P] && timer - lastClick[GLFW_KEY_P] > 0.2)
    {
      if (showPopulation)
      {
        showPopulation = false;
      }
      else
      {
        showPopulation = true;
      }
      lastClick[GLFW_KEY_P] = timer;
    }

    if(keys[GLFW_KEY_F] && timer - lastClick[GLFW_KEY_F] > 0.2)
    {
      if (fixed)
      {
        fixed = false;
      }
      else
      {
        fixed = true;
      }
      lastClick[GLFW_KEY_F] = timer;
    }

    if(keys[GLFW_KEY_V] && timer - lastClick[GLFW_KEY_V] > 0.2)
    {
      if (full)
      {
        full = false;
      }
      else
      {
        full = true;
      }
      lastClick[GLFW_KEY_V] = timer;
    }

    if(keys[GLFW_KEY_B] && timer - lastClick[GLFW_KEY_B] > 0.2)
    {
      if (showBlocks)
      {
        showBlocks = false;
      }
      else
      {
        showBlocks = true;
      }
      lastClick[GLFW_KEY_B] = timer;
    }

    if(keys[GLFW_KEY_O] && timer - lastClick[GLFW_KEY_O] > 0.2)
    {
      if (showLands)
      {
        showLands = false;
      }
      else
      {
        showLands = true;
      }
      lastClick[GLFW_KEY_O] = timer;
    }

    if(keys[GLFW_KEY_X] && timer - lastClick[GLFW_KEY_X] > 0.2)
    {
      if (showSkybox)
      {
        showSkybox = false;
      }
      else
      {
        showSkybox = true;
      }
      lastClick[GLFW_KEY_X] = timer;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

std::list<PositionSquare*>* getFullView(PositionMatrix* positionMatrix)
{
  std::list<PositionSquare*>* newView = new std::list<PositionSquare*>();
  for (int a = 0 ; a < positionMatrix->getNumberOfSquares();a++)
  {
    for (int b = 0 ; b < positionMatrix->getNumberOfSquares();b++)
    {
      newView->push_back(positionMatrix->getPositionSquares()[a][b]);
    }
  }
  return newView;
}

std::list<PositionSquare*>* getView(PositionMatrix* positionMatrix, int squaresAround, float angle, float distance)
{
  std::list<PositionSquare*>* newView = new std::list<PositionSquare*>();
  float sizeOfSquare = positionMatrix->getSizeOfSquare();
  int i = (int) ((camera.Position.x / biggy) / sizeOfSquare); // PRI ZMENE BIGGER + posunuti kamery o cubepositions
  int j = (int) ((camera.Position.z / biggy) / sizeOfSquare); // PRI ZMENE BIGGER + posunuti kamery o cubepositions
  for (int a = i-squaresAround; a < i+squaresAround; a++)
  {
    if (a >= 0 && a < positionMatrix->getNumberOfSquares())
    {
      for (int b = j-squaresAround; b < j+squaresAround; b++)
      {
        if (b >= 0 && b < positionMatrix->getNumberOfSquares())
        {
          newView->push_back(positionMatrix->getPositionSquares()[a][b]);
        }
//        PositionSquare*** ps = positionMatrix->getPositionSquares();
//        PositionSquare* pss = ps[a][b];
//        newView->push_back(positionMatrix->getPositionSquares()[a][b]);
      }
    }
  }

  float frontX = camera.Front.x;
  float frontY = camera.Front.z;
  // zjistit jestli je Front normalizovany --- neni, lze?
  float length = euclideanDistance(0.0,0.0,frontX,frontY);
  frontX /= length;
  frontY /= length;
  float p3x = frontX*distance+camera.Position.x;
  float p3y = frontY*distance+camera.Position.z;
  float dist = tan(angle * M_PI / 180.0)*distance;
  float plusX = p3x + frontX*dist;
  float plusY = p3y - frontY*dist;
  float minusX = p3x - frontX*dist;
  float minusY = p3y + frontY*dist;

  float maxX = camera.Position.x;
  if (plusX > maxX)
  {
    maxX = plusX;
  }
  if (minusX > maxX)
  {
    maxX = minusX;
  }
  float maxY = camera.Position.z;
  if (plusY > maxY)
  {
    maxY = plusY;
  }
  if (minusY > maxY)
  {
    maxY = minusY;
  }

  float minX = camera.Position.x;
  if (plusX < minX)
  {
    minX = plusX;
  }
  if (minusX < minX)
  {
    minX = minusX;
  }
  float minY = camera.Position.z;
  if (plusY < minY)
  {
    minY = plusY;
  }
  if (minusY < minY)
  {
    minY = minusY;
  }

  int x1 = (int) (((minX /*+ 60.0*/) / biggy) / sizeOfSquare); // PRI ZMENE BIGGER + posunuti kamery o cubepositions
  int x2 = (int) (((maxX /*+ 60.0*/) / biggy) / sizeOfSquare); // PRI ZMENE BIGGER + posunuti kamery o cubepositions
  int y1 = (int) (((minY /*+ 60.0*/) / biggy) / sizeOfSquare); // PRI ZMENE BIGGER + posunuti kamery o cubepositions
  int y2 = (int) (((maxY /*+ 60.0*/) / biggy) / sizeOfSquare); // PRI ZMENE BIGGER + posunuti kamery o cubepositions
  for (int a = x1; a <= x2; a++)
  {
    if (a >= 0 && a < positionMatrix->getNumberOfSquares())
    {
      for (int b = y1; b <= y2; b++)
      {
        if (b >= 0 && b < positionMatrix->getNumberOfSquares())
        {
          bool isInList = false;
          PositionSquare* newPS = positionMatrix->getPositionSquares()[a][b];
          for (std::list<PositionSquare*>::iterator psIt = newView->begin(); psIt != newView->end(); psIt++)
          {
            if (newPS == *psIt)
            {
              isInList = true;
              break;
            }
          }
          if (!isInList)
          {
            newView->push_back(newPS);
          }
        }
//        PositionSquare*** ps = positionMatrix->getPositionSquares();
//        PositionSquare* pss = ps[a][b];
//        newView->push_back(positionMatrix->getPositionSquares()[a][b]);
      }
    }
  }

  return newView;
}

void emptyCurrentLinks(std::list<PositionSquare*>* viewList)
{
  for (std::list<PositionSquare*>::iterator pSIt = viewList->begin(); pSIt != viewList->end(); pSIt++)
  {
    std::list<Object*>* objects = (*pSIt)->getObjects();
    for (std::list<Object*>::iterator objectsIt = objects->begin(); objectsIt != objects->end(); objectsIt++)
    {
      (*objectsIt)->setCurrentLinksToZero();
    }
  }
}

//void generateModels(std::list<PositionSquare*>* viewList, Terrain* terrain) // change list<PS*> -> PS*
void generateModels(std::list<PositionSquare*>* viewList, City* city) // change list<PS*> -> PS*
{
  for (std::list<PositionSquare*>::iterator pSIt = viewList->begin(); pSIt != viewList->end(); pSIt++)
  {
    PositionSquare* ps = *pSIt;
    std::list<Object*>* objects = ps->getObjects();
    std::list<Object*>::iterator objectsIt = objects->begin();
    for (int i = 0; i < objects->size(); i++, objectsIt++)
    {
      Object* object = *objectsIt;
      if (object->getModel() == NULL)
      {
        object->generateModel(city);
      }
//      if (i < ps->numberOfNodes)
//      {
//        Object* object = *modelsIt;
//        if (object->getModel() == NULL)
//        {
//          Node* node = (Node*) object;
//          std::list<Road*>* roads = node->road;
//          int numRoads = roads->size();
//          int sizeOfNode = (numRoads*4 + 1) * 8;
//          if (sizeOfNode <= (4+1)*8)
//          {
//            continue; // 1 - blind valley, think how to solve it
//          }
//          GLfloat* newModel = createVerticesFromNode(node, terrain, sizeOfNode, 8);
//
////          int numIndices = 6*numRoads + 3; //numRoads*4 + 2 + 1 + numRoads*2 + 1;
//          int numIndices = numRoads*2*6 + numRoads*2*3; //numRoads*4 + 2 + 1 + numRoads*2 + 1;
//          GLuint* newIndices = createElementsFromNode(node, numIndices);
////          addNormalsForStripTriangles(newModel,sizeOfNode,newIndices,numIndices,3,6);
//          addNormalsForTriangles(newModel,sizeOfNode,newIndices,numIndices,3,8);
//          addTexCoordsToNode(newModel,sizeOfNode,6,8);
//
//          GLuint nodeVAO, nodeVBO, nodeEBO;
//          glGenVertexArrays(1, &nodeVAO);
//          glGenBuffers(1, &nodeVBO);
//          glGenBuffers(1, &nodeEBO);
//          glBindVertexArray(nodeVAO);
//
//          glBindBuffer(GL_ARRAY_BUFFER, nodeVBO);
//          glBufferData(GL_ARRAY_BUFFER, sizeOfNode*sizeof(GLfloat), newModel, GL_STATIC_DRAW);
//
//          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nodeEBO);
//          glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), newIndices, GL_STATIC_DRAW);
//
//          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
//          glEnableVertexAttribArray(0);
//          glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
//          glEnableVertexAttribArray(1);
//          glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
//          glEnableVertexAttribArray(2);
//
//
//FILE* file = fopen("konstruktDestrukt.txt","a+");
//fprintf(file,"\n%f: Vytvarim model", glfwGetTime());
//fclose(file);
//          object->setModel(newModel);
//          object->setIndices(newIndices);
//          object->setVAO(nodeVAO);
//          object->setVBO(nodeVBO);
//          object->setEBO(nodeEBO);
//          object->setModelSize(sizeOfNode);
//          object->setIndicesSize(numIndices);
//        }
//      }
//      else
//      {
//        Object* object = *modelsIt;
//        if (object->getModel() == NULL)
//        {
//          Road* road = (Road*) object;
//          std::list<float*>* curvature = road->curvature;
//          int curve = curvature->size();
////          int sizeOfRoad = /*144+*/(curve+1)*108/*+144*/; // zmenit na (curve+1)*108 ???? zahodit ctverce na zacatku/konci ????
//          int sizeOfRoad = (4 + curve*4 + 4) * 8;
//          GLfloat* newModel = createVerticesFromRoad(road,terrain,sizeOfRoad,8);
////          addNormalsToMesh(newModel,sizeOfRoad,3,6);
//
//          int numIndices = 9 + curve*9;
//          GLuint* newIndices = createElementsFromRoad(road, numIndices);
//          addNormalsForStripTriangles(newModel,sizeOfRoad,newIndices,numIndices,3,8);
//          addTexCoordsToRoad(newModel,sizeOfRoad,6,8);
//
//          GLuint roadVAO, roadVBO, roadEBO;
//          glGenVertexArrays(1, &roadVAO);
//          glGenBuffers(1, &roadVBO);
//          glGenBuffers(1, &roadEBO);
//          glBindVertexArray(roadVAO);
//
//          glBindBuffer(GL_ARRAY_BUFFER, roadVBO);
//          glBufferData(GL_ARRAY_BUFFER, sizeOfRoad*sizeof(GLfloat), newModel, GL_STATIC_DRAW);
//
//          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, roadEBO);
//          glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), newIndices, GL_STATIC_DRAW);
//
//          glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
//          glEnableVertexAttribArray(0);
//          glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
//          glEnableVertexAttribArray(1);
//          glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
//          glEnableVertexAttribArray(2);
////          glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
////          glEnableVertexAttribArray(1);
//FILE* file = fopen("konstruktDestrukt.txt","a+");
//fprintf(file,"\n%f: Vytvarim model", glfwGetTime());
//fclose(file);
//          object->setModel(newModel);
//          object->setIndices(newIndices);
//          object->setVAO(roadVAO);
//          object->setVBO(roadVBO);
//          object->setEBO(roadEBO);
//          object->setModelSize(sizeOfRoad);
//          object->setIndicesSize(numIndices);
//        }
//      }
    }
  }
}
