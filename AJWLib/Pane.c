/*
	AJWLib - Pane
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $

*/
#include "DeskLib:WimpSWIs.h"
#include "DeskLib:Pane.h"
#include "AJWLib:Window.h"

void Pane_BringToFront(window_handle master)
{
	pane_data *data;
	window_state state;
	data=Pane_GetSysHandle(master);
	Window_BringToFront(data->pane);
	Wimp_GetWindowState(master,&state);
	state.openblock.behind=data->pane;
	Wimp_OpenWindow(&state.openblock);
}
