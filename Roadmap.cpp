#include "Roadmap.hpp"

#include "utilities.hpp"
#include "roadmapTools.hpp"
#include "constants.hpp"
#include "Node.hpp"
#include "Road.hpp"
#include "Terrain.hpp"

#include <cstdio> //temp
#include <cmath> //temp
#include <cstdlib> //temp

RoadMap::RoadMap(Terrain* terrain, Population* population)
{
  nodes = new std::list<Node*>();
  roads = new std::list<Road*>();
  std::list<std::list<float*> >* hslList = new std::list<std::list<float*> >();
  std::list<std::list<float*> >* majorHslList = new std::list<std::list<float*> >();
  std::list<std::list<float*> >* minorHslList = new std::list<std::list<float*> >();

  hyperstreamlines(terrain, population, hslList, majorHslList, minorHslList); // predelat na vraceci funkci (seznam nove pridanych ulic?)
  hslList->clear(); // postarat se o unik pameti?
  mergeOppositeHyperstreamlines(hslList,minorHslList);
  mergeOppositeHyperstreamlines(hslList,majorHslList);

  //graf = nazvat spravne nazvy promennych!!
  int hsl1_num = 0;
  int hsl2_num = 0;
  bool notInRM = false;
  std::list<CrossNode> crossNodeList;

  for (std::list<std::list<float*> >::iterator it = hslList->begin(); it != hslList->end(); ++it,++hsl1_num)
  {
    std::list<float*> hsl1 = *it;
    hsl2_num = 0;
    for (std::list<std::list<float*> >::iterator ite = hslList->begin(); ite != hslList->end(), hsl1_num!=hsl2_num; ++ite,++hsl2_num)
    {
      std::list<float*> hsl2 = *ite;
      for (std::list<float*>::iterator iter = hsl1.begin(); iter != hsl1.end(); ++iter)
      {
        float* bit1 = *iter;
        if ((++iter) == hsl1.end())
        {
          break; // vylepsit na dva iteratory
        }
        float* bit2 = *iter;
        iter--;

        for (std::list<float*>::iterator itera = hsl2.begin(); itera != hsl2.end(); ++itera)
        {
          float* bitr1 = *itera;
          if ((++itera) == hsl2.end())
          {
            break;
          }
          float* bitr2 = *itera;
          itera--;

          float* intersection = lineIntersection(bit1[0],bit1[1], bit2[0],bit2[1], bitr1[0],bitr1[1], bitr2[0],bitr2[1]);
          if (intersection != NULL)
          {
            Node* node = new Node();
            node->x = intersection[0];
            node->y = intersection[1];
//            node->road = new std::list<Road*> (); // NULL
            for (std::list<Node*>::iterator nodesIt = nodes->begin(); nodesIt != nodes->end(); ++nodesIt)
            {
              Node* blb = *nodesIt;
//              if (euclideanDistance(blb->x,blb->y,node->x,node->y) < 0.06)
              if (euclideanDistance(blb->x,blb->y, node->x,node->y) < 0.04)
              {
                notInRM = true;
                break;
              }
            }
            if(!notInRM)
            {
              nodes->push_back(node);
              CrossNode crossnode;
              crossnode.hsl1 = hsl1_num;
              crossnode.hsl2 = hsl2_num;
              crossNodeList.push_back(crossnode);
            }
            notInRM = false;
          }
          delete[] intersection;
        }
      }
    }
  }

  //            zapis bod a prirad mu cislo hyperstreamliny(dvou hyper), pak pokud je vic jak jeden bod na hyper najdi prvni a
//trasuj ho do druhyho.
//            a pridej hranu
// aktualizovat nazvy promennych

  int num = 0, counter = 0;
  std::list<int> cnNumList;
  std::list<Node*>::iterator roadMapIt;
  bool first = true;

  for (std::list<std::list<float*> >::iterator hslIt = hslList->begin(); hslIt != hslList->end(); ++hslIt, ++num)
  {
    std::list<float*> hsl = *hslIt;
    counter = 0;
    for (std::list<CrossNode>::iterator cnlIt = crossNodeList.begin(); cnlIt != crossNodeList.end(); ++cnlIt, ++counter)
    {
      CrossNode cn = *cnlIt;
      if(cn.hsl1 == num || cn.hsl2 == num)
      {
        cnNumList.push_back(counter);
        roadMapIt = nodes->begin();
        for(int i = 0; i < counter; i++)
        {
          ++roadMapIt;//vylepsit s std::advance
        }
//        Node* node = *roadMapIt;
      }
    }
    if (cnNumList.size() > 1)
    {
      Road roadtmp;
      for (std::list<float*>::iterator iter = hsl.begin(); iter != hsl.end(); ++iter)
      {
        float* point1 = *iter;
        if ((++iter) == hsl.end())
        {
          break;
        }
        float* point2 = *iter;
        iter--;
//opravene
        std::list<Node*> nodesOnSegment;
        std::list<float> distances;
        for (std::list<int>::iterator cnNumIt = cnNumList.begin(); cnNumIt != cnNumList.end(); cnNumIt++)
        {
          int cnNum = *cnNumIt;
          roadMapIt = nodes->begin();
          for(int i = 0; i < cnNum; i++)
          {
            ++roadMapIt;//vylepsit s std::advance
          }
          Node* node = *roadMapIt;
          float* point = new float[2];
          point[0] = node->x;
          point[1] = node->y;
          if (isPointOnSegment(point,point1,point2))
          {
            float distance = euclideanDistance(point[0],point[1],point1[0],point1[1]);
            if (nodesOnSegment.empty())
            {
              nodesOnSegment.push_back(node);
              distances.push_back(distance);
            }
            else
            {
              std::list<Node*>::iterator nodesIt = nodesOnSegment.begin();
              std::list<float>::iterator distIt = distances.begin();
//              for (; distIt != distances.end(); distIt++,nodesIt++)
//              {
//                if (distance < *distIt)
//                {
//                  break;
//                }
//              }
              nodesOnSegment.insert(nodesIt,node);
              distances.insert(distIt,distance);
            }
          }
          delete[] point;
        }
        if (nodesOnSegment.size() >= 1)
        {
          for (std::list<Node*>::iterator nodesIt = nodesOnSegment.begin(); nodesIt != nodesOnSegment.end(); nodesIt++)
          {
            Node* node = *nodesIt;
            if (first)
            {
              first = false;
              roadtmp.from = node;
              roadtmp.curvature = new std::list<float*>();
//              roadtmp->curvature = new std::list<float*>();
//              roadtmp->curvature->clear();
            }
            else
            {
              if (roadtmp.from == node) // delete roads with start = end;
              {
int debug = 6;
              }
              else
              {
                Road* road = new Road();
                road->from = roadtmp.from;
                road->to = node;
                road->curvature = roadtmp.curvature;
//                road->firstBlock = NULL;
//                road->secondBlock = NULL;
                road->to->roads->push_back(road);
                road->from->roads->push_back(road);
                roads->push_back(road);
              }
//              roadtmp = new Road();
              roadtmp.from = node;
              roadtmp.curvature = new std::list<float*>();
//              roadtmp->curvature->clear();
            }
          }
        }
        roadtmp.curvature->push_back(point2);
        // vytvorim hranu priradim od do hranu + paramtry
      }
//opravene
    }
    first = true;
    cnNumList.clear();
  }
//graf

//writeNode
  //pridat podminku if !exists vytvor a pak pokracuj append
  FILE* file = fopen("debug/nodes.svg","w+");
  int magnify = 10;
  int width = terrain->getWidth();
  int height = terrain->getHeight();
//  fprintf(file,"<svg width=\"100%\" height=\"100%\" viewBox=\"0 0 %d %d\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n", width*magnify, height*magnify);
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", width*magnify, height*magnify);
  float multiply = (float) magnify;
  for(std::list<Node*>::iterator nodeIt= nodes->begin(); nodeIt != nodes->end(); nodeIt++)
  {
    Node* node = *nodeIt;
    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",node->y*multiply,node->x*multiply);
  }
  fprintf(file,"</svg>");
  fclose(file);
//writeNode

//delete
//vykres hslaaa
//  drawHSL(10,256,256,hslList);
if (debug)
{
  file = fopen("debug/hslaaa.svg","w+");
  magnify = 10;
  multiply = (float) magnify;
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", width*magnify, height*magnify);
  for (std::list<std::list<float*> >::iterator it = hslList->begin(); it != hslList->end(); ++it)
  {
    std::list<float*> hsl1 = *it;
    for (std::list<float*>::iterator iter = hsl1.begin(); iter != hsl1.end(); ++iter)
    {
      float* bit1 = *iter;
      if ((++iter) == hsl1.end())
      {
        break; // vylepsit na dva iteratory
      }
      float* bit2 = *iter;
        iter--;
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n",bit1[1]*multiply,bit1[0]*multiply,bit2[1]*multiply,bit2[0]*multiply);
    }
  }
  fprintf(file,"</svg>");
  fclose(file);
}
//  hslList->clear();  // projit vsechny listy uvnitr a vycistit a pak teprve tento!
//vykres hslaaa

//repairNearNodesRoads();
//  repairDeadEnds();

//deleteNodesNearIntersection();

if (debug)
{
  drawSVGRoads(10,width,height); //problem s prazdnou curvaturou
  drawSVGRoadsCOLOR(10,width,height);
//  repairDeadEnds();
  drawSVGRoadsOneCOLOR(10,width,height);
}

//  // vykresleni jednotlivejch nodu
//  //debugNode
//  int i = 0;
//  for (std::list<Node*>::iterator nodeIt = nodes->begin(); nodeIt != nodes->end(); nodeIt++)
//  {
//    int heighta = 257;
//    int widtha = 257;
//    int magnifya = 10;
//    char buffera [50];
//    sprintf(buffera,"Nodes\\debugNode%d.svg",i);
//    FILE* filek = fopen(buffera,"w+");
//    fprintf(filek,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (widtha+1)*magnifya, (heighta+1)*magnifya);
//    Node* node = *nodeIt;
//    float multiply = (float) magnifya;
//    fprintf(filek,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\" stroke=\"red\" fill=\"red\" />\n",node->y*multiply,node->x*multiply);
//    for (std::list<Node*>::iterator nodeItq = nodes->begin(); nodeItq != nodes->end(); nodeItq++)
//    {
//      Node* nodeq = *nodeItq;
//      if (nodeIt != nodeItq)
//      {
//        fprintf(filek,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\" />\n",nodeq->y*multiply,nodeq->x*multiply);
//      }
//    }
//    for(std::list<Road*>::iterator roadIt= node->road.begin(); roadIt != node->road.end(); roadIt++)
//    {
//      Road* road = *roadIt;
//      std::list<float*>::iterator curvatureIt= road->curvature.begin();
//      float* pointTMP = (float*) *(curvatureIt++);
//      fprintf(filek,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,road->to->y*multiply,road->to->x*multiply,rand()%200,rand()%200,rand()%200);
//    }
//    fprintf(filek,"</svg>");
//    fclose(filek);
//    i++;
//  }
//  // vykresleni jednotlivejch nodu
//  //debugNode

//  hslList->clear();
//  majorHslList->clear();
//  minorHslList->clear();
//  delete hslList;
//  delete majorHslList;
//  delete minorHslList;
}

