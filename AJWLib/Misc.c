/*
	AJWLib - Misc
	© Alex Waugh 1998

	$Log: not supported by cvs2svn $

*/
#include "DeskLib:WimpSWIs.h"
#include "DeskLib:Error.h"

#include <stdio.h>

void Beep(void)
{
	putchar('\7');
}

void ReturnMessage(event_pollblock *block)
{
	switch (block->data.message.header.action) {
		case message_PREQUIT: /*Reply by sending an acknowledgement*/
			block->data.message.header.yourref=block->data.message.header.myref;
			Wimp_SendMessage(event_USERMESSAGEACK,&(block->data.message),block->data.message.header.sender,0);
			break;
		case message_DATALOAD: /*Reply by sending a dataloadack*/
		case message_DATAOPEN:
			block->data.message.header.action=message_DATALOADACK;
			block->data.message.header.yourref=block->data.message.header.myref;
			Wimp_SendMessage(event_USERMESSAGEACK,&(block->data.message),block->data.message.header.sender,0);
			break;
		default:
			Error_ReportInternal(1,"Unable to reply to message (unknown message type: %d)",block->data.message.header.action);
	}
}
