#ifndef BUILDING_H_INCLUDED
#define BUILDING_H_INCLUDED

#include "Object.hpp"
#include <list>

class Shader;

class Building : public Object
{
public:
  Building();
  ~Building();
  std::list<float*>* borders;
  int floors;
  void generateModel(City* city);
  void showModel(Shader* ourShader7, GLuint texture4, GLuint texture5);
};

#endif // BUILDING_H_INCLUDED
