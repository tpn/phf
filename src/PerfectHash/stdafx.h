/*++

Copyright (c) 2018 Trent Nelson <trent@trent.me>

Module Name:

    stdafx.h

Abstract:

    This is the precompiled header file for the PerfectHashTable component.

--*/

#ifndef _PERFECT_HASH_INTERNAL_BUILD
#error PerfectHash's stdafx.h being included but _PERFECT_HASH_INTERNAL_BUILD not set.
#endif

#pragma once

#include "targetver.h"

//
// N.B. The warning disable glue is necessary to get the system headers to
//      include with all errors enabled (/Wall).
//

//
// 4255:
//      winuser.h(6502): warning C4255: 'EnableMouseInPointerForThread':
//          no function prototype given: converting '()' to '(void)'
//
// 4668:
//      winioctl.h(8910): warning C4668: '_WIN32_WINNT_WIN10_TH2'
//          is not defined as a preprocessor macro, replacing with
//          '0' for '#if/#elif'
//
//

#pragma warning(push)
#pragma warning(disable: 4255 4668)
#include <Windows.h>
#pragma warning(pop)

//
// <concurrencysal.h> appears to need _PREFAST_ defined.
//

#ifndef _PREFAST_
#define _PREFAST_
#endif

#include <sal.h>
#include <concurrencysal.h>

#include <Windows.h>
#include <Strsafe.h>

//
// Include intrinsic headers if we're x64.
//

#ifdef _M_AMD64

//
// 4255: no function prototype given
//
// 4668: not defined as preprocessor macro
//
// 4820: padding added after member
//
// 28251: inconsistent SAL annotations
//
//

#pragma warning(push)
#pragma warning(disable: 4255 4668 4820 28251)
#include <intrin.h>
#include <mmintrin.h>
#pragma warning(pop)

#endif

#include <PerfectHash.h>
#include <PerfectHashErrors.h>

#include "Component.h"
#include "Rtl.h"
#include "RtlOutput.h"
#include "GuardedList.h"
#include "PerfectHashTls.h"
#include "PerfectHashPath.h"
#include "PerfectHashFile.h"
#include "PerfectHashDirectory.h"
#include "PerfectHashKeys.h"
#include "PerfectHashTable.h"
#include "PerfectHashPrimes.h"
#include "PerfectHashContext.h"
#include "PerfectHashPrivate.h"
#include "PerfectHashAllocator.h"
#include "PerfectHashConstants.h"
#include "PerfectHashErrorHandling.h"
#include "Graph.h"
#include "Chm01.h"

//
// warning C4820: '<unnamed-tag>': '4' bytes padding added after
//      data member 'MessageId'
//

#pragma warning(push)
#pragma warning(disable: 4820)
#include "PerfectHashErrors.dbg"
#pragma warning(pop)

// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :
