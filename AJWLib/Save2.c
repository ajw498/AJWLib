/*
	AJWLib - Save
	©Alex Waugh 1998
*/

#include "DeskLib:Window.h"
#include "DeskLib:Error.h"
#include "DeskLib:Event.h"
#include "DeskLib:EventMsg.h"
#include "DeskLib:Icon.h"
#include "DeskLib:Screen.h"
#include "DeskLib:Drag.h"
#include "DeskLib:File.h"
#include "DeskLib:Filing.h"
#include "DeskLib:KeyCodes.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define icon_FILE 0
#define icon_FILENAME 1
#define icon_OK 2
#define icon_CANCEL 3

typedef BOOL (*save_handler)(char *filename,BOOL safe,BOOL selection,void *reference);

static window_handle savewindow;
static int filetype=-99;
static int estimatedsize;
static BOOL selectionselected;
static save_handler savefile,saveram;
static BOOL closewindow;
static BOOL menu;

void Save_ChangeFileType(int newfiletype)
{
	Error_ReportFatalInternal(1,"You cannot at present change the filetype once the window has been created");
	/*Change file icon*/ /*Icon_FileIcon(...)?*/
	filetype=newfiletype;
}

void Save_ChangeEstimatedSize(int newsize)
{
	estimatedsize=newsize;
}

void Save_ChangeDefaultFilename(char *newname)
{
	Icon_SetText(savewindow,icon_FILENAME,newname);
}

static void Save_CloseWindow(void)
{
	if (closewindow==FALSE) return;
	if (menu) Wimp_CreateMenu((menu_block *)-1,0,0); else Window_Hide(savewindow);
}

static void Save_DragEnded(void *r)
{
	mouse_block mouseblk;
	message_block msgblk;
	char leafname[256];
	selectionselected=FALSE;
	Wimp_GetPointerInfo(&mouseblk);
	msgblk.header.size=56;
	msgblk.header.yourref=0;
	msgblk.header.action=message_DATASAVE;
	msgblk.data.datasave.window=mouseblk.window;
	msgblk.data.datasave.icon=mouseblk.icon;
	msgblk.data.datasave.pos=mouseblk.pos;
	msgblk.data.datasave.estsize=estimatedsize;
	msgblk.data.datasave.filetype=filetype;
	Filing_GetLeafname(Icon_GetTextPtr(savewindow,icon_FILENAME),leafname);
	if (strlen(leafname)>200) leafname[200]='\0';
	strcpy(msgblk.data.datasave.leafname,leafname);
	Wimp_SendMessage(event_USERMESSAGERECORDED,&msgblk,mouseblk.window,mouseblk.icon);
}

static BOOL Save_DataSaveAck(event_pollblock *block,void *r)
{
	BOOL safe=TRUE;
	if (block->data.message.data.datasaveack.estsize==-1) safe=FALSE;
	(*savefile)(block->data.message.data.datasaveack.filename,safe,selectionselected,r);
	block->data.message.header.yourref=block->data.message.header.myref;
	block->data.message.header.action=message_DATALOAD;
	Wimp_SendMessage(event_USERMESSAGERECORDED,&block->data.message,block->data.message.header.sender,0);
	return TRUE;
}

static BOOL Save_DataLoadAck(event_pollblock *block,void *r)
{
	Save_CloseWindow();
	return TRUE;
}

static BOOL Save_MessageAck(event_pollblock *block,void *r)
{
	switch (block->data.message.header.action) {
		case message_DATASAVE:
			/*Do nothing*/
			break;
		case message_DATALOAD:
			Error_Report(1,"Data transfer failed: Reciever died");
			break;
		default:
			return FALSE;
	}
	return TRUE;
}

static BOOL Save_FileDragged(event_pollblock *block,void *r)
{
	if (block->data.mouse.button.data.dragselect==FALSE && block->data.mouse.button.data.dragadjust==FALSE) return FALSE;
	Icon_StartSolidDrag(block->data.mouse.window,icon_FILE);
	Drag_SetHandlers(NULL,Save_DragEnded,r);
	if (block->data.mouse.button.data.dragselect) closewindow=TRUE; else closewindow=FALSE;
	return TRUE;
}

static BOOL Save_OkPressed(event_pollblock *block,void *r)
{
	char *filename;
	if (block->data.mouse.button.data.menu) return FALSE;
	filename=Icon_GetTextPtr(savewindow,icon_FILENAME);
	if (strlen(filename)<1 || strpbrk(filename,".:")==NULL) {
		Error_Report(1,"Enter a name for the file, then drag the icon to a directory display");
		return TRUE;
	}
	(*savefile)(filename,TRUE,FALSE,r);
	if (block->data.mouse.button.data.select) closewindow=TRUE; else closewindow=FALSE;
	Save_CloseWindow();
	return TRUE;
}

