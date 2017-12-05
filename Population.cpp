#include "Population.hpp"

#include "City.hpp"
#include "Terrain.hpp"
#include "utilities.hpp"
#include "constants.hpp"
#include "graphicUtilities.hpp"
#include <math.h>
#include <stdlib.h>

Population::Population (Terrain* terrain, float x, float y, float value, float k)
{
  height = terrain->getHeight();
  width = terrain->getWidth();

  density = new float* [height];
  nightDensity == density;
//  nightDensity = new float* [height];
  for(int i = 0; i < height; i++)
  {
    density[i] = new float[width];
//    nightDensity[i] = new float[width];
  }

  float** selectionMatrix = new float* [height];
  float** distanceGradField = new float* [height];
  for(int i = 0; i < height; i++)
  {
    selectionMatrix[i] = new float[width];
    distanceGradField[i] = new float[width];
  }
  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      selectionMatrix[i][j] = -1.0;
    }
  }

  float*** gradField = gradientField(terrain->getHeightMap(),terrain->getHeight(),terrain->getWidth());
  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      distanceGradField[i][j] = sizeOfVectorN(gradField[i][j],2);
//      distanceGradField[i][j] = abs(gradField[i][j][0])+abs(gradField[i][j][1]);
    }
  }

//  float average = 0.0;
//  for (int i=0; i<height; i++)
//  {
//    for (int j=0; j<width; j++)
//    {
//      average += distanceGradField[i][j];
//    }
//  }
//  average /= (height*width);

  int con = height/15;
//  int con = 55;
  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      selectionMatrix[i][j] = 0.0;
      int counter = 0;
      for (int k=i-con; k<i+con+1; k++)
      {
        if (k >= 0 && k < height)
        {
          for (int l=j-con; l<j+con+1; l++)
          {
  //          if (distanceGradField[i][j] < 0.01)
  //          {
  //            selectionMatrix[i][j] += 0.01;
  //          }
  //          else
  //          {
            if (l >= 0 && l < height)
            {
              selectionMatrix[i][j] += distanceGradField[k][l];
              counter++;
            }
  //          }
          }
        }
      }
      selectionMatrix[i][j] /= (float)counter;
    }
  }

  float** heightMap = terrain->getHeightMap();
  int minix = con, miniy = con;
  float minim = selectionMatrix[con][con];

  for (int i=con/2; i<height-con/2; i++)
  {
    for (int j=con/2; j<width-con/2; j++)
    {
      if(selectionMatrix[i][j] < minim && heightMap[i][j] > 0.0 ) // pridat rozdilnou podminku > 0 pro vodu (az bude)
      {
        minim = selectionMatrix [i][j];
        minix = i;
        miniy = j;
      }
    }
  }


//  float** heightMap = terrain->getHeightMap();
//  int minix=0,miniy=0;
//  float minim = heightMap[50][50];
//
//  for (int i=0; i<height; i++)
//  {
//    for (int j=0; j<width; j++)
//    {
//      if(heightMap[i][j] < minim && heightMap[i][j] > 0.0 && i > height*0.2 && i < height*0.8 && j > height*0.2 && j < height*0.8) // pridat rozdilnou podminku > 0 pro vodu (az bude)
//      {
//        minim = heightMap [i][j];
//        minix = i;
//        miniy = j;
//      }
//    }
//  }

  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      density[i][j] = 0.0;
