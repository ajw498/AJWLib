/*
	AJWLib - Save
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.1  1999/10/02 23:09:27  AJW
	Initial revision


*/

#include "Desk.Window.h"
#include "Desk.Error.h"
#include "Desk.DeskMem.h"
#include "Desk.Event.h"
#include "Desk.EventMsg.h"
#include "Desk.Icon.h"
#include "Desk.Screen.h"
#include "Desk.Drag.h"
#include "Desk.File.h"
#include "Desk.Filing.h"
#include "Desk.KeyCodes.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define icon_FILE 0
#define icon_FILENAME 1
#define icon_OK 2
#define icon_CANCEL 3

typedef Desk_bool (*save_handler)(char *filename,Desk_bool safe,Desk_bool selection,void *reference);
/*
static Desk_window_handle savewindow;
static int filetype=-99;
static int estimatedsize;
static Desk_bool selectionselected;
static save_handler savefile,saveram;
static Desk_bool closewindow;
static Desk_bool menu;

void Save_ChangeFileType(int newfiletype)
{
	Desk_Error_ReportFatalInternal(1,"You cannot at present change the filetype once the window has been created");
	Change file icon Icon_FileIcon(...)?
	filetype=newfiletype;
}

void Save_ChangeEstimatedSize(int newsize)
{
	estimatedsize=newsize;
}

void Save_ChangeDefaultFilename(char *newname)
{
	Desk_Icon_SetText(savewindow,icon_FILENAME,newname);
}

static void Save_CloseWindow(void)
{
	if (closewindow==Desk_FALSE) return;
	if (menu) Desk_Wimp_CreateMenu((Desk_menu_block *)-1,0,0); else Desk_Window_Hide(savewindow);
}

static void Save_DragEnded(void *r)
{
	Desk_mouse_block mouseblk;
	Desk_message_block msgblk;
	char leafname[256];
	selectionselected=Desk_FALSE;
	Desk_Wimp_GetPointerInfo(&mouseblk);
	msgblk.header.size=56;
	msgblk.header.yourref=0;
	msgblk.header.action=Desk_message_DATASAVE;
	msgblk.data.datasave.window=mouseblk.window;
	msgblk.data.datasave.icon=mouseblk.icon;
	msgblk.data.datasave.pos=mouseblk.pos;
	msgblk.data.datasave.estsize=estimatedsize;
	msgblk.data.datasave.filetype=filetype;
	Desk_Filing_GetLeafname(Desk_Icon_GetTextPtr(savewindow,icon_FILENAME),leafname);
	if (strlen(leafname)>200) leafname[200]='\0';
	strcpy(msgblk.data.datasave.leafname,leafname);
	Desk_Wimp_SendMessage(Desk_event_USERMESSAGERECORDED,&msgblk,mouseblk.window,mouseblk.icon);
}

static Desk_bool Save_DataSaveAck(Desk_event_pollblock *block,void *r)
{
	Desk_bool safe=Desk_TRUE;
	if (block->data.message.data.datasaveack.estsize==-1) safe=Desk_FALSE;
	(*savefile)(block->data.message.data.datasaveack.filename,safe,selectionselected,r);
	block->data.message.header.yourref=block->data.message.header.myref;
	block->data.message.header.action=Desk_message_DATALOAD;
	Desk_Wimp_SendMessage(Desk_event_USERMESSAGERECORDED,&block->data.message,block->data.message.header.sender,0);
	return Desk_TRUE;
}

static Desk_bool Save_DataLoadAck(Desk_event_pollblock *block,void *r)
{
	Save_CloseWindow();
	return Desk_TRUE;
}

static Desk_bool Save_MessageAck(Desk_event_pollblock *block,void *r)
{
	switch (block->data.message.header.action) {
		case Desk_message_DATASAVE:
			break;
		case Desk_message_DATALOAD:
			Desk_Error_Report(1,"Data transfer failed: Reciever died");
			break;
		default:
			return Desk_FALSE;
	}
	return Desk_TRUE;
}

static Desk_bool Save_FileDragged(Desk_event_pollblock *block,void *r)
{
	if (block->data.mouse.button.data.dragselect==Desk_FALSE && block->data.mouse.button.data.dragadjust==Desk_FALSE) return Desk_FALSE;
	Desk_Icon_StartSolidDrag(block->data.mouse.window,icon_FILE);
	Desk_Drag_SetHandlers(NULL,Save_DragEnded,r);
	if (block->data.mouse.button.data.dragselect) closewindow=Desk_TRUE; else closewindow=Desk_FALSE;
	return Desk_TRUE;
}

static Desk_bool Save_OkPressed(Desk_event_pollblock *block,void *r)
{
	char *filename;
	if (block->data.mouse.button.data.menu) return Desk_FALSE;
	filename=Desk_Icon_GetTextPtr(savewindow,icon_FILENAME);
	if (strlen(filename)<1 || strpbrk(filename,".:")==NULL) {
		Desk_Error_Report(1,"Enter a name for the file, then drag the icon to a directory display");
		return Desk_TRUE;
	}
	(*savefile)(filename,Desk_TRUE,Desk_FALSE,r);
	if (block->data.mouse.button.data.select) closewindow=Desk_TRUE; else closewindow=Desk_FALSE;
	Save_CloseWindow();
	return Desk_TRUE;
}

static Desk_bool Save_CancelPressed(Desk_event_pollblock *block,void *r)
{
	if (block->data.mouse.button.data.menu) return Desk_FALSE;
	closewindow=Desk_TRUE;
	Save_CloseWindow();
	return Desk_TRUE;
}

static Desk_bool Save_KeyPressed(Desk_event_pollblock *block,void *r)
{
	char *filename;
	switch (block->data.key.code) {
		case Desk_keycode_RETURN:
			filename=Desk_Icon_GetTextPtr(savewindow,icon_FILENAME);
			if (strlen(filename)<1 || strpbrk(filename,".:")==NULL) {
				Desk_Error_Report(1,"Enter a name for the file, then drag the icon to a directory display");
				return Desk_TRUE;
			}
            Desk_Icon_SetSelect(savewindow,icon_OK,Desk_TRUE);
			(*savefile)(filename,Desk_TRUE,Desk_FALSE,r);
			closewindow=Desk_TRUE;
			Save_CloseWindow();
			Desk_Icon_SetSelect(savewindow,icon_OK,Desk_FALSE);
			break;
		case Desk_keycode_ESCAPE:
			Desk_Icon_SetSelect(savewindow,icon_CANCEL,Desk_TRUE);
			closewindow=Desk_TRUE;
			Save_CloseWindow();
			Desk_Icon_SetSelect(savewindow,icon_CANCEL,Desk_FALSE);
            break;
		default:
			return Desk_FALSE;
	}
	return Desk_TRUE;
}

static Desk_window_handle Save_Create(void)
{
	struct {
		Desk_window_block win;
		Desk_icon_block icons[4];
	} blk;
	Desk_window_handle handle;
	char *buffer=Desk_DeskMem_Malloc(256);
	blk.win.screenrect.min.x=0;
	blk.win.screenrect.min.y=-236;
	blk.win.screenrect.max.x=268;
	blk.win.screenrect.max.y=0;
	blk.win.scroll.x=0;
	blk.win.scroll.y=0;
	blk.win.behind=-1;
	blk.win.flags.value=0x84000012;
	blk.win.colours[0]=7;
	blk.win.colours[1]=2;
	blk.win.colours[2]=7;
	blk.win.colours[3]=1;
	blk.win.colours[4]=3;
	blk.win.colours[5]=1;
	blk.win.colours[6]=2;
	blk.win.colours[7]=0;
	blk.win.workarearect.min.x=0;
	blk.win.workarearect.min.y=-236;
	blk.win.workarearect.max.x=268;
	blk.win.workarearect.max.y=0;
	blk.win.titleflags.value=0x17000019;
	blk.win.workflags.value=0x17000000;
	blk.win.spritearea=(void *)1;
	blk.win.minsize.x=268;
	blk.win.minsize.y=236;
	strcpy(blk.win.title.text,"Save as");
	blk.win.numicons=4;
	blk.icons[icon_FILE].workarearect.min.x=100;
	blk.icons[icon_FILE].workarearect.min.y=-80;
	blk.icons[icon_FILE].workarearect.max.x=168;
	blk.icons[icon_FILE].workarearect.max.y=-12;
	blk.icons[icon_FILE].flags.value=0x1700601A;
	sprintf(blk.icons[icon_FILE].data.spritename,"File_%3X",filetype);
	blk.icons[icon_FILENAME].workarearect.min.x=12;
	blk.icons[icon_FILENAME].workarearect.min.y=-144;
	blk.icons[icon_FILENAME].workarearect.max.x=256;
	blk.icons[icon_FILENAME].workarearect.max.y=-92;
	blk.icons[icon_FILENAME].flags.value=0x0700F13D;
	blk.icons[icon_FILENAME].data.indirecttext.buffer=buffer;
	blk.icons[icon_FILENAME].data.indirecttext.validstring="A~ ";
	blk.icons[icon_FILENAME].data.indirecttext.bufflen=256;
	blk.icons[icon_OK].workarearect.min.x=140;
	blk.icons[icon_OK].workarearect.min.y=-224;
	blk.icons[icon_OK].workarearect.max.x=256;
	blk.icons[icon_OK].workarearect.max.y=-156;
	blk.icons[icon_OK].flags.value=0x1700313D;
	blk.icons[icon_OK].data.indirecttext.buffer="Save";
	blk.icons[icon_OK].data.indirecttext.validstring="R6,3";
	blk.icons[icon_OK].data.indirecttext.bufflen=5;
	blk.icons[icon_CANCEL].workarearect.min.x=12;
	blk.icons[icon_CANCEL].workarearect.min.y=-216;
	blk.icons[icon_CANCEL].workarearect.max.x=128;
	blk.icons[icon_CANCEL].workarearect.max.y=-164;
	blk.icons[icon_CANCEL].flags.value=0x1700313D;
	blk.icons[icon_CANCEL].data.indirecttext.buffer="Cancel";
	blk.icons[icon_CANCEL].data.indirecttext.validstring="R5,3";
	blk.icons[icon_CANCEL].data.indirecttext.bufflen=7;
	buffer[0]='\0';
	Desk_Wimp_CreateWindow(&(blk.win),&handle);
	return handle;
}

Desk_window_handle Save_CreateWindow(int type,Desk_bool selectionbutton,int estsize,Desk_bool menuleaf,save_handler FileSave,save_handler RAMSave,void *reference)
{
	if (filetype!=-99) Desk_Error_ReportFatalInternal(1,"You cannot call Save_CreateWindow twice");
	filetype=type;
	estimatedsize=estsize;
	savefile=FileSave;
	saveram=RAMSave;
	menu=menuleaf;
	Desk_Drag_Initialise(Desk_FALSE);
	if (selectionbutton) Desk_Error_ReportFatalInternal(1,"Selection button in a save window is not supported yet"); 
	if (RAMSave!=NULL) Desk_Error_ReportFatalInternal(1,"RAM Transfer is not yet supported");
	savewindow=Save_Create();
	Desk_Event_Claim(Desk_event_CLICK,savewindow,icon_FILE,Save_FileDragged,reference);
	Desk_Event_Claim(Desk_event_CLICK,savewindow,icon_OK,Save_OkPressed,reference);
	Desk_Event_Claim(Desk_event_CLICK,savewindow,icon_CANCEL,Save_CancelPressed,reference);
	Desk_Event_Claim(Desk_event_KEY,savewindow,Desk_event_ANY,Save_KeyPressed,reference);
	Desk_Event_Claim(Desk_event_USERMESSAGEACK,Desk_event_ANY,Desk_event_ANY,Save_MessageAck,reference);
	Desk_EventMsg_Claim(Desk_message_DATASAVEACK,Desk_event_ANY,Save_DataSaveAck,reference);
	Desk_EventMsg_Claim(Desk_message_DATALOADACK,Desk_event_ANY,Save_DataLoadAck,reference);
	return savewindow;
}*/
