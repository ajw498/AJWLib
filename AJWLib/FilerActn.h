
#define AJWLib_options_VERBOSE 1
#define AJWLib_options_CONFIRM 2
#define AJWLib_options_FORCE   4
#define AJWLib_options_NEWER   8
#define AJWLib_options_RECURSE 16

#define AJWLib_FilerAction_Copy(source,destdir,options) AJWLib_FilerAction_Start(source,destdir,options,0)
#define AJWLib_FilerAction_Move(source,destdir,options) AJWLib_FilerAction_Start(source,destdir,options,1)

void AJWLib_FilerAction_Start(char *source,char *destdir,int options,int type);


