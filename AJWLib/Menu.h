#ifndef AJWLIB_MENU_H
#define AJWLIB_MENU_H

#define AJWLib_Menu_Shade(x,y) Desk_Menu_SetFlags(x,y,Desk_FALSE,Desk_TRUE)
#define AJWLib_Menu_UnShade(x,y) Desk_Menu_SetFlags(x,y,Desk_FALSE,Desk_FALSE)
#define AJWLib_Menu_Tick(x,y) Desk_Menu_SetFlags(x,y,Desk_TRUE,Desk_FALSE)
#define AJWLib_Menu_UnTick(x,y) Desk_Menu_SetFlags(x,y,Desk_FALSE,Desk_FALSE)

typedef void (*AJWLib_menufn)(int entry,void *reference);

Desk_menu_ptr AJWLib_Menu_Create(char *title,char *desc,AJWLib_menufn selectfn,void *reference);

Desk_menu_ptr AJWLib_Menu_CreateFromMsgs(char *titletag,char *desctag,AJWLib_menufn selectfn,void *reference);

void AJWLib_Menu_Register(Desk_menu_ptr menu,AJWLib_menufn selectfn,void *reference);

void AJWLib_Menu_Attach(Desk_window_handle window,Desk_icon_handle icon,Desk_menu_ptr menu,int button);

void AJWLib_Menu_AttachPopup(Desk_window_handle window,Desk_icon_handle popupicon,Desk_icon_handle dataicon,Desk_menu_ptr menu,int button);

void AJWLib_Menu_Release(Desk_menu_ptr menu);

void AJWLib_Menu_FullDispose(Desk_menu_ptr menu);

void AJWLib_Menu_FullFullDispose(Desk_menu_ptr menu);

void AJWLib_Menu_CheckAdjust(void);

void AJWLib_Menu_ToggleTick(Desk_menu_ptr menu,int entry);

void AJWLib_Menu_ToggleShade(Desk_menu_ptr menu,int entry);

#endif

