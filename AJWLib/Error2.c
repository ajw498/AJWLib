#include <stdlib.h>

#include "Desk.Error2.h"

static char buffer[256]="";

static int AJWLib_Error2_Print(void* reference,const char* format, ...)
{
	int	i;
	va_list	va;
	va_start( va, format);
	i=vsprintf(buffer,format,va);
	va_end( va);
	return i;
}

void AJWLib_Error2_Report(const Desk_error2_block* error)
{
	Desk_Error2_FDescribe(AJWLib_Error2_Print,NULL,error);
	Desk_Error_Report(1,buffer);
}

void AJWLib_Error2_ReportFatal(const Desk_error2_block* error)
{
	Desk_Error2_FDescribe(AJWLib_Error2_Print,NULL,error);
	Desk_Error_ReportFatal(1,buffer);
}
