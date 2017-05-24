#pragma once


#if defined(_WIN32) || defined(WIN32)

#define OS_WINDOWS

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#define NOMINMAX                // Stop conflicts with "min" and "max" macro names
// Windows Header Files:
#include <windows.h>
//#undef NOMINMAX
//#undef WIN32_LEAN_AND_MEAN

#elif defined(__unix__)

#define OS_UNIX

#endif


//RKR_API: Goes before any class/function/global var that should be visible to users of the DLL.
//RKR_EXIMP: Defined as "extern" if importing this DLL, or nothing otherwise.
#ifdef OS_WINDOWS
	#ifdef RGBKEYRENDER_EXPORTS
		#define RKR_API __declspec(dllexport)
        #define RKR_EXIMP
	#elif RGBKEYRENDER_STATIC
		#define RKR_API
        #define RKR_EXIMP
	#else
		#define RKR_API __declspec(dllimport)
        #define RKR_EXIMP extern
	#endif
#else
	#define RKR_API
    #define RKR_EXIMP
#endif