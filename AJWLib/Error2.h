
#define AJWLib_Error2_Init() { \
	Desk_Error2_HandleAllSignals(); \
	Desk_Error2_SetHandler(AJWLib_Error2_ReportFatal); \
}

#define AJWLib_Error2_HandleMsgs(tag) Desk_Error2_HandleText(AJWLib_Msgs_TempLookup(tag))

#define AJWLib_Error2_HandleMsgs2(tag,x) { \
	char buffer[256]; \
	sprintf(buffer,AJWLib_Msgs_TempLookup(tag),x); \
	Desk_Error2_HandleText(buffer); \
}

#define AJWLib_Error2_HandleMsgs3(tag,x,y) { \
	char buffer[256]; \
	sprintf(buffer,AJWLib_Msgs_TempLookup(tag),x,y); \
	Desk_Error2_HandleText(buffer); \
}

#define AJWLib_Error2_ReportMsgs(tag) Desk_Msgs_Report(1,AJWLib_Msgs_TempLookup(tag),AJWLib_Error2_Describe(&Desk_Error2_globalblock))

#define AJWLib_Error2_Report(x) Desk_Error_Report(1,x,AJWLib_Error2_Describe(&Desk_Error2_globalblock))


Desk_error2_block *AJWLib_Error2_ReportFatal(Desk_error2_block* error);

char* AJWLib_Error2_Describe(Desk_error2_block* error);

