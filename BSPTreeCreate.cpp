// Tree_sample_1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "GeometricalStructures.h"
#include "BSPTree.h"
#include <stdlib.h>

#define COINCIDENT 0
#define IN_BACK_OF 1
#define IN_FRONT_OF 2
#define SPANNING 3
#define MAXPTS 9999

void populateBSPTree_inner(Triangle* triangles, BSP_tree* tree, int total_triangles);

float dotProduct(float x1, float x2, float x3,
	float y1, float y2, float y3){
		return x1*y1 + x2*y2 + x3* y3;
}

Plane getPlane(Triangle t) {
	Plane* p = (Plane *) malloc(sizeof(Plane));
	float vec1_x = t.a.x - t.b.x;
	float vec2_x = t.b.x - t.c.x;
	float vec1_y = t.a.y - t.b.y;
	float vec2_y = t.b.y - t.c.y;
	float vec1_z = t.a.z - t.b.z;
	float vec2_z = t.b.z - t.c.z;

	p->a = vec1_y * vec2_z - vec1_z * vec2_y ;
	p->b = vec1_z * vec2_x - vec1_x * vec2_z ;
	p->c = vec1_x * vec2_y - vec1_y * vec2_x ;

	p->d = (- p->a * t.a.x - p->b * t.a.y - p->c * t.a.z);
	return *p;
}

Vertex getPlaneNormal(Plane p) {
	Vertex *v = (Vertex *) malloc(sizeof(Vertex));
	v->x = p.a;
	v->y = p.b;
	v->z = p.c;
	return *v;
}

void addTrainglesTo (Triangle* triList, Triangle t, int *index) {
	*(triList+ (*index)) = t;
	(*index)++;
}

int classifyTri(Triangle tri, Plane p) {
	if(isPositiveSide(p, &tri)) {
		return IN_FRONT_OF;
	} else if(isNegativeSide(p, &tri)) {
		return IN_BACK_OF;
	} else {
		return COINCIDENT;
	}
}

bool isPositiveSide(Plane hyp, Triangle* tri) {
	float val_1 = hyp.a * tri->a.x + hyp.b * tri->a.y +
		hyp.c * tri->a.z + hyp.d;
	float val_2 = hyp.a * tri->b.x + hyp.b * tri->b.y +
		hyp.c * tri->b.z + hyp.d;
	float val_3 = hyp.a * tri->c.x + hyp.b * tri->c.y +
		hyp.c * tri->c.z + hyp.d;

	if(val_1 > 0 && val_2 > 0 && val_3 > 0) {
		return true;
	} else {
		return false;
	}
}

bool isNegativeSide(Plane hyp, Triangle* tri) {
	float val_1 = hyp.a * tri->a.x + hyp.b * tri->a.y +
		hyp.c * tri->a.z + hyp.d;
	float val_2 = hyp.a * tri->b.x + hyp.b * tri->b.y +
		hyp.c * tri->b.z + hyp.d;
	float val_3 = hyp.a * tri->c.x + hyp.b * tri->c.y +
		hyp.c * tri->c.z + hyp.d;

	if(val_1 < 0 && val_2 < 0 && val_3 < 0) {
		return true;
	} else {
		return false;
	}
}

bool isNegativeSideVertex(Plane hyp, Vertex v) {
	float val = hyp.a * v.x + hyp.b * v.y +
		hyp.c * v.z + hyp.d;
	if(val < 0) {
		return true;
	} else {
		return false;
	}
}

bool isPositiveSideVertex(Plane hyp, Vertex v) {
	float val = hyp.a * v.x + hyp.b * v.y +
		hyp.c * v.z + hyp.d;
	if(val > 0) {
		return true;
	} else {
		return false;
	}
}

float classifyPoints(Plane p, Vertex v) {
	return p.a * v.x + p.b * v.y + p.c * v.z + p.d;
}

