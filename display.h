/*
 * This header file contains the functions and structure that will 
 * implement the display screen with XRES and YRES
 *
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "GlobalVariables.h"

struct Color{
	float red;
	float green;
	float blue;
};

struct Display{
	Color frameBuffer[XRES][YRES];
};

int putPixelToDisplay(Display *display, Color *color, int x, int y);

#endif