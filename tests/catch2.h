#pragma once

//catch config
#define CATCH_CONFIG_CPP11_TO_STRING
#define CATCH_CONFIG_CPP17_OPTIONAL
#define CATCH_CONFIG_CPP17_STRING_VIEW
#define CATCH_CONFIG_FAST_COMPILE
#define CATCH_CONFIG_CONSOLE_WIDTH 120

//windows.h config
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#define NOATOM //            - Atom Manager routines
	#define NOBITMAP
	#define NOCLIPBOARD //       - Clipboard routines
	#define NOCOLOR //           - Screen colors
	#define NOCOMM //            - COMM driver routines
	#define NOCTLMGR //          - Control and Dialog routines
	#define NODEFERWINDOWPOS //  - DeferWindowPos routines
	#define NODRAWTEXT //        - DrawText() and DT_*
	#define NOGDI //             - All GDI defines and routines
	#define NOGDICAPMASKS //     - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
	#define NOHELP //            - Help engine interface.
	#define NOICONS //           - IDI_*
	#define NOKANJI //           - Kanji support stuff.
	#define NOKEYSTATES //       - MK_*
	#define NOKERNEL //          - All KERNEL defines and routines
	#define NOMB //              - MB_* and MessageBox()
	#define NOMCX //             - Modem Configuration Extensions
	#define NOMENUS //           - MF_*
	#define NOMEMMGR //          - GMEM_*, LMEM_*, GHND, LHND, associated routines
	#define NOMETAFILE //        - typedef METAFILEPICT
	#define NOMINMAX //          - Macros min(a,b) and max(a,b)
	#define NOMSG //             - typedef MSG and associated routines
	#define NONLS //             - All NLS defines and routines
	#define NOOPENFILE //        - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
	#define NOPROFILER //        - Profiler interface.
	#define NORASTEROPS //       - Binary and Tertiary raster ops
	#define NOSCROLL //          - SB_* and scrolling routines
	#define NOSERVICE //         - All Service Controller routines, SERVICE_ equates, etc.
	#define NOSHOWWINDOW //      - SW_*
	#define NOSOUND //           - Sound driver routines
	#define NOSYSCOMMANDS //     - SC_*
	#define NOSYSMETRICS //      - SM_*
	#define NOTEXTMETRIC //      - typedef TEXTMETRIC and associated routines
	#define NOUSER //            - All USER defines and routines
	#define NOVIRTUALKEYCODES // - VK_*
	#define NOWH //              - SetWindowsHook and WH_*
	#define NOWINOFFSETS //      - GWL_*, GCL_*, associated routines
	#define NOWINMESSAGES //     - WM_*, EM_*, LB_*, CB_*
	#define NOWINSTYLES //       - WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#endif

#if __has_include(<Catch2/single_include/catch2/catch.hpp>)
	#include <Catch2/single_include/catch2/catch.hpp>
#else
	#error Catch2 is missing! You probably need to fetch submodules ("git submodule update --init extern/Catch2")
#endif
