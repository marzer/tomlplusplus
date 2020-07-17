#pragma once

// simulate some 'evil' macros to ensure the library is robust against them if they appear in the wild.

#ifdef _WIN32

#ifndef min
	#define min(...)	THIS_IS_AN_EVIL_MACRO
#endif

#ifndef max
	#define max(...)	THIS_IS_AN_EVIL_MACRO
#endif

#ifndef near
	#define near		THIS_IS_AN_EVIL_MACRO
#endif

#ifndef far
	#define far			THIS_IS_AN_EVIL_MACRO
#endif

#else

// ...

#endif
