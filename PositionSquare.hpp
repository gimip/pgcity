#ifndef POSITIONSQUARE_H_INCLUDED
#define POSITIONSQUARE_H_INCLUDED

#include <list>

class Object;

/** \brief PositionSquare is a unit in PositionMatrix. It contains information about objects in the scene where positionSquare is located.
 *         It also contains information about time of its deletion.
 */
class PositionSquare
{
public:
  PositionSquare();
  ~PositionSquare();
  std::list<Object*>* getObjects() { return objects; }
//    std::list<Block*>* getBlocks() { return blocks; }
  void addModel (Object* object);
//    void addBlock (Block* block);
  void generateModels ();
  void deleteModels ();
  double getTimeToBeDeleted() { return timeToBeDeleted; }
  void setTimeToBeDeleted(double val) { timeToBeDeleted = val; }
  int numberOfNodes; //tmp
  int numberOfRoads; //tmp
  int numberOfBuildings;
protected:
private:
  std::list<Object*>* objects;
//    std::list<Block*>* blocks;
  double timeToBeDeleted;
};

#endif // POSITIONSQUARE_H_INCLUDED
