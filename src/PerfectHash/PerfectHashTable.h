/*++

Copyright (c) 2018 Trent Nelson <trent@trent.me>

Module Name:

    PerfectHashTable.h

Abstract:

    This is the private header file for the PerfectHashTable component.  It
    defines function typedefs and function declarations for all major (i.e. not
    local to the module) functions available for use by individual modules
    within this component.

--*/

#ifndef _PERFECT_HASH_INTERNAL_BUILD
#error PerfectHashTablePrivate.h being included but _PERFECT_HASH_INTERNAL_BUILD not set.
#endif

#pragma once

#include "stdafx.h"

//
// Define the PERFECT_HASH_TABLE_STATE structure.
//

typedef union _PERFECT_HASH_TABLE_STATE {
    struct {

        //
        // When set, indicates the table is in a valid state.
        //

        ULONG Valid:1;

        //
        // When set, indicates the Table->TableInfoOnDisk structure is backed
        // by heap-allocated memory obtained from the allocator, and thus, must
        // be free'd by the allocator during rundown.
        //

        ULONG TableInfoOnDiskWasHeapAllocated:1;

        //
        // As above, but for the Assigned/TableDataBaseAddress pointer.
        //

        ULONG TableDataWasHeapAllocated:1;

        //
        // Unused bits.
        //

        ULONG Unused:29;
    };
    LONG AsLong;
    ULONG AsULong;
} PERFECT_HASH_TABLE_STATE;
C_ASSERT(sizeof(PERFECT_HASH_TABLE_STATE) == sizeof(ULONG));
typedef PERFECT_HASH_TABLE_STATE *PPERFECT_HASH_TABLE_STATE;

#define IsValidTable(Table) ((Table)->State.Valid == TRUE)
#define IsTableCreateOnly(Table) ((Table)->TableCreateFlags.CreateOnly == TRUE)

#define WasTableInfoOnDiskHeapAllocated(Table) \
    ((Table)->State.TableInfoOnDiskWasHeapAllocated == TRUE)

#define WasTableDataHeapAllocated(Table) \
    ((Table)->State.TableDataWasHeapAllocated == TRUE)

#define IncludeNumberOfTableResizeEventsInOutputPath(Table) (                  \
    ((Table)->TableCreateFlags.IncludeNumberOfTableResizeEventsInOutputPath == \
     TRUE)                                                                     \
)

#define IncludeNumberOfTableElementsInOutputPath(Table) (                  \
    ((Table)->TableCreateFlags.IncludeNumberOfTableElementsInOutputPath == \
     TRUE)                                                                 \
)

#define TableResizeRequiresRename(Table) (                 \
    IncludeNumberOfTableResizeEventsInOutputPath(Table) || \
    IncludeNumberOfTableElementsInOutputPath(Table)        \
)

//
// Define the PERFECT_HASH_TABLE structure.
//

