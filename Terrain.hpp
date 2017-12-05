#ifndef TERRAIN_H_INCLUDED
#define TERRAIN_H_INCLUDED

#include "Object.hpp"
#include <stdio.h>

class Terrain : public Object
{
private:
  float ibu;
  float dMax;
  int width;
  int height;
  float** heightMap;
  // temporary
  int hardness [257][257]; // prozatim prazdne, pevnost terenu/slozeni?
  // temp/
  void init(int height, int width, float ibu);
  bool loadSVGTerrain(const char filename [], int coefficient); //coefficient is for correct square loading (division)
  void writeRawTerrain(const char filename []);
  bool loadRawTerrain(const char filename []);

public:
  Terrain();
  Terrain(int choice);
  Terrain(int height, int width, float ibu);
  ~Terrain();
  float** getHeightMap();
  int getHeight();
  int getWidth();
  void drawSVGterrain(int magnify);
  void drawSVGterrain(int magnify, FILE* file);
  void generateModel(City* city);
  void showModel();
};

#endif // TERRAIN_H_INCLUDED
