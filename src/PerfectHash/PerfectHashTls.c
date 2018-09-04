/*++

Copyright (c) 2018 Trent Nelson <trent@trent.me>

Module Name:

    PerfectHashTls.c

Abstract:

    This module provides TLS glue to the perfect hash library.

--*/

#include "stdafx.h"

//
// Our TLS index.  Assigned at PROCESS_ATTACH, free'd at PROCESS_DETACH.
//

ULONG PerfectHashTlsIndex;

PERFECT_HASH_TLS_FUNCTION PerfectHashTlsProcessAttach;

_Use_decl_annotations_
BOOLEAN
PerfectHashTlsProcessAttach(
    HMODULE Module,
    ULONG   Reason,
    LPVOID  Reserved
    )
{
    UNREFERENCED_PARAMETER(Module);
    UNREFERENCED_PARAMETER(Reason);
    UNREFERENCED_PARAMETER(Reserved);

    PerfectHashTlsIndex = TlsAlloc();

    if (PerfectHashTlsIndex == TLS_OUT_OF_INDEXES) {
        return FALSE;
    }

    return TRUE;
}

PERFECT_HASH_TLS_FUNCTION PerfectHashTlsProcessDetach;

_Use_decl_annotations_
BOOLEAN
PerfectHashTlsProcessDetach(
    HMODULE Module,
    ULONG   Reason,
    LPVOID  Reserved
    )
{
    UNREFERENCED_PARAMETER(Module);
    UNREFERENCED_PARAMETER(Reason);

    BOOL IsProcessTerminating;

    IsProcessTerminating = (Reserved != NULL);

    if (IsProcessTerminating) {
        goto End;
    }

    if (PerfectHashTlsIndex == TLS_OUT_OF_INDEXES) {
        goto End;
    }

    if (!TlsFree(PerfectHashTlsIndex)) {

        //
        // Can't do anything here.
        //

        NOTHING;
    }

    //
    // Note that we always return TRUE here, even if we had a failure.  We're
    // only called at DLL_PROCESS_DETACH, so there's not much we can do when
    // there is actually an error anyway.
    //

End:

    return TRUE;
}

//
// TLS Set/Get Context functions.
//

PERFECT_HASH_TLS_SET_CONTEXT PerfectHashTlsSetContext;

_Use_decl_annotations_
BOOL
PerfectHashTlsSetContext(
    PPERFECT_HASH_TLS_CONTEXT Context
    )
{
    return TlsSetValue(PerfectHashTlsIndex, Context);
}

PERFECT_HASH_TLS_GET_CONTEXT PerfectHashTlsGetContext;

_Use_decl_annotations_
PPERFECT_HASH_TLS_CONTEXT
PerfectHashTlsGetContext(
    VOID
    )
{
    PVOID Value;

    Value = TlsGetValue(PerfectHashTlsIndex);

    return (PPERFECT_HASH_TLS_CONTEXT)Value;
}


// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :