
#ifndef NDEBUG

#define AJWLib_Assert(x) if (!(x)) { \
	Desk_Error_Report(1,"Assertion failed in file %s at line %d",__FILE__,__LINE__);\
	exit(1);\
}

#else

#define AJWLib_Assert(x)

#endif
