/*
	AJWLib - Window
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.5  2000/02/13 15:48:23  uid1
	Re-enabled files
	
	Revision 1.3  1999/10/09 18:43:30  AJW
	Added AJWLib_ prefix
	
	Revision 1.2  1999/10/03 00:40:42  AJW
	Modified to use Desk

	Revision 1.1  1999/10/02 23:10:51  AJW
	Initial revision


*/
#include "Desk.WimpSWIs.h"
#include "Desk.Msgs.h"
#include "Desk.Menu.h"
#include "Desk.Str.h"
#include "Desk.Screen.h"

#include <string.h>

void AJWLib_Window_OpenTransient(Desk_window_handle win)
{
	Desk_window_state blk;
	Desk_Wimp_GetWindowState(win,&blk);
	Desk_Wimp_CreateMenu((Desk_menu_ptr)win,(Desk_screen_size.x-blk.openblock.screenrect.max.x+blk.openblock.screenrect.min.x)/2,(Desk_screen_size.y+blk.openblock.screenrect.max.y-blk.openblock.screenrect.min.y)/2);
}

Desk_window_handle AJWLib_Window_CreateInfoWindow(char *name,char *purpose,char *author,char *version)
{
	struct
	{
		Desk_window_block win;
		Desk_icon_block icons[8];
	} blk;
	Desk_window_handle handle;
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
	Desk_Wimp_CreateWindow(&(blk.win),&handle);
	return handle;
}

Desk_window_handle AJWLib_Window_CreateInfoWindowFromMsgs(char *nametag,char *purposetag,char *author,char *version)
{
	char name[30];
	char purpose[30];
	char *name2,*purpose2;
	Desk_Msgs_Lookup(nametag,name,30);
	Desk_Msgs_Lookup(purposetag,purpose,30);
	name2=Desk_strdup(name);
	purpose2=Desk_strdup(purpose);
	if (name2==NULL || purpose2==NULL) return 0;
	return AJWLib_Window_CreateInfoWindow(name2,purpose2,author,version);
}