//      density[i][j] = selectionMatrix[i][j];
//      density[i][j] = 200.0 * (minim/selectionMatrix[i][j]);
    }
  }

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//  for (int i=0; i<height; i++)
//  {
//    for (int j=0; j<height; j++)
//    {
////      density[i][j] = 200.0 - pow(euclideanDistance(minix,miniy,i,j),1.15);//200.0 * (distanceGradField[minix][miniy]/distanceGradField[i][j]);//*euclideanDistance(minix,miniy,i,j);
////      olddensity[i][j] = 200.0 - 200.0*selectionMatrix[i][j];
////      olddensity[i][j] = 200.0 * minim/selectionMatrix[i][j] - pow(euclideanDistance(minix,miniy,i,j),0.9);
//      density[i][j] = 200.0 - pow(euclideanDistance(height*0.6,height*0.4,i,j),1.5);
////      olddensity[i][j] = 200.0 * minim/selectionMatrix[i][j] - pow(euclideanDistance(minix,miniy,i,j),1.4);
////      olddensity[i][j] = 200.0 * minim/selectionMatrix[i][j];
////      olddensity[i][j] = 200.0 * selectionMatrix[i][j];
//      if (density[i][j]<0.0)
//      {
//        density[i][j] = 0.0;
//      }
//    }
//  }
//if (debug)
//{
//  drawSVGpopulation(10);
//}
//return;
///////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//  average = 0.0;
//  for (int i=minix-con; i<minix+con; i++)
//  {
//    for (int j=miniy-con; j<miniy+con; j++)
//    {
//      average += selectionMatrix[i][j];
//    }
//  }
//  average /= (con*con);

  float** olddensity = new float* [height];
  for(int i = 0; i < height; i++)
  {
    olddensity[i] = new float[width];
  }
  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      olddensity[i][j] = 0.0;
    }
  }

  for (int i=0; i<height; i++)
  {
    for (int j=0; j<height; j++)
    {
//      density[i][j] = 200.0 - pow(euclideanDistance(minix,miniy,i,j),1.15);//200.0 * (distanceGradField[minix][miniy]/distanceGradField[i][j]);//*euclideanDistance(minix,miniy,i,j);
//      olddensity[i][j] = 200.0 - 200.0*selectionMatrix[i][j];
//      olddensity[i][j] = 200.0 * minim/selectionMatrix[i][j] - pow(euclideanDistance(minix,miniy,i,j),0.9);
      olddensity[i][j] = 150.0 * minim/selectionMatrix[i][j] - pow(euclideanDistance(minix,miniy,i,j),1.3) * selectionMatrix[i][j];
//      olddensity[i][j] = 150.0 * minim/selectionMatrix[i][j] - pow(euclideanDistance(minix,miniy,i,j),1.1) * selectionMatrix[i][j];
//      olddensity[i][j] = 200.0 * minim/selectionMatrix[i][j] - pow(euclideanDistance(minix,miniy,i,j),1.4);
//      olddensity[i][j] = 200.0 * minim/selectionMatrix[i][j];
//      olddensity[i][j] = 200.0 * selectionMatrix[i][j];
      if (olddensity[i][j]<0.0)
      {
        olddensity[i][j] = 0.0;
      }
    }
  }
//  for (int i=minix-con; i<minix+con; i++)
//  {
//    for (int j=miniy-con; j<miniy+con; j++)
//    {
////      density[i][j] = 200.0 - pow(euclideanDistance(minix,miniy,i,j),1.15);//200.0 * (distanceGradField[minix][miniy]/distanceGradField[i][j]);//*euclideanDistance(minix,miniy,i,j);
////      olddensity[i][j] = 200.0 - 0.1*selectionMatrix[i][j];
//      density[i][j] = 200.0 * minim/selectionMatrix[i][j] - pow(euclideanDistance(minix,miniy,i,j),0.75);
//    }
//  }

//  int con2 = 3;
//  float tmp;
//  for (int i=0+con; i<height-con; i++)
//  {
//    for (int j=0+con; j<height-con; j++)
//    {
//      tmp = 0.0;
//      for (int k=i-con2; k<i+con2; k++)
//      {
//        for (int l=j-con2; l<j+con2; l++)
//        {
//          tmp += olddensity[k][l];
//        }
//      }
//      density[i][j] = tmp/(float)((2*con2)*(2*con2));
////      density[i][j] = olddensity[i][j];
//    }
//  }

