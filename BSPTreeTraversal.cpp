#include "stdafx.h"
#include "BSPTreeTraversal.h"
#include "render.h"
#include "stdio.h"
#include "GlobalVariables.h"
#include "stdlib.h"
#include "display.h"
#include <Windows.h>

struct TraversalList{
	Triangle*  polygons; /* list of triangles at this node */
	int totalPolygons; // polygon count at this edge
	TraversalList *next;
};

TraversalList **currentTraversalListNode;
/* This function implements the back to front traversal method in BSP Tree
 * It will set the traversalList in back to front order
 */
int traverseBackToFront(BSP_tree *bspTree, Vertex viewPoint);

/* This function implements the front to back traversal method in BSP Tree
 * It will set the traversalList in back to front order
 */
int traverseFrontToBack(BSP_tree *bspTree, Vertex viewPoint);

/*This function will return the positive branch of the BSP Tree*/
BSP_tree *getPositiveBranch(BSP_tree *bspTree);

/* This function will return the negative branch of the BSP Tree*/
BSP_tree *getNegativeBranch(BSP_tree *bspTree);

int initDynamicScreen(EdgeList ***dynamicScreen);
int freeDynamicScreen(EdgeList **dynamicScreen);


int renderUsingBackToFront(BSP_tree *bspTree, Vertex viewPoint){
	bool renderUsingBackToFrontDebug = false;
	wchar_t debugStr[2048];
	if(renderUsingBackToFrontDebug){swprintf(debugStr,2048,L"renderUsingBackToFront Started\n");OutputDebugString(debugStr);}
	TraversalList *traversalList = NULL;
	
	//get the list of nodes in back to front traversal order
	currentTraversalListNode = &traversalList;
	traverseBackToFront(bspTree, viewPoint);
	return 0;
	//paint the traversalList directly to the frame buffer
	//get new display
	Display display;
	
	int nodeId = 0;
	TraversalList *currentTraversalList = traversalList;
	while(currentTraversalList != NULL){
		if(renderUsingBackToFrontDebug){swprintf(debugStr,2048,L"\t TraversalListNodeId:%d\n",nodeId);OutputDebugString(debugStr);}
		for(int polygonId=0;polygonId<currentTraversalList->totalPolygons;polygonId++){
			if(renderUsingBackToFrontDebug){wchar_t *stringPolygon = stringTriangle(&currentTraversalList->polygons[polygonId]);swprintf(debugStr,2048,L"\t\t triangle: %s\n",stringPolygon);OutputDebugString(debugStr);free(stringPolygon);}
			paintPolygonUsingStaticScreen(&currentTraversalList->polygons[polygonId],&display);
		}//end for loop
		currentTraversalList = currentTraversalList->next;
		nodeId++;
	}//end while loop

	if(renderUsingBackToFrontDebug){swprintf(debugStr,2048,L"renderUsingBackToFront Ended\n");OutputDebugString(debugStr);}
	return 0;
}


int renderUsingFrontToBack(BSP_tree *bspTree, Vertex viewPoint){
	bool renderUsingFrontToBackDebug = false;
	wchar_t debugStr[2048];
	if(renderUsingFrontToBackDebug){swprintf(debugStr,2048,L"renderUsingFrontToBack Started\n");OutputDebugString(debugStr);}

	TraversalList *traversalList = NULL;
	//get the list of nodes in back to front traversal order
	currentTraversalListNode = &traversalList;
	traverseFrontToBack(bspTree, viewPoint);
	//get new display
	Display display;

	//paint the traversalList to the frame buffer using the Dynamic Screen Data Strucuture
	EdgeList **dynamicScreen = NULL;
	
	//init the dyanmic Screen
	initDynamicScreen(&dynamicScreen);
	int nodeId = 0;
	TraversalList *currentTraversalList = traversalList;
	while(currentTraversalList){
		if(renderUsingFrontToBackDebug){swprintf(debugStr,2048,L"\t TraversalListNodeId:%d\n",nodeId);OutputDebugString(debugStr);}
		for(int polygonId=0;polygonId<currentTraversalList->totalPolygons;polygonId++){
			if(renderUsingFrontToBackDebug){wchar_t *stringPolygon = stringTriangle(&currentTraversalList->polygons[polygonId]);swprintf(debugStr,2048,L"\t\t triangle: %s\n",stringPolygon);OutputDebugString(debugStr);free(stringPolygon);}
			paintPolygonUsingDynamicScreen(&currentTraversalList->polygons[polygonId],dynamicScreen,&display);
		}//end for loop
		currentTraversalList = currentTraversalList->next;
		nodeId++;
	}//end while loop
	freeDynamicScreen(dynamicScreen);
	if(renderUsingFrontToBackDebug){swprintf(debugStr,2048,L"renderUsingFrontToBack Ended\n");OutputDebugString(debugStr);}
	return 0;
}

