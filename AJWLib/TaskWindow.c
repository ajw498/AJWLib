/*
	TaskWindow
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.2  1999/10/09 18:42:03  AJW
	Modfied to use Desk
	Added AJWLib_ prefix

	Revision 1.1  1999/10/04 15:19:02  AJW
	Initial revision

	
*/

#include "Desk.Window.h"
#include "Desk.Event.h"
#include "Desk.EventMsg.h"
#include "Desk.Handler.h"
#include "Desk.Icon.h"
#include "Desk.Template.h"
#include "Desk.Pane.h"
#include "Desk.Error.h"
#include "Desk.GFX.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "AJWLib.Pane.h"
#include "AJWLib.Flex.h"

#define max_TEXTLINES 640

typedef enum statustype {
	status_CLOSED,
	status_RUNNING,
	status_SUSPENDED,
	status_FINISHED,
	status_STARTED
} statustype;

static Desk_window_handle master,pane;
static Desk_task_handle child;
static statustype status=status_CLOSED;
static char *text[max_TEXTLINES];
static int numberoflines=0;
static int tasknumber=0;
static char description[22];
static void (*killedfn)(Desk_bool);


void AJWLib_TaskWindow_BringToFront(void)
{
	if (status!=status_CLOSED) AJWLib_Pane_BringToFront(master);
}

static void AJWLib_TaskWindow_ExtractTitle(char *command)
{
	char *title, *terminator, copy[256];
	strcpy(copy,command);
	if ((title=strstr(copy,"-name"))==NULL)
		if ((title=strstr(copy,"-Name"))==NULL)
			title=strstr(copy,"-NAME");
	if (title==NULL) {
		title="Task Display";
	} else {
		title+=6;
		while (*title==' ') title++;
		if (title[0]=='"') {
			terminator="\"";
			title++;
		} else terminator=" ";
		strtok(title,terminator);
	}
	Desk_Window_SetTitle(master,title);
}

void AJWLib_TaskWindow_Suspend(void)
{
	char temp[30];
	Desk_message_block block;
	if (status!=status_RUNNING) return;
	block.header.size=20;
	block.header.action=Desk_message_TASKWINDOW_SUSPEND;
	block.header.yourref=0;
	Desk_Wimp_SendMessage(Desk_event_USERMESSAGE,&block,child,0);
	strcpy(temp,description);
	strcat(temp," (Paused)");
	Desk_Icon_SetText(master,2,temp);
	Desk_Icon_SetText(master,0,"Resume");
	status=status_SUSPENDED;
}

void AJWLib_TaskWindow_Resume(void)
{
	Desk_message_block block;
	if (status!=status_SUSPENDED) return;
	block.header.size=20;
	block.header.action=Desk_message_TASKWINDOW_RESUME;
	block.header.yourref=0;
	Desk_Wimp_SendMessage(Desk_event_USERMESSAGE,&block,child,0);
	Desk_Icon_SetText(master,2,description);
	Desk_Icon_SetText(master,0,"Suspend");
	status=status_RUNNING;
}

void AJWLib_TaskWindow_Close(void)
{
	if (status!=status_FINISHED) return;
	status=status_CLOSED;
	Desk_Pane_Hide(master);
}

void AJWLib_TaskWindow_Kill(void)
{
	int i;
	Desk_message_block block;
	if (status==status_RUNNING || status==status_SUSPENDED) {
		block.header.size=20;
		block.header.action=Desk_message_TASKWINDOW_MORITE;
		block.header.yourref=0;
		Desk_Wimp_SendMessage(Desk_event_USERMESSAGE,&block,child,0);
		if (killedfn!=NULL) (*killedfn)(Desk_TRUE);
	}
	if (numberoflines>max_TEXTLINES-1) numberoflines=max_TEXTLINES-1;
	for(i=0;i<numberoflines;i++) AJWLib_Flex_Free((flex_ptr)&(text[i]));
	numberoflines=0;
	status=status_CLOSED;
	Desk_Pane_Hide(master);
}

static Desk_bool AJWLib_TaskWindow_Morio(Desk_event_pollblock *block, void *r)
{
	char temp[35];
	if (status!=status_RUNNING) return Desk_TRUE;
	status=status_FINISHED;
	Desk_Icon_SetText(master,1,"Close");
	Desk_Icon_SetShade(master,0,Desk_TRUE);
	strcpy(temp,description);
	strcat(temp," (Finished)");
	Desk_Icon_SetText(master,2,temp);
	if (killedfn!=NULL) (*killedfn)(Desk_FALSE);
	return Desk_TRUE;
}

static Desk_bool AJWLib_TaskWindow_Ego(Desk_event_pollblock *block, void *r)
{
	if (block->data.message.data.words[0]!=tasknumber) return Desk_TRUE;
	child=block->data.message.header.sender;
	status=status_RUNNING;
	Desk_Pane_Show(master,Desk_open_WHEREVER);
	AJWLib_Pane_BringToFront(master);
	AJWLib_Flex_Alloc((flex_ptr)&(text[0]),72);
	numberoflines=1;
	text[0][0]='\0';
	return Desk_TRUE;
}

