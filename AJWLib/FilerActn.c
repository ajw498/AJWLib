/*
	AJWLib - FilerAction
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.3  1999/10/03 00:48:08  AJW
	Removed Macros

	Revision 1.2  1999/10/02 23:40:03  AJW
	Modified to use Desk

	Revision 1.1  1999/10/02 23:04:42  AJW
	Initial revision


*/

#include "Desk.Wimp.h"
#include "Desk.WimpSWIs.h"
#include "Desk.Filing.h"

#include <string.h>

#define AJWLib_options_VERBOSE 1
#define AJWLib_options_CONFIRM 2
#define AJWLib_options_FORCE   4
#define AJWLib_options_NEWER   8
#define AJWLib_options_RECURSE 16

void AJWLib_FilerAction_Start(char *source,char *destdir,int options,int type)
{
	Desk_task_handle task;
	Desk_message_block msg;
	Desk_Wimp_StartTask3("Filer_Action",&task);
	if (task==0) {
		Desk_Error2_HandleText("FilerAction failed to start");
		return;
	}
	msg.header.size=256;
	msg.header.yourref=0;
	msg.header.action=Desk_message_FILERSELECTIONDIRECTORY;
	Desk_Filing_GetPathname(source,msg.data.bytes);
	Desk_Wimp_SendMessage(Desk_event_USERMESSAGE,&msg,task,0);
	msg.header.action=Desk_message_FILERADDSELECTION;
	Desk_Filing_GetLeafname(source,msg.data.bytes);
	Desk_Wimp_SendMessage(Desk_event_USERMESSAGE,&msg,task,0);
	msg.header.action=Desk_message_FILERACTION;
	msg.data.words[0]=type;
	msg.data.words[1]=options;
	strcpy(msg.data.bytes+8,destdir);
	Desk_Wimp_SendMessage(Desk_event_USERMESSAGE,&msg,task,0);
}


