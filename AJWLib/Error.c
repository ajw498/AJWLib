/*
	AJWLib - Error
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $

*/

#include "DeskLib:Msgs.h"

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include "kernel.h"

static BOOL error=FALSE;
os_error *lasterror=NULL;
os_error lasterrorblock;

static void Error_SignalHandler(int sig)
{
	_kernel_oserror *err;
	if (error) abort();
	error=TRUE;
	err=_kernel_last_oserror();
	if (err==NULL)
		Msgs_Report(1,"Error.Fatal:Fatal error (%s)","Unknown Error");
	else Msgs_Report(1,"Error.Fatal:Fatal error (%s)",err->errmess);
	exit(EXIT_FAILURE);
}

void Error_RegisterSignals(void)
{
	signal(SIGINT,Error_SignalHandler);
	signal(SIGFPE,Error_SignalHandler);
	signal(SIGILL,Error_SignalHandler);
	signal(SIGSEGV,Error_SignalHandler);
	signal(SIGSTAK,Error_SignalHandler);
	signal(SIGOSERROR,Error_SignalHandler);
}
