/*
	AJWLib - ColourPicker
	©Alex Waugh 2001

	$Log: not supported by cvs2svn $
	Revision 1.1  2001/05/12 17:42:26  AJW
	Added ColourPicker files
	

*/
#include "Desk/DeskMem.h"
#include "Desk/KernelSWIs.h"
#include "Desk/SWI.h"
#include "Desk/Icon.h"
#include "Desk/Event.h"
#include "Desk/EventMsg.h"
#include "Desk/Error2.h"

#include "AJWLib/Assert.h"

#include "ColourPicker.h"

#define SWI_ColourPicker_OpenDialogue 0x47702
#define SWI_ColourPicker_CloseDialogue 0x47703
#define SWI_ColourPicker_UpdateDialogue 0x47704
#define AJWLib_colourpicker_COLOURCHOICE 0x47700
#define AJWLib_colourpicker_CLOSEDIALOGUEREQUEST 0x47702
#define AJWLib_colourpicker_RESETCOLOURREQUEST 0x47704

static int lastinitcolour=0;
static Desk_window_handle windowhandle=0;

static Desk_bool AJWLib_ColourPicker_MenusDeleted(Desk_event_pollblock *block,void *ref)
{
	Desk_UNUSED(ref);
	Desk_UNUSED(block);

	AJWLib_Assert(windowhandle!=0);

	Desk_Event_ReleaseWindow(windowhandle);

	Desk_EventMsg_ReleaseMessage((Desk_message_action)AJWLib_colourpicker_COLOURCHOICE);
	Desk_EventMsg_ReleaseMessage((Desk_message_action)AJWLib_colourpicker_CLOSEDIALOGUEREQUEST);
	Desk_EventMsg_ReleaseMessage((Desk_message_action)AJWLib_colourpicker_RESETCOLOURREQUEST);

	Desk_EventMsg_Release(Desk_message_MENUSDELETED,Desk_event_ANY,AJWLib_ColourPicker_MenusDeleted);
	windowhandle=0;
	return Desk_TRUE;
}

static Desk_bool AJWLib_ColourPicker_CloseRequest(Desk_event_pollblock *block,void *ref)
{
	Desk_UNUSED(ref);

	AJWLib_ColourPicker_MenusDeleted(NULL,NULL);
	Desk_Error2_CheckOS(Desk_SWI(2,0,SWI_ColourPicker_CloseDialogue,0,block->data.words[4]));
	return Desk_TRUE;
}

static Desk_bool AJWLib_ColourPicker_Handler(Desk_event_pollblock *block,void *ref)
{
	Desk_UNUSED(ref);
	Desk_UNUSED(block);

	return Desk_TRUE;
}

static Desk_bool AJWLib_ColourPicker_ResetColourRequest(Desk_event_pollblock *block,void *ref)
{
	int blk[10]={0,0,0,0x80000000,0x7FFFFFFF,0,0,0,0,0};

	Desk_UNUSED(ref);

	blk[8]=lastinitcolour;
	Desk_Error2_CheckOS(Desk_SWI(3,0,SWI_ColourPicker_UpdateDialogue,1<<6,block->data.words[5],blk));
	return Desk_TRUE;
}

AJWLib_colourpicker_handle AJWLib_ColourPicker_Open(AJWLib_colourpicker_type type,int x,int y,unsigned int initcolour,Desk_bool colournone,char *title,Desk_event_handler handler, void *ref)
{
	int blk[10];
	AJWLib_colourpicker_handle handle;

	blk[0]=colournone & 1;
	blk[1]=(int)title; /*Evil cast*/
	blk[2]=x;
	blk[3]=0x80000000;
	blk[4]=0x7FFFFFFF;
	blk[5]=y;
	blk[6]=0;
	blk[7]=0;
	blk[8]=initcolour;
	blk[9]=0;
	Desk_Error2_CheckOS(Desk_SWI(2,2,SWI_ColourPicker_OpenDialogue,type,blk,&handle,&windowhandle));
	lastinitcolour=initcolour;
	Desk_EventMsg_Claim((Desk_message_action)AJWLib_colourpicker_COLOURCHOICE,Desk_event_ANY,handler,ref);
	Desk_EventMsg_Claim((Desk_message_action)AJWLib_colourpicker_CLOSEDIALOGUEREQUEST,Desk_event_ANY,AJWLib_ColourPicker_CloseRequest,NULL);
	Desk_EventMsg_Claim((Desk_message_action)AJWLib_colourpicker_RESETCOLOURREQUEST,Desk_event_ANY,AJWLib_ColourPicker_ResetColourRequest,NULL);
	if (type & 1) Desk_EventMsg_Claim(Desk_message_MENUSDELETED,Desk_event_ANY,AJWLib_ColourPicker_MenusDeleted,NULL);
	Desk_Event_Claim(Desk_event_KEY,windowhandle,Desk_event_ANY,AJWLib_ColourPicker_Handler,NULL);
	Desk_Event_Claim(Desk_event_REDRAW,windowhandle,Desk_event_ANY,AJWLib_ColourPicker_Handler,NULL);
	return handle;
}
