/*
	AJWLib - Icon
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.1  1999/10/02 23:06:48  AJW
	Initial revision


*/
#include "Desk.DeskMem.h"
#include "Desk.KernelSWIs.h"
#include "Desk.SWI.h"
#include "Desk.Icon.h"
#include "Desk.Event.h"
#include "Desk.Error2.h"

#include <stdlib.h>
#include <string.h>

#define SWI_Wimp_SpriteOp 0x400E9
#define SWI_Wimp_CreateIcon 0x400C2

static Desk_bool Icon_CheckAdjust(Desk_event_pollblock *block,void *r)
{
	if (!block->data.mouse.button.data.adjust) return Desk_FALSE;
	if (!Desk_Icon_GetSelect(block->data.mouse.window,block->data.mouse.icon)) Desk_Icon_Select(block->data.mouse.window,block->data.mouse.icon);
	return Desk_FALSE;
}

void Icon_RegisterCheckAdjust(Desk_window_handle win,Desk_icon_handle icon)
{
	Desk_Error2_CheckBOOL(Desk_Event_Claim(Desk_event_CLICK,win,icon,Icon_CheckAdjust,NULL));
}

Desk_icon_handle Icon_FullBarIcon(char *text,char *sprite,int position,int priority)
{
	Desk_icon_handle handle;
	Desk_icon_createblock iconblk;
	int width,height;
	int factor,textlen=strlen(text);
	int mode;
	char *validation=Desk_DeskMem_Malloc(14);
    iconblk.window=Desk_window_ICONBAR;
	Desk_Error2_CheckOS(Desk_SWI(3,7,SWI_Wimp_SpriteOp,40,NULL,sprite,NULL,NULL,NULL,&width,&height,NULL,&mode));
    Desk_Error2_CheckOS(Desk_SWI(2,3,Desk_SWI_OS_ReadModeVariable,mode,4,NULL,NULL,&factor));
    width<<=factor;
    if (textlen*16>width) width=textlen*16;
    Desk_Error2_CheckOS(Desk_SWI(2,3,Desk_SWI_OS_ReadModeVariable,mode,5,NULL,NULL,&factor));
    height<<=factor;
    if (*text!='\0') {
    	validation[0]='S';
    	validation[1]='\0';
    	strcat(validation,sprite);
		iconblk.icondata.workarearect.min.x=0;
		iconblk.icondata.workarearect.min.y=-16;
		iconblk.icondata.workarearect.max.x=width;
		iconblk.icondata.workarearect.max.y=20+height;
		iconblk.icondata.flags.value=0x1600310B;
		iconblk.icondata.data.indirecttext.buffer=text;
		iconblk.icondata.data.indirecttext.validstring=validation;
		iconblk.icondata.data.indirecttext.bufflen=textlen+1;
    }
    else {
		iconblk.icondata.workarearect.min.x=0;
		iconblk.icondata.workarearect.min.y=0;
		iconblk.icondata.workarearect.max.x=width;
		iconblk.icondata.workarearect.max.y=height;
		iconblk.icondata.flags.value=0x00003002;
		strcpy(iconblk.icondata.data.spritename,sprite);
    }
	Desk_Error2_CheckOS(Desk_SWI(2,1,SWI_Wimp_CreateIcon,priority,&iconblk,&handle));
	return handle;
}

