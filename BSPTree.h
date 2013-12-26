/*
 * This header file contains the  functions that will be implemented
 * for BSP Tree (front-to-back and back-to-front implementations)
 *
 */
#ifndef BSP_TREE_H
#define BSP_TREE_H

#include "GeometricalStructures.h"

/* BSP tree structure. */
struct  BSP_tree {
   Plane     partition;  /* partition plane eq coeffs ABCD */
   Triangle*  polygons; /* list of polys in partition plane */
   Triangle*  front_list;
   Triangle*  back_list;
   BSP_tree  *front;     /* "+" list of polys in front subspace */
   BSP_tree  *back;      /* "-" list of polys in back subspace */
   int front_index;
   int back_index;
   int total_polygons;
};

/*
 * Send me all the triangles and I will populated the tree
 * and send you back the tree. Navigate this tree and you have
 * the BSP tree with you.
 */
void populateBSPTree(Triangle* triangles, BSP_tree** tree, int total_triangles);
bool isPositiveSide(Plane hyp, Triangle* triangles);
bool isNegativeSide(Plane hyp, Triangle* triangles);
bool isNegativeSideVertex(Plane hyp, Vertex v);
bool isPositiveSideVertex(Plane hyp, Vertex v);

#endif