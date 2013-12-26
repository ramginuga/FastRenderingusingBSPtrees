#include "stdafx.h"
#include "render.h"
#include "GlobalVariables.h"
#include "math.h"
#include "stdlib.h"
#include <Windows.h>

typedef struct {
	Vertex start;
	Vertex end;
	Vertex current;
	float slopeX;
	float slopeZ;
	int edgeType; //0 : left edge, 1 : right edge, 2: top edge, 3: bottom edge
} EdgeDDA;

typedef struct {
	Vertex start;
	Vertex end;
	Vertex current;
	float slopeZ;
} SpanDDA;
//tells wheather the edge is top/bottom or right/left of the triangle
typedef struct {
	int top; //0 for bottom 1 for top
	int right; //0 for left 1 for right
} EdgeType;

typedef struct {
	float A; //x coefficient
	float B; // y coefficient
	float C ; // constant
} LineEq;

int swapTwoVertex(Vertex *v1, Vertex *v2);


int mergeEdgeTable(EdgeTable *edgeTable, EdgeList **dynamicScreen, Display *display);
int changeEdgeSegmentBounds(int newStart,int newEnd,EdgeList *edgeSegment);
int deleteEdgeSegment(EdgeList *edgeSegment, EdgeList **dynamicScreen, int dynamicScreenIndex);
int splitEdgeSegment(int newEndOfEdgeSegment,int startOfNewEdgeSegment,EdgeList *edgeSegment);
int paintScanLine(int start, int end, Display *display);

int scanPolygon(Triangle *polygon, EdgeTable **edgeTable);
int sortTriangleVertexForSequenceOfUse(Triangle *polygon, EdgeType *edgeType);
int initSpanDDA(EdgeDDA *edgeDDA12, EdgeDDA *edgeDDA13, SpanDDA * spanDDA);
int initEdgeDDA(Vertex *vertex1, Vertex *vertex2, EdgeDDA * edgeDDA, int edgeType);
int getLineEq(LineEq *lineEq, float x1, float y1, float x2, float y2);
int getEdgeType(EdgeType *edgeType);

int freeEdgeTable(EdgeTable *edgeTable);

