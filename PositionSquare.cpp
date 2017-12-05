#include "PositionSquare.hpp"

#include "Object.hpp"

PositionSquare::PositionSquare()
{
   objects = new std::list<Object*>();
//   blocks = new std::list<Block*>();
   numberOfNodes = 0;
   numberOfRoads = 0;
   numberOfBuildings = 0;
   timeToBeDeleted = 0.0;
}

PositionSquare::~PositionSquare()
{
  //dtor
}

void PositionSquare::addModel (Object* model)
{
  objects->push_back(model);
}

//void PositionSquare::addBlock (Block* block)
//{
//  blocks->push_back(block);
//}

void PositionSquare::generateModels ()
{

}

#include <stdio.h>
void PositionSquare::deleteModels ()
{
  for (std::list<Object*>::iterator objectIt = objects->begin(); objectIt != objects->end(); objectIt++)
  {
    Object* object = *objectIt;
//FILE* file = fopen("konstruktDestrukt.txt","a+");
//fprintf(file,"\n%f: Odstranuju model", glfwGetTime());
//fclose(file);
//    if (object->getCurrentLinks() == 0 && object->getModel() != NULL)
    if (object->getCurrentLinks() == 0)
    {
      object->deleteModel();
    }
  }
  timeToBeDeleted = 0.0;
}
