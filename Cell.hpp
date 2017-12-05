#ifndef CELL_H_INCLUDED
#define CELL_H_INCLUDED

/** \brief Cell is helper class used in terrainTools. It contains information about cell in the matrix and has correclty overloaded operators. Probably not absolutely safely.
 */
class Cell
{
private:
  int row;
  int col;
public:
  Cell ();
  Cell (int row, int col);
  int getRow ();
  int getCol ();
  bool operator () (const Cell& a, const Cell& b) const;
  bool operator == (const Cell& a) const;
  bool operator < (const Cell& a) const;
  Cell operator = (Cell& c);
};

#endif // CELL_H_INCLUDED
