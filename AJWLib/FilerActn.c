/*
	AJWLib - FilerAction
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $

*/

#include "DeskLib:Wimp.h"
#include "DeskLib:SWI.h"
#include "DeskLib:WimpSWIs.h"
#include "DeskLib:Error.h"
#include "DeskLib:Event.h"
#include "DeskLib:EventMsg.h"
#include "DeskLib:Filing.h"
#include "AJWLib:Error.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define options_VERBOSE 1
#define options_CONFIRM 2
#define options_FORCE   4
#define options_NEWER   8
#define options_RECURSE 16

#define SWI_Wimp_StartTask 0x400DE

#define ReturnOSError(x) if ((lasterror=x)!=NULL) return lasterror;

static os_error *Filer_Action(char *source,char *destdir,int options,int type)
{
	task_handle task;
	message_block msg;
	ReturnOSError(SWI(1,1,SWI_Wimp_StartTask,"Filer_Action",&task));
	if (task==0) return NULL;
	msg.header.size=256;
	msg.header.yourref=0;
	msg.header.action=message_FILERSELECTIONDIRECTORY;
	Filing_GetPathname(source,msg.data.bytes);
	ReturnOSError(Wimp_SendMessage(event_USERMESSAGE,&msg,task,0));
	msg.header.action=message_FILERADDSELECTION;
	Filing_GetLeafname(source,msg.data.bytes);
	ReturnOSError(Wimp_SendMessage(event_USERMESSAGE,&msg,task,0));
	msg.header.action=message_FILERACTION;
	msg.data.words[0]=type;
	msg.data.words[1]=options;
	strcpy(msg.data.bytes+8,destdir);
	ReturnOSError(Wimp_SendMessage(event_USERMESSAGE,&msg,task,0));
	return NULL;
}

os_error *FilerAction_Copy(char *source,char *destdir,int options)
{
	return Filer_Action(source,destdir,options,0);
}

os_error *FilerAction_Move(char *source,char *destdir,int options)
{
	return Filer_Action(source,destdir,options,1);
}

