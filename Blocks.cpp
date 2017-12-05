#include "Blocks.hpp"

#include "Block.hpp"
#include "City.hpp"
#include "Node.hpp"
#include "Road.hpp"
#include "Roadmap.hpp"
#include "constants.hpp"
#include "utilities.hpp"
#include <math.h>
#include <stdlib.h>

Road* leftRoad (Node* here,std::list<Road*>* previousRoads,float* angle)//zobecnit a vylepsit (vyuzit curvature)
{
//  atan2(y,x);
  std::list<Road*>* possibleRoads = here->roads;
  Road* previousRoad = *(previousRoads->begin()); // WARNING here using push_front!
  Node* previousNode;

  previousNode = getOtherEndOfRoad(here,previousRoad);

  float previousAngle = /*360.0 -*/ ((atan2(here->x - previousNode->x,here->y - previousNode->y) * 180.0 / M_PI) + 180.0);
  Road* leftRoad = NULL;

  for (std::list<Road*>::iterator possibleRoadIt = possibleRoads->begin(); possibleRoadIt != possibleRoads->end(); ++possibleRoadIt)
  {
    Road* newRoad = *possibleRoadIt;
    Node* newNode = getOtherEndOfRoad(here,newRoad);
    bool alreadyVisited = false;

    for (std::list<Road*>::iterator previousRoadIt = previousRoads->begin(); previousRoadIt != previousRoads->end(); ++previousRoadIt)
    {
      if (newRoad == *previousRoadIt)
      {
        alreadyVisited = true;
        break;
      }
    }

    if (!alreadyVisited)
    {
      float newAngle = (atan2(here->x - newNode->x,here->y - newNode->y) * 180.0 / M_PI) + 180.0;
      if (newAngle < previousAngle)
      {
        newAngle += 360.0;
      }
      if (newAngle - previousAngle <= *angle)
      {
        *angle = newAngle - previousAngle;
        leftRoad = newRoad;
      }
    }
  }
  return leftRoad; //NULL - blind alley
}

std::list<Road*>* Blocks::findBlock(Node* init, Road* nextRoad, Node* here)
{
  std::list<Road*>* borders = new std::list<Road*> ();
  borders->push_back(nextRoad);
  float sumOfAngles = 0.0;

  int numberOfDeadEndsInRow = 0;
  while (here != init)
  {
    float* angle = new float (360.0);
    nextRoad = leftRoad(here,borders,angle);

////bordersDebug
//    FILE* file = fopen("bordersDEBUG.svg","w+");
//    fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", 257*10, 257*10);
//    int i = 0;
//    for (std::list<Road*>::iterator bordersIt = borders->begin(); bordersIt != borders->end(); bordersIt++)
//    {
//      Road* border = *bordersIt;
//      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",border->from->y*10.0,border->from->x*10.0,border->to->y*10.0,border->to->x*10.0,(10*i)%200,(10*i)%200,(10*i)%200); // pridat curvature
//      ++i;
//    }
//    fprintf(file,"</svg>");
//    fclose(file);
////bordersDebug

    if (nextRoad == NULL)
    {
//      return borders;
////      delete borders; // slepa ulicka?
////      delete angle;
//    pokud pro tuhle moznost plati ze je to slepa ulice, pak  pridej posledni ulici, zajistit ze angle je 360 a pak by to melo pokracovat v poradku.
      std::list<Road*>::iterator borderIt = borders->begin();
      for (int i=0;i<2*numberOfDeadEndsInRow;i++)
      {
        ++borderIt;
      }
      nextRoad = *borderIt;
      int p = 6;
      // slepy ulice budou fungovat az udelam opravovaci system na ulice aby neobsahovali uvnitr zadnej bod, jinak muze nastat situace ze se potrebuju dvakrat vratit pak smycka
      //      return NULL;
      numberOfDeadEndsInRow++;
    }
    else
    {
      numberOfDeadEndsInRow = 0;
    }
    borders->push_front(nextRoad); // WARNING using push_front in leftRoad
    here = getOtherEndOfRoad(here,nextRoad);
    sumOfAngles += *angle;
    delete angle;
  }

  // Angle of last and first road
  float* angle = new float (360.0);
  std::list<Road*>* tmp = new std::list<Road*> ();
  tmp->push_back(nextRoad);
  nextRoad = leftRoad(here,tmp,angle);
  sumOfAngles += *angle;
  delete tmp;
  delete angle;
  // Angle of last and first road

  if (fabs(sumOfAngles - float(borders->size()-2)*180.0) <= 1.0)
  {
    return borders;
  }
  else
  {
    boundary->borders = borders;
    assignBlockToRoads(boundary);
    return NULL;
  }
//  delete borders;

////debugBlock
//  static int iq = 0;
//  int heighta = 257;
//  int widtha = 257;
//  int magnifya = 10;
//  char buffera [50];
//
//    sprintf(buffera,"debugBlock\\debugBlock%d.svg",iq);
//    FILE* filek = fopen(buffera,"w+");
//    fprintf(filek,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (widtha+1)*magnifya, (heighta+1)*magnifya);
//    int i=0;
//  for (std::list<Road*>::iterator bordersIt = borders->begin(); bordersIt != borders->end(); bordersIt++)
//  {
//Road* border = *bordersIt;
//    fprintf(filek,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",border->from->y*10.0,border->from->x*10.0,border->to->y*10.0,border->to->x*10.0,(10*i)%200,(10*i)%200,(10*i)%200); // pridat curvature
//  i++;
//  }
//    fprintf(filek,"</svg>");
//    fclose(filek);
//    iq++;
//    //debugBlock

  return NULL; //neplatný blok
}