//void RoadMap::repairNearNodesRoads()
//{
//  for(std::list<Node*>::iterator nodeIt= nodes->begin(); nodeIt != nodes->end(); nodeIt++)
//  {
//    Node* node = *nodeIt;
//    std::list<Road*>* nodesRoads = node->road;
//    if (nodesRoads->size() > 1)
//    {
//      for(std::list<Road*>::iterator nodesRoadsIt= nodesRoads->begin(); nodesRoadsIt != nodesRoads->end(); nodesRoadsIt++)
//      {
//        Road* firRoad = *nodesRoadsIt;
//        for(std::list<Road*>::iterator secNodesRoadsIt= (nodesRoads->begin()); secNodesRoadsIt != nodesRoads->end() && secNodesRoadsIt != nodesRoadsIt;secNodesRoadsIt++)
//        {
//          Road* secRoad = *secNodesRoadsIt;
//          if(getOtherEndOfRoad(node,firRoad) == getOtherEndOfRoad(node,secRoad))
//          {
//            std::list<float*>* fCur = firRoad->curvature;
//            std::list<float*>* sCur = secRoad->curvature;
//            Node* secNode = getOtherEndOfRoad(node,firRoad);
//            if (fCur->empty() && sCur->empty() || euclideanDistance(secNode->x,secNode->y,node->x,node->y) < 0.05)
//            {
//              std::list<Road*>* nnn = secNode->road;
//              nnn->remove(firRoad);
//              roads->remove(firRoad);
//              nodesRoadsIt = nodesRoads.erase(nodesRoadsIt);
//              break;
//            }
//          }
//        }
//      }
//    }
//  }
//}

