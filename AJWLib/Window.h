#include "Desk.Sprite.h"

extern void AJWLib_Window_KeyHandler(Desk_window_handle window,Desk_icon_handle ok,Desk_event_handler okfn,Desk_icon_handle cancel,Desk_event_handler cancelfn,void *ref);

extern void AJWLib_Window_OpenTransient(Desk_window_handle win);

extern void AJWLib_Window_RegisterDCS(Desk_window_handle win,Desk_icon_handle discard,Desk_icon_handle cancel,Desk_icon_handle save,void (*discardfn)(void),void (*savefn)(void));

extern Desk_window_handle AJWLib_Window_CreateInfoWindow(char *name,char *purpose,char *author,char *version);

extern Desk_window_handle AJWLib_Window_CreateInfoWindowFromMsgs(char *nametag,char *purposetag,char *author,char *version);

extern Desk_window_handle AJWLib_Window_CreateUserSprite(char *windowname, int maxtitlesize, Desk_sprite_area windowcontrolblock, int *iconcontrolblocks);
