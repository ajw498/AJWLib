/*
	TaskWindow
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $
	
*/

#include "DeskLib:Window.h"
#include "DeskLib:Event.h"
#include "DeskLib:EventMsg.h"
#include "DeskLib:Handler.h"
#include "DeskLib:Icon.h"
#include "DeskLib:Template.h"
#include "DeskLib:Pane.h"
#include "DeskLib:Error.h"
#include "DeskLib:GFX.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "AJWLib:Pane.h"
#include "AJWLib:Flex.h"

#define max_TEXTLINES 640

typedef enum statustype {
	status_CLOSED,
	status_RUNNING,
	status_SUSPENDED,
	status_FINISHED,
	status_STARTED
} statustype;

static window_handle master,pane;
static task_handle child;
static statustype status=status_CLOSED;
static char *text[max_TEXTLINES];
static int numberoflines=0;
static int tasknumber=0;
static char description[22];
static void (*killedfn)(BOOL);


void TaskWindow_BringToFront(void)
{
	if (status!=status_CLOSED) Pane_BringToFront(master);
}

static void TaskWindow_ExtractTitle(char *command)
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
	Window_SetTitle(master,title);
}

void TaskWindow_Suspend(void)
{
	char temp[30];
	message_block block;
	if (status!=status_RUNNING) return;
	block.header.size=20;
	block.header.action=message_TASKWINDOW_SUSPEND;
	block.header.yourref=0;
	Wimp_SendMessage(event_USERMESSAGE,&block,child,0);
	strcpy(temp,description);
	strcat(temp," (Paused)");
	Icon_SetText(master,2,temp);
	Icon_SetText(master,0,"Resume");
	status=status_SUSPENDED;
}

void TaskWindow_Resume(void)
{
	message_block block;
	if (status!=status_SUSPENDED) return;
	block.header.size=20;
	block.header.action=message_TASKWINDOW_RESUME;
	block.header.yourref=0;
	Wimp_SendMessage(event_USERMESSAGE,&block,child,0);
	Icon_SetText(master,2,description);
	Icon_SetText(master,0,"Suspend");
	status=status_RUNNING;
}

void TaskWindow_Close(void)
{
	if (status!=status_FINISHED) return;
	status=status_CLOSED;
	Pane_Hide(master);
}

void TaskWindow_Kill(void)
{
	int i;
	message_block block;
	if (status==status_RUNNING || status==status_SUSPENDED) {
		block.header.size=20;
		block.header.action=message_TASKWINDOW_MORITE;
		block.header.yourref=0;
		Wimp_SendMessage(event_USERMESSAGE,&block,child,0);
		if (killedfn!=NULL) (*killedfn)(TRUE);
	}
	if (numberoflines>max_TEXTLINES-1) numberoflines=max_TEXTLINES-1;
	for(i=0;i<numberoflines;i++) Flex_Free((flex_ptr)&(text[i]));
	numberoflines=0;
	status=status_CLOSED;
	Pane_Hide(master);
}

static BOOL TaskWindow_Morio(event_pollblock *block, void *r)
{
	char temp[35];
	if (status!=status_RUNNING) return TRUE;
	if (killedfn!=NULL) (*killedfn)(FALSE);
	status=status_FINISHED;
	Icon_SetText(master,1,"Close");
	Icon_SetShade(master,0,TRUE);
	strcpy(temp,description);
	strcat(temp," (Finished)");
	Icon_SetText(master,2,temp);
	return TRUE;
}

static BOOL TaskWindow_Ego(event_pollblock *block, void *r)
{
	if (block->data.message.data.words[0]!=tasknumber) return TRUE;
	child=block->data.message.header.sender;
	status=status_RUNNING;
	Pane_Show(master,open_WHEREVER);
	Flex_Alloc((flex_ptr)&(text[0]),72);
	numberoflines=1;
	text[0][0]='\0';
	return TRUE;
}

