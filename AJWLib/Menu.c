/*
	AJWLib - Menu
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $

*/
#include <stdlib.h>
#include "DeskLib:WimpSWIs.h"
#include "DeskLib:Menu.h"
#include "DeskLib:Msgs.h"
#include "DeskLib:Icon.h"
#include "DeskLib:Screen.h"


typedef void (*menufn)(int entry,void *reference);

typedef struct {
	menu_ptr menu;
	menufn selectfn;
	void *reference;
} menu_struct;

typedef struct {
	menu_ptr menu;
	int button;
	icon_handle popupicon;
} attach_data;

static menu_struct *menus=NULL;
static int numberofmenus=0;
static BOOL registered=FALSE;

void Menu_CheckAdjust(void)
{
	mouse_block blk;
	Wimp_GetPointerInfo(&blk);
	if (blk.button.data.adjust==TRUE) Menu_ShowLast();
}

static BOOL Menu_Handler(event_pollblock *block,void *reference)
{
	int i,entry;
	BOOL found=FALSE;
	menu_ptr currentmenu=menu_currentopen;
	menu_item *items=NULL;
	menufn selectfn=NULL;
	for (i=0;block->data.selection[i+1]!=-1;i++) {
		items=(menu_item *)(currentmenu+1);
		currentmenu=items[block->data.selection[i]].submenu.menu;
		if (currentmenu==(menu_ptr)-1) return FALSE;
	}
	entry=block->data.selection[i];
	for (i=0;i<numberofmenus;i++) {
		if (menus[i].menu==currentmenu) {
			found=TRUE;
			break;
		}
	}
	if (!found) return FALSE;
	selectfn=menus[i].selectfn;
	if (selectfn!=NULL) (*selectfn)(entry,menus[i].reference);
	Menu_CheckAdjust();
	return TRUE;
}

menu_ptr Menu_Create(char *title,char *desc,menufn selectfn,void *reference)
{
	menu_ptr handle;
	menu_struct *newmenus;
	handle=Menu_New(title,desc);
	if (handle==NULL) return NULL;
	newmenus=realloc(menus,(++numberofmenus)*sizeof(menu_struct));
	if (newmenus==NULL) return NULL;
	menus=newmenus;
	menus[numberofmenus-1].menu=handle;
	menus[numberofmenus-1].selectfn=selectfn;
	menus[numberofmenus-1].reference=reference;
	if (!registered) {
		if (!Event_Claim(event_MENU,event_ANY,event_ANY,Menu_Handler,NULL)) return NULL;
		registered=TRUE;
	}
	return handle;
}

static BOOL Menu_ClickHandler(event_pollblock *block,void *reference)
{
	attach_data *data=(attach_data *)reference;
	if ((data->button & block->data.mouse.button.value & button_SELECT) || (data->button & block->data.mouse.button.value & button_MENU) || (data->button & block->data.mouse.button.value & button_ADJUST)) {
		if (data->popupicon==-1) {
			Menu_Show(data->menu,block->data.mouse.pos.x,(block->data.mouse.window<0 ? -1 : block->data.mouse.pos.y));
			return TRUE;
		} else {
			wimp_rect coords;
		    Icon_ScreenPos(block->data.mouse.window,data->popupicon,&coords);
		    Menu_Show(data->menu,coords.max.x+64+screen_delta.x,coords.max.y);
			return TRUE;
		}
	}
	return FALSE;
}

menu_ptr Menu_CreateFromMsgs(char *titletag,char *desctag,menufn selectfn,void *reference)
{
	char title[256];
	char desc[256];
	Msgs_Lookup(titletag,title,256);
	Msgs_Lookup(desctag,desc,256);
	return Menu_Create(title,desc,selectfn,reference);
}

BOOL Menu_Attach(window_handle window,icon_handle icon,menu_ptr menu,int button)
{
	attach_data *data=malloc(sizeof(attach_data));
	if (data==NULL) return FALSE;
	data->menu=menu;
	data->button=button;
	data->popupicon=(icon_handle)-1;
	return Event_Claim(event_CLICK,window,icon,Menu_ClickHandler,data);
}

BOOL Menu_AttachPopup(window_handle window,icon_handle popupicon,icon_handle dataicon,menu_ptr menu,int button)
{
	attach_data *data=malloc(sizeof(attach_data));
	if (data==NULL) return FALSE;
	data->menu=menu;
	data->button=button;
	data->popupicon=popupicon;
	if (!Event_Claim(event_CLICK,window,popupicon,Menu_ClickHandler,data)) return FALSE;
	return Event_Claim(event_CLICK,window,dataicon,Menu_ClickHandler,data);
}

void Menu_ToggleTick(menu_ptr menu,int entry)
{
	int tick,shade;
	Menu_GetFlags(menu,entry,&tick,&shade);
	Menu_SetFlags(menu,entry,!tick,shade);
}

void Menu_ToggleShade(menu_ptr menu,int entry)
{
	int tick,shade;
	Menu_GetFlags(menu,entry,&tick,&shade);
	Menu_SetFlags(menu,entry,tick,!shade);
}
