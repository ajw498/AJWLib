
#define AJWLib_Error2_Init() { \
	Desk_Error2_HandleAllSignals(); \
	Desk_Error2_SetHandler(AJWLib_Error2_ReportFatal); \
}

Desk_error2_block *AJWLib_Error2_ReportFatal(Desk_error2_block* error);

Desk_error2_block *AJWLib_Error2_Report(Desk_error2_block* error);

char* AJWLib_Error2_Describe(Desk_error2_block* error);
