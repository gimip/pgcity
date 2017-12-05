#ifndef POPULATION_H_INCLUDED
#define POPULATION_H_INCLUDED

#include "Object.hpp"
#include <stdio.h>

class City;
class Terrain;

/** \brief Class Population contains infromation about population density in the city.
 */
class Population : public Object
{
private:
  float** density; // day population
  float** nightDensity; // day population
//  int height;
//  int width;
public:
  int height;
  int width;
  //docasne, udelat getHeight,getWidth
  Population (Terrain* terrain, float x, float y, float value, float k);
  ~Population ();
  float** getDensity ();
  void drawSVGpopulation(int magnify);
  void drawSVGpopulation(int magnify, FILE* file);
  void generateModel(City* city);
  void showModel();
};

#endif // POPULATION_H_INCLUDED
