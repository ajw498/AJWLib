/*
	AJWLib - Draw
	© Alex Waugh 1999

	$Log: not supported by cvs2svn $

*/

#include "DeskLib:Core.h"
#include "DeskLib:Wimp.h"
#include "DeskLib:SWI.h"
#include "AJWLib:Drawfile.h"

void Draw_CreateRectanglePath(int x,int y,int width,int height,int *block)
{
	block[0]=2;
	block[1]=x;
	block[2]=y;
	block[3]=8;
	block[4]=x+width;
	block[5]=y;
	block[6]=8;
	block[7]=x+width;
	block[8]=y+height;
	block[9]=8;
	block[10]=x;
	block[11]=y+height;
	block[12]=5;
	block[13]=0;
	block[14]=1;
}

void Draw_CreateLinePath(int x0,int y0,int x1,int y1,int *block)
{
	block[0]=2;
	block[1]=x0;
	block[2]=y0;
	block[3]=8;
	block[4]=x1;
	block[5]=y1;
	block[6]=4;
	block[7]=0;
	block[8]=1;
}

os_error *Draw_PlotLine(int x0,int y0,int x1,int y1,int thickness,os_trfm *matrix)
{
	int block[10];
	Draw_CreateLinePath(x0,y0,x1,y1,block);
	return SWI(7,0,SWI_Draw_Stroke,block,0,matrix,0,thickness,0,0);
}

os_error *Draw_PlotRectangle(int x,int y,int width,int height,int thickness,os_trfm *matrix)
{
	int block[16];
	Draw_CreateRectanglePath(x,y,width,height,block);
	return SWI(7,0,SWI_Draw_Stroke,block,0,matrix,0,thickness,0,0);
}

os_error *Draw_PlotRectangleFilled(int x,int y,int width,int height,os_trfm *matrix)
{
	int block[16];
	Draw_CreateRectanglePath(x,y,width,height,block);
	return SWI(4,0,SWI_Draw_Fill,block,0,matrix,0);
}

