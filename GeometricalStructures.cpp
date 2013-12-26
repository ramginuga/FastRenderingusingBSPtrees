#include "stdafx.h"
#include "GeometricalStructures.h"
#include <stdlib.h>
#include <Windows.h>

wchar_t *stringVertex(Vertex *vertex){
	wchar_t debugStr[512];
	if (vertex == NULL){
		swprintf(debugStr,512,L"Error: stringVertex vertex=NULL parameter passed\n");OutputDebugString(debugStr);return NULL;
	}
	wchar_t *vertexInStringFormat = (wchar_t *)malloc(512*sizeof(wchar_t));
	swprintf(vertexInStringFormat,512,L"(x:%f,y:%f,z:%f)",vertex->x,vertex->y,vertex->z);
	return vertexInStringFormat;
}

wchar_t *stringTriangle(Triangle *triangle){
	wchar_t debugStr[512];
	if (triangle == NULL){
		swprintf(debugStr,512,L"Error: stringTriangle triangle=NULL parameter passed\n");OutputDebugString(debugStr);return NULL;
	}
	wchar_t *triangleInStringFormat = (wchar_t *)malloc(1024*sizeof(wchar_t));
	wchar_t *stringVertexA = stringVertex(&(triangle->a));
	wchar_t *stringVertexB = stringVertex(&(triangle->b));
	wchar_t *stringVertexC = stringVertex(&(triangle->c));
	swprintf(triangleInStringFormat,1024,L"(a:%s \t b:%s \t c:%s)",stringVertexA,stringVertexB,stringVertexC);
	free(stringVertexA);
	free(stringVertexB);
	free(stringVertexC);
	return triangleInStringFormat;
}

wchar_t *stringPlane(Plane *plane){
	wchar_t debugStr[512];
	if (plane == NULL){
		swprintf(debugStr,512,L"Error: stringPlane plane=NULL parameter passed\n");OutputDebugString(debugStr);return NULL;
	}
	wchar_t *planeInStringFormat = (wchar_t *)malloc(512*sizeof(wchar_t));
	swprintf(planeInStringFormat,512,L"(a:%f,b:%f,c:%f,d:%f)",plane->a,plane->b,plane->c,plane->d);
	return planeInStringFormat;
}