/*
	AJWLib - Str
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $

*/

#include <ctype.h>

void Str_LowerCase(char *str)
{
	int i;
	for(i=0;str[i]!='\0';i++) str[i]=tolower(str[i]);
}

void Str_LowerCaseCopy(char *dest,char *source)
{
	int i;
	for(i=0;source[i]!='\0';i++) dest[i]=tolower(source[i]);
	dest[i]='\0';
}