void splitPolygon(Triangle *tri, Plane p,
	Triangle   *front,
	Triangle   *back,
	int front_index,
	int back_index) {
		int   count = 3, out_c = 0, in_c = 0;
		Vertex ptA, ptB, outpts[MAXPTS], inpts[MAXPTS];
		// Two points where the plane intersects the triangle
		Vertex *intersects = (Vertex *) malloc(sizeof(Vertex)*2);
		int ii = 0;
		float sideA, sideB;
		ptA = tri->a;
		sideA = classifyPoints(p, ptA);

		for (short i = 1; i <= 3; i++)
		{
			if(i == 1) {
				ptB = tri->b;
				//ptB = poly->Vertex (i);
			} else if (i ==2) {
				ptB = tri->c;
			} else if (i ==3) {
				ptA = tri->b;
				sideA = classifyPoints(p, ptA);
				ptB = tri->c;
			}
			sideB = classifyPoints(p, ptB);
			if (sideB > 0)
			{
				if (sideA < 0)
				{
					// compute the intersection point of the line
					// from point A to point B with the partition
					// plane. This is a simple ray-plane intersection.
					float vx = ptB.x - ptA.x;
					float vy = ptB.y - ptA.y;
					float vz = ptB.z - ptA.z;
					Vertex* v = (Vertex*)malloc(sizeof(Vertex) * 1);

					Vertex n = getPlaneNormal(p);

					float sect = - classifyPoints (p, ptA) / dotProduct(n.x, n.y, n.z, vx, vy, vz);
					v->x = vx * sect + ptA.x;
					v->y = vy * sect + ptA.y;
					v->z = vz * sect + ptA.z;
					(intersects+ii)->x = v->x;
					(intersects+ii)->y = v->y;
					(intersects+ii)->z = v->z;
					ii++;
					outpts[out_c++] = inpts[in_c++] = *v;
				}
				outpts[out_c++] = ptB;
			}
			else if (sideB < 0)
			{
				if (sideA > 0)
				{
					// compute the intersection point of the line
					// from point A to point B with the partition
					// plane. This is a simple ray-plane intersection.
					float vx = ptB.x - ptA.x;
					float vy = ptB.y - ptA.y;
					float vz = ptB.z - ptA.z;
					Vertex* v = (Vertex*)malloc(sizeof(Vertex) * 1);

					Vertex n = getPlaneNormal(p);
					float sect = - classifyPoints (p, ptA) / dotProduct(n.x, n.y, n.z, vx, vy, vz);
					v->x = vx * sect + ptA.x;
					v->y = vy * sect + ptA.y;
					v->z = vz * sect + ptA.z;
					(intersects+ii)->x = v->x;
					(intersects+ii)->y = v->y;
					(intersects+ii)->z = v->z;
					ii++;
					outpts[out_c++] = inpts[in_c++] = *v;
				}
				inpts[in_c++] = ptB;
			}
			//else
			//outpts[out_c++] = inpts[in_c++] = ptB;
			//ptA = ptB;
			//sideA = sideB;
		}
		//front = new polygon (outpts, out_c);
		//back = new polygon (inpts, in_c);
		for(short i=0; i < 3; i++) {
			Vertex v;
			if(i==0) {
				v = tri->a;
			} else if(i==1) {
				v = tri->b;
			} else if (i ==2) {
				v = tri->c;
			}

			if(isPositiveSideVertex(p, v)) {
				Triangle t;
				t.a = v;
				t.b = *(intersects+0);
				t.c = *(intersects+1);
				addTrainglesTo(front, t, &front_index);
			} else {
				Triangle t;
				t.a = v;
				t.b = *(intersects+0);
				t.c = *(intersects+1);
				addTrainglesTo(back, t, &back_index);
			}
		}

}

bool isEmptyList(Triangle* list) {
	if(list == 0) {
		return true;
	}
	return false;
}

void populateBSPTree (Triangle* triangles, BSP_tree** tree, int total_triangles){
	*tree = (BSP_tree *) malloc (sizeof(BSP_tree));
	(*tree)->back_index = 0;
	(*tree)->front_index = 0;
	(*tree)->total_polygons = 0;
	(*tree)->back = NULL;
	(*tree)->front = NULL;
	populateBSPTree_inner(triangles, *tree, total_triangles);
}

