/*
	AJWLib - Str
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.1  1999/10/02 23:10:24  AJW
	Initial revision


*/

#include <ctype.h>

void AJWLib_Str_LowerCase(char *str)
{
	int i;
	for(i=0;str[i]!='\0';i++) str[i]=tolower(str[i]);
}

void AJWLib_Str_LowerCaseCopy(char *dest,char *source)
{
	int i;
	for(i=0;source[i]!='\0';i++) dest[i]=tolower(source[i]);
	dest[i]='\0';
}
