
#define Desk_Icon_InitIncDecHandler AJWLib_Icon_InitIncDecHandler
#define Desk_Icon_RegisterCheckAdjust AJWLib_Icon_RegisterCheckAdjust

Desk_icon_incdecblock *AJWLib_Icon_InitIncDecHandler(const Desk_window_handle window,
                                         const Desk_icon_handle   texticon,
                                         const Desk_icon_handle   incrementicon,
                                         const Desk_icon_handle   decrementicon,
                                         const Desk_bool          loop,
                                         const unsigned int  step,
                                         const int           min,
                                         const int           max,
                                         const int           start);

Desk_bool AJWLib_Icon_ReleaseIncDecHandler(Desk_icon_incdecblock *incdecblock);


void AJWLib_Icon_RegisterCheckAdjust(Desk_window_handle win,Desk_icon_handle icon);

Desk_icon_handle AJWLib_Icon_FullBarIcon(char *text,char *sprite,int position,int priority);