typedef struct _Struct_size_bytes_(SizeOfStruct) _PERFECT_HASH_TABLE {

    COMMON_COMPONENT_HEADER(PERFECT_HASH_TABLE);

    //
    // Capture any flags provided during table creation, loading and
    // compilation.
    //

    PERFECT_HASH_TABLE_CREATE_FLAGS TableCreateFlags;
    PERFECT_HASH_TABLE_LOAD_FLAGS TableLoadFlags;
    PERFECT_HASH_TABLE_COMPILE_FLAGS TableCompileFlags;

    //
    // Optional table creation parameters specified to Create().
    //

    ULONG NumberOfTableCreateParameters;
    PPERFECT_HASH_TABLE_CREATE_PARAMETER TableCreateParameters;

    //
    // Pointer to the active on-disk structure describing the table.  This may
    // be backed by stack-allocated memory (during creation), heap-allocated
    // memory (after creation), or memory mapped memory (if loaded from disk).
    //

    struct _TABLE_INFO_ON_DISK *TableInfoOnDisk;

    //
    // Optional pointer to a string representation of the Index() routine's
    // implementation in C, if one is available.
    //

    PCSTRING IndexImplString;

    //
    // If a table is loaded or created successfully, an array will be allocated
    // for storing values (as part of the Insert()/Lookup() API), the base
    // address for which is captured by the next field.
    //

    union {
        PVOID ValuesBaseAddress;
        PULONG Values;
    };

    //
    // Pointer to the base address of the table data.  During creation, this
    // is referred to as the "Assigned" array.  During the load phase, it is
    // referred to as "table data".
    //

    union {
        PULONG Assigned;
        PULONG TableData;
        PVOID TableDataBaseAddress;
    };

    //
    // Capture the number of elements in the underlying perfect hash table.
    // This refers to the number of vertices for the CHM algorithm, or can
    // mean the rounded-up power-of-2 size.  The masking implementations need
    // an agnostic way to access this value, which is why it is provided here
    // at the table level (despite being obtainable from things like the number
    // of vertices or Keys->NumberOfElements).
    //

    ULONG HashSize;
    ULONG IndexSize;

    //
    // Similarly, provide a convenient way to access the table "shift" amount
    // if a shifting mask routine is active.  This value is the number of
    // trailing zeros of the Size above for tables whose size is a power of 2.
    // It is not used if modulus masking is active.
    //

    ULONG HashShift;
    ULONG IndexShift;

    //
    // Mask.
    //

    ULONG HashMask;
    ULONG IndexMask;

    //
    // The following value represents how many times we need to XOR the high
    // part of a word with the low part of a word -- where word is being used
    // in the general computer word sense (i.e. not a 2-byte short) -- such
    // that the final value is within the bounds of the table mask.  It is
    // also the value of log2(Table->Shift).
    //

    ULONG HashFold;
    ULONG IndexFold;

    //
    // If modulus masking is active, this represents the modulus that will be
    // used for masking, e.g. Input %= Table->Modulus.
    //

    ULONG HashModulus;
    ULONG IndexModulus;

    //
    // If a resize event is triggered, this field will capture the new number
    // of vertices to use in search of a perfect hash table solution.  (This
    // will always be at least equal to or greater than the number of keys.)
    //

    ULARGE_INTEGER RequestedNumberOfTableElements;

    //
    // The algorithm in use.
    //

    PERFECT_HASH_ALGORITHM_ID AlgorithmId;

    //
    // The hash function in use.
    //

    PERFECT_HASH_HASH_FUNCTION_ID HashFunctionId;

    //
    // The masking type in use.
    //

    PERFECT_HASH_MASK_FUNCTION_ID MaskFunctionId;

    ULONG Padding3;

    //
    // Pointer to the path for the output directory (below).
    //

    PPERFECT_HASH_PATH OutputPath;

    //
    // Pointer to the output directory for this table.
    //

    PPERFECT_HASH_DIRECTORY OutputDirectory;

    //
    // Pointer to the keys corresponding to this perfect hash table.  May be
    // NULL.
    //

    PPERFECT_HASH_KEYS Keys;

    //
    // Pointer to the PERFECT_HASH_CONTEXT structure in use.
    //

    PPERFECT_HASH_CONTEXT Context;

    //
    // Pointers to files associated with the table.
    //

#define EXPAND_AS_FIRST_FILE(Verb, VUpper, Name, Upper) \
    union {                                             \
        PPERFECT_HASH_FILE Name;                        \
        PPERFECT_HASH_FILE FirstFile;                   \
    };

#define EXPAND_AS_LAST_FILE(Verb, VUpper, Name, Upper) \
    union {                                            \
        PPERFECT_HASH_FILE Name;                       \
        PPERFECT_HASH_FILE LastFile;                   \
    };

#define EXPAND_AS_FILE(Verb, VUpper, Name, Upper) PPERFECT_HASH_FILE Name;

    FILE_WORK_TABLE(EXPAND_AS_FIRST_FILE,
                    EXPAND_AS_FILE,
                    EXPAND_AS_LAST_FILE)

    //
    // Backing vtbl.
    //

    PERFECT_HASH_TABLE_VTBL Interface;

    //PVOID Padding4;

} PERFECT_HASH_TABLE;
typedef PERFECT_HASH_TABLE *PPERFECT_HASH_TABLE;

#define TryAcquirePerfectHashTableLockExclusive(Table) \
    TryAcquireSRWLockExclusive(&Table->Lock)

#define AcquirePerfectHashTableLockExclusive(Table) \
    AcquireSRWLockExclusive(&Table->Lock)

#define ReleasePerfectHashTableLockExclusive(Table) \
    ReleaseSRWLockExclusive(&Table->Lock)

#define TryAcquirePerfectHashTableLockShared(Table) \
    TryAcquireSRWLockShared(&Table->Lock)

#define AcquirePerfectHashTableLockShared(Table) \
    AcquireSRWLockShared(&Table->Lock)

#define ReleasePerfectHashTableLockShared(Table) \
    ReleaseSRWLockShared(&Table->Lock)

#define PerfectHashTableName(Table) \
    &Table->TableFile->Path->BaseNameA

//
// Internal method typedefs.
//

typedef
HRESULT
(NTAPI PERFECT_HASH_TABLE_INITIALIZE)(
    _In_ PPERFECT_HASH_TABLE Table
    );
typedef PERFECT_HASH_TABLE_INITIALIZE *PPERFECT_HASH_TABLE_INITIALIZE;

