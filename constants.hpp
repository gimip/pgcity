#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED
#include <limits.h>

const int TIME_NULL = 0;// 0 = random
//Types of Land -- ENUM?
const int VEGETATION = 0;
const int SMALL_BUILDING = 1;
const int BLOCK_BUILDING = 2; //blokovy barak
const int FLATS_BUILDING = 3; //panelak
const int SIDEWALK = 4;
const float RUNGEKUTTA_STEP = 0.25;

const unsigned int PRIMITIVE_RESTART = UINT_MAX;

const bool debug = true;

//const float D_REG = 2.2;
//const float DEPTHOfLand = 1.3;
//const float WIDTHOfLand = 1.1;
//const float DEPTHOfLand = 0.6;
const float DEPTH_OF_LAND = 0.8;
//const float WIDTHOfLand = 0.4;
const float WIDTH_OF_LAND = 0.6;

const float HEIGHT_OF_BUILDING = 0.25;

//const float mapScale = 20.0;

////const float widthOfRoad = 0.03; //correct?
//const float widthOfRoad = 0.05;
const float WIDTH_OF_ROAD = 0.07;
////const float heightOfRoad = 0.008;
//const float heightOfRoad = 0.01;
const float HEIGHT_OF_ROAD = 0.01;

const float widthOfSidewalk = 0.01;

//const float sizeOfNode = 0.07;
const float SIZE_OF_NODE = 0.12;

const int D_MAX = 45;
// hledat dsep roadmapTools.cpp


//lands.cpp 975+-
//
const float DEPTH_OF_BUILDING = 0.7;
const float WIDTH_OF_BUILDING = 0.8;

const float MIN_SIZE = 10.0;
const float MAX_SIZE = 200.0;
const float MIN_DENSITY = 5.0;


const float DELTA_RT = 0.3;
const float DELTA_RS = 36.0;
const int DELTA_N = 2;
const float DELTA_H = 0.42;
#endif // CONSTANTS_H_INCLUDED
