/*
	AJWLib - Window
	� Alex Waugh 1998

	$Log: not supported by cvs2svn $

*/
#include "DeskLib:WimpSWIs.h"
#include "DeskLib:Msgs.h"
#include "DeskLib:Str.h"

#include <string.h>

/*void Window_BringToFront(window_handle win)
{
	window_state state;
	Wimp_GetWindowState(win,&state);
	state.openblock.behind=-1;
	Wimp_OpenWindow(&state.openblock);
} I have just discovered that this function is already in DeskLib!
*/

window_handle Window_CreateInfoWindow(char *name,char *purpose,char *author,char *version)
{
	struct
	{
		window_block win;
		icon_block icons[8];
	} blk;
	window_handle handle;
	int i;
	blk.win.screenrect.min.x=0;
	blk.win.screenrect.min.y=-248;
	blk.win.screenrect.max.x=636;
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
	blk.win.colours[6]=12;
	blk.win.colours[7]=0;
	blk.win.workarearect.min.x=0;
	blk.win.workarearect.min.y=-248;
	blk.win.workarearect.max.x=636;
	blk.win.workarearect.max.y=0;
	blk.win.titleflags.value=0x17000119;
	blk.win.workflags.value=0x17000000;
	blk.win.spritearea=(void *)1;
	blk.win.minsize.x=636;
	blk.win.minsize.y=248;
	blk.win.title.indirecttext.buffer="About this program";
	blk.win.title.indirecttext.validstring="";
	blk.win.title.indirecttext.bufflen=19;
	blk.win.numicons=8;
	for (i=0;i<4;i++) {
		blk.icons[2*i].workarearect.min.x=8;
		blk.icons[2*i].workarearect.min.y=-60*(i+1);
		blk.icons[2*i].workarearect.max.x=152;
		blk.icons[2*i].workarearect.max.y=-12-60*i;
		blk.icons[2*i].flags.value=0x17000211;
		blk.icons[1+2*i].workarearect.min.x=168;
		blk.icons[1+2*i].workarearect.min.y=-60*(i+1);
		blk.icons[1+2*i].workarearect.max.x=628;
		blk.icons[1+2*i].workarearect.max.y=-8-60*i;
		blk.icons[1+2*i].flags.value=0x1700013D;
		blk.icons[1+2*i].data.indirecttext.validstring="R2";
	}
	strcpy(blk.icons[0].data.text,"Name:");
	strcpy(blk.icons[2].data.text,"Purpose:");
	strcpy(blk.icons[4].data.text,"Author:");
	strcpy(blk.icons[6].data.text,"Version:");
	blk.icons[1].data.indirecttext.buffer=name;
	blk.icons[1].data.indirecttext.bufflen=strlen(name);
	blk.icons[3].data.indirecttext.buffer=purpose;
	blk.icons[3].data.indirecttext.bufflen=strlen(purpose);
	blk.icons[5].data.indirecttext.buffer=author;
	blk.icons[5].data.indirecttext.bufflen=strlen(author);
	blk.icons[7].data.indirecttext.buffer=version;
	blk.icons[7].data.indirecttext.bufflen=strlen(version);
	Wimp_CreateWindow(&(blk.win),&handle);
	return handle;
}

window_handle Window_CreateInfoWindowFromMsgs(char *nametag,char *purposetag,char *author,char *version)
{
	char name[30];
	char purpose[30];
	char *name2,*purpose2;
	Msgs_Lookup(nametag,name,30);
	Msgs_Lookup(purposetag,purpose,30);
	name2=strdup(name);
	purpose2=strdup(purpose);
	if (name2==NULL || purpose2==NULL) return 0;
	return Window_CreateInfoWindow(name2,purpose2,author,version);
}
