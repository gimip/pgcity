#ifndef POSITIONMATRIX_H_INCLUDED
#define POSITIONMATRIX_H_INCLUDED

class City;
class PositionSquare;

/** \brief Class PositionMatrix is used for storing information about positions of objects in the terrain.
 *         It contains Matrix of PositionSquares of size sizeOfOneSquare.
 */
class PositionMatrix
{
public:
  PositionMatrix(City* city, float sizeOfSquare);
  ~PositionMatrix();
  PositionSquare*** getPositionSquares() { return positionSquares; }
  float getNumberOfSquares() { return numberOfSquares; }
  float getSizeOfSquare() { return sizeOfOneSquare; }

private:
  PositionSquare*** positionSquares;
  int numberOfSquares;
  float sizeOfOneSquare;
};

#endif // POSITIONMATRIX_H_INCLUDED
