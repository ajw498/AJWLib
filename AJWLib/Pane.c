/*
	AJWLib - Pane
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.2  1999/10/03 00:27:32  AJW
	Modified to use Desk

	Revision 1.1  1999/10/02 23:08:53  AJW
	Initial revision


*/
#include "Desk.WimpSWIs.h"
#include "Desk.Pane.h"

void AJWLib_Pane_BringToFront(Desk_window_handle master)
{
	Desk_pane_data *data;
	Desk_window_state state;
	data=Desk_Pane_GetSysHandle(master);
	Desk_Window_BringToFront(data->pane);
	Desk_Wimp_GetWindowState(master,&state);
	state.openblock.behind=data->pane;
	Desk_Wimp_OpenWindow(&state.openblock);
}
