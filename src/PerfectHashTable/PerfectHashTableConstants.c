/*++

Copyright (c) 2018 Trent Nelson <trent@trent.me>

Module Name:

    PerfectHashTableConstants.c

Abstract:

    This module declares constants used by the perfect hash table library.

--*/

#include "stdafx.h"

//
// Declare the array of creation routines.
//

const PCREATE_PERFECT_HASH_TABLE_IMPL CreationRoutines[] = {
    NULL,
    CreatePerfectHashTableImplChm01,
    NULL
};

//
// Define the array of loader routines.
//

const PLOAD_PERFECT_HASH_TABLE_IMPL LoaderRoutines[] = {
    NULL,
    LoadPerfectHashTableImplChm01,
    NULL
};

//
// Define the array of hash routines.
//

const PPERFECT_HASH_TABLE_HASH HashRoutines[] = {
    NULL,
    PerfectHashTableHashCrc32Rotate,
    PerfectHashTableHashJenkins,
    PerfectHashTableHashRotateXor,
    PerfectHashTableHashAddSubXor,
    PerfectHashTableHashXor,
    NULL
};

//
// Define the array of seeded hash routines.
//

const PPERFECT_HASH_TABLE_SEEDED_HASH SeededHashRoutines[] = {
    NULL,
    PerfectHashTableSeededHashCrc32Rotate,
    PerfectHashTableSeededHashJenkins,
    PerfectHashTableSeededHashRotateXor,
    PerfectHashTableSeededHashAddSubXor,
    PerfectHashTableSeededHashXor,
    NULL
};

//
// Define the array of hash mask routines.
//

const PPERFECT_HASH_TABLE_MASK_HASH MaskHashRoutines[] = {
    NULL,
    PerfectHashTableMaskHashModulus,
    PerfectHashTableMaskHashAnd,
    PerfectHashTableMaskHashXorAnd,

    //
    // The PerfectHashTableFoldAutoMaskFunctionId slot is next.  This is a
    // psuedo ID that don't actually match to a mask implementation.  The
    // algorithm is required to detect when this mask function is being used
    // and swap out the Vtbl pointer to one of the following fold methods
    // depending on the table size.  Thus, we use a NULL pointer in this array
    // such that we'll trap on the first attempt to mask if this hasn't been
    // done.
    //

    NULL,

    PerfectHashTableMaskHashFoldOnce,
    PerfectHashTableMaskHashFoldTwice,
    PerfectHashTableMaskHashFoldThrice,
    NULL
};

//
// Define the array of index mask routines.
//

const PPERFECT_HASH_TABLE_MASK_INDEX MaskIndexRoutines[] = {
    NULL,
    PerfectHashTableMaskIndexModulus,
    PerfectHashTableMaskIndexAnd,
    PerfectHashTableMaskIndexXorAnd,

    //
    // See above description regarding the following NULL slot.
    //

    NULL,

    PerfectHashTableMaskIndexFoldOnce,
    PerfectHashTableMaskIndexFoldTwice,
    PerfectHashTableMaskIndexFoldThrice,
    NULL
};

//
// Define the array of index routines.
//

const PPERFECT_HASH_TABLE_INDEX IndexRoutines[] = {
    NULL,
    PerfectHashTableIndexImplChm01,
    NULL
};

//
// Define the array of fast-index routines.
//

const PERFECT_HASH_TABLE_FAST_INDEX_TUPLE FastIndexRoutines[] = {

    {
        PerfectHashTableChm01AlgorithmId,
        PerfectHashTableHashCrc32RotateFunctionId,
        PerfectHashTableAndMaskFunctionId,
        0,
        PerfectHashTableFastIndexImplChm01Crc32RotateHashAndMask,
    },

    {
        PerfectHashTableChm01AlgorithmId,
        PerfectHashTableHashJenkinsFunctionId,
        PerfectHashTableAndMaskFunctionId,
        0,
        PerfectHashTableFastIndexImplChm01JenkinsHashAndMask,
    },

};

const BYTE NumberOfFastIndexRoutines = ARRAYSIZE(FastIndexRoutines);

//
// Define UNICODE_STRING structures for each algorithm name.
//

const UNICODE_STRING PerfectHashTableChm01AlgorithmName =
    RTL_CONSTANT_STRING(L"Chm01");

