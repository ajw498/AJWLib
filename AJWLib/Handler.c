/*
	AJWLib - Handler
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $

*/
#include "DeskLib:WimpSWIs.h"

BOOL Handler_KeyPress(event_pollblock *block,void *r)
{
	Wimp_ProcessKey(block->data.key.code);
	return TRUE;
}