typedef
_Must_inspect_result_
_Success_(return >= 0)
HRESULT
(NTAPI PERFECT_HASH_TABLE_INITIALIZE_TABLE_SUFFIX)(
    _In_ PPERFECT_HASH_TABLE Table,
    _In_ PUNICODE_STRING Suffix,
    _In_opt_ PULONG NumberOfResizeEvents,
    _In_opt_ PULARGE_INTEGER NumberOfTableElements,
    _In_opt_ PERFECT_HASH_ALGORITHM_ID AlgorithmId,
    _In_opt_ PERFECT_HASH_HASH_FUNCTION_ID HashFunctionId,
    _In_opt_ PERFECT_HASH_MASK_FUNCTION_ID MaskFunctionId,
    _In_opt_ PCUNICODE_STRING AdditionalSuffix,
    _Out_ PUSHORT AlgorithmOffset
    );
typedef PERFECT_HASH_TABLE_INITIALIZE_TABLE_SUFFIX
      *PPERFECT_HASH_TABLE_INITIALIZE_TABLE_SUFFIX;

typedef
_Must_inspect_result_
_Success_(return >= 0)
HRESULT
(NTAPI PERFECT_HASH_TABLE_CREATE_PATH)(
    _In_ PPERFECT_HASH_TABLE Table,
    _In_ PPERFECT_HASH_PATH ExistingPath,
    _In_opt_ PULONG NumberOfResizeEvents,
    _In_opt_ PULARGE_INTEGER NumberOfTableElements,
    _In_opt_ PERFECT_HASH_ALGORITHM_ID AlgorithmId,
    _In_opt_ PERFECT_HASH_HASH_FUNCTION_ID HashFunctionId,
    _In_opt_ PERFECT_HASH_MASK_FUNCTION_ID MaskFunctionId,
    _In_opt_ PCUNICODE_STRING NewDirectory,
    _In_opt_ PCUNICODE_STRING NewBaseName,
    _In_opt_ PCUNICODE_STRING AdditionalSuffix,
    _In_opt_ PCUNICODE_STRING NewExtension,
    _In_opt_ PCUNICODE_STRING NewStreamName,
    _Inout_opt_ PPERFECT_HASH_PATH *Path,
    _Inout_opt_ PPERFECT_HASH_PATH_PARTS *Parts
    );
typedef PERFECT_HASH_TABLE_CREATE_PATH *PPERFECT_HASH_TABLE_CREATE_PATH;

typedef
_Must_inspect_result_
_Success_(return >= 0)
_Requires_exclusive_lock_held_(Table->Lock)
HRESULT
(NTAPI PERFECT_HASH_TABLE_CREATE_VALUES_ARRAY)(
    _In_ PPERFECT_HASH_TABLE Table,
    _In_opt_ ULONG ValueSizeInBytes
    );
typedef PERFECT_HASH_TABLE_CREATE_VALUES_ARRAY
      *PPERFECT_HASH_TABLE_CREATE_VALUES_ARRAY;

typedef
VOID
(NTAPI PERFECT_HASH_TABLE_RUNDOWN)(
    _In_ _Post_ptr_invalid_ PPERFECT_HASH_TABLE Table
    );
typedef PERFECT_HASH_TABLE_RUNDOWN *PPERFECT_HASH_TABLE_RUNDOWN;

//
// Function decls.
//

extern PERFECT_HASH_TABLE_INITIALIZE PerfectHashTableInitialize;
extern PERFECT_HASH_TABLE_INITIALIZE_TABLE_SUFFIX
    PerfectHashTableInitializeTableSuffix;
extern PERFECT_HASH_TABLE_CREATE_PATH PerfectHashTableCreatePath;
extern PERFECT_HASH_TABLE_CREATE_VALUES_ARRAY
    PerfectHashTableCreateValuesArray;
extern PERFECT_HASH_TABLE_RUNDOWN PerfectHashTableRundown;
extern PERFECT_HASH_TABLE_CREATE PerfectHashTableCreate;
extern PERFECT_HASH_TABLE_LOAD PerfectHashTableLoad;
extern PERFECT_HASH_TABLE_GET_FLAGS PerfectHashTableGetFlags;
extern PERFECT_HASH_TABLE_COMPILE PerfectHashTableCompile;
extern PERFECT_HASH_TABLE_TEST PerfectHashTableTest;
extern PERFECT_HASH_TABLE_INSERT PerfectHashTableInsert;
extern PERFECT_HASH_TABLE_LOOKUP PerfectHashTableLookup;
extern PERFECT_HASH_TABLE_DELETE PerfectHashTableDelete;
extern PERFECT_HASH_TABLE_INDEX PerfectHashTableIndex;
extern PERFECT_HASH_TABLE_GET_ALGORITHM_NAME
    PerfectHashTableGetAlgorithmName;
extern PERFECT_HASH_TABLE_GET_HASH_FUNCTION_NAME
    PerfectHashTableGetHashFunctionName;
extern PERFECT_HASH_TABLE_GET_MASK_FUNCTION_NAME
    PerfectHashTableGetMaskFunctionName;
