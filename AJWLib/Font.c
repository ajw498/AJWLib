/*
	AJWLib - Font
	© Alex Waugh 1999

	$Log: not supported by cvs2svn $
	Revision 1.4  2000/02/26 20:19:52  uid1
	Added GetWidthAndHeight from a font handle
	
	Revision 1.3  2000/01/10 15:40:26  AJW
	Added AJWLib_Font_GetWidthAndHeight
	
	Revision 1.2  1999/10/25 17:23:12  AJW
	Added #includes and #defines so it actually compiled

	Revision 1.1  1999/10/25 17:17:02  AJW
	Initial revision


*/

#include "Desk.Font.h"
#include "Desk.Error2.h"
#include "Desk.SWI.h"

#include "AJWLib.Font.h"

#define SWI_Font_ScanString 0x400A1
#define INFINITY 999999

int AJWLib_Font_GetWidth(const Desk_font_handle handle,const char *str)
/*Returns the width in os units of a string using the given font handle*/
{
	int block[]={0,0,0,0,-1,0,0,0,0};

	Desk_Error2_CheckOS(Desk_SWI(6,0,SWI_Font_ScanString,handle,str,(1<<5)+(1<<8)+(1<<18),INFINITY,INFINITY,block));
	Desk_Font_ConverttoOS(block[7]-block[5],block[8]-block[6],&block[0],&block[1]);
	return block[0];
}

Desk_wimp_point *AJWLib_Font_GetWidthAndHeightGiven(const char *font,const int size,const char *str)
/*Returns the width and height in OS units of a string in a given font. size is in sixteenths of a point*/
{
	Desk_font_handle handle;
	static Desk_wimp_point coords;
	int block[]={0,0,0,0,-1,0,0,0,0};

	Desk_Font_FindFont(&handle,font,size,size,0,0);
	Desk_Error2_CheckOS(Desk_SWI(6,0,SWI_Font_ScanString,handle,str,(1<<5)+(1<<8)+(1<<18),INFINITY,INFINITY,block));
	Desk_Font_ConverttoOS(block[7]-block[5],block[8]-block[6],&coords.x,&coords.y);
	Desk_Font_LoseFont(handle);
	return &coords;
}

Desk_wimp_point *AJWLib_Font_GetWidthAndHeight(const Desk_font_handle handle,const char *str)
/*Returns the width and height in OS units of a string in a given font handle*/
{
	static Desk_wimp_point coords;
	int block[]={0,0,0,0,-1,0,0,0,0};

	Desk_Error2_CheckOS(Desk_SWI(6,0,SWI_Font_ScanString,handle,str,(1<<5)+(1<<8)+(1<<18),INFINITY,INFINITY,block));
	Desk_Font_ConverttoOS(block[7]-block[5],block[8]-block[6],&coords.x,&coords.y);
	return &coords;
}
