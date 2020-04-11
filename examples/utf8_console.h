//# This file is a part of toml++ and is subject to the the terms of the MIT license.
//# Copyright (c) 2019-2020 Mark Gillard <mark.gillard@outlook.com.au>
//# See https://github.com/marzer/tomlplusplus/blob/master/LICENSE for the full license text.
// SPDX-License-Identifier: MIT

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
	#include <Windows.h>

	inline void init_utf8_console() noexcept
	{
		SetConsoleOutputCP(65001); //CP_UTF8
	}

#else

	inline void init_utf8_console() noexcept
	{
		// no-op
	}

#endif
