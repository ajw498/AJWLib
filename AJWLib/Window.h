#include "Desk.Sprite.h"

extern Desk_window_handle Window_CreateInfoWindow(char *name,char *purpose,char *author,char *version);

extern Desk_window_handle Window_CreateInfoWindowFromMsgs(char *nametag,char *purposetag,char *author,char *version);

extern Desk_window_handle Window_CreateUserSprite(char *windowname, int maxtitlesize, Desk_sprite_area windowcontrolblock, int *iconcontrolblocks);