/*
	AJWLib - File
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $

*/

#include "Desk.Error2.h"

#include <stdio.h>
#include <stdlib.h>

#include "kernel.h"

FILE *AJWLib_fopen(const char *filename,const char *mode)
{
	FILE *file;
	file=fopen(filename,mode);
	if (file==NULL) {
		Desk_Error2_CheckOS(_kernel_last_oserror());
		exit(EXIT_FAILURE);
	}
	return file;
}