extern PERFECT_HASH_TABLE_GET_FILE PerfectHashTableGetFile;

//
// Add some helper macros that improve the aesthetics of using the index,
// hash and mask COM-style routines.  All macros assume a Table variable is
// in scope, as well as an Error: label that can be jumped to if the method
// fails.
//

#define INDEX(Key, Result)                                \
    if (FAILED(Table->Vtbl->Index(Table, Key, Result))) { \
        goto Error;                                       \
    }

#define HASH(Key, Result)                                \
    if (FAILED(Table->Vtbl->Hash(Table, Key, Result))) { \
        goto Error;                                      \
    }

#define MASK_HASH(Hash, Result)                               \
    if (FAILED(Table->Vtbl->MaskHash(Table, Hash, Result))) { \
        goto Error;                                           \
    }

#define MASK_INDEX(Hash, Result)                               \
    if (FAILED(Table->Vtbl->MaskIndex(Table, Hash, Result))) { \
        goto Error;                                            \
    }


PERFECT_HASH_TABLE_HASH PerfectHashTableHashCrc32Rotate;
PERFECT_HASH_TABLE_HASH PerfectHashTableHashJenkins;
PERFECT_HASH_TABLE_HASH PerfectHashTableHashRotateXor;
PERFECT_HASH_TABLE_HASH PerfectHashTableHashAddSubXor;
PERFECT_HASH_TABLE_HASH PerfectHashTableHashXor;
PERFECT_HASH_TABLE_HASH PerfectHashTableHashCrc32RotateXor;
PERFECT_HASH_TABLE_HASH PerfectHashTableHashScratch;
PERFECT_HASH_TABLE_HASH PerfectHashTableHashCrc32;
PERFECT_HASH_TABLE_HASH PerfectHashTableHashDjb;
PERFECT_HASH_TABLE_HASH PerfectHashTableHashDjbXor;
PERFECT_HASH_TABLE_HASH PerfectHashTableHashFnv;
PERFECT_HASH_TABLE_HASH PerfectHashTableHashCrc32Not;

PERFECT_HASH_TABLE_SEEDED_HASH PerfectHashTableSeededHashCrc32Rotate;
PERFECT_HASH_TABLE_SEEDED_HASH PerfectHashTableSeededHashJenkins;
PERFECT_HASH_TABLE_SEEDED_HASH PerfectHashTableSeededHashRotateXor;
PERFECT_HASH_TABLE_SEEDED_HASH PerfectHashTableSeededHashAddSubXor;
PERFECT_HASH_TABLE_SEEDED_HASH PerfectHashTableSeededHashXor;
PERFECT_HASH_TABLE_SEEDED_HASH PerfectHashTableSeededHashCrc32RotateXor;
PERFECT_HASH_TABLE_SEEDED_HASH PerfectHashTableSeededHashScratch;
PERFECT_HASH_TABLE_SEEDED_HASH PerfectHashTableSeededHashCrc32;
PERFECT_HASH_TABLE_SEEDED_HASH PerfectHashTableSeededHashDjb;
PERFECT_HASH_TABLE_SEEDED_HASH PerfectHashTableSeededHashDjbXor;
PERFECT_HASH_TABLE_SEEDED_HASH PerfectHashTableSeededHashFnv;
PERFECT_HASH_TABLE_SEEDED_HASH PerfectHashTableSeededHashCrc32Not;

PERFECT_HASH_TABLE_MASK_HASH PerfectHashTableMaskHashModulus;
PERFECT_HASH_TABLE_MASK_HASH PerfectHashTableMaskHashAnd;
PERFECT_HASH_TABLE_MASK_HASH PerfectHashTableMaskHashXorAnd;
PERFECT_HASH_TABLE_MASK_HASH PerfectHashTableMaskHashFoldOnce;
PERFECT_HASH_TABLE_MASK_HASH PerfectHashTableMaskHashFoldTwice;
PERFECT_HASH_TABLE_MASK_HASH PerfectHashTableMaskHashFoldThrice;

PERFECT_HASH_TABLE_MASK_INDEX PerfectHashTableMaskIndexModulus;
PERFECT_HASH_TABLE_MASK_INDEX PerfectHashTableMaskIndexAnd;
PERFECT_HASH_TABLE_MASK_INDEX PerfectHashTableMaskIndexXorAnd;
PERFECT_HASH_TABLE_MASK_INDEX PerfectHashTableMaskIndexFoldOnce;
PERFECT_HASH_TABLE_MASK_INDEX PerfectHashTableMaskIndexFoldTwice;
PERFECT_HASH_TABLE_MASK_INDEX PerfectHashTableMaskIndexFoldThrice;

//
// Helper method for initializing a table suffix from a given algorithm, mask
// and hash function.
//


// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :
