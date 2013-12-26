#include "stdafx.h"
#include "display.h"
#include <Windows.h>
#include <string>

int clampPixelPositionInBounds(int *x, int *y);

int putPixelToDisplay(Display *display, Color *color, int x, int y){
	wchar_t debugStr[512];
	if (display == NULL){
		swprintf(debugStr,512,L"Error: putPixelToDisplay display=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if (color == NULL){
		swprintf(debugStr,512,L"Error: putPixelToDisplay color=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	clampPixelPositionInBounds(&x,&y);
	display->frameBuffer[x][y].red = color->red;
	display->frameBuffer[x][y].green = color->green;
	display->frameBuffer[x][y].blue = color->blue;

	return 0;
}


int clampPixelPositionInBounds(int *x, int *y){
	wchar_t debugStr[512];
	if (x == NULL){
		swprintf(debugStr,512,L"Error: clampPixelPositionInBounds x=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	if (y == NULL){
		swprintf(debugStr,512,L"Error: clampPixelPositionInBounds y=NULL parameter passed\n");OutputDebugString(debugStr);
		return 1;
	}
	
	if(*x < 0)
		*x = 0;
	if(*x >= XRES)
		*x = XRES-1;
	
	if(*y < 0)
		*y = 0;
	if(*y >= YRES)
		*y = YRES-1;

	return 0;
}