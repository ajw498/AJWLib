/*
	AJWLib - File
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.1  1999/10/11 20:27:38  AJW
	Initial revision


*/

#include "Desk.Error2.h"

#include <stdio.h>
#include <stdlib.h>

#include "kernel.h"

FILE *AJWLib_File_fopen(const char *filename,const char *mode)
/*Same as fopen(), but guarantees a valid file pointer, or creates and error2*/
{
	FILE *file;
	file=fopen(filename,mode);
	if (file==NULL) {
		Desk_Error2_CheckOS(_kernel_last_oserror());
		exit(EXIT_FAILURE);
	}
	return file;
}
