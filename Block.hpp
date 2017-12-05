#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED

#include <list>

class City;
class Road;
class Land;
class Population;

class Block
{
private:
  Road* findLongestRoad(Block* block);
  float sumUpPopulation(Population* population);// az bude typeOfBlock presunuty, tohle presunout do utilities
  int setTypeOfBlock(float averagePop);//presunout do konstruktoru bloku? zakomponovat do toho i rozmery a tvar pozemku
  Road* findLongestRoad();
public:
  Block();
  ~Block();
  std::list<Road*>* borders;
  std::list<Land*>* lands;
  int typeOfBlock;
  void generateBlock(City* city);
  void generateLands();
};

#endif // BLOCK_H_INCLUDED