void Blocks::assignBlockToRoads(Block* newBlock)
{
  if (newBlock->borders->empty()) //HOW?
  {
    return;
  }
  for (std::list<Road*>::iterator borderIt = newBlock->borders->begin(); borderIt != newBlock->borders->end(); ++borderIt)
  {
    if (*borderIt == NULL)// how?
    {
      continue;
    }
    Road* road = *borderIt;
    if(road->firstBlock == NULL)
    {
      road->firstBlock = newBlock;
    }
    else //if(road->secondBlock == NULL)
    {
      road->secondBlock = newBlock;
    }
  }
}

void Blocks::checkAllBlocks(std::list<Road*>* roads)
{
  for(std::list<Road*>::iterator roadIt = roads->begin(); roadIt!= roads->end(); ++roadIt)
  {
    Road* road = *roadIt;
    if (road->from->roads->size() > 1 || road->to->roads->size() > 1)
    {
      road->secondBlock = road->firstBlock;
    }
    if (/*road->firstBlock == NULL ||*/ road->secondBlock == NULL)
    {
      Node* here = road->from;
      Node* to = road->to;
      std::list<Road*>* borders = new std::list<Road*> ();
      borders->push_back(road);
      float* angle = new float (360.0);
      Road* nextRoad = leftRoad(here,borders,angle);
      if (
          nextRoad!=NULL  //zamyslet se ma-li to tu byt, je to check na slepy ulice?
          ||
          !(nextRoad->firstBlock == NULL && nextRoad->secondBlock == NULL))
      {
        here = to;
        to = road->from;
        *angle = 360.0;
        nextRoad = leftRoad(here,borders,angle);
//      }//experimentalne
      borders = findBlock(here,road,to);
      Block* newBlock = new Block ();
      newBlock->borders = borders;
      this->blocks->push_back(newBlock);
      assignBlockToRoads(newBlock);
    }//experimentalne
    }
  }
}

// TODO: for each block it will generate lands in the block
void Blocks::generateBlocksLandsWithBuildings(City* city)
{
  for (std::list<Block*>::iterator blocksIt = blocks->begin(); blocksIt!= blocks->end(); ++blocksIt)
  {
    (*blocksIt)->generateBlock(city);
  }
}

