/*
	AJWLib - Draw
	© Alex Waugh 1999
*/
#include "AJWLib:DrawFile.h"


void Draw_CreateRectanglePath(int x,int y,int width,int height,int *block);

void Draw_CreateLinePath(int x0,int y0,int x1,int y1,int *block);

void Draw_PlotLine(int x0,int y0,int x1,int y1,int thickness,os_trfm *matrix);

void Draw_PlotRectangle(int x,int y,int width,int height,int thickness,os_trfm *matrix);

void Draw_PlotRectangleFilled(int x,int y,int width,int height,os_trfm *matrix);

