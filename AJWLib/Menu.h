#ifndef AJWLIB_MENU_H
#define AJWLIB_MENU_H

#define Menu_Shade(x,y) Menu_SetFlags(x,y,Desk_FALSE,Desk_TRUE)
#define Menu_UnShade(x,y) Menu_SetFlags(x,y,Desk_FALSE,Desk_FALSE)
#define Menu_Tick(x,y) Menu_SetFlags(x,y,Desk_TRUE,Desk_FALSE)
#define Menu_UnTick(x,y) Menu_SetFlags(x,y,Desk_FALSE,Desk_FALSE)

typedef void (*menufn)(int entry,void *reference);

extern Desk_menu_ptr Menu_Create(char *title,char *desc,menufn selectfn,void *reference);

extern Desk_menu_ptr Menu_CreateFromMsgs(char *titletag,char *desctag,menufn selectfn,void *reference);

extern Desk_bool Menu_Attach(Desk_window_handle window,Desk_icon_handle icon,Desk_menu_ptr menu,int button);

extern Desk_bool Menu_AttachPopup(Desk_window_handle window,Desk_icon_handle popupicon,Desk_icon_handle dataicon,Desk_menu_ptr menu,int button);

extern void Menu_CheckAdjust(void);

extern void Menu_ToggleTick(Desk_menu_ptr menu,int entry);

extern void Menu_ToggleShade(Desk_menu_ptr menu,int entry);

#endif

