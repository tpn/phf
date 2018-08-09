/*++

Copyright (c) 2018 Trent Nelson <trent@trent.me>

Module Name:

    PerfectHashTableKeys.h

Abstract:

    This is the private header file for the PERFECT_HASH_TABLE_KEYS
    component of the perfect hash table library.  It defines the structure,
    and function pointer typedefs for the initialize and rundown functions.

--*/

#pragma once

#include "stdafx.h"

//
// Define the PERFECT_HASH_TABLE_KEYS_FLAGS structure.
//

typedef union _PERFECT_HASH_TABLE_KEYS_STATE {
    struct _Struct_size_bytes_(sizeof(ULONG)) {

        //
        // When set, indicates a keys file has been loaded successfully.
        //

        ULONG Loaded:1;

        //
        // Unused bits.
        //

        ULONG Unused:31;
    };

    LONG AsLong;
    ULONG AsULong;
} PERFECT_HASH_TABLE_KEYS_STATE;
C_ASSERT(sizeof(PERFECT_HASH_TABLE_KEYS_STATE) == sizeof(ULONG));
typedef PERFECT_HASH_TABLE_KEYS_STATE *PPERFECT_HASH_TABLE_KEYS_STATE;

typedef union _PERFECT_HASH_TABLE_KEYS_FLAGS {
    struct _Struct_size_bytes_(sizeof(ULONG)) {

        //
        // When set, indicates the keys were mapped using large pages.
        //

        ULONG MappedWithLargePages:1;

        //
        // Unused bits.
        //

        ULONG Unused:31;
    };

    LONG AsLong;
    ULONG AsULong;
} PERFECT_HASH_TABLE_KEYS_FLAGS;
C_ASSERT(sizeof(PERFECT_HASH_TABLE_KEYS_FLAGS) == sizeof(ULONG));
typedef PERFECT_HASH_TABLE_KEYS_FLAGS *PPERFECT_HASH_TABLE_KEYS_FLAGS;

//
// Define the PERFECT_HASH_TABLE_KEYS structure.
//

typedef struct _Struct_size_bytes_(SizeOfStruct) _PERFECT_HASH_TABLE_KEYS {

    COMMON_COMPONENT_HEADER(PERFECT_HASH_TABLE_KEYS);

    //
    // Slim read/write lock guarding the structure.
    //

    SRWLOCK Lock;

    //
    // Pointer to an initialized RTL structure.
    //

    PRTL Rtl;

    //
    // Pointer to an initialized ALLOCATOR structure.
    //

    PALLOCATOR Allocator;

    //
    // Number of keys in the mapping.
    //

    ULARGE_INTEGER NumberOfElements;

    //
    // Handle to the underlying keys file.
    //

    HANDLE FileHandle;

    //
    // Handle to the memory mapping for the keys file.
    //

    HANDLE MappingHandle;

    //
    // Base address of the memory map.
    //

    union {
        PVOID BaseAddress;
        PULONG Keys;
    };

    //
    // Fully-qualified, NULL-terminated path of the source keys file.
    //

    UNICODE_STRING Path;

    //
    // Backing interface.
    //

    PERFECT_HASH_TABLE_KEYS_VTBL Interface;

} PERFECT_HASH_TABLE_KEYS;
typedef PERFECT_HASH_TABLE_KEYS *PPERFECT_HASH_TABLE_KEYS;

#define TryAcquirePerfectHashTableKeysLockExclusive(Keys) \
    TryAcquireSRWLockExclusive(&Keys->Lock)

#define ReleasePerfectHashTableKeysLockExclusive(Keys) \
    ReleaseSRWLockExclusive(&Keys->Lock)

typedef
HRESULT
(NTAPI PERFECT_HASH_TABLE_KEYS_INITIALIZE)(
    _In_ PPERFECT_HASH_TABLE_KEYS Keys
    );
typedef PERFECT_HASH_TABLE_KEYS_INITIALIZE
      *PPERFECT_HASH_TABLE_KEYS_INITIALIZE;

typedef
VOID
(NTAPI PERFECT_HASH_TABLE_KEYS_RUNDOWN)(
    _In_ _Post_ptr_invalid_ PPERFECT_HASH_TABLE_KEYS Keys
    );
typedef PERFECT_HASH_TABLE_KEYS_RUNDOWN
      *PPERFECT_HASH_TABLE_KEYS_RUNDOWN;

typedef
HRESULT
(NTAPI PERFECT_HASH_TABLE_KEYS_VERIFY_UNIQUE)(
    _In_ PPERFECT_HASH_TABLE_KEYS Keys
    );
typedef PERFECT_HASH_TABLE_KEYS_VERIFY_UNIQUE
      *PPERFECT_HASH_TABLE_KEYS_VERIFY_UNIQUE;

extern PERFECT_HASH_TABLE_KEYS_INITIALIZE PerfectHashTableKeysInitialize;
extern PERFECT_HASH_TABLE_KEYS_RUNDOWN PerfectHashTableKeysRundown;
extern PERFECT_HASH_TABLE_KEYS_VERIFY_UNIQUE PerfectHashTableKeysVerifyUnique;

// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :