/*
	AJWLib - Handler
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.1  1999/10/02 23:06:04  AJW
	Initial revision


*/
#include "Desk.WimpSWIs.h"

Desk_bool Handler_KeyPress(Desk_event_pollblock *block,void *r)
{
	Desk_Wimp_ProcessKey(block->data.key.code);
	return Desk_TRUE;
}