static BOOL Save_CancelPressed(event_pollblock *block,void *r)
{
	if (block->data.mouse.button.data.menu) return FALSE;
	closewindow=TRUE;
	Save_CloseWindow();
	return TRUE;
}

static BOOL Save_KeyPressed(event_pollblock *block,void *r)
{
	char *filename;
	switch (block->data.key.code) {
		case keycode_RETURN:
			filename=Icon_GetTextPtr(savewindow,icon_FILENAME);
			if (strlen(filename)<1 || strpbrk(filename,".:")==NULL) {
				Error_Report(1,"Enter a name for the file, then drag the icon to a directory display");
				return TRUE;
			}
            Icon_SetSelect(savewindow,icon_OK,TRUE);
			(*savefile)(filename,TRUE,FALSE,r);
			closewindow=TRUE;
			Save_CloseWindow();
			Icon_SetSelect(savewindow,icon_OK,FALSE);
			break;
		case keycode_ESCAPE:
			Icon_SetSelect(savewindow,icon_CANCEL,TRUE);
			closewindow=TRUE;
			Save_CloseWindow();
			Icon_SetSelect(savewindow,icon_CANCEL,FALSE);
            break;
		default:
			return FALSE;
	}
	return TRUE;
}

static window_handle Save_Create(void)
{
	struct {
		window_block win;
		icon_block icons[4];
	} blk;
	window_handle handle;
	char *buffer=malloc(256);
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
	if (buffer==NULL) Error_ReportFatal(1,"Out of memory");
	buffer[0]='\0';
	Wimp_CreateWindow(&(blk.win),&handle);
	return handle;
}

 Save_RegisterWindow(window_handle savewin,int filetype,sizefn getsize,BOOL menuleaf,save_handler FileSave,save_handler RAMSave,void *reference)
{

	filetype=type;
	estimatedsize=estsize;
	savefile=FileSave;
	saveram=RAMSave;
	menu=menuleaf;
	Drag_Initialise(FALSE);
	if (selectionbutton) Error_ReportFatalInternal(1,"Selection button in a save window is not supported yet"); 
	if (RAMSave!=NULL) Error_ReportFatalInternal(1,"RAM Transfer is not yet supported");
	savewindow=Save_Create();
	Event_Claim(event_CLICK,savewindow,icon_FILE,Save_FileDragged,reference);
	Event_Claim(event_CLICK,savewindow,icon_OK,Save_OkPressed,reference);
	Event_Claim(event_CLICK,savewindow,icon_CANCEL,Save_CancelPressed,reference);
	Event_Claim(event_KEY,savewindow,event_ANY,Save_KeyPressed,reference);
	Event_Claim(event_USERMESSAGEACK,event_ANY,event_ANY,Save_MessageAck,reference);
	EventMsg_Claim(message_DATASAVEACK,event_ANY,Save_DataSaveAck,reference);
	EventMsg_Claim(message_DATALOADACK,event_ANY,Save_DataLoadAck,reference);
	return savewindow;
}

window_handle Save_CreateWindow(int type,BOOL selectionbutton,int estsize,BOOL menuleaf,save_handler FileSave,save_handler RAMSave,void *reference)
{
	if (filetype!=-99) Error_ReportFatalInternal(1,"You cannot call Save_CreateWindow twice");
	filetype=type;
	estimatedsize=estsize;
	savefile=FileSave;
	saveram=RAMSave;
	menu=menuleaf;
	Drag_Initialise(FALSE);
	if (selectionbutton) Error_ReportFatalInternal(1,"Selection button in a save window is not supported yet"); 
	if (RAMSave!=NULL) Error_ReportFatalInternal(1,"RAM Transfer is not yet supported");
	savewindow=Save_Create();
	Event_Claim(event_CLICK,savewindow,icon_FILE,Save_FileDragged,reference);
	Event_Claim(event_CLICK,savewindow,icon_OK,Save_OkPressed,reference);
	Event_Claim(event_CLICK,savewindow,icon_CANCEL,Save_CancelPressed,reference);
	Event_Claim(event_KEY,savewindow,event_ANY,Save_KeyPressed,reference);
	Event_Claim(event_USERMESSAGEACK,event_ANY,event_ANY,Save_MessageAck,reference);
	EventMsg_Claim(message_DATASAVEACK,event_ANY,Save_DataSaveAck,reference);
	EventMsg_Claim(message_DATALOADACK,event_ANY,Save_DataLoadAck,reference);
	return savewindow;
}
