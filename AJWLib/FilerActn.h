
#define options_VERBOSE 1
#define options_CONFIRM 2
#define options_FORCE   4
#define options_NEWER   8
#define options_RECURSE 16

#define FilerAction_Copy(source,destdir,options) FilerAction_Start(source,destdir,options,0)
#define FilerAction_Move(source,destdir,options) FilerAction_Start(source,destdir,options,1)

void FilerAction_Start(char *source,char *destdir,int options,int type);