//
// Define the array of algorithm names.  This is intended to be indexed by the
// PERFECT_HASH_TABLE_ALGORITHM_ID enum.
//

const PCUNICODE_STRING AlgorithmNames[] = {
    NULL,
    &PerfectHashTableChm01AlgorithmName,
    NULL,
};

//
// Define UNICODE_STRING structures for each hash function name.
//

const UNICODE_STRING PerfectHashTableHashCrc32RotateFunctionName =
    RTL_CONSTANT_STRING(L"Crc32Rotate");

const UNICODE_STRING PerfectHashTableHashJenkinsFunctionName =
    RTL_CONSTANT_STRING(L"Jenkins");

const UNICODE_STRING PerfectHashTableHashRotateXorFunctionName =
    RTL_CONSTANT_STRING(L"RotateXor");

const UNICODE_STRING PerfectHashTableHashAddSubXorFunctionName =
    RTL_CONSTANT_STRING(L"AddSubXor");

const UNICODE_STRING PerfectHashTableHashXorFunctionName =
    RTL_CONSTANT_STRING(L"Xor");

//
// Define the array of hash function names.  This is intended to be indexed by
// the PERFECT_HASH_TABLE_HASH_FUNCTION_ID enum.
//

const PCUNICODE_STRING HashFunctionNames[] = {
    NULL,
    &PerfectHashTableHashCrc32RotateFunctionName,
    &PerfectHashTableHashJenkinsFunctionName,
    &PerfectHashTableHashRotateXorFunctionName,
    &PerfectHashTableHashAddSubXorFunctionName,
    &PerfectHashTableHashXorFunctionName,
    NULL,
};

//
// Define UNICODE_STRING structures for each mask function name.
//

const UNICODE_STRING PerfectHashTableModulusMaskFunctionName =
    RTL_CONSTANT_STRING(L"Modulus");

const UNICODE_STRING PerfectHashTableAndMaskFunctionName =
    RTL_CONSTANT_STRING(L"And");

const UNICODE_STRING PerfectHashTableXorAndMaskFunctionName =
    RTL_CONSTANT_STRING(L"XorAnd");

const UNICODE_STRING PerfectHashTableFoldAutoMaskFunctionName =
    RTL_CONSTANT_STRING(L"FoldAuto");

const UNICODE_STRING PerfectHashTableFoldOnceMaskFunctionName =
    RTL_CONSTANT_STRING(L"FoldOnce");

const UNICODE_STRING PerfectHashTableFoldTwiceMaskFunctionName =
    RTL_CONSTANT_STRING(L"FoldTwice");

const UNICODE_STRING PerfectHashTableFoldThriceMaskFunctionName =
    RTL_CONSTANT_STRING(L"FoldThrice");

//
// Define the array of mask function names.  This is intended to be indexed by
// the PERFECT_HASH_TABLE_MASK_FUNCTION_ID enum.
//

const PCUNICODE_STRING MaskFunctionNames[] = {
    NULL,
    &PerfectHashTableModulusMaskFunctionName,
    &PerfectHashTableAndMaskFunctionName,
    &PerfectHashTableXorAndMaskFunctionName,
    &PerfectHashTableFoldAutoMaskFunctionName,
    &PerfectHashTableFoldOnceMaskFunctionName,
    &PerfectHashTableFoldTwiceMaskFunctionName,
    &PerfectHashTableFoldThriceMaskFunctionName,
    NULL,
};

//
// Array of UNICODE_STRING event prefix names used by the runtime context.
//

const UNICODE_STRING ContextShutdownEventPrefix =
    RTL_CONSTANT_STRING(L"PerfectHashTableContext_ShutdownEvent_");

const UNICODE_STRING ContextSucceededEventPrefix =
    RTL_CONSTANT_STRING(L"PerfectHashTableContext_SucceededEvent_");

const UNICODE_STRING ContextFailedEventPrefix =
    RTL_CONSTANT_STRING(L"PerfectHashTableContext_FailedEvent_");

const UNICODE_STRING ContextCompletedEventPrefix =
    RTL_CONSTANT_STRING(L"PerfectHashTableContext_CompletedEvent_");

const UNICODE_STRING ContextTryLargerTableSizeEventPrefix =
    RTL_CONSTANT_STRING(L"PerfectHashTableContext_TryLargerTableSizeEvent_");