void RoadMap::repairDeadEnds()
{
  int i = 0;
  for(std::list<Node*>::iterator nodeIt= nodes->begin(); nodeIt != nodes->end(); ++nodeIt)
  {
    i++;
    Node* node = *nodeIt;
    std::list<Road*>* nodeRoads = node->roads;
    if (nodeRoads->size() == 1)
    {
      std::list<Road*>::iterator nodeRoadsIt = nodeRoads->begin();
      Road* firstRoad = *nodeRoadsIt;
      Node* secNode = getOtherEndOfRoad(node,firstRoad);
      std::list<Road*>* secNodeRoads = secNode->roads;
      while (secNodeRoads->size() == 2)
      {
        std::list<Road*>::iterator secNodeRoadsIt = secNodeRoads->begin();
        Road* secondRoad = *secNodeRoadsIt;
        if (firstRoad == secondRoad)
        {
          ++secNodeRoadsIt;
          secondRoad = *secNodeRoadsIt;
        }
        std::list<float*>* sRCurvature = secondRoad->curvature;
        if (firstRoad->from == getOtherEndOfRoad(secNode,firstRoad))
        {
          if (secondRoad->from == getOtherEndOfRoad(secNode,secondRoad))
          {
            std::list<float*>::iterator sRCurvatureIt = sRCurvature->end();
            sRCurvatureIt--;
            for (sRCurvatureIt; sRCurvatureIt != sRCurvature->begin(); sRCurvatureIt--)
            {
              firstRoad->curvature->push_back(*sRCurvatureIt);
            }
          }
          else
          {
            std::list<float*>::iterator sRCurvatureIt = sRCurvature->begin();
            for (sRCurvatureIt; sRCurvatureIt != sRCurvature->end(); sRCurvatureIt++)
            {
              firstRoad->curvature->push_back(*sRCurvatureIt);
            }
          }
          firstRoad->to = getOtherEndOfRoad(secNode,secondRoad);
        }
        else
        {
          if (secondRoad->from == getOtherEndOfRoad(secNode,secondRoad))
          {
            std::list<float*>::iterator sRCurvatureIt = sRCurvature->end();
            sRCurvatureIt--;
            for (sRCurvatureIt; sRCurvatureIt != sRCurvature->begin(); sRCurvatureIt--)
            {
              firstRoad->curvature->push_front(*sRCurvatureIt);
            }
          }
          else
          {
            std::list<float*>::iterator sRCurvatureIt = sRCurvature->begin();
            for (sRCurvatureIt; sRCurvatureIt != sRCurvature->end(); sRCurvatureIt++)
            {
              firstRoad->curvature->push_front(*sRCurvatureIt);
            }
          }
          firstRoad->from = getOtherEndOfRoad(secNode,secondRoad);
        }
        getOtherEndOfRoad(secNode,secondRoad)->roads->push_front(firstRoad);
        getOtherEndOfRoad(secNode,secondRoad)->roads->remove(secondRoad);
        roads->remove(secondRoad);
        nodeIt = nodes->erase(nodeIt);
        delete secondRoad; // musi se smazat taky ostatni promenny uvnitr ulice--checknout
        delete secNode;  // musi se smazat taky ostatni promenny uvnitr nodu--checknout

        secNode = getOtherEndOfRoad(node,firstRoad);
        secNodeRoads = secNode->roads;
      }
    }
  }
}