static BOOL TaskWindow_Output(event_pollblock *block, void *r)
{
	int linelength,i;
	char *newtext=block->data.message.data.bytes+4;
	int size=block->data.message.data.words[0];
	if (status!=status_RUNNING) {
		Error_Report(1,"A Task is outputting after it has been killed off!?!");
		return TRUE;
	}
	linelength=strlen(text[numberoflines-1]);
	for (i=0;i<size;i++) {
		if (numberoflines>=max_TEXTLINES) {
			strcpy(text[max_TEXTLINES-3],"No room in task display for any more lines.");
			Window_ForceRedraw(pane,0,-99999999,99999999,0);
			return TRUE;
		}
		if (newtext[i]!=10 && newtext[i]!=13) {
			if (linelength>=70) {
				text[numberoflines-1][linelength]='\0';
				Flex_Alloc((flex_ptr)&(text[numberoflines++]),72);
				linelength=0;
			}
			text[numberoflines-1][linelength++]=newtext[i];
		} else {
			text[numberoflines-1][linelength]='\0';
			Flex_Alloc((flex_ptr)&(text[numberoflines++]),72);
			linelength=0;
			if ((newtext[i+1]==10 || newtext[i+1]==13) && newtext[i+1]!=newtext[i]) i++;
		}
	}
	text[numberoflines-1][linelength]='\0';
	{
		wimp_box extent;
    	window_state block;
		extent.min.x=0;
		extent.max.x=1172;
		if (numberoflines>6) extent.min.y=-44*numberoflines; else extent.min.y=-44*6;
		extent.max.y=0;
    	Wimp_SetExtent(pane,&extent);
    	Wimp_GetWindowState(pane,&block);
    	block.openblock.scroll.y=-44*(numberoflines-6);
    	Wimp_OpenWindow(&(block.openblock));
    	Window_ForceRedraw(pane,0,-99999999,99999999,0);
    }
	return TRUE;
}

static BOOL TaskWindow_RedrawPane(event_pollblock *block,void *r)
{
	BOOL more;
	window_redrawblock *rblock;
	int ox,oy;
	rblock=(window_redrawblock *)block->data.bytes;
	Wimp_RedrawWindow(rblock,&more);
    ox=rblock->rect.min.x+rblock->scroll.x;
    oy=rblock->rect.max.y-rblock->scroll.y;
	while (more) {
		int i,min,max;
		Wimp_SetColour(7);
		min=(oy-rblock->cliprect.max.y)/44-1;
		if (min<0) min=0;
		max=(oy-rblock->cliprect.min.y)/44+1;
		if (max>numberoflines) max=numberoflines;
		for (i=min;i<max;i++) {
			GFX_Move(ox+16,oy-44*i-6);
			GFX_Write0(text[i]);
		}
		Wimp_GetRectangle(rblock,&more);
	}
	return TRUE;
}

static BOOL TaskWindow_KillPressed(event_pollblock *blk,void *r)
{
	TaskWindow_Kill();
	return FALSE;
}

static BOOL TaskWindow_SuspendPressed(event_pollblock *blk,void *r)
{
	TaskWindow_Suspend();
	return FALSE;
}

static BOOL TaskWindow_ResumePressed(event_pollblock *blk,void *r)
{
	TaskWindow_Resume();
	return FALSE;
}

static void TaskWindow_Init(void)
{
	wimp_point offset={32,32};
	killedfn=NULL;
	master=Window_Create("Task",template_TITLEMIN);
	pane=Window_Create("TaskPane",template_TITLEMIN);
	Pane_Link(master,pane,&offset,NULL,pane_FIXED);
	Event_Claim(event_REDRAW,pane,event_ANY,TaskWindow_RedrawPane,NULL);
	Event_Claim(event_CLICK,master,1,TaskWindow_KillPressed,NULL);
	Event_Claim(event_CLICK,master,0,TaskWindow_SuspendPressed,NULL);
	Event_Claim(event_CLICK,master,0,TaskWindow_ResumePressed,NULL);
	Event_Claim(event_CLOSE,master,event_ANY,TaskWindow_KillPressed,NULL);
	Event_Claim(event_OPEN,master,event_ANY,Pane_OpenEventHandler,NULL);
	EventMsg_Claim(message_TASKWINDOW_EGO,event_ANY,TaskWindow_Ego,NULL);
	EventMsg_Claim(message_TASKWINDOW_MORIO,event_ANY,TaskWindow_Morio,NULL);
	EventMsg_Claim(message_TASKWINDOW_OUTPUT,event_ANY,TaskWindow_Output,NULL);
}

void TaskWindow_Start(char *commandtext,char *message,void (*fn)(BOOL))
{
	char command[280]="",commandtail[30]="";
	if (status==status_RUNNING || status==status_SUSPENDED) return;
	if (tasknumber==0) TaskWindow_Init();
	killedfn=fn;
	status=status_STARTED;
	child=0;
	Icon_SetText(master,0,"Suspend");
	Icon_SetText(master,1,"Kill");
	Icon_SetShade(master,0,FALSE);
	strcpy(description,message);
	Icon_SetText(master,2,message);
	strcpy(command,commandtext);
	TaskWindow_ExtractTitle(command);
	sprintf(commandtail," -task &%X -txt &%X",event_taskhandle,++tasknumber);
	strcat(command,commandtail);
	Wimp_StartTask(command);
}