const UNICODE_STRING ContextPreparedFileEventPrefix =
    RTL_CONSTANT_STRING(L"PerfectHashTableContext_PreparedFileEvent_");

const UNICODE_STRING ContextVerifiedEventPrefix =
    RTL_CONSTANT_STRING(L"PerfectHashTableContext_VerifiedEvent_");

const UNICODE_STRING ContextSavedFileEventPrefix =
    RTL_CONSTANT_STRING(L"PerfectHashTableContext_SavedFileEvent_");

const PCUNICODE_STRING ContextObjectPrefixes[] = {
    &ContextShutdownEventPrefix,
    &ContextSucceededEventPrefix,
    &ContextFailedEventPrefix,
    &ContextCompletedEventPrefix,
    &ContextTryLargerTableSizeEventPrefix,
    &ContextPreparedFileEventPrefix,
    &ContextVerifiedEventPrefix,
    &ContextSavedFileEventPrefix,
};

//
// We only have events at the moment so number of event prefixes will equal
// number of object prefixes.
//

const BYTE NumberOfContextEventPrefixes = ARRAYSIZE(ContextObjectPrefixes);
const BYTE NumberOfContextObjectPrefixes = ARRAYSIZE(ContextObjectPrefixes);

//
// Miscellaneous string constants.
//

const UNICODE_STRING No = RTL_CONSTANT_STRING(L"No.\n");
const UNICODE_STRING Yes = RTL_CONSTANT_STRING(L"Yes.\n");
const UNICODE_STRING KeysSuffix = RTL_CONSTANT_STRING(L"keys");
const UNICODE_STRING TableSuffix = RTL_CONSTANT_STRING(L"pht1");
const UNICODE_STRING DotKeysSuffix = RTL_CONSTANT_STRING(L".keys");
const UNICODE_STRING DotTableSuffix = RTL_CONSTANT_STRING(L".pht1");
const UNICODE_STRING KeysWildcardSuffix = RTL_CONSTANT_STRING(L"*.keys");

//
// Placeholders for values we patch in the FastIndexEx() instruction streams.
//

const ULONG Seed1Placeholder = 0x11111111;
const ULONG Seed2Placeholder = 0x22222222;
const ULONG Seed3Placeholder = 0x33333333;
const ULONG Seed4Placeholder = 0x44444444;
const ULONG HashMaskPlaceholder = 0xaaaaaaaa;
const ULONG IndexMaskPlaceholder = 0xbbbbbbbb;

//
// COM glue.
//

//
// Bump this every time an interface is added.  This allows us to succeed every
// array declaration (for arrays intended to be indexed via the interface ID
// enum) with a VERIFY_ARRAY_SIZE() static assertion that ensures we've got
// entries for each ID.  The +2 on the EXPECTED_ARRAY_SIZE macro accounts for
// the leading NullInterfaceId and trailing InvalidInterfaceId slots.
//

#define NUMBER_OF_INTERFACES 7
#define EXPECTED_ARRAY_SIZE NUMBER_OF_INTERFACES+2
#define VERIFY_ARRAY_SIZE(Name) C_ASSERT(ARRAYSIZE(Name) == EXPECTED_ARRAY_SIZE)

C_ASSERT(EXPECTED_ARRAY_SIZE == PerfectHashTableInvalidInterfaceId+1);

const USHORT ComponentSizes[] = {
    0,

    sizeof(IUNKNOWN),
    sizeof(ICLASSFACTORY),
    sizeof(PERFECT_HASH_TABLE_KEYS),
    sizeof(PERFECT_HASH_TABLE_CONTEXT),
    sizeof(PERFECT_HASH_TABLE),
    sizeof(RTL),
    sizeof(ALLOCATOR),

    0,
};
VERIFY_ARRAY_SIZE(ComponentSizes);

const USHORT ComponentInterfaceSizes[] = {
    0,

    sizeof(IUNKNOWN_VTBL),
    sizeof(ICLASSFACTORY_VTBL),
    sizeof(PERFECT_HASH_TABLE_KEYS_VTBL),
    sizeof(PERFECT_HASH_TABLE_CONTEXT_VTBL),
    sizeof(PERFECT_HASH_TABLE_VTBL),
    sizeof(RTL_VTBL),
    sizeof(ALLOCATOR_VTBL),

    0,
};
VERIFY_ARRAY_SIZE(ComponentInterfaceSizes);