void populateBSPTree_inner(Triangle* triangles, BSP_tree* tree, int total_triangles) {
	Triangle *root = &triangles[0];
	tree->partition = getPlane(*root);

	tree->polygons = (Triangle *) malloc(sizeof(Triangle) * MAXPTS);
	tree->front_list = (Triangle *) malloc(sizeof(Triangle) * MAXPTS);
	tree->back_list = (Triangle *) malloc(sizeof(Triangle) * MAXPTS);
	

	addTrainglesTo(tree->polygons, *root, &(tree->total_polygons));

	//Triangle* front_list;
	//Triangle* back_list;

	Triangle   *tri;
	int i = 1;
	while(i < total_triangles)
	{
		tri = (triangles+i);
		int   result = classifyTri(*tri, tree->partition);
		switch (result) {
			case COINCIDENT:
				addTrainglesTo(tree->polygons, *tri, &(tree->total_polygons));
				break;
			case IN_BACK_OF:
				addTrainglesTo(tree->back_list, *tri, &(tree->back_index));
				break;
			case IN_FRONT_OF:
				addTrainglesTo(tree->front_list, *tri, &(tree->front_index));
					break;
			case SPANNING:
					//Triangle *front_piece, *back_piece;
					splitPolygon (tri, tree->partition,
						tree->front_list, tree->back_list,
						tree->front_index, tree->back_index);
					//addTrainglesTo(front_list, *tri);
					//addTrainglesTo(back_list, *tri);
					break;
		}
	  i++;
	}
	//if (!isEmptyList(tree->front_list))
	if (tree->front_index != 0)
	{
		tree->front = (BSP_tree *) malloc(sizeof(BSP_tree));
		tree->front->back = NULL;
		tree->front->front = NULL;
		tree->front->back_index = 0;
		tree->front->front_index = 0;
		tree->front->total_polygons = 0;
		populateBSPTree_inner(tree->front_list, tree->front, tree->front_index);
	}
	//if (!isEmptyList(tree->back_list))
	if (tree->back_index != 0)
	{
		tree->back = (BSP_tree *) malloc(sizeof(BSP_tree));
		tree->back->back = NULL;
		tree->back->front = NULL;
		tree->back->back_index = 0;
		tree->back->front_index = 0;
		tree->back->total_polygons = 0;
		populateBSPTree_inner(tree->back_list, tree->back, tree->back_index);
	}
}

/*
int _tmain(int argc, _TCHAR* argv[])
{
	Triangle t1, t2, t3, t4;
	Triangle *tris = NULL;
	BSP_tree *tree = NULL;
	tree = (BSP_tree *) malloc(sizeof(BSP_tree));
	tris = (Triangle *) malloc(sizeof(Triangle)*5);

	t1.a.x = 2; t1.a.y = 1; t1.a.z = 2;
	t1.b.x = 1; t1.b.y = 2; t1.b.z = 2;
	t1.c.x = 3; t1.c.y = 2; t1.c.z = 2;

	t2.a.x = 2; t2.a.y = 1; t2.a.z = 1;
	t2.b.x = 1; t2.b.y = 2; t2.b.z = 1;
	t2.c.x = 3; t2.c.y = 2; t2.c.z = 1;

	t3.a.x = 2; t3.a.y = 1; t3.a.z = 3;
	t3.b.x = 1; t3.b.y = 2; t3.b.z = 3;
	t3.c.x = 3; t3.c.y = 2; t3.c.z = 3;

	t4.a.x = 2; t4.a.y = 1; t4.a.z = 4;
	t4.b.x = 1; t4.b.y = 2; t4.b.z = 4;
	t4.c.x = 3; t4.c.y = 2; t4.c.z = 4;
	*tris = t1;
	*(tris+1) = t2;
	*(tris+2) = t3;
	*(tris+3) = t4;
	tree->back_index = 0;
	tree->front_index = 0;
	tree->total_polygons = 0;
	//populateBSPTree_inner(tris, tree, 4);
	tree->front;
	tree->back;
	//============== TEST SPLIT ================
	Plane p;
	Triangle tri;
	tri.a.x = 2; tri.a.y = 3; tri.a.z = 2;
	tri.b.x = 1; tri.b.y = 5; tri.b.z = 2;
	tri.c.x = 4; tri.c.y = 7; tri.c.z = 2;

	p.a = 0; p.b = 1.0; p.c = 0; p.d = -4.0;
	Triangle *front_piece = (Triangle *) malloc(sizeof(Triangle)*5);
	Triangle *back_piece =  (Triangle *) malloc(sizeof(Triangle)*5);
	splitPolygon (&tri, p,
					front_piece, back_piece,
					tree->front_index, tree->back_index);
	int x = 0;
	x++;
	return 0;
}*/

