/*
	AJWLib - Msgs
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $

*/
#include "DeskLib:Icon.h"
#include "DeskLib:Msgs.h"
#include "DeskLib:Window.h"

#define BUFFERSIZE 256

static char buffer[BUFFERSIZE];

void Msgs_SetText(window_handle window,icon_handle icon,char *tag)
{
	Msgs_Lookup(tag,buffer,BUFFERSIZE);
	Icon_SetText(window,icon,buffer);
}

void Msgs_SetTitle(window_handle window,char *tag)
{
	Msgs_Lookup(tag,buffer,BUFFERSIZE);
	Window_SetTitle(window,buffer);
}

char *Msgs_TempLookup(char *tag)
{
	Msgs_Lookup(tag,buffer,BUFFERSIZE);
	return buffer;
}
