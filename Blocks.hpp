#ifndef BLOCKS_H_INCLUDED
#define BLOCKS_H_INCLUDED

#include <list>
#include <stdio.h>

class Block;
class City;
class Road;
class Node;

/** \brief Class Blocks is a wrapper class for blocks of the city. Blocks are stored in variable blocks, boundary is container for "block" around a city.
 *         It contains getter for blocks and draw function into svg format.
 */
class Blocks
{
private:
  std::list<Block*>* blocks;
  Block* boundary;
  std::list<Road*>* findBlock(Node* init, Road* nextRoad, Node* here);
  void assignBlockToRoads(Block* newBlock);
  void checkAllBlocks(std::list<Road*>* roads);
  void generateBlocksLandsWithBuildings(City* city); // population need to constructor

public:
  Blocks(City* city);
  ~Blocks();
  std::list<Block*>* getBlocksList();
  void drawBlocks(int magnify, int width, int height);
  void drawBlocksCurvature(int magnify, int width, int height);
  void debugdrawBlocks(int magnify, int height, int width, char name [],std::list<Node*> nlist);

  void drawBlocksCurvature(int magnify, FILE* file);
};

#endif // BLOCKS_H_INCLUDED
