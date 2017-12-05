#ifndef ROADMAP_H_INCLUDED
#define ROADMAP_H_INCLUDED

#include <list>
#include <stdio.h>

class Terrain;
class Population;
class Node;
class Road;
/** \brief Class RoadMap store roads and intersections in the city. Roads are listed in roads and intersections in nodes.
 */
class RoadMap
{
private:
  std::list<Node*>* nodes; //udelat ukazatel na list
  std::list<Road*>* roads; //udelat ukazatel na list
public:
  RoadMap(Terrain* terrain, Population* population);
  void drawSVGRoads(int magnify,int width,int height);
  void drawSVGRoadsCOLOR(int magnify,int width,int height);
  void drawSVGRoadsOneCOLOR(int magnify,int width,int height);
  std::list<Node*>* getNodes();
  std::list<Road*>* getRoads();
//  void repairNearNodesRoads();//presun do roadmapTools
  void repairDeadEnds();//presun do roadmapTools
  void deleteNodesNearIntersection();//presun do roadmapTools
  ~RoadMap();

  void drawSVGRoads(int magnify, FILE* file);
  void drawSVGNodes(int magnify, FILE* file);
};
void drawHSL(int magnify,int width,int height,std::list<std::list<float*> > hslList);
#endif // ROADMAP_H_INCLUDED
