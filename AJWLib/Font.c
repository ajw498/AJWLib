/*
	AJWLib - Font
	© Alex Waugh 1999

	$Log: not supported by cvs2svn $

*/


int AJWLib_Font_GetWidth(Desk_font_handle handle,const char *str)
{
	int width,height;
	Desk_Error2_CheckOS(Desk_SWI(5,5,SWI_Font_ScanString,handle,str,1<<8,INFINITY,INFINITY,NULL,NULL,NULL,&width,&height));
	Desk_Font_ConverttoOS(width,height,&width,&height);
	return width;
}