void RoadMap::deleteNodesNearIntersection()
{
//  for (nodes)
//  {
//    for (nodesRoads)
//    {
//      if (curve = 0) // mam problem
//      secnode
//      if (node = from)
//      {
//        change secnode
//      }
//      spocti vzdalenost node secnode < sizeOfNode
//      smaz secnode z road
//    }
//  }
  for (std::list<Node*>::iterator nodeIt = nodes->begin(); nodeIt != nodes->end(); nodeIt++)
  {
    Node* node = *nodeIt;
if (abs(node->x - 297.764)< 0.001 && abs(node->y - 85.868)< 0.001)
{
int i = 111;
}
    std::list<Road*>* nodeRoads = node->roads;
    for (std::list<Road*>::iterator nodeRoadIt = nodeRoads->begin(); nodeRoadIt != nodeRoads->end(); nodeRoadIt++)
    {
      Road* road = *nodeRoadIt;
      std::list<float*>* curvature = road->curvature;
      if (curvature->size() == 0) // delete road?
      {
        continue;
      }
      if (node == road->from)
      {
        float* checkPoint = (*curvature->begin());
        float distance = euclideanDistance(node->x,node->y,checkPoint[0],checkPoint[1]);
        if (distance < SIZE_OF_NODE)
        {
//          curvature->pop_front();
        }
      }
      else
      {
        float* checkPoint = (*curvature->rbegin());
        float distance = euclideanDistance(node->x,node->y,checkPoint[0],checkPoint[1]);
        if (distance < SIZE_OF_NODE)
        {
          curvature->pop_back();
        }
      }
      if (curvature->empty())
      {
        delete curvature;
        road->curvature = NULL;
      }
    }

for (std::list<Road*>::iterator nodeRoadIt = nodeRoads->begin(); nodeRoadIt != nodeRoads->end(); nodeRoadIt++)
{
Road* road = *nodeRoadIt;
std::list<float*>* curvature = road->curvature;
float* first = *curvature->begin();
}

  }
}

