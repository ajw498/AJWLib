#ifndef AJWLIB_FILE_H
#define AJWLIB_FILE_H

FILE *AJWLib_File_fopen(const char *filename,const char *mode);

void AJWLib_File_fread(void *ptr,size_t size,size_t nobj,FILE *stream);

void AJWLib_File_fwrite(void *ptr,size_t size,size_t nobj,FILE *stream);

#define AJWLib_File_fclose(stream) { \
	if (fclose(stream)) { \
		stream=NULL; \
		Desk_Error2_CheckOS((Desk_os_error *)_kernel_last_oserror()); \
	} else { \
		stream=NULL; \
	} \
}

#endif

