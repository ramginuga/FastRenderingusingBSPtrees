// bsp_trees.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <conio.h>
#include "PolygonGenerator.h"
#include "BSPTree.h"
#include "BSPTreeTraversal.h"

#define SMALL 10
#define MEDIUM 11
#define LARGE 12
#define ePIXEL 13
using namespace std;

double PCFreq = 0.0;
__int64 CounterStart = 0;

Vertex viewpoint = { 128, 128, 128}; 

//This function marks the start counter
void StartCounter()
{
	LARGE_INTEGER li;
	if(!QueryPerformanceFrequency(&li))
		cout << "QueryPerformanceFrequency failed!\n";

	PCFreq = double(li.QuadPart)/1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
	//cout<< " start:"<< CounterStart;
}

// This function gives the time period given task takes(End time- Start time)
double GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	//cout<< " end:"<<li.QuadPart;

	//cout<<"cout"<<double(li.QuadPart-CounterStart)/PCFreq<<"\n";
	return double(li.QuadPart-CounterStart)/PCFreq;
}
void polygonfactor(int numberOfTriangles, SizeOfTriangle sizeOfTriangle, Triangle *polygons)
{
	PolygonGenerator p;	
	vector<Triangle> ps = p.Generate(sizeOfTriangle, numberOfTriangles);
	std::copy(ps.begin(), ps.end(), polygons);
}

void BSP(Triangle* triangles, BSP_tree** tree, int total_triangles)
{
	populateBSPTree(triangles, tree, total_triangles);
}