int paintPolygonUsingStaticScreen(Triangle *polygon, Display *display){
	wchar_t debugStr[2048];
	if(polygon == NULL){
		swprintf(debugStr,2048,L"Error: paintPolygonUsingDynamicScreen polygon=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if(display == NULL){
		swprintf(debugStr,2048,L"Error: paintPolygonUsingDynamicScreen display=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}

	EdgeTable *edgeTable = NULL;
	scanPolygon(polygon,&edgeTable);

	//write all edgeList in edgeTable to the display
	EdgeTable *currentEdgeTableNode = edgeTable;
	while(currentEdgeTableNode){
		EdgeList *edgeList = currentEdgeTableNode->edgeList;
		EdgeList *currentEdgeListNode = edgeList;
		while(currentEdgeListNode){
			//paint the currentEdgeListNode to the display
			paintScanLine(currentEdgeListNode->start,currentEdgeListNode->end,display);
			currentEdgeListNode = currentEdgeListNode->next;
		}//end of edgeList loop
		currentEdgeTableNode = currentEdgeTableNode->next;
	}//end of edgTable loop
	freeEdgeTable(edgeTable);
	return 0;
}

int paintPolygonUsingDynamicScreen(Triangle *polygon, EdgeList **dynamicScreen, Display *display){
	wchar_t debugStr[2048];
	bool paintPolygonUsingDynamicScreenDebug = false;
	if(paintPolygonUsingDynamicScreenDebug){swprintf(debugStr,2048,L"paintPolygonUsingDynamicScreen Started\n");OutputDebugString(debugStr);}
	if(polygon == NULL){
		swprintf(debugStr,2048,L"Error: paintPolygonUsingDynamicScreen polygon=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if(dynamicScreen == NULL){
		swprintf(debugStr,2048,L"Error: paintPolygonUsingDynamicScreen dynamicScreen=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if(display == NULL){
		swprintf(debugStr,2048,L"Error: paintPolygonUsingDynamicScreen display=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	EdgeTable *edgeTable = NULL;
	scanPolygon(polygon,&edgeTable);
	mergeEdgeTable(edgeTable,dynamicScreen,display);
	freeEdgeTable(edgeTable);
	if(paintPolygonUsingDynamicScreenDebug){swprintf(debugStr,2048,L"paintPolygonUsingDynamicScreen Ended\n");OutputDebugString(debugStr);}
	return 0;
}

int scanPolygon(Triangle *polygon, EdgeTable **edgeTable){
	wchar_t debugStr[2048];
	bool scanPolygonDebug = false;
	if(scanPolygonDebug){swprintf(debugStr,2048,L"scanPolygon Started\n");OutputDebugString(debugStr);}
	if(polygon == NULL){
		swprintf(debugStr,2048,L"Error: scanPolygon polygon=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	EdgeTable *currentEdgeTableNode = NULL;
	//first sort vertex for sequence of use
	//data structure to store edge types top/bottom left/relation relation
	EdgeType edgeType[3];
	//initilize them
	for(int m=0;m<3;m++){
		edgeType[m].right = 0;
		edgeType[m].top = 0;
	}
	if(sortTriangleVertexForSequenceOfUse(polygon,edgeType))
		return 1;

	//setup edge DDA's
	EdgeDDA edgeDDA[3];
	if(initEdgeDDA(&polygon->a,&polygon->b,&edgeDDA[0],getEdgeType(&edgeType[0])))
			return 1;
	if(initEdgeDDA(&polygon->b,&polygon->c,&edgeDDA[1],getEdgeType(&edgeType[1])))
			return 1;
	if(initEdgeDDA(&polygon->a,&polygon->c,&edgeDDA[2],getEdgeType(&edgeType[2])))
			return 1;
	EdgeDDA *edgeDDA12 = &edgeDDA[0];
	EdgeDDA *edgeDDA13 = &edgeDDA[2];

	//advance edgeDDA's to the top line
	float deltaY = ((ceil(edgeDDA12->start.y))-edgeDDA12->start.y);
	edgeDDA12->current.x += (edgeDDA12->slopeX*deltaY);
	edgeDDA12->current.y += (deltaY);
	edgeDDA12->current.z += (edgeDDA12->slopeZ*deltaY);

	edgeDDA13->current.x += (edgeDDA13->slopeX*deltaY);
	edgeDDA13->current.y += (deltaY);
	edgeDDA13->current.z += (edgeDDA13->slopeZ*deltaY);
	deltaY = 1.0;

	while(true){
		if(edgeDDA13->current.y > edgeDDA13->end.y){
			//done rasterizing
			break;
		}
		if(edgeDDA12->current.y > edgeDDA12->end.y){
			//change 1-2 to 2-3 edge
			edgeDDA12 = &edgeDDA[1];
			deltaY = ((ceil(edgeDDA12->start.y))-edgeDDA12->start.y);
			edgeDDA12->current.x += (edgeDDA12->slopeX*deltaY);
			edgeDDA12->current.y += (deltaY);
			edgeDDA12->current.z += (edgeDDA12->slopeZ*deltaY);
			deltaY = 1.0;
		}
		
		//init span DDA's
		SpanDDA spanDDA;
		if(initSpanDDA(edgeDDA12,edgeDDA13,&spanDDA))
			return 1;

		//set span DDA to left most covered pixel
		float deltaX = ((ceil(spanDDA.start.x)) - (spanDDA.start.x));
		spanDDA.current.x = (spanDDA.current.x + deltaX);
		spanDDA.current.z = (spanDDA.current.z + deltaX*spanDDA.slopeZ);
		deltaX = 1.0;
		
		//create edgeTable Node and set it to current
		if(*edgeTable == NULL){
			//create a root node
			*edgeTable = (EdgeTable *) malloc(sizeof(EdgeTable));
			currentEdgeTableNode = *edgeTable;
		}
		else{
			//allocate memeory to next node and update current node
			currentEdgeTableNode->next = (EdgeTable *) malloc(sizeof(EdgeTable));
			currentEdgeTableNode = currentEdgeTableNode->next;
		}
		currentEdgeTableNode->y = (int)edgeDDA13->current.y;
		currentEdgeTableNode->next = NULL;

		//set edgeList in edgeTable for current y
		currentEdgeTableNode->edgeList = (EdgeList *)malloc(sizeof(EdgeList));
		currentEdgeTableNode->edgeList->start = (int)spanDDA.current.x;
		currentEdgeTableNode->edgeList->end = (int)floor(spanDDA.end.x);
		//we just have one node in EdgeList as of now
		currentEdgeTableNode->edgeList->next = NULL;
		currentEdgeTableNode->edgeList->previous = NULL;

		//update currentEdgeTableNode
		//currentEdgeTableNode = currentEdgeTableNode->next;
		
		//advance edgeDDA's
		edgeDDA12->current.x += (edgeDDA12->slopeX*deltaY);
		edgeDDA12->current.y += (deltaY);
		edgeDDA12->current.z += (edgeDDA12->slopeZ*deltaY);

		edgeDDA13->current.x += (edgeDDA13->slopeX*deltaY);
		edgeDDA13->current.y += (deltaY);
		edgeDDA13->current.z += (edgeDDA13->slopeZ*deltaY);

	}//end oF EdgeDDA's
	if(scanPolygonDebug){swprintf(debugStr,2048,L"scanPolygon Ended\n");OutputDebugString(debugStr);}
	return 0;
}

int mergeEdgeTable(EdgeTable *edgeTable, EdgeList **dynamicScreen, Display *display){
	wchar_t debugStr[2048];
	bool mergeEdgeTableDebug = false;
	if(mergeEdgeTableDebug){swprintf(debugStr,2048,L"mergeEdgeTable Started\n");OutputDebugString(debugStr);}
	if(edgeTable == NULL){
		swprintf(debugStr,2048,L"Error: mergeEdgeTable edgeTable=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if(dynamicScreen == NULL){
		swprintf(debugStr,2048,L"Error: mergeEdgeTable dynamicScreen=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if(display == NULL){
		swprintf(debugStr,2048,L"Error: mergeEdgeTable display=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}

	EdgeTable *currentEdgeTable = edgeTable;
	while(currentEdgeTable){
		EdgeList *objectEdgeSegment = currentEdgeTable->edgeList;
		EdgeList *currentObjectEdgeSegment = objectEdgeSegment;
		EdgeList *screenEdgeSegment = dynamicScreen[int(currentEdgeTable->y)];
		EdgeList *currentScreenEdgeSegment = screenEdgeSegment;
		while(currentObjectEdgeSegment && currentScreenEdgeSegment){
			EdgeList *nextScreenSegment = currentScreenEdgeSegment->next;
			EdgeList *nextObjectSegment = currentObjectEdgeSegment->next;
			//TODO: process all the cases and write to the display for matching screen edge segments
			//case1: action: next object segment
			if(currentObjectEdgeSegment->end < currentScreenEdgeSegment->start){
				currentObjectEdgeSegment = nextObjectSegment;
			}
			//case2: paint(K.MIN,J), change(J+1,K.MAX,K), next object segment
			else if((currentObjectEdgeSegment->start <= currentScreenEdgeSegment->start) && 
				((currentScreenEdgeSegment->start <= currentObjectEdgeSegment->end) && 
				(currentObjectEdgeSegment->end < currentScreenEdgeSegment->end))){
					paintScanLine(currentScreenEdgeSegment->start,currentObjectEdgeSegment->end,display);
					changeEdgeSegmentBounds(currentObjectEdgeSegment->end+1,currentScreenEdgeSegment->end,currentScreenEdgeSegment);
					currentObjectEdgeSegment = nextObjectSegment;
			}
			//case3: paint(I,J), split(I-1,J+1,K), next object segment 
			else if((currentObjectEdgeSegment->start > currentScreenEdgeSegment->start) && 
				(currentObjectEdgeSegment->end < currentScreenEdgeSegment->end)){
					paintScanLine(currentObjectEdgeSegment->start,currentObjectEdgeSegment->end,display);
					splitEdgeSegment(currentObjectEdgeSegment->start-1,currentObjectEdgeSegment->end+1,currentScreenEdgeSegment);
					currentObjectEdgeSegment = nextObjectSegment;
			}
			//case4 paint(K.MIN,K.MAX), delete(K), next object segment, next screen segment
			else if((currentObjectEdgeSegment->start <= currentScreenEdgeSegment->start) && 
				(currentObjectEdgeSegment->end == currentScreenEdgeSegment->end)){
					paintScanLine(currentScreenEdgeSegment->start,currentScreenEdgeSegment->end,display);
					deleteEdgeSegment(currentScreenEdgeSegment,dynamicScreen,currentEdgeTable->y);
					currentObjectEdgeSegment = nextObjectSegment;
					currentScreenEdgeSegment = nextScreenSegment;
			}
			//case5 paint(I,J), change(K.MIN,I-1,K), next object segment, next screen segment
			else if((currentObjectEdgeSegment->start > currentScreenEdgeSegment->start) && 
				(currentObjectEdgeSegment->end == currentScreenEdgeSegment->end)){
					paintScanLine(currentObjectEdgeSegment->start,currentObjectEdgeSegment->end,display);
					changeEdgeSegmentBounds(currentScreenEdgeSegment->start,currentObjectEdgeSegment->start-1,currentScreenEdgeSegment);
					currentObjectEdgeSegment = nextObjectSegment;
					currentScreenEdgeSegment = nextScreenSegment;
			}
			//case6 paint(K.MIN,K.MAX), delete(K), next screen segment
			else if((currentObjectEdgeSegment->start <= currentScreenEdgeSegment->start) && 
				(currentObjectEdgeSegment->end > currentScreenEdgeSegment->end)){
					paintScanLine(currentScreenEdgeSegment->start,currentScreenEdgeSegment->end,display);
					deleteEdgeSegment(currentScreenEdgeSegment,dynamicScreen,currentEdgeTable->y);
					currentScreenEdgeSegment = nextScreenSegment;
			}
			//case7 paint(I,K.MAX), change(K.MIN,I-1,K), next screen segment
			else if(((currentScreenEdgeSegment->end >= currentObjectEdgeSegment->start) &&
				(currentObjectEdgeSegment->start > currentScreenEdgeSegment->start)) &&
				(currentObjectEdgeSegment->end > currentScreenEdgeSegment->end)){
					paintScanLine(currentObjectEdgeSegment->start,currentScreenEdgeSegment->end,display);
					changeEdgeSegmentBounds(currentScreenEdgeSegment->start,currentObjectEdgeSegment->start-1,currentScreenEdgeSegment);
					currentScreenEdgeSegment = nextScreenSegment;
			}
			//case8 next screen segment
			else if(currentObjectEdgeSegment->start > currentScreenEdgeSegment->end){
				currentScreenEdgeSegment = nextScreenSegment;
			}
			else{
				swprintf(debugStr,2048,L"Error: mergeEdgeTable Invalid Case, It should not reach here\n");OutputDebugString(debugStr);
			}
		}//end of object edge segments
		currentEdgeTable = currentEdgeTable->next;
	}//end of edge table
	if(mergeEdgeTableDebug){swprintf(debugStr,2048,L"mergeEdgeTable Ended\n");OutputDebugString(debugStr);}
	return 0;
}

int paintScanLine(int start, int end, Display *display){
	wchar_t debugStr[512];
	if (display == NULL){
		swprintf(debugStr,512,L"Error: paintScanLine display=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}

	Color flatColor = {RED,GREEN,BLUE};
	putPixelToDisplay(display,&flatColor,start,end);
	return 1;
}

int changeEdgeSegmentBounds(int newStart, int newEnd, EdgeList *edgeSegment){
	wchar_t debugStr[512];

	if (edgeSegment == NULL){
		swprintf(debugStr,512,L"Error: changeEdgeSegmentBounds edgeSegment=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	edgeSegment->start = newStart;
	edgeSegment->end = newEnd;
	return 0;
}

int deleteEdgeSegment(EdgeList *edgeSegment, EdgeList **dynamicScreen, int dynamicScreenIndex){
	wchar_t debugStr[512];
	if (edgeSegment == NULL){
		swprintf(debugStr,512,L"Error: deleteEdgeSegment edgeSegment=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if(edgeSegment->previous == NULL && edgeSegment->next == NULL){
		// edgeSegment is the first node we are about to delete 
		// and edgeSegment is the only node at dynamicScreenIndex in dynamicScreen
		// need to update the dynamic screen array
		dynamicScreen[dynamicScreenIndex] = NULL;
	}
	else if(edgeSegment->previous == NULL && edgeSegment->next != NULL){
		//edgeSegment is the first node we are about to delete and there are nodes after this 
		dynamicScreen[dynamicScreenIndex] = edgeSegment->next;
		dynamicScreen[dynamicScreenIndex]->previous = NULL;
	}
	else if(edgeSegment->previous != NULL && edgeSegment->next == NULL){
		//edgeSegment is the last node we are about to delete and there are nodes before this 
		edgeSegment->previous->next = NULL;
	}
	else{
		// there is at-least one node before and after edgeSegment
		edgeSegment->previous->next = edgeSegment->next;
		edgeSegment->next->previous = edgeSegment->previous;
	}
	free(edgeSegment);
	return 0;
}

int splitEdgeSegment(int newEndOfEdgeSegment, int startOfNewEdgeSegment, EdgeList *edgeSegment){
	wchar_t debugStr[512];

	if (edgeSegment == NULL){
		swprintf(debugStr,512,L"Error: splitEdgeSegment edgeSegment=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	//create new edge segment
	EdgeList *newEdgeSegment = (EdgeList *)malloc(sizeof(EdgeList));;
	newEdgeSegment->start = startOfNewEdgeSegment;
	newEdgeSegment->end = edgeSegment->end;
	newEdgeSegment->next = edgeSegment->next;
	newEdgeSegment->previous = edgeSegment;

	//update old edge segment
	edgeSegment->end = newEndOfEdgeSegment;
	edgeSegment->next = newEdgeSegment;

	return 0;
}

int initSpanDDA(EdgeDDA *edgeDDA12, EdgeDDA *edgeDDA13, SpanDDA * spanDDA){
	wchar_t debugStr[512];

	if (edgeDDA12 == NULL){
		swprintf(debugStr,512,L"Error: initSpanDDA edgeDDA12=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if (edgeDDA13 == NULL){
		swprintf(debugStr,512,L"Error: initSpanDDA edgeDDA13=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if (spanDDA == NULL){
		swprintf(debugStr,512,L"Error: initSpanDDA spanDDA=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	//find the left and right edge
	EdgeDDA *leftEdgeDDA = edgeDDA12;
	EdgeDDA *rightEdgeDDA = edgeDDA13;
	if(edgeDDA12->edgeType == 1){
		leftEdgeDDA = edgeDDA13;
		rightEdgeDDA = edgeDDA12;
	}
	
	//now setup span DDA
	spanDDA->start.x = leftEdgeDDA->current.x;
	spanDDA->start.y = leftEdgeDDA->current.y;
	spanDDA->start.z = leftEdgeDDA->current.z;

	spanDDA->end.x = rightEdgeDDA->current.x;
	spanDDA->end.y = rightEdgeDDA->current.y;
	spanDDA->end.z = rightEdgeDDA->current.z;

	spanDDA->current.x = leftEdgeDDA->current.x;
	spanDDA->current.y = leftEdgeDDA->current.y;
	spanDDA->current.z = leftEdgeDDA->current.z;
	if((rightEdgeDDA->current.x - leftEdgeDDA->current.x) == (float)0.0){
		spanDDA->slopeZ = (float)0.2;
	}
	else{
		spanDDA->slopeZ = ((rightEdgeDDA->current.z - leftEdgeDDA->current.z)/(rightEdgeDDA->current.x - leftEdgeDDA->current.x));
	}
	
	return 0;
}

int initEdgeDDA(Vertex *vertex1, Vertex *vertex2, EdgeDDA * edgeDDA, int edgeType){
	wchar_t debugStr[512];
	bool initEdgeDDADebug = false;
	if (vertex1 == NULL){
		swprintf(debugStr,512,L"Error: initEdgeDDA vertex1=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if (vertex2 == NULL){
		swprintf(debugStr,512,L"Error: initEdgeDDA vertex2=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if (edgeDDA == NULL){
		swprintf(debugStr,512,L"Error: initEdgeDDA edgeDDA=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if(initEdgeDDADebug){wchar_t *stringVertexA = stringVertex(vertex1);wchar_t *stringVertexB = stringVertex(vertex2);wchar_t stringVertices[1024];swprintf(stringVertices,1024,L"(vertex1:%s \t vertex1:%s\n)",stringVertexA,stringVertexB);OutputDebugString(stringVertices);free(stringVertexA);free(stringVertexB);}
	edgeDDA->start.x = vertex1->x;
	edgeDDA->start.y = vertex1->y;
	edgeDDA->start.z = vertex1->z;

	edgeDDA->current.x = vertex1->x;
	edgeDDA->current.y = vertex1->y;
	edgeDDA->current.z = vertex1->z;

	edgeDDA->end.x = vertex2->x;
	edgeDDA->end.y = vertex2->y;
	edgeDDA->end.z = vertex2->z;

	edgeDDA->slopeX = ((edgeDDA->end.x-edgeDDA->start.x)/(edgeDDA->end.y-edgeDDA->start.y));
	edgeDDA->slopeZ = ((edgeDDA->end.z-edgeDDA->start.z)/(edgeDDA->end.y-edgeDDA->start.y));

	edgeDDA->edgeType = edgeType;

	if(initEdgeDDADebug){swprintf(debugStr,512,L"initSpanDDA: start=%f,%f,%f current=%f,%f,%f end=%f,%f,%f\n",edgeDDA->start.x,edgeDDA->start.y,edgeDDA->start.z,edgeDDA->current.x,edgeDDA->current.y,edgeDDA->current.z,edgeDDA->end.x,edgeDDA->end.y,edgeDDA->end.z);OutputDebugString(debugStr);}
	if(initEdgeDDADebug){swprintf(debugStr,512,L"initEdgeDDA: End\n");OutputDebugString(debugStr);}
	return 0;
}

int sortTriangleVertexForSequenceOfUse(Triangle *polygon, EdgeType *edgeType){
	wchar_t debugStr[2048];
	bool sortTriangleVertexForSequenceOfUseDebug = false;
	if(sortTriangleVertexForSequenceOfUseDebug){swprintf(debugStr,2048,L"sortTriangleVertexForSequenceOfUse: Start\n");OutputDebugString(debugStr);}
	if (polygon == NULL){
		swprintf(debugStr,2048,L"Error: sortTriangleVertexForSequenceOfUse polygon=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if (edgeType == NULL){
		swprintf(debugStr,2048,L"Error: sortTriangleVertexForSequenceOfUse edgeType=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	//first y sort them and also check for special case
	bool isTwoVertexOnSameHorizontalLine = false;
	if(polygon->a.y > polygon->b.y){
		swapTwoVertex(&polygon->a,&polygon->b);
	}
	else if(polygon->a.y == polygon->b.y){
		isTwoVertexOnSameHorizontalLine = true;
		if(polygon->a.x > polygon->b.x){
			swapTwoVertex(&polygon->a,&polygon->b);
		}
	}
	if(polygon->a.y > polygon->c.y){
		swapTwoVertex(&polygon->a,&polygon->c);
	}
	else if(polygon->a.y == polygon->c.y){
		isTwoVertexOnSameHorizontalLine = true;
		if(polygon->a.x > polygon->c.x){
			swapTwoVertex(&polygon->a,&polygon->c);
		}
	}
	if(polygon->b.y > polygon->c.y){
		swapTwoVertex(&polygon->b,&polygon->c);
	}
	else if(polygon->b.y == polygon->c.y){
		isTwoVertexOnSameHorizontalLine = true;
		if(polygon->b.x > polygon->c.x){
			swapTwoVertex(&polygon->b,&polygon->c);
		}
	}

	//assign left,right,top,bottom to the a-b(index 0),b-c(index 1) & a-c(index 2) edges
	if(isTwoVertexOnSameHorizontalLine){
		if(polygon->a.y == polygon->b.y){
			//since vertexs are y & then x sorted, vertex(2) is bottom vertex
			edgeType[0].top = 1;
			edgeType[1].right = 1;
		}
		if(polygon->b.y == polygon->c.y){
			//since vertexs are y & then x sorted, vertex(0) is top vertex
			edgeType[2].right = 1;
		}
	}
	else{
		//getLine equation from two vertex
		LineEq lineEq;
		if(getLineEq(&lineEq,polygon->a.x,polygon->a.y,polygon->c.x,polygon->c.y))
			return 1;
		//find X coord on Line passing through Y of vertex with index 1 (or mid-vertex)
		float intersectX;
		intersectX = (-lineEq.B*polygon->b.y-lineEq.C)/lineEq.A;
		if(intersectX > polygon->b.x){
			//vertex b is a left vertex
			edgeType[2].right = 1;
		}
		else{
			//vertex b is a right vertex
			edgeType[0].right = 1;
			edgeType[1].right = 1;
		}
	}
	
	return 0;
}

int swapTwoVertex(Vertex *v1, Vertex *v2){
	wchar_t debugStr[2048];
	if (v1 == NULL){
		swprintf(debugStr,2048,L"Error: swapTwoVertex: v1=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if (v2 == NULL){
		swprintf(debugStr,2048,L"Error: swapTwoVertex: v2=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	//swap two vertex
	Vertex temp;
	temp.x = v1->x;
	temp.y = v1->y;
	temp.z = v1->z;

	v1->x = v2->x;
	v1->y = v2->y;
	v1->z = v2->z;

	v2->x = temp.x;
	v2->y = temp.y;
	v2->z = temp.z;
	return 0;
}

int getLineEq(LineEq *lineEq, float x1, float y1, float x2, float y2){
	wchar_t debugStr[2048];
	if (lineEq == NULL){
		swprintf(debugStr,2048,L"Error: getLineEq: lineEq=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}	
	float dy = y2 - y1;
	float dx = x2 - x1;
	
	lineEq->A = y2-y1;
	lineEq->B = x1-x2;
	lineEq->C = (x2-x1)*y1+(y1-y2)*x1;
	
	return 0;
}

int getEdgeType(EdgeType *edgeType){
	wchar_t debugStr[2048];
	if (edgeType == NULL){
		swprintf(debugStr,2048,L"Error: getEdgeType edgeType=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	int edgeTypeIntValue = 0;
	if(edgeType->right)
		edgeTypeIntValue = 1;

	return edgeTypeIntValue;
}

int freeEdgeList(EdgeList *edgeList){
	wchar_t debugStr[2048];
	bool freeEdgeListDebug = false;
	if(freeEdgeListDebug){swprintf(debugStr,2048,L"freeEdgeList Started\n");OutputDebugString(debugStr);}
	if(edgeList){
		freeEdgeList(edgeList->next);
		free(edgeList);
	}
	if(freeEdgeListDebug){swprintf(debugStr,2048,L"freeEdgeList Ended\n");OutputDebugString(debugStr);}
	return 0;
}

int freeEdgeTable(EdgeTable *edgeTable){
	wchar_t debugStr[2048];
	bool freeEdgeTableDebug = false;
	if(freeEdgeTableDebug){swprintf(debugStr,2048,L"freeEdgeTable Started\n");OutputDebugString(debugStr);}
	if (edgeTable){
		freeEdgeTable(edgeTable->next);
		freeEdgeList(edgeTable->edgeList);
		free(edgeTable);
	}
	if(freeEdgeTableDebug){swprintf(debugStr,2048,L"freeEdgeTable Ended\n");OutputDebugString(debugStr);}
	return 0;
}
