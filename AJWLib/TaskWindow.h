/*
	TaskWindow
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.3  1999/10/10 14:43:41  AJW
	Added AJWLib_ prefix

	Revision 1.2  1999/10/04 15:33:56  AJW
	Added semi colons at end of lines!

	Revision 1.1  1999/10/04 15:19:43  AJW
	Initial revision

	
*/


void AJWLib_TaskWindow_BringToFront(void);

void AJWLib_TaskWindow_Suspend(void);

void AJWLib_TaskWindow_Resume(void);

void AJWLib_TaskWindow_Close(void);

void AJWLib_TaskWindow_Kill(void);

void AJWLib_TaskWindow_Start(char *commandtext,char *message,void (*fn)(Desk_bool));


