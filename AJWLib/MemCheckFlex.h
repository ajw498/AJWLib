
#ifndef __AJWLib_MemCheck_Flex_h
#define __AJWLib_MemCheck_Flex_h


void AJWLib_Flex__Alloc(flex_ptr anchor,int n);
void AJWLib_Flex__Free(flex_ptr anchor);
void AJWLib_Flex__InitX(char *program_name, int *error_fd,Desk_bool da,int maxsize,Desk_bool virtualise);
void AJWLib_Flex__Init(void);
void AJWLib_Flex__InitDA(char *name,char *maxsize);
void AJWLib_Flex__Extend(flex_ptr anchor,int newsize);
void AJWLib_Flex__MidExtend(flex_ptr anchor,int at,int by);

#if defined(MemCheck_MEMCHECK) && !defined(__AJWLib_MemCheck_Flex_c)
	
	#define	AJWLib_Flex_Alloc(anchor,n) AJWLib_Flex__Alloc(anchor,n)
	#define	AJWLib_Flex_Free(anchor) AJWLib_Flex__Free(anchor)
	#define	AJWLib_Flex_Extend(anchor,newsize) AJWLib_Flex__Extend(anchor,newsize)
	#define	AJWLib_Flex_MidExtend(anchor,at,by) AJWLib_Flex__MidExtend(anchor,at,by)
	#define	AJWLib_Flex_Init() AJWLib_Flex__Init()
	#define	AJWLib_Flex_InitDA(program_name,maxsize) AJWLib_Flex__InitDA(program_name,maxsize)
	#define	AJWLib_Flex_InitX(program_name,error_fd,da,maxsize,virtualise) AJWLib_Flex__InitX(program_name,error_fd,da,maxsize,virtualise)

#endif

#endif

