/*
	AJWLib - Msgs
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.3  1999/10/09 18:07:13  AJW
	Added AJWLib_ prefix

	Revision 1.2  1999/10/03 00:25:12  AJW
	Modified to use Desk

	Revision 1.1  1999/10/02 23:08:28  AJW
	Initial revision


*/
#include "Desk.Icon.h"
#include "Desk.Msgs.h"
#include "Desk.Window.h"
#include "Desk.DeskMem.h"
#include "Desk.Error2.h"

#define BUFFERSIZE 256

static char buffer[BUFFERSIZE];

void AJWLib_Msgs_SetText(Desk_window_handle window,Desk_icon_handle icon,char *tag)
{
	Desk_Msgs_Lookup(tag,buffer,BUFFERSIZE);
	Desk_Icon_SetText(window,icon,buffer);
}

void AJWLib_Msgs_SetTitle(Desk_window_handle window,char *tag)
{
	Desk_Msgs_Lookup(tag,buffer,BUFFERSIZE);
	Desk_Window_SetTitle(window,buffer);
}

char *AJWLib_Msgs_TempLookup(char *tag)
{
	Desk_Msgs_Lookup(tag,buffer,BUFFERSIZE);
	return buffer;
}

char *AJWLib_Msgs_Lookup(char *tag)
{
	char *buffer;
	buffer=Desk_DeskMem_Malloc(BUFFERSIZE);
	Desk_Msgs_Lookup(tag,buffer,BUFFERSIZE);
	return buffer;
}
