#ifndef ROADMAPTOOLS_H_INCLUDED
#define ROADMAPTOOLS_H_INCLUDED

#include <list>
// presunout nektere veci z utilites do roadmapTools?

class Terrain;
class Population;

typedef struct CrossNode{
  int hsl1;
  int hsl2;
  }CrossNode;

float* findInitSeed(std::list<std::list<float*> > *hslList/*, std::list<float*> hyperstreamline*/, float dsep, float*** vectorField, int width, int height, float** popDensity);

bool hyperstreamline(float** heightMap, int width, int height, std::list<std::list<float*> > *hslList, float*** vectorField, float seedX, float seedY, float dsep, float** popDensity, float gauge);
void hyperstreamlines(Terrain* terrain, Population* population, std::list<std::list<float*> > *hslList, std::list<std::list<float*> > *majorHslList, std::list<std::list<float*> > *minorHslList);
void mergeOppositeHyperstreamlines(std::list<std::list<float*> > *hslList, std::list<std::list<float*> > *mHslList);

void orthogonalField(float*** vectorField, int width, int height);
void minusField(float*** vectorField, int width, int height);

void refreshDsep(float* dsep, float* seed, Population* population, bool major);
#endif // ROADMAPTOOLS_H_INCLUDED