void RoadMap::drawSVGRoads(int magnify,int width,int height) //problem s prazdnou curvaturou
{
  //pridat podminku if !exists vytvor a pak pokracuj append
  FILE* file = fopen("debug/roadmap.svg","w+");
//  fprintf(file,"<svg width=\"100%\" height=\"100%\" viewBox=\"0 0 %d %d\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n", width*magnify, height*magnify);
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", width*magnify, height*magnify);
  float multiply = (float) magnify;
  for(std::list<Road*>::iterator roadIt = this->roads->begin();  roadIt != this->roads->end(); roadIt++)
  {
    Road* road = *roadIt;
    if (road->curvature->empty())
    {
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,road->to->y*multiply,road->to->x*multiply);
    }
    else
    {
      std::list<float*>::iterator curvatureIt= road->curvature->begin();
      float* pointTMP = *(curvatureIt++);
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,pointTMP[1]*multiply,pointTMP[0]*multiply);
  //    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",pointTMP[1]*multiply,pointTMP[0]*multiply);
      curvatureIt++;
      for (curvatureIt; curvatureIt != road->curvature->end(); curvatureIt++)
      {
        float* point = *curvatureIt;
        fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n",pointTMP[1]*multiply,pointTMP[0]*multiply,point[1]*multiply,point[0]*multiply);
  //      fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",point[1]*multiply,point[0]*multiply);
        pointTMP = point;
      }
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n",pointTMP[1]*multiply,pointTMP[0]*multiply,road->to->y*multiply,road->to->x*multiply);
      //fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",road.to->y*multiply,road.to->x*multiply);
//      if (road->curvature.empty())
//      {
//        fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,road->to->y*multiply,road->to->x*multiply);
//      }
    }
    fprintf(file,"\n");
  }
  for(std::list<Node*>::iterator nodeIt= nodes->begin(); nodeIt != nodes->end(); nodeIt++)
  {
    Node* node = *nodeIt;
    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\"/>\n",node->y*multiply,node->x*multiply);
  }
  fprintf(file,"</svg>");
  fclose(file);
}