int filterSize = 3;
density = convolution(createOnesFilter(filterSize),filterSize,olddensity,height);
//density = olddensity;
/*old
  if (value==0 && x==0 && y==0 && k==0.0)
  {
    value=180;
    x=float (height)/2;
    y=float (height)/2;
    k=0.05;
  }
  for (int i=1; i<height-1; i++)
  {
    for (int j=1; j<width-1; j++)
    {
//      density[i][j] = 230.0 - euclideanDistance(i,j,115.0,115.0) * 0.5;
//      density[i][j] = 230.0 - euclideanDistance(i,j,115.0,115.0) * 3.5;
//      density[i][j] = 230.0 - euclideanDistance(i,j,115.0,115.0) * 7.5;
//      density[i][j] = 230.0 - euclideanDistance(i,j,66.0,99.0) * 8.6; //prvni mesto
//      density[i][j] = 200.0 - euclideanDistance(i,j,50.0,50.0) * 0.2; //druhe mesto - nutne 2/-2

//      density[i][j] = value - euclideanDistance(i,j,x,y) * k; //treti mesto
//      density[i][j] = 7000.0*(1.0/(2.0*2.0*M_PI*M_PI))*exp(-0.0005*(((float)i-height/2.0)*((float)i-height/2.0)+((float)j-width/2.0)*((float)j-width/2.0)));
      density[i][j] = 7000.0*(1.0/(2.0*2.0*M_PI*M_PI))*exp(-0.0005*(((float)i-minix)*((float)i-minix)+((float)j-miniy)*((float)j-miniy)));

//      density[i][j] = 220.0 - euclideanDistance(i,j,48.0,88.0) * 5.1; //treti mesto
//      density[i][j] = 230.0 - euclideanDistance(i,j,66.0,99.0) * 8.6; //prvni mesto
//      density[i][j] = 230.0 - euclideanDistance(i,j,100.0,100.0) * 10.5;
//      density[i][j] = 230.0 - euclideanDistance(i,j,100.0,100.0) * 12.6;
//      density[i][j] = 230.0 - euclideanDistance(i,j,100.0,100.0) * 13.4;
//      density[i][j] = 230.0 - euclideanDistance(i,j,100.0,100.0) * 14.5;
//      density[i][j] = 230.0 - euclideanDistance(i,j,100.0,100.0) * 18.5;
//      density[i][j] = 230.0 - euclideanDistance(i,j,100.0,100.0) * 25.1;
//      density[i][j] = 230.0 - euclideanDistance(i,j,100.0,100.0) * 33.1;
//      density[i][j] = 200.0; //druhe
//      density[33][33] = 220.0; // druhe
      if (density[i][j] < 0.0)
      {
        density[i][j] = 0.0;
      }
      //density[i][j] = 255 - heightMap[i][j];
    }
  }


////  //test - start
//  height /= 2;
//
//  int maxx = 500000;// * (int) pow((float) (height / 128), 2.0);
//  maxx = (int) pow(height,2.0);
//  int maxR = 2500;
//  for (int i = 0 ; i< maxx ;i++)
//  {
//    int x = rand() % (height/2);
//    int y = rand() % (height/2);
//    int s = 3 + rand() % 10;
//    int val = rand() % 25;
//    int newX = rand() % (height/2);
//    int newY = rand() % (height/2);
//    int minus = rand() % 2;
//    if (minus == 1)
//    {
//      x *= -1;
//    }
//    minus = rand() % 2;
//    if (minus == 1)
//    {
//      y *= -1;
//    }
//    minus = rand() % 2;
//    if (minus == 1)
//    {
//      newX *= -1;
//    }
//    minus = rand() % 2;
//    if (minus == 1)
//    {
//      newY *= -1;
//    }
//    x += height/2;
//    y += height/2;
//    newX += height/2;
//    newY += height/2;
//    s += height/2;
//    for (int j = height/2; j< s && x+j < 1.5*height && newX+j < 1.5*height; j++)
//    {
//      for (int k = height/2; k< s && y+k < 1.5*height && newY+k < 1.5*height; k++)
//      {
//        int random = rand() % maxR;
//        float value = random*(1.0/(2.0*2.0*M_PI*M_PI)) * exp(-0.0005*(((float)(x+j)-height/2.0)*((float)(x+j)-height/2.0)+((float)(y+k)-width/2.0)*((float)(y+k)-width/2.0)));
//        density[newX+j][newY+k] += value;
//        density[x+j][y+k] -= value;
//      }
//    }
//  }
//  for (int i=1; i<height-1; i++)
//  {
//    for (int j=1; j<width-1; j++)
//    {
//      if (density[i][j] < 0.0)
//      {
//        density[i][j] = 0.0;
//      }
//    }
//  }
//
//    height *= 2;
//    height++;
////  //test - end


//
//  for (int i=0; i<height; i++)
//  {
//    for (int j=0; j<width; j++)
//    {
//      nightDensity[i][j] = density[i][j];
//    }
//  }
//
//
//  //test2 - start
//  for (int i = 0 ; i< maxx/2 ;i++)
//  {
//    int x = rand()%(height/3);
//    int y = rand()%(height/3);
//    int s = 3+rand()%10;
//    int val = rand()%25;
//    int newX = rand()%(height/2);
//    int newY = rand()%(height/2);
//    int minus = rand() %2;
//    if (minus == 1)
//    {
//      x *= -1;
//    }
//    minus = rand() %2;
//    if (minus == 1)
//    {
//      y *= -1;
//    }
//    minus = rand() %2;
//    if (minus == 1)
//    {
//      newX *= -1;
//    }
//    minus = rand() %2;
//    if (minus == 1)
//    {
//      newY *= -1;
//    }
//    x+=height/2;
//    y+=height/2;
//    newX+=height/2;
//    newY+=height/2;
//    for (int j = 0; j< s && x+j < height && newX+j<height; j++)
//    {
//      for (int k = 0; k< s && y+k < height && newY+k<height; k++)
//      {
//        int random = rand()%maxR;
//        float value = random*(1.0/(2.0*2.0*M_PI*M_PI))*exp(-0.0005*(((float)(x+j)-height/2.0)*((float)(x+j)-height/2.0)+((float)(y+k)-width/2.0)*((float)(y+k)-width/2.0)));
//        float pop = nightDensity[x+j][y+k];
//        if (pop < value)
//        {
//          nightDensity[newX+j][newY+k] += pop;
//          nightDensity[x+j][y+k] -= pop;
//        }
//        else
//        {
//          nightDensity[newX+j][newY+k] += value;
//          nightDensity[x+j][y+k] -= value;
//        }
//      }
//    }
//  }
//  //test2 - end
//  drawSVGpopulation(10);
//  for (int i=1;i<height-1;i++)
//  {
//    for (int j=1;j<height-1;j++)
//    {
////      if(density[i][j]<10.0)
////      {
////        if(density[i+1][j]>10.0 ||
////           density[i-1][j]>10.0 ||
////           density[i][j+1]>10.0 ||
////           density[i][j-1]>10.0)
////        {
////          nightDensity[i][j]=(nightDensity[i][j]-density[i][j]+250.0)/2.0;
////        }
////      }
//    }
//  }


//  test3
float*** gradField = gradientField(heightMap,height,width);
float maxMove = 0.10;
float maxMovePop = maxMove;
float minMove = 0.015;
float minMovePop = minMove;
float movePop = maxMovePop - minMovePop * 4;
for(int a = 0; a < 200; a++)
{
  for(int i = 0; i < height; i++)
  {
    for(int j = 0; j < width; j++)
    {
      if (density[i][j] > 0.00002)
      {
        if (density[i][j] > 120)
        {
          maxMovePop = 0.15;
          minMovePop = 0.02;
        }
        if (density[i][j] > 180)
        {
          maxMovePop = 0.3;
          minMovePop = 0.05;
        }
        if (density[i][j] > 300)
        {
          maxMovePop = 0.4;
          minMovePop = 0.08;
        }
        if (density[i][j] > 500)
        {
          maxMovePop = 0.6;
          minMovePop = 0.1;
        }
        if (density[i][j] > 700)
        {
          maxMovePop = 0.7;
          minMovePop = 0.15;
        }
        movePop = maxMovePop - minMovePop * 4;
        float* grad = gradField[i][j];
        float gradRow = grad[1];
        float gradCol = grad[0];
        float sumxy = fabs(gradRow) + fabs(gradCol);
        float plusRowFactor = -1.0;
        if (gradRow < 0)
        {
          gradRow *= -1.0;
          plusRowFactor = 1.0;
        }
        float plusColFactor = -1.0;
        if (gradCol < 0)
        {
          gradCol *= -1.0;
          plusColFactor = 1.0;
        }

        if (i == 0 && j == 0)
        {
          density[i+1][j] += density[i][j] * (minMovePop + movePop * (gradRow / sumxy));
          density[i][j+1] += density[i][j] * (minMovePop + movePop * (gradCol / sumxy));
          density[i][j] -= (density[i][j] * maxMovePop);
        }
        else if (i == 0 && j == width-1)
        {
          density[i+1][j] += density[i][j] * (minMovePop + movePop * (gradRow / sumxy));
          density[i][j-1] += density[i][j] * (minMovePop + movePop * (gradCol / sumxy));
          density[i][j] -= (density[i][j] * maxMovePop);
        }
        else if (i == height-1 && j == 0)
        {
          density[i-1][j] += density[i][j] * (minMovePop + movePop * (gradRow / sumxy));
          density[i][j+1] += density[i][j] * (minMovePop + movePop * (gradCol / sumxy));
          density[i][j] -= (density[i][j] * maxMovePop);
        }
        else if (i == height-1 && j == width-1)
        {
          density[i-1][j] += density[i][j] * (minMovePop + movePop * (gradRow / sumxy));
          density[i][j-1] += density[i][j] * (minMovePop + movePop * (gradCol / sumxy));
          density[i][j] -= (density[i][j] * maxMovePop);
        }
        else if (i == 0)
        {
          density[i+1][j] += density[i][j] * (minMovePop + movePop * (gradRow / sumxy));
          density[i][j-1] += density[i][j] * (minMovePop + movePop * (gradCol / (sumxy*2.0) - plusColFactor * (gradCol / (sumxy*4.0))));
          density[i][j+1] += density[i][j] * (minMovePop + movePop * (gradCol / (sumxy*2.0) + plusColFactor * (gradCol / (sumxy*4.0))));
          density[i][j] -= (density[i][j] * maxMovePop);
        }
        else if (i == height-1)
        {
          density[i-1][j] += density[i][j] * (minMovePop + movePop * (gradRow / sumxy));
          density[i][j-1] += density[i][j] * (minMovePop + movePop * (gradCol / (sumxy*2.0) - plusColFactor * (gradCol / (sumxy*4.0))));
          density[i][j+1] += density[i][j] * (minMovePop + movePop * (gradCol / (sumxy*2.0) + plusColFactor * (gradCol / (sumxy*4.0))));
          density[i][j] -= (density[i][j] * maxMovePop);
        }
        else if (j == 0)
        {
          density[i-1][j] += density[i][j] * (minMovePop + movePop * (gradRow / (sumxy*2.0) - plusRowFactor * (gradRow / (sumxy*4.0))));
          density[i+1][j] += density[i][j] * (minMovePop + movePop * (gradRow / (sumxy*2.0) + plusRowFactor * (gradRow / (sumxy*4.0))));
          density[i][j+1] += density[i][j] * (minMovePop + movePop * (gradCol / sumxy));
          density[i][j] -= (density[i][j] * maxMovePop);
        }
        else if (j == width-1)
        {
          density[i-1][j] += density[i][j] * (minMovePop + movePop * (gradRow / (sumxy*2.0) - plusRowFactor * (gradRow / (sumxy*4.0))));
          density[i+1][j] += density[i][j] * (minMovePop + movePop * (gradRow / (sumxy*2.0) + plusRowFactor * (gradRow / (sumxy*4.0))));
          density[i][j-1] += density[i][j] * (minMovePop + movePop * (gradCol / sumxy));
          density[i][j] -= (density[i][j] * maxMovePop);
        }
        else
        {
          density[i-1][j] += density[i][j] * (minMovePop + movePop * (gradRow / (sumxy*2.0) - plusRowFactor * (gradRow / (sumxy*4.0))));
          density[i+1][j] += density[i][j] * (minMovePop + movePop * (gradRow / (sumxy*2.0) + plusRowFactor * (gradRow / (sumxy*4.0))));
          density[i][j-1] += density[i][j] * (minMovePop + movePop * (gradCol / (sumxy*2.0) - plusColFactor * (gradCol / (sumxy*4.0))));
          density[i][j+1] += density[i][j] * (minMovePop + movePop * (gradCol / (sumxy*2.0) + plusColFactor * (gradCol / (sumxy*4.0))));
          density[i][j] -= (density[i][j] * maxMovePop);
        }
      }
      maxMovePop = maxMove;
      minMovePop = minMove;
    }
  }
}
//  /test3

  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      if (density[i][j] > 1000)
      {
        density[i][j] /= 2.0;
      }
      density[i][j] /= 2.0;
    }
  }
old*/
if (debug)
{
  drawSVGpopulation(10);
}
}