Blocks::Blocks(City* city)
{
  RoadMap* roadMap = city->getRoadMap();
  std::list<Node*>* nodes = roadMap->getNodes(); //az budou predelany na ukazatele, pak je presunout primo do funkce
  std::list<Road*>* roads = roadMap->getRoads(); //az budou predelany na ukazatele, pak je presunout primo do funkce
  this->blocks = new std::list<Block*> ();
  this->boundary = new Block ();

  std::list<Node*>::iterator nodeIt = nodes->begin();
  Node* firstNode = *nodeIt;
  std::list<Node*>* toBuildNodes = new std::list<Node*>();
  toBuildNodes->push_back(firstNode);

  for (std::list<Node*>::iterator nIt = toBuildNodes->begin(); nIt != toBuildNodes->end(); ++nIt)
  {
    firstNode = *nIt;
    std::list<Road*> findBlockRoads = *firstNode->roads;
    if (findBlockRoads.size() > 1)
    {
      for (std::list<Road*>::iterator firstIt = firstNode->roads->begin(); firstIt != firstNode->roads->end(); ++firstIt)
      {
        Road* roadFirstNode = *firstIt;
        if (roadFirstNode->firstBlock != NULL && roadFirstNode->secondBlock != NULL)
        {
          float* angle = new float(360.0);
          std::list<Road*>* tmp = new std::list<Road*> ();
          tmp->push_back(roadFirstNode);
          Road* newRoad = leftRoad(firstNode,tmp,angle);
          delete tmp;
          delete angle;
          findBlockRoads.remove(newRoad);
          findBlockRoads.remove(roadFirstNode);
        }
        Node* secondNode = getOtherEndOfRoad(firstNode,roadFirstNode);

        // nahradit radkem 285??
        bool add = true;
        for (std::list<Node*>::iterator nIt2 = toBuildNodes->begin(); nIt2 != toBuildNodes->end(); ++nIt2)
        {
          Node* fnode2 = *nIt2;
          if (fnode2 == secondNode)
          {
            add = false;
            break;
          }
        }
        if (add)
        {
          toBuildNodes->push_back(secondNode);
        }
        // nahradit radkem 285??
      }

      for (std::list<Road*>::iterator firstIt = findBlockRoads.begin(); firstIt != findBlockRoads.end(); ++firstIt)
      {
        Road* roadFirstNode2 = *firstIt;
        Node* snode = getOtherEndOfRoad(firstNode,roadFirstNode2);
        std::list<Road*>* newBlockBorders = findBlock(firstNode,roadFirstNode2,snode);
        if (newBlockBorders != NULL) // == NULL => boundary was found and added inside findBlock
        {
          Block* newBlock = new Block ();
          newBlock->borders = newBlockBorders;
          newBlock->lands = new std::list<Land*> ();
          this->blocks->push_back(newBlock);
          assignBlockToRoads(newBlock);
//          toBuildnodes->push_back(snode);
        }
      }
    }
  }
checkAllBlocks(roads);
if (debug)
{
 drawBlocksCurvature(10,256,256);
}
generateBlocksLandsWithBuildings(city);
}

Blocks::~Blocks()
{

}

std::list<Block*>* Blocks::getBlocksList()
{
  return this->blocks;
}

void Blocks::drawBlocks (int magnify, int height, int width)
{
  FILE* file = fopen("debug/blocks.svg","w+");
  float multiply = (float) magnify;
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);

  for (std::list<Block*>::iterator blockIt = this->blocks->begin(); blockIt != this->blocks->end(); blockIt++)
  {
    Block* blocks = *blockIt;
    int i = 0;
    int q = blocks->borders->size();

    for (std::list<Road*>::iterator bordersIt = blocks->borders->begin(); bordersIt != blocks->borders->end(); bordersIt++)
    {
      Road* border = *bordersIt;
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,0,%d);stroke-width:1\" />\n",border->from->y*multiply,border->from->x*multiply,border->to->y*multiply,border->to->x*multiply,(50*i)%255,(50*i)%255); // pridat curvature
//      fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\"/>\n",border->from->y*multiply,border->from->x*multiply);
//      fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\"/>\n",border->to->y*multiply,border->to->x*multiply);
      ++i;
    }

    std::list<Road*>::iterator bordersIt = blocks->borders->begin();
    if (bordersIt == blocks->borders->end())
    {
      break;
    }
    int aasdf=blocks->borders->size();
    Road* froad = *bordersIt;
    Node* fnode1 = froad->from;
    Node* fnode2 = froad->to;
    ++bordersIt;
    Road* sroad = *bordersIt;
    Node* snode1 = sroad->from;
    Node* snode2 = sroad->to;
    Node* start = fnode1;
    if (fnode1 == snode1 || fnode1 == snode2)
    {
      start = fnode2;
    }
    fprintf(file,"<polygon points=\"");
    fprintf(file,"%f,%f ",start->y*multiply,start->x*multiply);

    Node* start2;
    while (bordersIt != blocks->borders->end())
    {
      froad = *bordersIt;
      fnode1 = froad->from;
      fnode2 = froad->to;
      ++bordersIt;
      start2 = fnode1;
      if (bordersIt == blocks->borders->end())
      {
        if (fnode1 == start)
        {
          start2 = fnode2;
        }
        fprintf(file,"%f,%f ",start2->y*multiply,start2->x*multiply);
        fprintf(file,"%f,%f ",start->y*multiply,start->x*multiply);
        break;
      }
      sroad = *bordersIt;
      snode1 = sroad->from;
      snode2 = sroad->to;
      if (fnode1 == snode1 || fnode1 == snode2)
      {
        start2 = fnode2;
      }
      fprintf(file,"%f,%f ",start2->y*multiply,start2->x*multiply);
    }
    fprintf(file,"\" style=\"fill:rgb(%d,%d,%d)\" />\n",rand()%128,rand()%128,rand()%128);

  }
  fprintf(file,"</svg>");
  fclose(file);
}

