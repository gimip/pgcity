#include "City.hpp"

#include "Blocks.hpp"
#include "Population.hpp"
#include "Node.hpp"
#include "Road.hpp"
#include "Roadmap.hpp"
#include "Terrain.hpp"
#include "utilities.hpp"
#include "stdlib.h"
#include "constants.hpp"
#include <time.h>

City::City()
{
//  int a;
//  float b,x,y,k,value;
//  printf("[1]Predvytvoreny teren\n");
//  printf("[2]Vygenerovany teren\n");
//  scanf("%d",&a);
//  if(a==1)
//  {
//    printf("[1]Svah\n");
//    printf("[2]Udoli\n");
//    printf("[3]Kopec\n");
//    printf("[4]Vlnita Krajina\n");
//    scanf("%d",&a);
//    terrain = new Terrain();
//  }
//  else if(a==2)
//  {
////    printf("Zadejte velikost mapy: (2^n+1)\n");
////    scanf("%d",&a);
////    printf("Zadejte parametr Ibu:\n");
////    scanf("%f",&b);
////    terrain = new Terrain (a,a,b);
//    terrain = new Terrain (257,257,1.0);
//  }

//  printf("\n[1]Konstantni populaci\n");
//  printf("[2]Vygenerovanou populaci\n");
//  scanf("%d",&a);
//  if (a==1)
//  {
//    population = new Population (terrain->getHeightMap(), terrain->getHeight(), terrain->getWidth(), 0.0,0,0,0.0);
//  }
//  if (a==2)
//  {
//    printf("X souradnici centra\n");
//    scanf("%f",&x);
//    printf("Y souradnici centra\n");
//    scanf("%f",&y);
//    printf("hodnotu centra\n");
//    scanf("%f",&value);
//    printf("parametr k\n");
//    scanf("%f",&k);
//    population = new Population (terrain->getHeightMap(), terrain->getHeight(), terrain->getWidth(), x, y, value, k);
//    population = new Population(terrain, 50.0, 50.0, 200.0, 5.0);
//  }
//  population = new Population (2,257,257);


//if (debug)
//{
//  drawCity(10);
////  drawCityWithPop(10);
//  drawCityWithoutCurvature(10);
////  drawCityNodes(10);
//}

  int init_num = time(NULL);
  bool next_init = true;
  while (next_init)
  {
    srand (init_num);
    next_init = false;
    terrain = new Terrain(257,257,1);
//    terrain = new Terrain();
    if (terrain == NULL)
    {
      next_init = true;
    }
    if (!next_init)
    {
      population = new Population(terrain, 50.0, 50.0, 200.0, 5.0);
      nightPopulation = NULL;
    }

    if (!next_init)
    {
      roadMap = new RoadMap(terrain, population);
    }
    if (roadMap == NULL)
    {
      next_init = true;
    }

    if (!next_init)
    {
      std::list<Road*>* roads = roadMap->getRoads();
      if (roads->size() > 3)
      {
        blocks = new Blocks(this);
      }
      else
      {
        blocks = NULL;
      }
    }
    if (blocks == NULL)
    {
      next_init = true;
    }

    printf("srand init :%d", init_num);
    if(next_init)
    {
      printf(" (unsuccessful)");
    }
    printf("\n");
    init_num++;
  }

//  lands = new Lands (blocks,roadMap->getRoads(),population);
//  blocks->drawBlocks(10,terrain->getHeight(), terrain->getWidth());

  if (blocks != NULL)
  {
    draw(10.0,true,false,false,true,false,"debug/terrainRoads.svg");
    draw(10.0,false,true,false,true,false,"debug/populationRoads.svg");
    draw(10.0,false,false,false,false,true,"debug/blocksA.svg");
  }
}

City::~City ()
{
  delete terrain;
  delete population;
  delete roadMap;
  delete blocks;
//  delete lands;
}

void City::draw(int magnify, bool drawTerrain, bool drawPopulation, bool drawRoadMapNodes, bool drawRoadMapRoads, bool drawBlocks, const char nameSVG [])
{
  int height = terrain->getHeight();
  int width = terrain->getWidth();
  FILE* file = fopen(nameSVG,"w+");
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);

  if (drawTerrain)
  {
    terrain->drawSVGterrain(magnify,file);
  }
  if (drawPopulation)
  {
    population->drawSVGpopulation(magnify,file);
  }
  if (drawRoadMapNodes)
  {
    roadMap->drawSVGNodes(magnify,file);
  }
  if (drawRoadMapRoads)
  {
    roadMap->drawSVGRoads(magnify,file);
  }
  if (drawBlocks)
  {
    blocks->drawBlocksCurvature(magnify,file);
  }

  fprintf(file,"</svg>");
  fclose(file);
}

