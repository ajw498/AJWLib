#include "Desk.Sprite.h"

extern void AJWLib_Window_OpenTransient(Desk_window_handle win);

extern Desk_window_handle AJWLib_Window_CreateInfoWindow(char *name,char *purpose,char *author,char *version);

extern Desk_window_handle AJWLib_Window_CreateInfoWindowFromMsgs(char *nametag,char *purposetag,char *author,char *version);

extern Desk_window_handle AJWLib_Window_CreateUserSprite(char *windowname, int maxtitlesize, Desk_sprite_area windowcontrolblock, int *iconcontrolblocks);
