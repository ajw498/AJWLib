/*
	AJWLib - Menu
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.1  1999/10/02 23:07:20  AJW
	Initial revision


*/
#include <stdlib.h>
#include "Desk.WimpSWIs.h"
#include "Desk.Error2.h"
#include "Desk.DeskMem.h"
#include "Desk.Menu.h"
#include "Desk.Msgs.h"
#include "Desk.Icon.h"
#include "Desk.Screen.h"


typedef void (*menufn)(int entry,void *reference);

typedef struct {
	Desk_menu_ptr menu;
	menufn selectfn;
	void *reference;
} menu_struct;

typedef struct {
	Desk_menu_ptr menu;
	int button;
	Desk_icon_handle popupicon;
} attach_data;

static menu_struct *menus=NULL;
static int numberofmenus=0;

void Menu_CheckAdjust(void)
{
	Desk_mouse_block blk;
	Desk_Wimp_GetPointerInfo(&blk);
	if (blk.button.data.adjust) Desk_Menu_ShowLast();
}

static Desk_bool Menu_Handler(Desk_event_pollblock *block,void *reference)
{
	int i,entry;
	Desk_bool found=Desk_FALSE;
	Desk_menu_ptr currentmenu=Desk_menu_currentopen;
	Desk_menu_item *items=NULL;
	menufn selectfn=NULL;
	for (i=0;block->data.selection[i+1]!=-1;i++) {
		items=(Desk_menu_item *)(currentmenu+1);
		currentmenu=items[block->data.selection[i]].submenu.menu;
		if (currentmenu==(Desk_menu_ptr)-1) return Desk_FALSE;
	}
	entry=block->data.selection[i];
	for (i=0;i<numberofmenus;i++) {
		if (menus[i].menu==currentmenu) {
			found=Desk_TRUE;
			i=numberofmenus;
		}
	}
	if (!found) return Desk_FALSE;
	selectfn=menus[i].selectfn;
	if (selectfn!=NULL) (*selectfn)(entry,menus[i].reference);
	Menu_CheckAdjust();
	return Desk_TRUE;
}

Desk_menu_ptr Menu_Create(char *title,char *desc,menufn selectfn,void *reference)
{
	Desk_menu_ptr handle;
	menu_struct *newmenus;
	static Desk_bool registered=Desk_FALSE;
	handle=Desk_Menu_New(title,desc);
	if (handle==NULL) return NULL;
	newmenus=Desk_DeskMem_Realloc(menus,(++numberofmenus)*sizeof(menu_struct));
	menus=newmenus;
	menus[numberofmenus-1].menu=handle;
	menus[numberofmenus-1].selectfn=selectfn;
	menus[numberofmenus-1].reference=reference;
	if (!registered) {
		Desk_Error2_CheckBOOL(Desk_Event_Claim(Desk_event_MENU,Desk_event_ANY,Desk_event_ANY,Menu_Handler,NULL));
		registered=Desk_TRUE;
	}
	return handle;
}

static Desk_bool Menu_ClickHandler(Desk_event_pollblock *block,void *reference)
{
	attach_data *data=(attach_data *)reference;
	if ((data->button & block->data.mouse.button.value & Desk_button_SELECT) || (data->button & block->data.mouse.button.value & Desk_button_MENU) || (data->button & block->data.mouse.button.value & Desk_button_ADJUST)) {
		if (data->popupicon==-1) {
			Desk_Menu_Show(data->menu,block->data.mouse.pos.x,(block->data.mouse.window<0 ? -1 : block->data.mouse.pos.y));
			return Desk_TRUE;
		} else {
			Desk_wimp_rect coords;
		    Desk_Icon_ScreenPos(block->data.mouse.window,data->popupicon,&coords);
		    Desk_Menu_Show(data->menu,coords.max.x+32*Desk_screen_delta.x,coords.max.y);
			return Desk_TRUE;
		}
	}
	return Desk_FALSE;
}

Desk_menu_ptr Menu_CreateFromMsgs(char *titletag,char *desctag,menufn selectfn,void *reference)
{
	char title[256];
	char desc[256];
	Desk_Msgs_Lookup(titletag,title,256);
	Desk_Msgs_Lookup(desctag,desc,256);
	return Menu_Create(title,desc,selectfn,reference);
}

void Menu_Attach(Desk_window_handle window,Desk_icon_handle icon,Desk_menu_ptr menu,int button)
{
	attach_data *data=Desk_DeskMem_Malloc(sizeof(attach_data));
	data->menu=menu;
	data->button=button;
	data->popupicon=(Desk_icon_handle)-1;
	Desk_Error2_CheckBOOL(Desk_Event_Claim(Desk_event_CLICK,window,icon,Menu_ClickHandler,data));
}

void Menu_AttachPopup(Desk_window_handle window,Desk_icon_handle popupicon,Desk_icon_handle dataicon,Desk_menu_ptr menu,int button)
{
	attach_data *data=Desk_DeskMem_Malloc(sizeof(attach_data));
	data->menu=menu;
	data->button=button;
	data->popupicon=popupicon;
	Desk_Error2_CheckBOOL(Desk_Event_Claim(Desk_event_CLICK,window,popupicon,Menu_ClickHandler,data));
	Desk_Error2_CheckBOOL(Desk_Event_Claim(Desk_event_CLICK,window,dataicon,Menu_ClickHandler,data));
}

void Menu_ToggleTick(Desk_menu_ptr menu,int entry)
{
	int tick,shade;
	Desk_Menu_GetFlags(menu,entry,&tick,&shade);
	Desk_Menu_SetFlags(menu,entry,!tick,shade);
}

void Menu_ToggleShade(Desk_menu_ptr menu,int entry)
{
	int tick,shade;
	Desk_Menu_GetFlags(menu,entry,&tick,&shade);
	Desk_Menu_SetFlags(menu,entry,tick,!shade);
}