void RoadMap::drawSVGRoadsCOLOR(int magnify,int width,int height)
{
  //pridat podminku if !exists vytvor a pak pokracuj append
  FILE* file = fopen("debug/roadmapCOLOR.svg","w+");
//  fprintf(file,"<svg width=\"100%\" height=\"100%\" viewBox=\"0 0 %d %d\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n", width*magnify, height*magnify);
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", width*magnify, height*magnify);
  float multiply = (float) magnify;
  for(std::list<Road*>::iterator roadIt= roads->begin(); roadIt != roads->end(); roadIt++)
  {
    Road* road = *roadIt;
//    if (road->curvature.empty())
//    {
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,road->to->y*multiply,road->to->x*multiply,rand()%256,rand()%256,rand()%256);
//    }
//    else
//    {
//      std::list<float*>::iterator curvatureIt= road->curvature.begin();
//      float* pointTMP = *(curvatureIt++);
//      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,pointTMP[1]*multiply,pointTMP[0]*multiply,rand()%256,rand()%256,rand()%256);
//  //    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",pointTMP[1]*multiply,pointTMP[0]*multiply);
//      curvatureIt++;
//      for (curvatureIt; curvatureIt != road->curvature.end(); curvatureIt++)
//      {
//        float* point = *curvatureIt;
//        fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",pointTMP[1]*multiply,pointTMP[0]*multiply,point[1]*multiply,point[0]*multiply,rand()%256,rand()%256,rand()%256);
//  //      fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",point[1]*multiply,point[0]*multiply);
//        pointTMP = point;
//      }
//      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",pointTMP[1]*multiply,pointTMP[0]*multiply,road->to->y*multiply,road->to->x*multiply,rand()%256,rand()%256,rand()%256);
//      //fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",road.to->y*multiply,road.to->x*multiply);
//  //    if (road->curvature.empty())
//  //    {
//  //      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,road->to->y*multiply,road->to->x*multiply,rand()%256,rand()%256,rand()%256);
//  //    }
//    }
  }
  for(std::list<Node*>::iterator nodeIt= nodes->begin(); nodeIt != nodes->end(); nodeIt++)
  {
    Node* node = *nodeIt;
    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\"/>\n",node->y*multiply,node->x*multiply);
  }
  fprintf(file,"</svg>");
  fclose(file);
}

void RoadMap::drawSVGRoadsOneCOLOR(int magnify,int width,int height)
{
  //pridat podminku if !exists vytvor a pak pokracuj append
  FILE* file = fopen("debug/roadmapOneCOLOR.svg","w+");
//  fprintf(file,"<svg width=\"100%\" height=\"100%\" viewBox=\"0 0 %d %d\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n", width*magnify, height*magnify);
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", width*magnify, height*magnify);
  float multiply = (float) magnify;
  for(std::list<Road*>::iterator roadIt= roads->begin(); roadIt != roads->end(); roadIt++)
  {
    Road* road = *roadIt;
    if (road->curvature->empty())
    {
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,road->to->y*multiply,road->to->x*multiply,rand()%256,rand()%256,rand()%256);
    }
    else
    {
      int color [3] = {rand()%256,rand()%256,rand()%256};
      std::list<float*>::iterator curvatureIt= road->curvature->begin();
      float* pointTMP = *(curvatureIt++);
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,pointTMP[1]*multiply,pointTMP[0]*multiply,color[0],color[1],color[2]);
  //    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",pointTMP[1]*multiply,pointTMP[0]*multiply);
      curvatureIt++;
      for (curvatureIt; curvatureIt != road->curvature->end(); curvatureIt++)
      {
        float* point = *curvatureIt;
        fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",pointTMP[1]*multiply,pointTMP[0]*multiply,point[1]*multiply,point[0]*multiply,color[0],color[1],color[2]);
  //      fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",point[1]*multiply,point[0]*multiply);
        pointTMP = point;
      }
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",pointTMP[1]*multiply,pointTMP[0]*multiply,road->to->y*multiply,road->to->x*multiply,color[0],color[1],color[2]);
      //fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"2\"/>\n",road.to->y*multiply,road.to->x*multiply);
  //    if (road->curvature.empty())
  //    {
  //      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(%d,%d,%d);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,road->to->y*multiply,road->to->x*multiply,rand()%256,rand()%256,rand()%256);
  //    }
    }
  }
