
#define AJWLib_Error2_Init() { \
	Desk_Error2_HandleAllSignals(); \
	Desk_Error2_SetHandler(AJWLib_Error2_ReportFatal); \
}

void AJWLib_Error2_HandleMsgs(char *tag,...);

#define AJWLib_Error2_ReportMsgs(tag) Desk_Msgs_Report(1,tag,AJWLib_Error2_Describe(&Desk_Error2_globalblock))

#define AJWLib_Error2_Report(x) Desk_Error_Report(1,x,AJWLib_Error2_Describe(&Desk_Error2_globalblock))


Desk_error2_block *AJWLib_Error2_ReportFatal(Desk_error2_block* error);

char* AJWLib_Error2_Describe(Desk_error2_block* error);

