#ifndef AJWLIB_MENU_H
#define AJWLIB_MENU_H

#define Menu_Shade(x,y) Menu_SetFlags(x,y,FALSE,TRUE)
#define Menu_UnShade(x,y) Menu_SetFlags(x,y,FALSE,FALSE)
#define Menu_Tick(x,y) Menu_SetFlags(x,y,TRUE,FALSE)
#define Menu_UnTick(x,y) Menu_SetFlags(x,y,FALSE,FALSE)

typedef void (*menufn)(int entry,void *reference);

extern menu_ptr Menu_Create(char *title,char *desc,menufn selectfn,void *reference);

extern menu_ptr Menu_CreateFromMsgs(char *titletag,char *desctag,menufn selectfn,void *reference);

extern BOOL Menu_Attach(window_handle window,icon_handle icon,menu_ptr menu,int button);

extern BOOL Menu_AttachPopup(window_handle window,icon_handle popupicon,icon_handle dataicon,menu_ptr menu,int button);

extern void Menu_CheckAdjust(void);

extern void Menu_ToggleTick(menu_ptr menu,int entry);

extern void Menu_ToggleShade(menu_ptr menu,int entry);

#endif

