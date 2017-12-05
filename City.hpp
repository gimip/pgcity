#ifndef CITY_H_INCLUDED
#define CITY_H_INCLUDED

#include <stdio.h>

class Blocks;
class Population;
class RoadMap;
class Terrain;

/** \brief Wrapper for a whole city. It does contain parts for visualization of city. It contains terrain, population, road map, blocks, lands and buildings.
 *         It has getters for all variables and can draw them in svg together with main function draw.
 */
class City
{
private:
  Terrain* terrain;
  Population* population;
  Population* nightPopulation;
  RoadMap* roadMap;
  Blocks* blocks;

public:
  City();
  ~City();
  Terrain* getTerrain() {return terrain;};
  Population* getPopulation() {return population;};
  RoadMap* getRoadMap() {return roadMap;};
  Blocks* getBlocks() {return blocks;};
  void draw(int magnify, bool drawTerrain, bool drawPopulation, bool drawRoadMapNodes, bool drawRoadMapRoads, bool drawBlocks, const char nameSVG []);
  void drawCity(int magnify);
  void drawCityWithPop(int magnify);
  void drawCityWithoutCurvature(int magnify);
  void drawCityNodes(int magnify);

};


#endif // CITY_H_INCLUDED
