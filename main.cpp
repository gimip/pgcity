#include "City.hpp"
#include "graphics.hpp"

int main(int argc, char** argv)
{
  (void)(argc);
  (void)(argv);

  City* city = new City();
  graphicsLoop(city);

  return 0;
}