void Blocks::drawBlocksCurvature (int magnify, int height, int width)
{
  FILE* file = fopen("debug/blocksqqq.svg","w+");
  float multiply = (float) 1;
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);

  for (std::list<Block*>::iterator blockIt = this->blocks->begin(); blockIt != this->blocks->end(); blockIt++)
  {
    Block* blocks = *blockIt;

    std::list<Road*>::iterator bordersIt = blocks->borders->begin();
    if (bordersIt == blocks->borders->end())
    {
      continue; // zjistit u kolika?? a proc a kdy?
    }

    Road* firstRoad = *bordersIt;
    Node* firstNode1 = firstRoad->from;
    Node* firstNode2 = firstRoad->to;

    ++bordersIt;
    Road* seconRoad = *bordersIt;
    Node* secondNode1 = seconRoad->from;
    Node* secondNode2 = seconRoad->to;

    Node* start = firstNode1;
    if (firstNode1 == secondNode1 || firstNode1 == secondNode2)
    {
      start = firstNode2;
    }
    fprintf(file,"<polygon points=\"");
    fprintf(file,"%f,%f ",start->y*multiply,start->x*multiply);
    if (start == firstNode1 && !firstRoad->curvature->empty())
    {
      for(std::list<float*>::iterator rIt = firstRoad->curvature->begin(); rIt != firstRoad->curvature->end(); rIt++)
      {
        float* ccc = *rIt;
        fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
      }
    }
    else if (!firstRoad->curvature->empty())
    {
      std::list<float*>::iterator rIt = firstRoad->curvature->end();
      for(--rIt; rIt != firstRoad->curvature->begin(); rIt--)
      {
        float* ccc = *rIt;
        fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
      }
      float* ccc = *(firstRoad->curvature->begin());
      fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
    }
    Node* start2;
    while (bordersIt != blocks->borders->end())
    {
      firstRoad = *bordersIt;
      firstNode1 = firstRoad->from;
      firstNode2 = firstRoad->to;
      ++bordersIt;
      start2 = firstNode1;
      if (bordersIt == blocks->borders->end())
      {
        if (firstNode1 == start)
        {
          start2 = firstNode2;
        }
        fprintf(file,"%f,%f ",start2->y*multiply,start2->x*multiply);

        if (start2 == firstNode1 && !firstRoad->curvature->empty())
        {
          for(std::list<float*>::iterator rIt = firstRoad->curvature->begin(); rIt != firstRoad->curvature->end(); rIt++)
          {
            float* ccc = *rIt;
            fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
          }
        }
        else if (!firstRoad->curvature->empty())
        {
          std::list<float*>::iterator rIt = firstRoad->curvature->end();
          for(--rIt; rIt != firstRoad->curvature->begin(); rIt--)
          {
            float* ccc = *rIt;
            fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
          }
          float* ccc = *(firstRoad->curvature->begin());
          fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
        }
        fprintf(file,"%f,%f ",start->y*multiply,start->x*multiply);
        break;
      }
      seconRoad = *bordersIt;
      secondNode1 = seconRoad->from;
      secondNode2 = seconRoad->to;
      if (firstNode1 == secondNode1 || firstNode1 == secondNode2)
      {
        start2 = firstNode2;
      }
      fprintf(file,"%f,%f ",start2->y*multiply,start2->x*multiply);
      if (start2 == firstNode1 && !firstRoad->curvature->empty())
      {
        for(std::list<float*>::iterator rIt = firstRoad->curvature->begin(); rIt != firstRoad->curvature->end(); rIt++)
        {
          float* ccc = *rIt;
          fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
        }
      }
      else if (!firstRoad->curvature->empty())
      {
        std::list<float*>::iterator rIt = firstRoad->curvature->end();
        for(--rIt; rIt != firstRoad->curvature->begin(); rIt--)
        {
          float* ccc = *rIt;
          fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
        }
        float* ccc = *(firstRoad->curvature->begin());
        fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
      }
    }
    fprintf(file,"\" style=\"fill:rgb(%d,%d,%d)\" />\n",rand()%128,rand()%128,rand()%128);
  }

  fprintf(file,"</svg>");
  fclose(file);
}

