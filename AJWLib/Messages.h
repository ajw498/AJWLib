
#define MESSAGE_BASE 0x040380 /*SWI base of debugger module*/

#define message_Task_Start      (MESSAGE_BASE+0)
#define message_Task_Started    (MESSAGE_BASE+1)
#define message_Task_Finished   (MESSAGE_BASE+2)
#define message_Task_Suspend    (MESSAGE_BASE+3)
#define message_Task_Resume     (MESSAGE_BASE+4)
#define message_Task_Kill       (MESSAGE_BASE+5)
#define message_Task_Killed     (MESSAGE_BASE+6)
#define message_Task_Register   (MESSAGE_BASE+7)
#define message_Task_Registered (MESSAGE_BASE+8)
#define message_Task_DeRegister (MESSAGE_BASE+9)

#define message_Compiler_Start  (MESSAGE_BASE+16)
#define message_Linker_AddFile  (MESSAGE_BASE+17)
#define message_Make_Start      (MESSAGE_BASE+18)
