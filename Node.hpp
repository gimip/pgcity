#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "Object.hpp"
#include <list>

class Road;
class Block;
class City;

/** \brief Node correspond with intersection in the city, store information of which roads are connected to it.
 */
class Node : public Object
{
public:
  float x;
  float y;
  std::list<Road*>* roads;
  Node() {roads = new std::list<Road*>();};
  void generateModel(City* city);
};

#endif // NODE_H_INCLUDED
