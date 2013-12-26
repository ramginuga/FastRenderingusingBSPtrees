/*
 * This header file contains the  data strucutures for common geometrical 
 * objects and associated functions 
 */
#ifndef GEOMETRICAL_STRUCTURES_H
#define GEOMETRICAL_STRUCTURES_H


struct Vertex {
	float x; float y; float z;
};

/* This plane is a partition plane only.*/
struct Plane {
	/* ax + by + cz + d = 0 */
	float a;
	float b;
	float c;
	float d;
};

/*
 * The triangle has three vertices. The values of the x,y,z 
 * of the vertices are stored here.
 */
struct Triangle {
	Vertex a;
	Vertex b;
	Vertex c;	
};
wchar_t *stringVertex(Vertex *vertex);
wchar_t *stringTriangle(Triangle *triangle);
wchar_t *stringPlane(Plane *plane);

#endif