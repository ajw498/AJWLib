
typedef enum {
	colourpicker_NORMAL,
	colourpicker_MENU,
	colourpicker_TOOLBOX,
	colourpicker_SUBMENU
} AJWLib_colourpicker_type;

typedef int AJWLib_colourpicker_handle;

AJWLib_colourpicker_handle AJWLib_ColourPicker_Open(AJWLib_colourpicker_type type,int x,int y,unsigned int initcolour,Desk_bool colournone,char *title,Desk_event_handler handler, void *ref);