void Blocks::debugdrawBlocks (int magnify, int height, int width, char name [], std::list<Node*> nlist)
{
  FILE* file = fopen(name,"w+");
  float multiply = (float) magnify;
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);

  for (std::list<Block*>::iterator blockIt = this->blocks->begin(); blockIt != this->blocks->end(); blockIt++)
  {
    Block* blocks = *blockIt;
    int i = 0;
    int q = blocks->borders->size();

    for (std::list<Road*>::iterator bordersIt = blocks->borders->begin(); bordersIt != blocks->borders->end(); bordersIt++)
    {
      Road* border = *bordersIt;
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,0,%d);stroke-width:1\" />\n",border->from->y*multiply,border->from->x*multiply,border->to->y*multiply,border->to->x*multiply,(50*i)%255,(50*i)%255); // pridat curvature
//      fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\"/>\n",border->from->y*multiply,border->from->x*multiply);
//      fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\"/>\n",border->to->y*multiply,border->to->x*multiply);
      ++i;
    }

    std::list<Road*>::iterator bordersIt = blocks->borders->begin();
    if (bordersIt == blocks->borders->end())
    {
      break;
    }
    int aasdf=blocks->borders->size();
    Road* froad = *bordersIt;
    Node* fnode1 = froad->from;
    Node* fnode2 = froad->to;
    ++bordersIt;
    Road* sroad = *bordersIt;
    Node* snode1 = sroad->from;
    Node* snode2 = sroad->to;
    Node* start = fnode1;
    if (fnode1 == snode1 || fnode1 == snode2)
    {
      start = fnode2;
    }
    fprintf(file,"<polygon points=\"");
    fprintf(file,"%f,%f ",start->y*multiply,start->x*multiply);

    Node* start2;
    while (bordersIt != blocks->borders->end())
    {
      froad = *bordersIt;
      fnode1 = froad->from;
      fnode2 = froad->to;
      ++bordersIt;
      start2 = fnode1;
      if (bordersIt == blocks->borders->end())
      {
        if (fnode1 == start)
        {
          start2 = fnode2;
        }
        fprintf(file,"%f,%f ",start2->y*multiply,start2->x*multiply);
        fprintf(file,"%f,%f ",start->y*multiply,start->x*multiply);
        break;
      }
      sroad = *bordersIt;
      snode1 = sroad->from;
      snode2 = sroad->to;
      if (fnode1 == snode1 || fnode1 == snode2)
      {
        start2 = fnode2;
      }
      fprintf(file,"%f,%f ",start2->y*multiply,start2->x*multiply);
    }
    fprintf(file,"\" style=\"fill:rgb(%d,%d,%d)\" />\n",rand()%128,rand()%128,rand()%128);

  }

  for (std::list<Node*>::iterator nIt = nlist.begin(); nIt != nlist.end(); ++nIt)
  {
    Node* mm = *nIt;
    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\"/>\n",mm->y*multiply,mm->x*multiply);
  }
  fprintf(file,"</svg>");
  fclose(file);
}