const USHORT ComponentInterfaceOffsets[] = {
    0,

    (USHORT)FIELD_OFFSET(IUNKNOWN, Interface),
    (USHORT)FIELD_OFFSET(ICLASSFACTORY, Interface),
    (USHORT)FIELD_OFFSET(PERFECT_HASH_TABLE_KEYS, Interface),
    (USHORT)FIELD_OFFSET(PERFECT_HASH_TABLE_CONTEXT, Interface),
    (USHORT)FIELD_OFFSET(PERFECT_HASH_TABLE, Interface),
    (USHORT)FIELD_OFFSET(RTL, Interface),
    (USHORT)FIELD_OFFSET(ALLOCATOR, Interface),

    0,
};
VERIFY_ARRAY_SIZE(ComponentInterfaceOffsets);

extern COMPONENT_QUERY_INTERFACE ComponentQueryInterface;
extern COMPONENT_ADD_REF ComponentAddRef;
extern COMPONENT_RELEASE ComponentRelease;
extern COMPONENT_CREATE_INSTANCE ComponentCreateInstance;
extern COMPONENT_LOCK_SERVER ComponentLockServer;

//
// IUnknown
//

const IUNKNOWN_VTBL IUnknownInterface = {
    (PIUNKNOWN_QUERY_INTERFACE)&ComponentQueryInterface,
    (PIUNKNOWN_ADD_REF)&ComponentAddRef,
    (PIUNKNOWN_RELEASE)&ComponentRelease,
    (PIUNKNOWN_CREATE_INSTANCE)&ComponentCreateInstance,
    (PIUNKNOWN_LOCK_SERVER)&ComponentLockServer,
};

//
// IClassFactory
//

const ICLASSFACTORY_VTBL IClassFactoryInterface = {
    (PICLASSFACTORY_QUERY_INTERFACE)&ComponentQueryInterface,
    (PICLASSFACTORY_ADD_REF)&ComponentAddRef,
    (PICLASSFACTORY_RELEASE)&ComponentRelease,
    (PICLASSFACTORY_CREATE_INSTANCE)&ComponentCreateInstance,
    (PICLASSFACTORY_LOCK_SERVER)&ComponentLockServer,
};

//
// PerfectHashTableKeys
//

extern PERFECT_HASH_TABLE_KEYS_LOAD PerfectHashTableKeysLoad;

const PERFECT_HASH_TABLE_KEYS_VTBL PerfectHashTableKeysInterface = {
    (PPERFECT_HASH_TABLE_KEYS_QUERY_INTERFACE)&ComponentQueryInterface,
    (PPERFECT_HASH_TABLE_KEYS_ADD_REF)&ComponentAddRef,
    (PPERFECT_HASH_TABLE_KEYS_RELEASE)&ComponentRelease,
    (PPERFECT_HASH_TABLE_KEYS_CREATE_INSTANCE)&ComponentCreateInstance,
    (PPERFECT_HASH_TABLE_KEYS_LOCK_SERVER)&ComponentLockServer,
    &PerfectHashTableKeysLoad,
};

//
// PerfectHashTableContext
//

const PERFECT_HASH_TABLE_CONTEXT_VTBL PerfectHashTableContextInterface = {
    (PPERFECT_HASH_TABLE_CONTEXT_QUERY_INTERFACE)&ComponentQueryInterface,
    (PPERFECT_HASH_TABLE_CONTEXT_ADD_REF)&ComponentAddRef,
    (PPERFECT_HASH_TABLE_CONTEXT_RELEASE)&ComponentRelease,
    (PPERFECT_HASH_TABLE_CONTEXT_CREATE_INSTANCE)&ComponentCreateInstance,
    (PPERFECT_HASH_TABLE_CONTEXT_LOCK_SERVER)&ComponentLockServer,
    &PerfectHashTableContextSetMaximumConcurrency,
    &PerfectHashTableContextGetMaximumConcurrency,
    &PerfectHashTableContextCreateTable,
    &PerfectHashTableContextSelfTest,
    &PerfectHashTableContextSelfTestArgvW,
    &PerfectHashTableContextExtractSelfTestArgsFromArgvW,
};

//
// PerfectHashTable
//

