extern os_error *lasterror;
extern os_error lasterrorblock;

#define Check(x,y,z) if (x==FALSE) {\
		lasterrorblock.errnum=1;\
		strcpy(lasterrorblock.errmess,y);\
		lasterror=&lasterrorblock;\
		return z;\
	}
#define CheckOS(x,y) if ((lasterror=x)!=NULL) return y;
#define CheckError(x,y) if (x==y) Error_Check(lasterror);
#define CheckErrorFatal(x,y) if (x==y) Error_CheckFatal(lasterror);

void Error_RegisterSignals(void);
