/*
 * This header file contains the  functions that will render the
 * polygons using static and dynamic screen
 *
 */
#ifndef RENDER_H
#define RENDER_H
#include "GeometricalStructures.h"
#include "display.h"

struct EdgeList{
	int start;
	int end;
	EdgeList *next; //node after me
	EdgeList *previous; //node before me
};

struct EdgeTable{
	int y;
	EdgeList *edgeList; //edge list for y
	EdgeTable *next;
};

int paintPolygonUsingDynamicScreen(Triangle *polygon, EdgeList **dynamicScreen, Display *display);
int paintPolygonUsingStaticScreen(Triangle *polygon, Display *display);

int freeEdgeList(EdgeList *edgeList);
#define	DSINDEX(y)	(y*sizeof(EdgeList))
#endif