#ifndef ASSERT_H
#define ASSERT_H

#ifdef AJWLIB_ASSERT_NONE

	#define AJWLib_Assert(x)
	#define AJWLib_AssertWarning(x)

#else
	#ifdef AJWLIB_ASSERT_ERROR2
	
		#define AJWLib_Assert(x) if (!(x)) Desk_Error2_HandleText("Assertion failed. (" #x ") "Desk_error_PLACE);
		
		#define AJWLib_AssertWarning(x)
	
	#else
	
		#define AJWLib_Assert(x) if (!(x)) { \
			Desk_Error_Report(1,"Assertion failed. (" #x ") "Desk_error_PLACE);\
			exit(1);\
		}
		
		#define AJWLib_AssertWarning(x) if (!(x)) { \
			Desk_Error_Report(1,"Assertion warning. (" #x ") "Desk_error_PLACE);\
		}
	
	#endif
#endif

#endif

