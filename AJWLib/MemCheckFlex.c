#define MemCheck_MEMCHECK
#define __AJWLib_MemCheck_Flex_c

#include "MemCheck:MemCheck.h"

#include "Desk.Error2.h"

#include "AJWLib.Flex.h"
#include "AJWLib.MemCheckFlex.h"


void AJWLib_Flex__Alloc(flex_ptr anchor,int n)
{
	volatile MemCheck_checking oldchecking=MemCheck_SetChecking(0,0);
	Desk_Error2_Try {
		AJWLib_Flex_Alloc(anchor,n);
		MemCheck_RegisterFlexBlock(anchor, n);
		MemCheck_RestoreChecking(oldchecking);
	} Desk_Error2_Catch {
		MemCheck_RestoreChecking(oldchecking);
		Desk_Error2_ReThrow();
	} Desk_Error2_EndCatch
}

void AJWLib_Flex__Free(flex_ptr anchor)
{
	MemCheck_checking oldchecking=MemCheck_SetChecking(0,0);
	AJWLib_Flex_Free(anchor);
	MemCheck_UnRegisterFlexBlock(anchor);
	MemCheck_RestoreChecking(oldchecking);
}

void AJWLib_Flex__InitX(char *program_name, int *error_fd,Desk_bool da,int maxsize,Desk_bool virtualise)
{
	MemCheck_checking oldchecking=MemCheck_SetChecking(0,0);
	AJWLib_Flex_InitX(program_name,error_fd,da,maxsize,virtualise);
	MemCheck_RestoreChecking(oldchecking);
}

void AJWLib_Flex__Init(void)
{
	MemCheck_checking oldchecking=MemCheck_SetChecking(0,0);
	AJWLib_Flex_Init();
	MemCheck_RestoreChecking(oldchecking);
}

void AJWLib_Flex__InitDA(char *name,char *maxsize)
{
	MemCheck_checking oldchecking=MemCheck_SetChecking(0,0);
	AJWLib_Flex_InitDA(name,maxsize);
	MemCheck_RestoreChecking(oldchecking);
}

void AJWLib_Flex__Extend(flex_ptr anchor,int newsize)
{
	volatile MemCheck_checking oldchecking=MemCheck_SetChecking(0,0);
	Desk_Error2_Try {
		AJWLib_Flex_Extend(anchor,newsize);
		MemCheck_ResizeFlexBlock(anchor,newsize);
		MemCheck_RestoreChecking(oldchecking);
	} Desk_Error2_Catch {
		MemCheck_RestoreChecking(oldchecking);
		Desk_Error2_ReThrow();
	} Desk_Error2_EndCatch
}

void AJWLib_Flex__MidExtend(flex_ptr anchor,int at,int by)
{
	volatile MemCheck_checking oldchecking=MemCheck_SetChecking(0,0);
	Desk_Error2_Try {
		int newsize=AJWLib_Flex_Size(anchor)+by;
		AJWLib_Flex_MidExtend(anchor,at,by);
		MemCheck_ResizeFlexBlock(anchor,newsize);
		MemCheck_RestoreChecking(oldchecking);
	} Desk_Error2_Catch {
		MemCheck_RestoreChecking(oldchecking);
		Desk_Error2_ReThrow();
	} Desk_Error2_EndCatch
}