Population::~Population ()
{
  for(int i = 0; i < width; i++)
    {
      delete [] density[i];
    }
  delete [] density;
}

float** Population::getDensity ()
{
  return density;
}

void Population::drawSVGpopulation(int magnify)
{
  FILE* file = fopen("debug/populationDAY.svg","w+");
  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);

  float multiply = (float) magnify;

  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      fprintf(file,"\t<rect x=\"%d\" y=\"%d\" width=\"%f\" height=\"%f\" style=\"fill:rgb(%d,%d,%d)\"/>\n",j*magnify,i*magnify,multiply,multiply,(int)density[i][j],(int)density[i][j],(int)density[i][j]);
    }
  }

  fprintf(file,"</svg>");
  fclose(file);

//  file = fopen("debug/populationNIGHT.svg","w+");
//  fprintf(file,"<svg width=\"%dpx\" height=\"%dpx\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" >\n", (width+1)*magnify, (height+1)*magnify);
//
//  for (int i=0; i<height; i++)
//  {
//    for (int j=0; j<width; j++)
//    {
//      fprintf(file,"\t<rect x=\"%d\" y=\"%d\" width=\"%f\" height=\"%f\" style=\"fill:rgb(%d,%d,%d)\"/>\n",j*magnify,i*magnify,multiply,multiply,(int)nightDensity[i][j],(int)nightDensity[i][j],(int)nightDensity[i][j]);
//    }
//  }

