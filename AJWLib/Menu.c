/*
	AJWLib - Menu
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.4  1999/10/09 18:04:58  AJW
	Added AJWLib_ prefix
	
	Revision 1.3  1999/10/07 19:10:00  AJW
	Fixed bug in Menu_Handler - selectfn got corrupted

	Revision 1.2  1999/10/03 00:21:53  AJW
	Modified to use Desk

	Revision 1.1  1999/10/02 23:07:20  AJW
	Initial revision


*/
#include "Desk.WimpSWIs.h"
#include "Desk.Error2.h"
#include "Desk.DeskMem.h"
#include "Desk.Menu.h"
#include "Desk.Msgs.h"
#include "Desk.Icon.h"
#include "Desk.Screen.h"

#include <stdlib.h>

typedef void (*AJWLib_menufn)(int entry,void *reference);

typedef struct menu_struct {
	Desk_menu_ptr menu;
	AJWLib_menufn selectfn;
	void *reference;
	struct menu_struct *next;
} menu_struct;

typedef struct attach_data {
	Desk_menu_ptr menu;
	int button;
	Desk_window_handle window;
	Desk_icon_handle icon;
	Desk_icon_handle popupicon;
	struct attach_data *next;
} attach_data;

static attach_data *attachhead=NULL;
static menu_struct *menuhead=NULL;

void AJWLib_Menu_CheckAdjust(void)
/*Reopen menu if adjust button was used to select item*/
{
	Desk_mouse_block blk;
	Desk_Wimp_GetPointerInfo(&blk);
	if (blk.button.data.adjust) Desk_Menu_ShowLast();
}

static Desk_bool AJWLib_Menu_Handler(Desk_event_pollblock *block,void *reference)
/*Handler called whenever a menu is clicked on*/
{
	int i,entry;
	Desk_bool found=Desk_FALSE;
	Desk_menu_ptr currentmenu=Desk_menu_currentopen;
	Desk_menu_item *items=NULL;
	AJWLib_menufn selectfn=NULL;
	menu_struct *menudata=menuhead;
	for (i=0;block->data.selection[i+1]!=-1;i++) {
		items=(Desk_menu_item *)(currentmenu+1);
		currentmenu=items[block->data.selection[i]].submenu.menu;
		if (currentmenu==(Desk_menu_ptr)-1) return Desk_FALSE;
	}
	entry=block->data.selection[i];
	while (menudata) {
		if (menudata->menu==currentmenu) {
			found=Desk_TRUE;
			selectfn=menudata->selectfn;
			break;
		}
		menudata=menudata->next;
	}
	if (!found) return Desk_FALSE;
	if (selectfn!=NULL) selectfn(entry,menudata->reference);
	AJWLib_Menu_CheckAdjust();
	return Desk_TRUE;
}

void AJWLib_Menu_Register(Desk_menu_ptr menu,AJWLib_menufn selectfn,void *reference)
/*Register an already existing menu with the menu handler*/
{
	menu_struct *newmenu;
	static Desk_bool registered=Desk_FALSE;
	newmenu=Desk_DeskMem_Malloc(sizeof(menu_struct));
	newmenu->next=menuhead;
	menuhead=newmenu;
	newmenu->menu=menu;
	newmenu->selectfn=selectfn;
	newmenu->reference=reference;
	if (!registered) {
		Desk_Event_Claim(Desk_event_MENU,Desk_event_ANY,Desk_event_ANY,AJWLib_Menu_Handler,NULL);
		registered=Desk_TRUE;
	}
}

Desk_menu_ptr AJWLib_Menu_Create(char *title,char *desc,AJWLib_menufn selectfn,void *reference)
/*Create a new menu then register it with the menu handler*/
{
	Desk_menu_ptr handle;
	handle=Desk_Menu_New(title,desc);
	if (handle==NULL) return NULL;
	AJWLib_Menu_Register(handle,selectfn,reference);
	return handle;
}