int runExperiment()
{
	wchar_t debugStr[512];
	int k;			
	int range= ePIXEL;
	int noofpolygon=10;		//no of polygons

	FILE *smallfile, *mediumfile, *largefile, *epixelfile;
	errno_t fileOpenError;
	fileOpenError = fopen_s(&smallfile,"smallfile.txt","w");
	if(fileOpenError != 0){
		swprintf(debugStr,512,L"Error: file 'smallfile.txt' opening error\n");OutputDebugString(debugStr);return 1;
	}
	fileOpenError = fopen_s(&mediumfile,"mediumfile.txt","w");
	if(fileOpenError != 0){
		swprintf(debugStr,512,L"Error: file 'mediumfile.txt' opening error\n");OutputDebugString(debugStr);return 1;
	}
	fileOpenError = fopen_s(&largefile,"largefile.txt","w");
	if(fileOpenError != 0){
		swprintf(debugStr,512,L"Error: file 'largefile.txt' opening error\n");OutputDebugString(debugStr);return 1;
	}
	fileOpenError = fopen_s(&epixelfile,"epixelfile.txt","w");
	if(fileOpenError != 0){
		swprintf(debugStr,512,L"Error: file 'epixelfile.txt' opening error\n");OutputDebugString(debugStr);return 1;
	}
	while(range <= ePIXEL)
	{
		switch(range)
		{
		case SMALL:
			for(k=5;k<56;k++)
			{
				//Triangle *polygons = (Triangle *)malloc(sizeof(Triangle) * k);
				Triangle *polygons = new Triangle[k];;
				/*
				(polygons[0]).a.x = 2.0;(polygons[0]).a.y = 0.0;(polygons[0]).a.z = 0.0;
				(polygons[0]).b.x = 0.0;(polygons[0]).b.y = 2.0;(polygons[0]).b.z = 0.0;
				(polygons[0]).c.x = 4.0;(polygons[0]).c.y = 2.0;(polygons[0]).c.z = 0.0;
				
				(polygons[1]).a.x = 5.0;(polygons[1]).a.y = 0.0;(polygons[1]).a.z = 20.0;
				(polygons[1]).b.x = 4.0;(polygons[1]).b.y = 2.0;(polygons[1]).b.z = 20.0;
				(polygons[1]).c.x = 8.0;(polygons[1]).c.y = 3.0;(polygons[1]).c.z = 20.0;

				(polygons[2]).a.x = 1.0;(polygons[2]).a.y = 3.0;(polygons[2]).a.z = 40.0;
				(polygons[2]).b.x = 1.0;(polygons[2]).b.y = 4.0;(polygons[2]).b.z = 40.0;
				(polygons[2]).c.x = 2.0;(polygons[2]).c.y = 4.0;(polygons[2]).c.z = 40.0;
				*/
				BSP_tree* bsp_tree = NULL;
				polygonfactor(k, P_SMALL, polygons);

				BSP(polygons, &bsp_tree, k);
				StartCounter();
				//renderUsingFrontToBack(bsp_tree, viewpoint);
				//traversalFTB();
				//cout << k<<",					"<<GetCounter() <<"  FTB\n";
				///
				fprintf(smallfile, "%d\t%lf\t", k, GetCounter());
				fflush(smallfile);
				StartCounter();
				renderUsingBackToFront(bsp_tree, viewpoint);				
				//cout << k<<",	"<<GetCounter() <<"  BTF\n";

				fprintf(smallfile, "%lf\n", GetCounter());
				fflush(smallfile);
				exit(0);
				///
			}
			fclose(smallfile);
			break;
		case MEDIUM:
			for(k=1;k<noofpolygon;k++)
			{
				Triangle *polygons = new Triangle[k];
				BSP_tree* bsp_tree = NULL;
				polygonfactor(k, P_MEDIUM, polygons);
				BSP(polygons, &bsp_tree, k);
				StartCounter();
				renderUsingFrontToBack(bsp_tree, viewpoint);
				cout << GetCounter() <<"\n";
				fprintf(mediumfile, "%d\t%lf\t", k, GetCounter());
				fflush(mediumfile);
				StartCounter();
				renderUsingBackToFront(bsp_tree, viewpoint);	
				cout << GetCounter() <<"\n";
				fprintf(mediumfile, "%lf\n", GetCounter());
				fflush(mediumfile);
			}
			fclose(mediumfile);
			break;
		case LARGE:
			for(k=1;k<noofpolygon;k++)
			{
				Triangle *polygons = new Triangle[k];
				BSP_tree* bsp_tree = NULL;
				polygonfactor(k, P_LARGE, polygons);
				BSP(polygons, &bsp_tree, k);
				StartCounter();
				renderUsingFrontToBack(bsp_tree, viewpoint);
				cout << GetCounter() <<"\n";
				fprintf(largefile, "%d\t%lf\t", k, GetCounter());
				fflush(largefile);
				StartCounter();
				renderUsingBackToFront(bsp_tree, viewpoint);

				cout << GetCounter() <<"\n";
				fprintf(largefile, "%lf\n", GetCounter());
				fflush(largefile);
			}
			fclose(largefile);
			break;
		case ePIXEL:
			for(k=0; k<ePIXEL; k++)
			{
				PolygonGenerator p;
				BSP_tree *bsp_tree;

				//polygon generation
				vector<Triangle> triangles_covered_by_each_pixel = p.Generate(k);
				Triangle *polygons = new Triangle[k * 2];;
				std::copy(triangles_covered_by_each_pixel.begin(), triangles_covered_by_each_pixel.end(), polygons);

				//BSP();
				BSP(polygons, &bsp_tree, k * 2);
				StartCounter();
				renderUsingFrontToBack(bsp_tree, viewpoint);
				cout << GetCounter() <<"\n";
				fprintf(epixelfile, "%d\t%lf\t", k, GetCounter());
				fflush(epixelfile);
				StartCounter();
				renderUsingBackToFront(bsp_tree, viewpoint);
				cout << GetCounter() <<"\n";
				fprintf(epixelfile, "%lf\n", GetCounter());
				fflush(epixelfile);				
			}
			fclose(epixelfile);
			break;

		default:
			cout<<"Something is fishy :p";
			break;
		}
		range++;
	}
	/*
	//TCanvas *c1 = new TCanvas("c1","A Simple Graph Example",200,10,700,500);
	//while (	fscanf(smallfile, "%d%ld%ld", k, time1, time2) !=NULL )
	{

	}
	*/
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	return runExperiment();
}