void Blocks::drawBlocksCurvature (int magnify, FILE* file)
{
  float multiply = (float) magnify;
  for (std::list<Block*>::iterator blockIt = this->blocks->begin(); blockIt != this->blocks->end(); blockIt++)
  {
    Block* blocks = *blockIt;

    std::list<Road*>::iterator bordersIt = blocks->borders->begin();
    if (bordersIt == blocks->borders->end())
    {
      continue; // zjistit u kolika?? a proc a kdy?
    }

    Road* firstRoad = *bordersIt;
    Node* firstNode1 = firstRoad->from;
    Node* firstNode2 = firstRoad->to;

    ++bordersIt;
    Road* seconRoad = *bordersIt;
    Node* secondNode1 = seconRoad->from;
    Node* secondNode2 = seconRoad->to;

    Node* start = firstNode1;
    if (firstNode1 == secondNode1 || firstNode1 == secondNode2)
    {
      start = firstNode2;
    }
    fprintf(file,"<polygon points=\"");
    fprintf(file,"%f,%f ",start->y*multiply,start->x*multiply);
    if (start == firstNode1 && !firstRoad->curvature->empty())
    {
      for(std::list<float*>::iterator rIt = firstRoad->curvature->begin(); rIt != firstRoad->curvature->end(); rIt++)
      {
        float* ccc = *rIt;
        fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
      }
    }
    else if (!firstRoad->curvature->empty())
    {
      std::list<float*>::iterator rIt = firstRoad->curvature->end();
      for(--rIt; rIt != firstRoad->curvature->begin(); rIt--)
      {
        float* ccc = *rIt;
        fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
      }
      float* ccc = *(firstRoad->curvature->begin());
      fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
    }
    Node* start2;
    while (bordersIt != blocks->borders->end())
    {
      firstRoad = *bordersIt;
      firstNode1 = firstRoad->from;
      firstNode2 = firstRoad->to;
      ++bordersIt;
      start2 = firstNode1;
      if (bordersIt == blocks->borders->end())
      {
        if (firstNode1 == start)
        {
          start2 = firstNode2;
        }
        fprintf(file,"%f,%f ",start2->y*multiply,start2->x*multiply);

        if (start2 == firstNode1 && !firstRoad->curvature->empty())
        {
          for(std::list<float*>::iterator rIt = firstRoad->curvature->begin(); rIt != firstRoad->curvature->end(); rIt++)
          {
            float* ccc = *rIt;
            fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
          }
        }
        else if (!firstRoad->curvature->empty())
        {
          std::list<float*>::iterator rIt = firstRoad->curvature->end();
          for(--rIt; rIt != firstRoad->curvature->begin(); rIt--)
          {
            float* ccc = *rIt;
            fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
          }
          float* ccc = *(firstRoad->curvature->begin());
          fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
        }
        fprintf(file,"%f,%f ",start->y*multiply,start->x*multiply);
        break;
      }
      seconRoad = *bordersIt;
      secondNode1 = seconRoad->from;
      secondNode2 = seconRoad->to;
      if (firstNode1 == secondNode1 || firstNode1 == secondNode2)
      {
        start2 = firstNode2;
      }
      fprintf(file,"%f,%f ",start2->y*multiply,start2->x*multiply);
      if (start2 == firstNode1 && !firstRoad->curvature->empty())
      {
        for(std::list<float*>::iterator rIt = firstRoad->curvature->begin(); rIt != firstRoad->curvature->end(); rIt++)
        {
          float* ccc = *rIt;
          fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
        }
      }
      else if (!firstRoad->curvature->empty())
      {
        std::list<float*>::iterator rIt = firstRoad->curvature->end();
        for(--rIt; rIt != firstRoad->curvature->begin(); rIt--)
        {
          float* ccc = *rIt;
          fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
        }
        float* ccc = *(firstRoad->curvature->begin());
        fprintf(file,"%f,%f ",ccc[1]*multiply,ccc[0]*multiply);
      }
    }
    fprintf(file,"\" style=\"fill:rgb(%d,%d,%d)\" />\n",rand()%128,rand()%128,rand()%128);
  }
}