int initTraversalListNode(BSP_tree *bspTree, TraversalList **node){
	(*node) = (TraversalList *) malloc(sizeof(TraversalList));
	//add list of triangles at the current node
	(*node)->polygons = bspTree->polygons;
	(*node)->totalPolygons = bspTree->total_polygons;
	(*node)->next = NULL;
	return 0;
}
int traverseBackToFront(BSP_tree *bspTree, Vertex viewPoint){
	bool traverseBackToFrontDebug = false;
	wchar_t debugStr[2048];
	if(traverseBackToFrontDebug){swprintf(debugStr,2048,L"traverseBackToFront Started\n");OutputDebugString(debugStr);}
	/* first traverse the branch which is opposite to the view point
	 * then traverse the branch in which view point exists
	 */
	if(bspTree){
		if(isPositiveSideVertex(bspTree->partition, viewPoint)){
			traverseBackToFront(getNegativeBranch(bspTree),viewPoint);
			//set current Node
			initTraversalListNode(bspTree,currentTraversalListNode);
			currentTraversalListNode = (&(*currentTraversalListNode)->next);
			traverseBackToFront(getPositiveBranch(bspTree),viewPoint);
		}
		else{
			traverseBackToFront(getPositiveBranch(bspTree),viewPoint);
			//set current Node
			initTraversalListNode(bspTree,currentTraversalListNode);
			currentTraversalListNode = (&(*currentTraversalListNode)->next);
			traverseBackToFront(getNegativeBranch(bspTree),viewPoint);
		}
	}
	if(traverseBackToFrontDebug){swprintf(debugStr,2048,L"traverseBackToFront Ended\n");OutputDebugString(debugStr);}
	return 0;
}

int traverseFrontToBack(BSP_tree *bspTree, Vertex viewPoint){
	bool traverseFrontToBackDebug = false;
	wchar_t debugStr[2048];
	if(traverseFrontToBackDebug){swprintf(debugStr,2048,L"traverseFrontToBack Started\n");OutputDebugString(debugStr);}
	/* first traverse the branch in which view point exists
	 * then traverse the branch which is opposite to the view point
	 */
	if(bspTree){
		if(isPositiveSideVertex(bspTree->partition, viewPoint)){
			traverseFrontToBack(getPositiveBranch(bspTree),viewPoint);
			//set current Node
			initTraversalListNode(bspTree,currentTraversalListNode);
			currentTraversalListNode = (&(*currentTraversalListNode)->next);
			traverseFrontToBack(getNegativeBranch(bspTree),viewPoint);
			
		}
		else{
			traverseFrontToBack(getNegativeBranch(bspTree),viewPoint);
			//set current Node
			initTraversalListNode(bspTree,currentTraversalListNode);
			currentTraversalListNode = (&(*currentTraversalListNode)->next);
			traverseFrontToBack(getPositiveBranch(bspTree),viewPoint);
		}
	}
	if(traverseFrontToBackDebug){swprintf(debugStr,2048,L"traverseFrontToBack Ended\n");OutputDebugString(debugStr);}
	return 0;
}

BSP_tree *getPositiveBranch(BSP_tree *bspTree){
	return bspTree->front;
}

BSP_tree *getNegativeBranch(BSP_tree *bspTree){
	return bspTree->back;
}

int initDynamicScreen(EdgeList ***dynamicScreen){
	*dynamicScreen = (EdgeList **)malloc(YRES*sizeof(EdgeList *));
	for(int y=0;y<YRES;y++){
		(*dynamicScreen)[y] = (EdgeList *)malloc(sizeof(EdgeList));
		(*dynamicScreen)[y]->start = 0;
		(*dynamicScreen)[y]->end = XRES-1;
		(*dynamicScreen)[y]->next = NULL;
		(*dynamicScreen)[y]->previous = NULL;
	}
	return 0;
}

int freeDynamicScreen(EdgeList **dynamicScreen){
	char debugStr[256];
	if(dynamicScreen == NULL){
		sprintf_s(debugStr,"Error: freeDynamicScreen dynamicScreen=NULL parameter passed\n");printf(debugStr);
		return 1;
	}
	//TODO: freeing memory can cause issue check later
	for(int y=0;y<YRES;y++){
		if(dynamicScreen[y] !=NULL){
			freeEdgeList(dynamicScreen[y]);
		}
	}
	return 0;
}