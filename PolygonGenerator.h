#ifndef POLYGON_GENERATOR_H
#define POLYGON_GENERATOR_H

#include <iostream>
#include <vector>
#include <time.h>
#include "GeometricalStructures.h"

enum SizeOfTriangle
{
	P_VERY_SMALL,
	P_SMALL,
	P_MEDIUM,
	P_LARGE,
	P_VERY_LARGE
};

class PolygonGenerator
{
public:
	PolygonGenerator();
	PolygonGenerator(int xres, int yres);
	~PolygonGenerator();

	std::vector<Triangle> Generate(SizeOfTriangle sizeOfTriangle, int numberOfTriangles); 
	std::vector<Triangle> Generate(int pixelsCovered);

private:
	float getRandomNumber(int start, int end);
	
	Triangle getUpperEdgeTriangle(int);
	Triangle getLowerEdgeTriangle(int);

	Triangle getRandomTriangle(SizeOfTriangle sizeOfTriangle, int z);
	Triangle getTriangle( int z, int blockSize );
	int xres, yres;
};

#endif