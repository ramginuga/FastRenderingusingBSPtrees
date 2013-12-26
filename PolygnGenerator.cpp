#include "stdafx.h"
#include "PolygonGenerator.h"
#include "GlobalVariables.h"

PolygonGenerator::PolygonGenerator()
{
	xres =  XRES;
	yres = YRES;
	srand(	time(NULL)	);
}

PolygonGenerator::PolygonGenerator(int xres, int yres)
{
	PolygonGenerator();
	this->xres = xres;
	this->yres = yres;
}

PolygonGenerator::~PolygonGenerator()
{
}


float PolygonGenerator::getRandomNumber( int start, int end )
{
	return (start + ( std::rand() % ( end - start + 1 ) )) * 1.0f;
}

//input sizeOfTriangles, number of triangles
//output : vector of triangles

std::vector<Triangle> PolygonGenerator::Generate( SizeOfTriangle sizeOfTriangle, int numberOfTriangles )
{
	std::vector<Triangle> generated;

	for (int i = 0; i < numberOfTriangles; i++)
	{
		generated.push_back(getRandomTriangle(sizeOfTriangle, i * 20));
	}
	
	return generated;
}

//input range expected: 0 < pixels covered
//output format : vector of triangles

std::vector<Triangle> PolygonGenerator::Generate( int pixelsCovered )
{
	std::vector<Triangle> generated;

	for(int i=0; i< pixelsCovered; i++){

		//float zRand = getRandomNumber(10 * i ,10 * (i+1) - 1);

		generated.push_back(getUpperEdgeTriangle(i * 20));
		generated.push_back(getLowerEdgeTriangle(i* 20));
	}

	return generated;
}

Triangle PolygonGenerator::getUpperEdgeTriangle(int zRand)
{
	Triangle t;

	t.a.x = t.a.y= 0.0f;
	t.a.z = zRand;

	t.b.x = xres * 1.0f;
	t.b.y = 0.0f;
	t.b.z = zRand * 1.0f;

	t.c.x = 0;
	t.c.y = yres * 1.0f;
	t.c.z = zRand * 1.0f;

	return t;
}

Triangle PolygonGenerator::getLowerEdgeTriangle(int zRand)
{
	Triangle t;

	t.a.x = xres * 1.0f;
	t.a.y= yres * 1.0f;
	t.a.z = zRand * 1.0f;

	t.b.x = xres * 1.0f;
	t.b.y = 0.0f;
	t.b.z = zRand * 1.0f;

	t.c.x = 0.0f;
	t.c.y = yres * 1.0f;
	t.c.z = zRand * 1.0f;

	return t;
}

// 16 blocks , 8 , 4 , 1
Triangle PolygonGenerator::getTriangle( int z, int blockSize ) 
{
	Vertex a,b,c;
	int sizeOfXblock = xres / blockSize;
	int sizeOfYblock = yres / blockSize;

	int xblock = getRandomNumber(0, blockSize - 1 ); // 0 to 15
	int yblock = getRandomNumber(0, blockSize - 1 ); // 0 to 15

	a.x = getRandomNumber( xblock * sizeOfXblock, xblock * sizeOfXblock + sizeOfXblock - 1 );
	a.y = getRandomNumber( yblock * sizeOfYblock, yblock * sizeOfYblock + sizeOfYblock - 1 );	

	b.x = getRandomNumber( xblock * sizeOfXblock, xblock * sizeOfXblock + sizeOfXblock - 1 );
	b.y = getRandomNumber( yblock * sizeOfYblock, yblock * sizeOfYblock + sizeOfYblock - 1 );	

	c.x = getRandomNumber( xblock * sizeOfXblock, xblock * sizeOfXblock + sizeOfXblock - 1 );
	c.y = getRandomNumber( yblock * sizeOfYblock, yblock * sizeOfYblock + sizeOfYblock - 1 );	

	a.z = b.z = c.z = z;

	Triangle t ;
	t.a = a;
	t.b = b;
	t.c = c;

	return t;
}

Triangle PolygonGenerator::getRandomTriangle(SizeOfTriangle sizeOfTriangle, int z) 
{
	Triangle t;
	switch (sizeOfTriangle)
	{
		case P_VERY_SMALL:
			return getTriangle(z, 16); 
			break;

		case P_SMALL:
			return getTriangle(z, 8); 
			break;

		case P_MEDIUM:
			return getTriangle(z, 4); 
			break;

		case P_LARGE:
			return getTriangle(z, 2); 
			break;

		case P_VERY_LARGE:
			return getTriangle(z, 1); 
			break;

	}
	
}