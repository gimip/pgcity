#include "Cell.hpp"

Cell::Cell ()
{
  this->row = 0;
  this->col = 0;
}

Cell::Cell (int row,int col)
{
  this->row = row;
  this->col = col;
}

int Cell::getRow ()
{
  return this->row;
}

int Cell::getCol ()
{
  return this->col;
}

bool Cell::operator () (const Cell& a,const Cell& b) const
{
  return a < b;
}

bool Cell::operator == (const Cell& a) const //zefektivnit(neinicializovat cele Cell, jen int)
{
  Cell c = a;
  Cell d = *this;
  if (c.getCol() != d.getCol())
  {
    return false;
  }
  if (c.getRow() != d.getRow())
  {
    return false;
  }
  return true;
}

Cell Cell::operator = (Cell& c)
{
  Cell cell(c.getRow(), c.getCol());
  return cell;
}

bool Cell::operator < (const Cell& a) const //zefektivnit(neinicializovat cele Cell)
{
  Cell c = a;
  Cell d = *this;
  if (c.getCol() > d.getCol())
  {
    return true;
  }
  if (c.getCol() < d.getCol())
  {
    return false;
  }
  if (c.getRow() > d.getRow())
  {
    return true;
  }
  return false;
}
