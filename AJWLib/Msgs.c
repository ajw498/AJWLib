/*
	AJWLib - Msgs
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.1  1999/10/02 23:08:28  AJW
	Initial revision


*/
#include "Desk.Icon.h"
#include "Desk.Msgs.h"
#include "Desk.Window.h"
#include "Desk.Error2.h"

#define BUFFERSIZE 256

static char buffer[BUFFERSIZE];

void Msgs_SetText(Desk_window_handle window,Desk_icon_handle icon,char *tag)
{
	Desk_Msgs_Lookup(tag,buffer,BUFFERSIZE);
	Desk_Icon_SetText(window,icon,buffer);
}

void Msgs_SetTitle(Desk_window_handle window,char *tag)
{
	Desk_Msgs_Lookup(tag,buffer,BUFFERSIZE);
	Desk_Window_SetTitle(window,buffer);
}

char *Msgs_TempLookup(char *tag)
{
	Desk_Msgs_Lookup(tag,buffer,BUFFERSIZE);
	return buffer;
}
