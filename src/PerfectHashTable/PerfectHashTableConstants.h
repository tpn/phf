/*++

Copyright (c) 2018 Trent Nelson <trent@trent.me>

Module Name:

    PerfectHashTableConstants.h

Abstract:

    This module declares constants related to the PerfectHashTable component.

--*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stdafx.h"

//
// Define two magic numbers for the Magic field of the TABLE_INFO_ON_DISK_HEADER
// structure.
//

#define TABLE_INFO_ON_DISK_MAGIC_LOWPART  0x25101981
#define TABLE_INFO_ON_DISK_MAGIC_HIGHPART 0x17071953

//
// Declare an array of creation routines.  This is intended to be indexed by
// the PERFECT_HASH_TABLE_ALGORITHM_ID enumeration.
//

extern const PCREATE_PERFECT_HASH_TABLE_IMPL CreationRoutines[];

//
// Declare an array of loader routines.  This is intended to be indexed by
// the PERFECT_HASH_TABLE_ALGORITHM_ID enumeration.
//

extern const PLOAD_PERFECT_HASH_TABLE_IMPL LoaderRoutines[];

//
// Declare an array of index routines.  This is intended to be indexed by
// the PERFECT_HASH_TABLE_ALGORITHM_ID enumeration.
//

extern const PPERFECT_HASH_TABLE_INDEX IndexRoutines[];

//
// Declare an array of fast-index routines.  Unlike our other arrays that are
// all indexed by enumeration IDs, this array captures <algorith, hash, mask,
// func> tuples of supporting fast index routines.  The InitializeExtendedVtbl()
// routine below is responsible for walking the array and determining if there
// is an entry present for the requested IDs.  This approach has been selected
// over a 3-dimensional array as there will only typically be a small number of
// fast-index routines and maintaining a 3D array of mostly NULLs is cumbersome.
//

extern const PERFECT_HASH_TABLE_FAST_INDEX_TUPLE FastIndexRoutines[];
extern const BYTE NumberOfFastIndexRoutines;

//
// Declare an array of hash routines.  This is intended to be indexed by
// the PERFECT_HASH_TABLE_HASH_FUNCTION_ID enumeration.
//

extern const PPERFECT_HASH_TABLE_HASH HashRoutines[];

//
// Declare an array of hash mask routines.  This is intended to be indexed by
// the PERFECT_HASH_TABLE_HASH_FUNCTION_ID enumeration.
//

extern const PPERFECT_HASH_TABLE_MASK_HASH MaskHashRoutines[];

//
// Declare an array of index mask routines.  This is intended to be indexed by
// the PERFECT_HASH_TABLE_HASH_FUNCTION_ID enumeration.
//

extern const PPERFECT_HASH_TABLE_MASK_INDEX MaskIndexRoutines[];

//
// Declare an array of seeded hash routines.  This is intended to be indexed by the
// PERFECT_HASH_TABLE_HASH_FUNCTION_ID enumeration.
//

extern const PPERFECT_HASH_TABLE_SEEDED_HASH SeededHashRoutines[];

//
// Helper inline routine for initializing the extended vtbl interface.
//

FORCEINLINE
VOID
InitializePerfectHashTableVtbl(
    _In_ PPERFECT_HASH_TABLE Table,
    _Inout_ PPERFECT_HASH_TABLE_VTBL Vtbl
    )
{
    BYTE Index;
    PERFECT_HASH_TABLE_ALGORITHM_ID AlgorithmId;
    PERFECT_HASH_TABLE_HASH_FUNCTION_ID HashFunctionId;
    PERFECT_HASH_TABLE_MASK_FUNCTION_ID MaskFunctionId;
    PCPERFECT_HASH_TABLE_FAST_INDEX_TUPLE Tuple;

    //
    // Initialize aliases.
    //

    AlgorithmId = Table->AlgorithmId;
    HashFunctionId = Table->HashFunctionId;
    MaskFunctionId = Table->MaskFunctionId;

    //
    // Wire up the table to the vtbl.
    //

    Table->Vtbl = Vtbl;

    //
    // Initialize the specific routines.
    //

    Vtbl->Hash = HashRoutines[HashFunctionId];
    Vtbl->MaskHash = MaskHashRoutines[MaskFunctionId];
    Vtbl->MaskIndex = MaskIndexRoutines[MaskFunctionId];
    Vtbl->SeededHash = SeededHashRoutines[HashFunctionId];

    //
    // Default the slow index to the normal index routine.
    //

    Vtbl->SlowIndex = IndexRoutines[AlgorithmId];

    //
    // Walk the fast index routine tuples and see if any of the entries match
    // the IDs being requested.  If so, save the routine to Vtbl->FastIndex.
    //

    Vtbl->FastIndex = NULL;

    for (Index = 0; Index < NumberOfFastIndexRoutines; Index++) {
        BOOLEAN IsMatch;

        Tuple = &FastIndexRoutines[Index];

        IsMatch = (
            AlgorithmId == Tuple->AlgorithmId &&
            HashFunctionId == Tuple->HashFunctionId &&
            MaskFunctionId == Tuple->MaskFunctionId
        );

        if (IsMatch) {
            Vtbl->FastIndex = Tuple->FastIndex;
            break;
        }

    }

    Vtbl->Index = (Vtbl->FastIndex ? Vtbl->FastIndex : Vtbl->SlowIndex);
}

#ifdef WITH_PARALLEL

#endif//
// Declare the array of algorithm names.
//

extern const PCUNICODE_STRING AlgorithmNames[];

//
// Declare the array of hash function names.
//

extern const PCUNICODE_STRING HashFunctionNames[];

//
// Declare the array of mask function names.
//

extern const PCUNICODE_STRING MaskFunctionNames[];

//
// Object (e.g. events, shared memory sections) name prefixes for the runtime
// context.
//

extern const PCUNICODE_STRING ContextObjectPrefixes[];

extern const BYTE NumberOfContextEventPrefixes;
extern const BYTE NumberOfContextObjectPrefixes;

//
// Helper inline function for programmatically determining how many events
// are present in the context based on the FirstEvent and LastEvent addresses.
// Used by CreatePerfectHashTableContext() and DestroyPerfectHashTableContext().
//

FORCEINLINE
BYTE
GetNumberOfContextEvents(
    _In_ PPERFECT_HASH_TABLE_CONTEXT Context
    )
{
    BYTE NumberOfEvents;

    //
    // Calculate the number of event handles based on the first and last event
    // indicators in the context structure.  The additional sizeof(HANDLE)
    // accounts for the fact that we're going from 0-based address offsets
    // to 1-based counts.
    //

    NumberOfEvents = (BYTE)(

        (ULONG_PTR)(

            sizeof(HANDLE) +

            RtlOffsetFromPointer(
                &Context->LastEvent,
                &Context->FirstEvent
            )

        ) / (ULONG_PTR)sizeof(HANDLE)
    );

    //
    // Sanity check the number of events matches the number of event prefixes.
    //

    ASSERT(NumberOfEvents == NumberOfContextEventPrefixes);

    return NumberOfEvents;
}

//
// Declare miscellaneous strings.
//

extern const UNICODE_STRING No;
extern const UNICODE_STRING Yes;
extern const UNICODE_STRING KeysSuffix;
extern const UNICODE_STRING TableSuffix;
extern const UNICODE_STRING KeysWildcardSuffix;

//
// Declare placeholders for values we patch in the FastIndexEx() instruction
// streams.
//

extern const ULONG Seed1Placeholder;
extern const ULONG Seed2Placeholder;
extern const ULONG Seed3Placeholder;
extern const ULONG Seed4Placeholder;
extern const ULONG HashMaskPlaceholder;
extern const ULONG IndexMaskPlaceholder;

//
// COM glue.
//

extern const USHORT ComponentSizes[];
extern const USHORT ComponentInterfaceSizes[];
extern const USHORT ComponentInterfaceOffsets[];
extern const VOID *ComponentInterfaces[];

extern const IUNKNOWN_VTBL IUnknownInterface;
extern const ICLASSFACTORY_VTBL IClassFactoryInterface;
extern const ALLOCATOR_VTBL AllocatorInterface;
extern const RTL_VTBL RtlInterface;
extern const PERFECT_HASH_TABLE_KEYS_VTBL PerfectHashTableKeysInterface;
extern const PERFECT_HASH_TABLE_CONTEXT_VTBL PerfectHashTableContextInterface;
extern const PERFECT_HASH_TABLE_VTBL PerfectHashTableInterface;

extern const PCOMPONENT_INITIALIZE ComponentInitializeRoutines[];
extern const PCOMPONENT_RUNDOWN ComponentRundownRoutines[];

#ifdef __cplusplus
}; // extern "C" {
#endif

// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :
