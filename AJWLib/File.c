/*
	AJWLib - File
	© Alex Waugh 1998

	$Id: File.c,v 1.5 2000-02-20 21:33:22 uid1 Exp $

*/

#include "Desk.Error2.h"

#include <stdio.h>
#include <stdlib.h>

#include "kernel.h"

FILE *AJWLib_File_fopen(const char *filename,const char *mode)
/*Same as fopen(), but guarantees a valid file pointer, or creates an error2*/
{
	FILE *file;
	file=fopen(filename,mode);
	if (file==NULL) {
		Desk_Error2_CheckOS((Desk_os_error *)_kernel_last_oserror());
		Desk_Error2_HandleText("Unable to open file");
		exit(EXIT_FAILURE);
	}
	return file;
}

void AJWLib_File_fread(void *ptr,size_t size,size_t nobj,FILE *stream)
/*Same as fread(), but gives an error2 if needed*/
{
	if (nobj!=fread(ptr,size,nobj,stream)) Desk_Error2_CheckOS((Desk_os_error *)_kernel_last_oserror());
}

void AJWLib_File_fwrite(void *ptr,size_t size,size_t nobj,FILE *stream)
/*Same as fwrite(), but gives an error2 if needed*/
{
	if (nobj!=fwrite(ptr,size,nobj,stream)) Desk_Error2_CheckOS((Desk_os_error *)_kernel_last_oserror());
}

void AJWLib_File_fclose(FILE *stream)
/*Same as fclose(), but gives an error2 if needed*/
{
	if (fclose(stream)) Desk_Error2_CheckOS((Desk_os_error *)_kernel_last_oserror());
}