static Desk_bool AJWLib_TaskWindow_Output(Desk_event_pollblock *block, void *r)
{
	int linelength,i;
	char *newtext=block->data.message.data.bytes+4;
	int size=block->data.message.data.words[0];
	if (status!=status_RUNNING) {
		Desk_Error2_HandleText("A child task is outputting data after it has been killed off!?!");
		return Desk_TRUE;
	}
	linelength=strlen(text[numberoflines-1]);
	for (i=0;i<size;i++) {
		if (numberoflines>=max_TEXTLINES) {
			strcpy(text[max_TEXTLINES-3],"No room in task display for any more lines.");
			Desk_Window_ForceRedraw(pane,0,-99999999,99999999,0);
			return Desk_TRUE;
		}
		if (newtext[i]!=10 && newtext[i]!=13) {
			if (linelength>=70) {
				text[numberoflines-1][linelength]='\0';
				AJWLib_Flex_Alloc((flex_ptr)&(text[numberoflines++]),72);
				linelength=0;
			}
			text[numberoflines-1][linelength++]=newtext[i];
		} else {
			text[numberoflines-1][linelength]='\0';
			AJWLib_Flex_Alloc((flex_ptr)&(text[numberoflines++]),72);
			linelength=0;
			if ((newtext[i+1]==10 || newtext[i+1]==13) && newtext[i+1]!=newtext[i]) i++;
		}
	}
	text[numberoflines-1][linelength]='\0';
	{
		Desk_wimp_box extent;
    	Desk_window_state block;
		extent.min.x=0;
		extent.max.x=1172;
		if (numberoflines>6) extent.min.y=-44*numberoflines; else extent.min.y=-44*6;
		extent.max.y=0;
    	Desk_Wimp_SetExtent(pane,&extent);
    	Desk_Wimp_GetWindowState(pane,&block);
    	block.openblock.scroll.y=-44*(numberoflines-6);
    	Desk_Wimp_OpenWindow(&(block.openblock));
    	Desk_Window_ForceRedraw(pane,0,-99999999,99999999,0);
    }
	return Desk_TRUE;
}

static Desk_bool AJWLib_TaskWindow_RedrawPane(Desk_event_pollblock *block,void *r)
{
	Desk_bool more;
	Desk_window_redrawblock *rblock;
	int ox,oy;
	rblock=(Desk_window_redrawblock *)block->data.bytes;
	Desk_Wimp_RedrawWindow(rblock,&more);
    ox=rblock->rect.min.x+rblock->scroll.x;
    oy=rblock->rect.max.y-rblock->scroll.y;
	while (more) {
		int i,min,max;
		Desk_Wimp_SetColour(7);
		min=(oy-rblock->cliprect.max.y)/44-1;
		if (min<0) min=0;
		max=(oy-rblock->cliprect.min.y)/44+1;
		if (max>numberoflines) max=numberoflines;
		for (i=min;i<max;i++) {
			Desk_GFX_Move(ox+16,oy-44*i-6);
			Desk_GFX_Write0(text[i]);
		}
		Desk_Wimp_GetRectangle(rblock,&more);
	}
	return Desk_TRUE;
}

static Desk_bool AJWLib_TaskWindow_KillPressed(Desk_event_pollblock *blk,void *r)
{
	AJWLib_TaskWindow_Kill();
	return Desk_FALSE;
}

static Desk_bool AJWLib_TaskWindow_SuspendPressed(Desk_event_pollblock *blk,void *r)
{
	AJWLib_TaskWindow_Suspend();
	return Desk_FALSE;
}

static Desk_bool AJWLib_TaskWindow_ResumePressed(Desk_event_pollblock *blk,void *r)
{
	AJWLib_TaskWindow_Resume();
	return Desk_FALSE;
}

static void AJWLib_TaskWindow_Init(void)
{
	Desk_wimp_point offset={32,32};
	killedfn=NULL;
	master=Desk_Window_Create("Task",Desk_template_TITLEMIN);
	pane=Desk_Window_Create("TaskPane",Desk_template_TITLEMIN);
	Desk_Pane_Link(master,pane,&offset,NULL,Desk_pane_FIXED);
	Desk_Event_Claim(Desk_event_REDRAW,pane,Desk_event_ANY,AJWLib_TaskWindow_RedrawPane,NULL);
	Desk_Event_Claim(Desk_event_CLICK,master,1,AJWLib_TaskWindow_KillPressed,NULL);
	Desk_Event_Claim(Desk_event_CLICK,master,0,AJWLib_TaskWindow_SuspendPressed,NULL);
	Desk_Event_Claim(Desk_event_CLICK,master,0,AJWLib_TaskWindow_ResumePressed,NULL);
	Desk_Event_Claim(Desk_event_CLOSE,master,Desk_event_ANY,AJWLib_TaskWindow_KillPressed,NULL);
	Desk_Event_Claim(Desk_event_OPEN,master,Desk_event_ANY,Desk_Pane_OpenEventHandler,NULL);
	Desk_EventMsg_Claim(Desk_message_TASKWINDOW_EGO,Desk_event_ANY,AJWLib_TaskWindow_Ego,NULL);
	Desk_EventMsg_Claim(Desk_message_TASKWINDOW_MORIO,Desk_event_ANY,AJWLib_TaskWindow_Morio,NULL);
	Desk_EventMsg_Claim(Desk_message_TASKWINDOW_OUTPUT,Desk_event_ANY,AJWLib_TaskWindow_Output,NULL);
}

void AJWLib_TaskWindow_Start(char *commandtext,char *message,void (*fn)(Desk_bool))
{
	char command[280]="",commandtail[30]="";
	if (status==status_RUNNING || status==status_SUSPENDED) return;
	if (tasknumber==0) AJWLib_TaskWindow_Init();
	killedfn=fn;
	status=status_STARTED;
	child=0;
	Desk_Icon_SetText(master,0,"Suspend");
	Desk_Icon_SetText(master,1,"Kill");
	Desk_Icon_SetShade(master,0,Desk_FALSE);
	strcpy(description,message);
	Desk_Icon_SetText(master,2,message);
	strcpy(command,commandtext);
	AJWLib_TaskWindow_ExtractTitle(command);
	sprintf(commandtail," -task &%X -txt &%X",Desk_Event_taskhandle,++tasknumber);
	strcat(command,commandtail);
	Desk_Wimp_StartTask(command);
}