void City::drawCity (int magnify)
{
  int height = terrain->getHeight();
  int width = terrain->getWidth();
  float** heightMap = terrain->getHeightMap();
  std::list<Road*>* roads = roadMap->getRoads();
  std::list<Node*>* nodes = roadMap->getNodes();
  FILE* file = fopen("debug/city.svg","w+");
//  fprintf(file,"<svg width=\"100%\" height=\"100%\" viewBox=\"0 0 %d %d\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n", width*magnify, height*magnify);
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);

  float multiply = (float) magnify;

  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      fprintf(file,"\t<rect x=\"%d\" y=\"%d\" width=\"%f\" height=\"%f\" style=\"fill:rgb(%d,%d,%d)\"/>\n",j*magnify,i*magnify,multiply,multiply,(int)heightMap[i][j],(int)heightMap[i][j],(int)heightMap[i][j]);
    }
  }

  for(std::list<Road*>::iterator roadIt= roads->begin(); roadIt != roads->end(); roadIt++)
  {
    Road* road = *roadIt;
    if (road->curvature->empty())
    {
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,255);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,road->to->y*multiply,road->to->x*multiply);
    }
    else
    {
      std::list<float*>::iterator curvatureIt= road->curvature->begin();
      float* pointTMP = (float*) *(curvatureIt++);
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,255);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,pointTMP[1]*multiply,pointTMP[0]*multiply);
  //    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",pointTMP[1]*multiply,pointTMP[0]*multiply);
      curvatureIt++;
      for (curvatureIt; curvatureIt != road->curvature->end(); curvatureIt++)
      {
        float* point = *curvatureIt;
        fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,255);stroke-width:1\" />\n",pointTMP[1]*multiply,pointTMP[0]*multiply,point[1]*multiply,point[0]*multiply);
  //      fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",point[1]*multiply,point[0]*multiply);
        pointTMP = point;
      }
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,255);stroke-width:1\" />\n",pointTMP[1]*multiply,pointTMP[0]*multiply,road->to->y*multiply,road->to->x*multiply);
    }
  }
  for(std::list<Node*>::iterator nodeIt= nodes->begin(); nodeIt != nodes->end(); nodeIt++)
  {
    Node* node = *nodeIt;
//    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1.5\"/>\n",node.y*multiply,node.x*multiply);
  }

  fprintf(file,"</svg>");
  fclose(file);
}

void City::drawCityWithPop (int magnify)
{
  int height = terrain->getHeight();
  int width = terrain->getWidth();
  float** heightMap = population->getDensity();
  std::list<Road*>* roads = roadMap->getRoads();
  std::list<Node*>* nodes = roadMap->getNodes();
  FILE* file = fopen("debug/cityWithPop.svg","w+");
//  fprintf(file,"<svg width=\"100%\" height=\"100%\" viewBox=\"0 0 %d %d\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n", width*magnify, height*magnify);
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);

  float multiply = (float) magnify;

  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      fprintf(file,"\t<rect x=\"%d\" y=\"%d\" width=\"%f\" height=\"%f\" style=\"fill:rgb(%d,%d,%d)\"/>\n",j*magnify,i*magnify,multiply,multiply,(int)heightMap[i][j],(int)heightMap[i][j],(int)heightMap[i][j]);
    }
  }

  for(std::list<Road*>::iterator roadIt= roads->begin(); roadIt != roads->end(); roadIt++)
  {
    Road* road = *roadIt;
    std::list<float*>::iterator curvatureIt= road->curvature->begin();
    float* pointTMP = (float*) *(curvatureIt++);
    fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,255);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,pointTMP[1]*multiply,pointTMP[0]*multiply);
//    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",pointTMP[1]*multiply,pointTMP[0]*multiply);
    curvatureIt++;
    for (curvatureIt; curvatureIt != road->curvature->end(); curvatureIt++)
    {
      float* point = *curvatureIt;
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,255);stroke-width:1\" />\n",pointTMP[1]*multiply,pointTMP[0]*multiply,point[1]*multiply,point[0]*multiply);
//      fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",point[1]*multiply,point[0]*multiply);
      pointTMP = point;
    }
    fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,255);stroke-width:1\" />\n",pointTMP[1]*multiply,pointTMP[0]*multiply,road->to->y*multiply,road->to->x*multiply);
  }
  for(std::list<Node*>::iterator nodeIt= nodes->begin(); nodeIt != nodes->end(); nodeIt++)
  {
    Node* node = *nodeIt;
//    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1.5\"/>\n",node.y*multiply,node.x*multiply);
  }

  fprintf(file,"</svg>");
  fclose(file);
}

void City::drawCityWithoutCurvature (int magnify)
{
  int height = terrain->getHeight();
  int width = terrain->getWidth();
  float** heightMap = population->getDensity();
  std::list<Road*>* roads = roadMap->getRoads();
  std::list<Node*>* nodes = roadMap->getNodes();
  FILE* file = fopen("debug/cityWithoutCurvature.svg","w+");
//  fprintf(file,"<svg width=\"100%\" height=\"100%\" viewBox=\"0 0 %d %d\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n", width*magnify, height*magnify);
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);

  float multiply = (float) magnify;


  for(std::list<Road*>::iterator roadIt= roads->begin(); roadIt != roads->end(); roadIt++)
  {
    Road* road = *roadIt;
    std::list<float*>::iterator curvatureIt= road->curvature->begin();
    float* pointTMP = (float*) *(curvatureIt++);
    fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,road->to->y*multiply,road->to->x*multiply,rand()%255,rand()%255,rand()%255);
  }

  fprintf(file,"</svg>");
  fclose(file);
}

void City::drawCityNodes (int magnify)
{
  int height = terrain->getHeight();
  int width = terrain->getWidth();
  float** heightMap = terrain->getHeightMap();
  std::list<Road*>* roads = roadMap->getRoads();
  std::list<Node*>* nodes = roadMap->getNodes();
  FILE* file = fopen("debug/cityNodes.svg","w+");
//  fprintf(file,"<svg width=\"100%\" height=\"100%\" viewBox=\"0 0 %d %d\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n", width*magnify, height*magnify);
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);

  float multiply = (float) magnify;

  for(std::list<Node*>::iterator nodeIt= nodes->begin(); nodeIt != nodes->end(); nodeIt++)
  {
    Node* node = *nodeIt;
    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1.5\"/>\n",node->y*multiply,node->x*multiply);
  }

  fprintf(file,"</svg>");
  fclose(file);
}
