/*
	TaskWindow
	©Alex Waugh 1998

	$Log: not supported by cvs2svn $
	Revision 1.1  1999/10/04 15:19:43  AJW
	Initial revision

	
*/


void TaskWindow_BringToFront(void);

void TaskWindow_Suspend(void);

void TaskWindow_Resume(void);

void TaskWindow_Close(void);

void TaskWindow_Kill(void);

void TaskWindow_Start(char *commandtext,char *message,void (*fn)(BOOL));