static Desk_bool AJWLib_Menu_ClickHandler(Desk_event_pollblock *block,void *reference)
/*Called when an icon we are intereseted in is clicked upon*/
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

Desk_menu_ptr AJWLib_Menu_CreateFromMsgs(char *titletag,char *desctag,AJWLib_menufn selectfn,void *reference)
/*As AJWLib_Menu_Create but first looks up messages to find menu text*/
{
	char title[256];
	char desc[256];
	Desk_Msgs_Lookup(titletag,title,256);
	Desk_Msgs_Lookup(desctag,desc,256);
	return AJWLib_Menu_Create(title,desc,selectfn,reference);
}

void AJWLib_Menu_Attach(Desk_window_handle window,Desk_icon_handle icon,Desk_menu_ptr menu,int button)
/*Attaches a menu to an icon*/
{
	attach_data *data=Desk_DeskMem_Malloc(sizeof(attach_data));
	data->window=window;
	data->menu=menu;
	data->button=button;
	data->icon=icon;
	data->popupicon=(Desk_icon_handle)-1;
	data->next=attachhead;
	attachhead=data;
	Desk_Event_Claim(Desk_event_CLICK,window,icon,AJWLib_Menu_ClickHandler,data);
}

void AJWLib_Menu_AttachPopup(Desk_window_handle window,Desk_icon_handle popupicon,Desk_icon_handle dataicon,Desk_menu_ptr menu,int button)
/*Attaches a popup menu to an icon pair*/
{
	attach_data *data=Desk_DeskMem_Malloc(sizeof(attach_data));
	data->window=window;
	data->menu=menu;
	data->button=button;
	data->icon=dataicon;
	data->popupicon=popupicon;
	data->next=attachhead;
	attachhead=data;
	Desk_Event_Claim(Desk_event_CLICK,window,popupicon,AJWLib_Menu_ClickHandler,data);
	Desk_Event_Claim(Desk_event_CLICK,window,dataicon,AJWLib_Menu_ClickHandler,data);
}

void AJWLib_Menu_Release(Desk_menu_ptr menu)
/*Releases a previously attached menu (Does not dispose of the menu)*/
{
	attach_data *data=attachhead;
	attach_data *previous=NULL;
	while (data) {
		if (data->menu==menu) {
			attach_data *temp=data;
			Desk_Event_Release(Desk_event_CLICK,data->window,data->icon,AJWLib_Menu_ClickHandler,data);
			if (data->popupicon!=-1) Desk_Event_Release(Desk_event_CLICK,data->window,data->popupicon,AJWLib_Menu_ClickHandler,data);
			if (previous) previous->next=data->next; else attachhead=data->next;
			Desk_DeskMem_Free(data);
			data=temp->next;
		} else {
			previous=data;
			data=data->next;
		}
	}
}

void AJWLib_Menu_FullDispose(Desk_menu_ptr menu)
/*Releases all handlers asscociated with the menu then disposes of the menu (But no submenus)*/
{
	menu_struct *menudata=menuhead,*previous=NULL;
	AJWLib_Menu_Release(menu);
	while (menudata) {
		if (menudata->menu==menu) {
			menu_struct *temp=menudata;
			if (previous) previous->next=menudata->next; else menuhead=menudata->next;
			Desk_DeskMem_Free(menudata);
			menudata=temp->next;
		} else {
			previous=menudata;
			menudata=menudata->next;
		}
	}
	Desk_Menu_FullDispose(menu);
}

void AJWLib_Menu_ToggleTick(Desk_menu_ptr menu,int entry)
/*Toggles the tick on a menu item*/
{
	int tick,shade;
	Desk_Menu_GetFlags(menu,entry,&tick,&shade);
	Desk_Menu_SetFlags(menu,entry,!tick,shade);
}

void AJWLib_Menu_ToggleShade(Desk_menu_ptr menu,int entry)
/*Toggles the shading on an menu item*/
{
	int tick,shade;
	Desk_Menu_GetFlags(menu,entry,&tick,&shade);
	Desk_Menu_SetFlags(menu,entry,tick,!shade);
}
