
#define AJWLib_Error2_Init() { \
	Desk_Error2_HandleAllSignals(); \
	Desk_Error2_SetHandler(AJWLib_Error2_ReportFatal); \
}

#define AJWLib_Error2_HandleMsgs(tag) Desk_Error2_HandleText(AJWLib_Msgs_TempLookup(tag))

Desk_error2_block *AJWLib_Error2_ReportFatal(Desk_error2_block* error);

Desk_error2_block *AJWLib_Error2_Report(Desk_error2_block* error);

char* AJWLib_Error2_Describe(Desk_error2_block* error);

