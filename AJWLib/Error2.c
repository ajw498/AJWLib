#include <stdlib.h>
#include "signal.h"

#include "Desk.Error2.h"
#include "Desk.DeskMem.h"
#include "Desk.Msgs.h"

char* AJWLib_Error2_Describe(Desk_error2_block* error)
{
	static char buffer[256];
	if (error->type==Desk_error2_type_MISC)
		sprintf(buffer,"Misc error 0x%p",error->data.misc);
	else if (error->type==Desk_error2_type_OSERROR) {
		if (error->data.oserror) {
			#ifdef MemCheck_MEMCHECK
/*
				Turn off checking while we access the os_error - MemCheck
				will fault the access.
*/
				MemCheck_checking	oldchecking = MemCheck_SetChecking( 0, 0);
			#endif
			sprintf(buffer,"OS error. %i, '%s'",error->data.oserror->errnum,error->data.oserror->errmess);
			#ifdef MemCheck_MEMCHECK
				MemCheck_RestoreChecking( oldchecking);
			#endif
		} else sprintf(buffer,"OS error. NULL");
	} else if ( error->type==Desk_error2_type_DESKMEM) {
		if ( error->data.deskmem)
			sprintf(buffer,"DeskMem error Size=%i, ptr=0x%p",error->data.deskmem->size,error->data.deskmem->ptr);
		else sprintf(buffer,"DeskMem error");
	} else if ( error->type==Desk_error2_type_SIGNAL) {
		if (error->data.signal==SIGOSERROR) {
			Desk_os_error* e=(Desk_os_error*) _kernel_last_oserror();
			if (e) {
				sprintf(buffer,"Signal for OS error %i, '%s'",e->errnum,e->errmess);
			} else sprintf(buffer,"Signal for unknown OS error");
		} else sprintf(buffer,"Signal %i: %s",error->data.signal,Desk_Error2_signalnames[error->data.signal]);
	} else if ( error->type==Desk_error2_type_MEM) {
		sprintf(buffer,"Mem error %p", error->data.misc);
	} else if ( error->type==Desk_error2_type_TEXT)	{
		if (error->data.text) sprintf(buffer,"%s", error->data.text); else sprintf(buffer,"");
	} else sprintf(buffer,"Unknown error-type %i, data %p",(int) error->type,error->data.misc);
	return buffer;
}

Desk_error2_block* AJWLib_Error2_Report(Desk_error2_block* error)
{
	Desk_Error_Report(1,AJWLib_Error2_Describe(error));
	return Desk_error2_HANDLED;
}

Desk_error2_block* AJWLib_Error2_ReportFatal(Desk_error2_block* error)
{
	Desk_Msgs_Report(1,"Error.Fatal:%s",AJWLib_Error2_Describe(error));
	exit(EXIT_FAILURE);
	return Desk_error2_HANDLED;
}
