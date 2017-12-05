#include "PositionMatrix.hpp"

#include "Block.hpp"
#include "Blocks.hpp"
#include "Building.hpp"
#include "City.hpp"
#include "Land.hpp"
#include "Node.hpp"
#include "PositionSquare.hpp"
#include "Road.hpp"
#include "Roadmap.hpp"
#include "Terrain.hpp"

PositionMatrix::PositionMatrix(City* city, float sizeOfSquare)
{
  RoadMap* roadMap = city->getRoadMap();
  int height = city->getTerrain()->getHeight();
  sizeOfOneSquare = sizeOfSquare;
//  numberOfSquares = ((int) ((float) city->getTerrain()->getHeight() / sizeOfOneSquare) ) + 1;
  numberOfSquares = ((int) ((float) height / sizeOfOneSquare) ) + 1;
  positionSquares = new PositionSquare** [numberOfSquares];
  for (int i = 0; i < numberOfSquares; i++)
  {
    positionSquares[i] = new PositionSquare* [numberOfSquares];
    for (int j = 0; j < numberOfSquares; j++)
    {
      positionSquares[i][j] = new PositionSquare ();
    }
  }
//  RoadMap* roadMap = city->getRoadMap();
  std::list<Node*>* nodes = roadMap->getNodes();
  std::list<Road*>* roads = roadMap->getRoads();

  for (std::list<Node*>::iterator nodesIt = nodes->begin(); nodesIt != nodes->end(); nodesIt++)
  {
    Node* node = *nodesIt;
    int i = (int) (node->x / sizeOfOneSquare);
    int j = (int) (node->y / sizeOfOneSquare);
    positionSquares[i][j]->addModel(node);
//nahradit!!!
positionSquares[i][j]->numberOfNodes += 1;
//nahradit!!!
  }

  std::list<PositionSquare*> lastPSs;
  for (std::list<Road*>::iterator roadsIt = roads->begin(); roadsIt != roads->end(); roadsIt++)
  {
    Road* road = *roadsIt;
    int i = (int) (road->from->x / sizeOfOneSquare);
    int j = (int) (road->from->y / sizeOfOneSquare);
    lastPSs.push_back(positionSquares[i][j]);
    i = (int) (road->to->x / sizeOfOneSquare);
    j = (int) (road->to->y / sizeOfOneSquare);
    bool isInList = false;
    for (std::list<PositionSquare*>::iterator lastPSsIt = lastPSs.begin(); lastPSsIt != lastPSs.end(); lastPSsIt++)
    {
      PositionSquare* positionSquare = *lastPSsIt;
      if (positionSquare == positionSquares[i][j])
      {
        isInList = true;
        break;
      }
    }
    if (!isInList)
    {
      lastPSs.push_back(positionSquares[i][j]);
    }

    std::list<float*>* curvature = road->curvature;
    if (curvature->size() > 0) // nutne?
    {
      for (std::list<float*>::iterator curvatureIt = curvature->begin(); curvatureIt != curvature->end(); curvatureIt++)
      {
        float* curvatureCoords = *curvatureIt;
        i = (int) (curvatureCoords[0] / sizeOfOneSquare);
        j = (int) (curvatureCoords[1] / sizeOfOneSquare);
        bool isInList = false;
        for (std::list<PositionSquare*>::iterator lastPSsIt = lastPSs.begin(); lastPSsIt != lastPSs.end(); lastPSsIt++)
        {
          PositionSquare* positionSquare = *lastPSsIt;
          if (positionSquare == positionSquares[i][j])
          {
            isInList = true;
            break;
          }
        }
        if (!isInList)
        {
          lastPSs.push_back(positionSquares[i][j]);
        }
      }
    }

    for (std::list<PositionSquare*>::iterator lastPSsIt = lastPSs.begin(); lastPSsIt != lastPSs.end(); lastPSsIt++)
    {
      PositionSquare* positionSquare = *lastPSsIt;
      positionSquare->addModel(road);
positionSquares[i][j]->numberOfRoads += 1;
    }
    lastPSs.clear();
  }

  std::list<Block*>* blocks = city->getBlocks()->getBlocksList();
  for (std::list<Block*>::iterator blocksIt = blocks->begin(); blocksIt != blocks->end(); blocksIt++)
  {
//    Block* block = *blocksIt;
    for (std::list<Land*>::iterator landsIt = (*blocksIt)->lands->begin(); landsIt != (*blocksIt)->lands->end(); landsIt++)
    {
//      Land* land = *landsIt;
      for (std::list<Building*>::iterator buildingsIt = (*landsIt)->buildings->begin(); buildingsIt != (*landsIt)->buildings->end(); buildingsIt++)
      {
        Building* building = *buildingsIt;
        float* building_coords = building->borders->front();
        int i = (int) (building_coords[0] / sizeOfOneSquare);
        int j = (int) (building_coords[1] / sizeOfOneSquare);
        positionSquares[i][j]->addModel(building);
        //nahradit!!!
positionSquares[i][j]->numberOfBuildings += 1;
//nahradit!!!
      }
    }
  }

}

PositionMatrix::~PositionMatrix()
{
  //dtor
}
