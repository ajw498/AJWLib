/*
	AJWLib - Icon
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $

*/
#include "DeskLib:WimpSWIs.h"
#include "DeskLib:KernelSWIs.h"
#include "DeskLib:SWI.h"
#include "DeskLib:Icon.h"
#include "DeskLib:Event.h"

#include <stdlib.h>
#include <string.h>

static BOOL Icon_CheckAdjust(event_pollblock *block,void *r)
{
	if (!block->data.mouse.button.data.adjust) return FALSE;
	if (!Icon_GetSelect(block->data.mouse.window,block->data.mouse.icon)) Icon_Select(block->data.mouse.window,block->data.mouse.icon);
	return FALSE;
}

void Icon_RegisterCheckAdjust(window_handle win,icon_handle icon)
{
	Event_Claim(event_CLICK,win,icon,Icon_CheckAdjust,NULL);
}

icon_handle Icon_FullBarIcon(char *text,char *sprite,int position,int priority)
{
	icon_handle handle;
	icon_createblock iconblk;
	int width,height;
	int factor,textlen=strlen(text);
	int mode;
	char *validation=malloc(14);
    iconblk.window=window_ICONBAR;
	SWI(3,7,0x400E9,40,NULL,sprite,NULL,NULL,NULL,&width,&height,NULL,&mode); /*Wimp_SpriteOp*/
    SWI(2,3,SWI_OS_ReadModeVariable,mode,4,NULL,NULL,&factor);
    width<<=factor;
    if (textlen*16>width) width=textlen*16;
    SWI(2,3,SWI_OS_ReadModeVariable,mode,5,NULL,NULL,&factor);
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
	SWI(2,1,0x400c2,priority,&iconblk,&handle); /*Wimp_CreateIcon*/
	return handle;
}

