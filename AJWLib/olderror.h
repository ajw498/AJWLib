extern os_error *lasterror;
extern os_error lasterrorblock;

#define Check(x,y) if (x==FALSE) return y;
#define CheckMsg(x,y,z) if (x==FALSE) {\
		Error_Report(1,y)\
		return z;\
	}
#define CheckOS(x,y) if ((lasterror=x)!=NULL) return y;
#define ReturnError(x,y) {\
							lasterrorblock.errnum=1;\
							strcpy(lasterrorblock.errmess,x);\
							lasterror=&lasterrorblock;\
							return y;\
						 }
#define ReturnOSError(x) if ((lasterror=x)!=NULL) return lasterror;
#define CheckError(x,y) if (x==y) Error_Check(lasterror);
#define CheckErrorFatal(x,y) if (x==y) Error_CheckFatal(lasterror);

void Error_RegisterSignals(void);
