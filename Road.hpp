#ifndef ROAD_H_INCLUDED
#define ROAD_H_INCLUDED

#include "Object.hpp"
#include <list>

class Node;
class Block;

/** \brief Road in the city with object can be drawn 3D.
 */
class Road : public Object
{
public:
  Node* from;
  Node* to;
  std::list<float*>* curvature; // float*[0] - x, [1] - y
  Block* firstBlock;
  Block* secondBlock;
  int laneNumber;
  bool twoWay;
  int maxSpeed;
  // dalsi info...
  Road();
  void generateModel(City* city);
};

#endif // ROAD_H_INCLUDED