//  for(std::list<Node*>::iterator nodeIt= nodes->begin(); nodeIt != nodes->end(); nodeIt++)
//  {
//    Node* node = *nodeIt;
//    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\"/>\n",node->y*multiply,node->x*multiply);
//  }
  fprintf(file,"</svg>");
  fclose(file);
}

void drawHSL(int magnify,int width,int height,std::list<std::list<float*> > hslList)
{
  FILE* file = fopen("debug/hsl.svg","w+");
  float multiply = (float) magnify;
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);
  for (std::list<std::list<float*> >::iterator it = hslList.begin(); it != hslList.end(); ++it)
  {
    std::list<float*> hsl1 = *it;
    for (std::list<float*>::iterator iter = hsl1.begin(); iter != hsl1.end(); ++iter)
    {
      float* bit1 = *iter;
      if ((++iter) == hsl1.end())
      {
        break; // vylepsit na dva iteratory
      }
      float* bit2 = *iter;
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(0,0,0);stroke-width:1\" />\n",bit1[1]*multiply,bit1[0]*multiply,bit2[1]*multiply,bit2[0]*multiply);
    }
  }
  fprintf(file,"</svg>");
  fclose(file);
}


std::list<Node*>* RoadMap::getNodes ()
{
  return this->nodes;
}

std::list<Road*>* RoadMap::getRoads ()
{
  return this->roads;
}

RoadMap::~RoadMap()
{

}

void RoadMap::drawSVGRoads(int magnify, FILE* file) //problem s prazdnou curvaturou
{
  float multiply = (float) magnify;

  for (std::list<Road*>::iterator roadIt= roads->begin(); roadIt != roads->end(); roadIt++)
  {
    Road* road = *roadIt;
    if (road->curvature->empty())
    {
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(255,255,0);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,road->to->y*multiply,road->to->x*multiply);
    }
    else
    {
      std::list<float*>::iterator curvatureIt= road->curvature->begin();
      float* pointTMP = *(curvatureIt++);
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(255,255,0);stroke-width:1\" />\n",road->from->y*multiply,road->from->x*multiply,pointTMP[1]*multiply,pointTMP[0]*multiply);
      curvatureIt++;
      for (curvatureIt; curvatureIt != road->curvature->end(); curvatureIt++)
      {
        float* point = *curvatureIt;
        fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(255,255,0);stroke-width:1\" />\n",pointTMP[1]*multiply,pointTMP[0]*multiply,point[1]*multiply,point[0]*multiply);
        pointTMP = point;
      }
      fprintf(file,"\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" style=\"stroke:rgb(255,255,0);stroke-width:1\" />\n",pointTMP[1]*multiply,pointTMP[0]*multiply,road->to->y*multiply,road->to->x*multiply);

    }
    fprintf(file,"\n");
  }
}

void RoadMap::drawSVGNodes(int magnify, FILE* file) //problem s prazdnou curvaturou
{
  float multiply = (float) magnify;

  for (std::list<Node*>::iterator nodeIt = nodes->begin(); nodeIt != nodes->end(); nodeIt++)
  {
    Node* node = *nodeIt;
    fprintf(file,"\t<circle cx=\"%f\" cy=\"%f\" r=\"1\" style=\"fill:rgb(0,0,255)\"/>\n",node->y*multiply,node->x*multiply);
  }
}
