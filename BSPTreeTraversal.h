/*
 * This header file contains the  functions that will implement
 * front-to-back and back-to-front traversal methods in BSP Tree
 * 
 */

#ifndef BSP_TREE_TRAVERSAL_H
#define BSP_TREE_TRAVERSAL_H
#include "BSPTree.h"

/* This function implements the rendering of polygons in BSP tree 
 * using back to front traversal method in BSP Tree
 */
int renderUsingBackToFront(BSP_tree *bspTree, Vertex viewPoint);

/* This function implements the rendering of polygons in BSP tree 
 * using front to back traversal method in BSP Tree
 */
int renderUsingFrontToBack(BSP_tree *bspTree, Vertex viewPoint);

#endif