//  fprintf(file,"</svg>");
//  fclose(file);
}

void Population::drawSVGpopulation(int magnify, FILE* file)
{
  float multiply = (float) magnify;

  for (int i=0; i<height; i++)
  {
    for (int j=0; j<width; j++)
    {
      fprintf(file,"\t<rect x=\"%d\" y=\"%d\" width=\"%f\" height=\"%f\" style=\"fill:rgb(%d,%d,%d)\"/>\n",j*magnify,i*magnify,multiply,multiply,(int)density[i][j],(int)density[i][j],(int)density[i][j]);
    }
  }
}

void Population::generateModel(City* city)
{
  (void)(city);

//  int popNumber = terrain->getHeight() * terrain->getWidth() * 6;
  Terrain* terrain = city->getTerrain();
  this->modelSize = terrain->getHeight() * terrain->getWidth() * 6;
//  GLfloat* popVertices = createVerticesFromPopulation(population, number, 6);
  this->model = createVerticesFromPopulation(this, modelSize, 6);

//  int elementPopNumber = 2 * terrain->getWidth() * (terrain->getHeight()-1) + terrain->getHeight();
  this->indicesSize = 2 * terrain->getWidth() * (terrain->getHeight()-1) + terrain->getHeight();
//  GLuint* popElements = createElementsFromPopulation(population, elementPopNumber);
  this->indices = createElementsFromPopulation(city->getPopulation(), this->indicesSize);
  addNormalsForStripTriangles(model, modelSize, indices, indicesSize, 3, 6);

//  GLuint popVBO, popVAO, popEBO;
  glGenVertexArrays(1, &this->vao);
  glGenBuffers(1, &this->vbo);
  glGenBuffers(1, &this->ebo);
  // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, modelSize * sizeof(GLfloat), model, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(GLuint), indices, GL_STATIC_DRAW);
  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  // TexCoord attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0); // Unbind VAO
}

//#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
//extern Shader ourShader2;
extern glm::vec3 bigger;

void Population::showModel()
{
//    ourShader2.Use();
//    glBindVertexArray(this->vao);
//    glUniform3f(glGetUniformLocation(ourShader2.Program, "objectColor"), 1.0f, 1.0f, 0.0f);
////      for(GLuint i = 0; i < 1/*0*/; i++)
////      {
//        // Calculate the model matrix for each object and pass it to shader before drawing
//        glm::mat4 model;
////          model = glm::translate(model, cubePositions[i]);
////            GLfloat angle = 20.0f * i;
////            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
//      model = glm::scale(model, bigger);
//      glUniformMatrix4fv(glGetUniformLocation(ourShader2.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
//
////            glDrawArrays(GL_TRIANGLES, 0, 36);
////          glDrawArrays(GL_TRIANGLES, 0, number);
//        glDrawElements(GL_TRIANGLE_STRIP, this->indicesSize, GL_UNSIGNED_INT, 0);
////            glDrawArrays(GL_LINES, 0, 36);
////      }
}