const PERFECT_HASH_TABLE_VTBL PerfectHashTableInterface = {
    (PPERFECT_HASH_TABLE_QUERY_INTERFACE)&ComponentQueryInterface,
    (PPERFECT_HASH_TABLE_ADD_REF)&ComponentAddRef,
    (PPERFECT_HASH_TABLE_RELEASE)&ComponentRelease,
    (PPERFECT_HASH_TABLE_CREATE_INSTANCE)&ComponentCreateInstance,
    (PPERFECT_HASH_TABLE_LOCK_SERVER)&ComponentLockServer,
    &PerfectHashTableLoad,
    &PerfectHashTableTest,
    &PerfectHashTableInsert,
    &PerfectHashTableLookup,
    &PerfectHashTableDelete,
    NULL,   // Index
    NULL,   // Hash
    NULL,   // MaskHash
    NULL,   // MaskIndex
    NULL,   // SeededHash
    NULL,   // FastIndex
    NULL,   // SlowIndex
    &PerfectHashTableGetAlgorithmName,
    &PerfectHashTableGetHashFunctionName,
    &PerfectHashTableGetMaskFunctionName,
};

//
// Rtl
//

const RTL_VTBL RtlInterface = {
    (PRTL_QUERY_INTERFACE)&ComponentQueryInterface,
    (PRTL_ADD_REF)&ComponentAddRef,
    (PRTL_RELEASE)&ComponentRelease,
    (PRTL_CREATE_INSTANCE)&ComponentCreateInstance,
    (PRTL_LOCK_SERVER)&ComponentLockServer,
    &RtlGenerateRandomBytes,
    &RtlPrintSysError,
    &RtlCreateBuffer,
    &RtlCreateMultipleBuffers,
    &RtlDestroyBuffer,
    &RtlCopyPages,
    &RtlFillPages,
    &RtlCreateRandomObjectNames,
    &RtlCreateSingleRandomObjectName,
    &RtlTryLargePageVirtualAlloc,
    &RtlTryLargePageVirtualAllocEx,
    &RtlTryLargePageCreateFileMappingW,
};

//
// Allocator
//

const ALLOCATOR_VTBL AllocatorInterface = {
    (PALLOCATOR_QUERY_INTERFACE)&ComponentQueryInterface,
    (PALLOCATOR_ADD_REF)&ComponentAddRef,
    (PALLOCATOR_RELEASE)&ComponentRelease,
    (PALLOCATOR_CREATE_INSTANCE)&ComponentCreateInstance,
    (PALLOCATOR_LOCK_SERVER)&ComponentLockServer,
    &AllocatorMalloc,
    &AllocatorCalloc,
    &AllocatorFree,
    &AllocatorFreePointer,
};

//
// Interface array.
//

const VOID *ComponentInterfaces[] = {
    NULL,

    &IUnknownInterface,
    &IClassFactoryInterface,
    &PerfectHashTableKeysInterface,
    &PerfectHashTableContextInterface,
    &PerfectHashTableInterface,
    &RtlInterface,
    &AllocatorInterface,

    NULL,
};
VERIFY_ARRAY_SIZE(ComponentInterfaces);

const PCOMPONENT_INITIALIZE ComponentInitializeRoutines[] = {
    NULL,

    NULL, // IUnknown
    NULL, // IClassFactory

    (PCOMPONENT_INITIALIZE)&PerfectHashTableKeysInitialize,
    (PCOMPONENT_INITIALIZE)&PerfectHashTableContextInitialize,
    (PCOMPONENT_INITIALIZE)&PerfectHashTableInitialize,
    (PCOMPONENT_INITIALIZE)&RtlInitialize,
    (PCOMPONENT_INITIALIZE)&AllocatorInitialize,

    NULL,
};
VERIFY_ARRAY_SIZE(ComponentInitializeRoutines);

const PCOMPONENT_RUNDOWN ComponentRundownRoutines[] = {
    NULL,

    NULL, // IUnknown
    NULL, // IClassFactory

    (PCOMPONENT_RUNDOWN)&PerfectHashTableKeysRundown,
    (PCOMPONENT_RUNDOWN)&PerfectHashTableContextRundown,
    (PCOMPONENT_RUNDOWN)&PerfectHashTableRundown,
    (PCOMPONENT_RUNDOWN)&RtlRundown,
    (PCOMPONENT_RUNDOWN)&AllocatorRundown,

    NULL,
};
VERIFY_ARRAY_SIZE(ComponentRundownRoutines);

// vim:set ts=8 sw=4 sts=4 tw=80 expandtab nowrap                              :
