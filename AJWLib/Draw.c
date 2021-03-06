/*
	AJWLib - Draw
	� Alex Waugh 1999

	$Log: not supported by cvs2svn $
	Revision 1.5  2000/11/06 16:26:33  AJW
	Changes to the error handler to make it more reentrant
	
	Revision 1.4  2000/10/02 11:38:10  AJW
	Added specification of join cap in plotting, so different line thicknesses work
	
	Revision 1.3  1999/10/09 17:23:05  AJW
	Added AJWLib_ prefixes
	
	Revision 1.2  1999/10/02 23:52:01  AJW
	Modified to use Desk

	Revision 1.1  1999/10/02 19:10:16  AJW
	Initial revision


*/

#include "Desk.SWI.h"
#include "Desk.Error2.h"

#include "AJWLib.Drawfile.h"
#include "AJWLib.Draw.h"

static unsigned char joincap[8]={0,0,0,0,0,0,4,0};
/*static unsigned char *joincap=NULL;*/

void AJWLib_Draw_CreateRectanglePath(int x,int y,int width,int height,int *block)
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

void AJWLib_Draw_CreateLinePath(int x0,int y0,int x1,int y1,int *block)
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

void AJWLib_Draw_PlotLine(int x0,int y0,int x1,int y1,int thickness,os_trfm *matrix)
{
	int block[10];
	AJWLib_Draw_CreateLinePath(x0,y0,x1,y1,block);
	Desk_Error2_CheckOS(Desk_SWI(7,0,Desk_SWI_Draw_Stroke,block,0x0,matrix,0,thickness,joincap,0));
}

void AJWLib_Draw_PlotRectangle(int x,int y,int width,int height,int thickness,os_trfm *matrix)
{
	int block[16];
	AJWLib_Draw_CreateRectanglePath(x,y,width,height,block);
	Desk_Error2_CheckOS(Desk_SWI(7,0,Desk_SWI_Draw_Stroke,block,0x0,matrix,0,thickness,joincap,0));
}

void AJWLib_Draw_PlotRectangleFilled(int x,int y,int width,int height,os_trfm *matrix)
{
	int block[16];
	AJWLib_Draw_CreateRectanglePath(x,y,width,height,block);
	Desk_Error2_CheckOS(Desk_SWI(4,0,Desk_SWI_Draw_Fill,block,0x30,matrix,0));
}

