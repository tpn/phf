/*++

Copyright (c) 2018 Trent Nelson <trent@trent.me>

Module Name:

    CompiledPerfectHashTable.h

Abstract:

    This is the main public header file for the compiled perfect hash table
    library.  It defines structures and functions related to loading and using
    compiled perfect hash tables.

--*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <sal.h>

//
// Define basic NT types and macros used by this header file.
//

#define CPHAPI __stdcall

typedef char BOOLEAN;
typedef unsigned long ULONG;
typedef unsigned long *PULONG;
typedef unsigned long long ULONGLONG;

//
// Disable the anonymous union/struct warning.
//

#pragma warning(push)
#pragma warning(disable: 4201)

typedef union _ULARGE_INTEGER {
    struct {
        ULONG LowPart;
        ULONG HighPart;
    };
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

#pragma warning(pop)

//
// Define the main functions exposed by a compiled perfect hash table: index,
// lookup, insert and delete.
//

typedef
ULONG
(CPHAPI COMPILED_PERFECT_HASH_TABLE_INDEX)(
    _In_ ULONG Key
    );
/*++

Routine Description:

    Looks up given key in a compiled perfect hash table and returns its index.

    N.B. If the given key did not appear in the original set the hash table was
         created from, the behavior of this routine is undefined.  (In practice, the
         key will hash to either an existing key's location or an empty slot,
         so there is potential for returning a non-unique index.)

Arguments:

    Key - Supplies the key to look up.

Return Value:

    The index associated with the given key.

--*/
typedef COMPILED_PERFECT_HASH_TABLE_INDEX *PCOMPILED_PERFECT_HASH_TABLE_INDEX;


typedef
ULONG
(CPHAPI COMPILED_PERFECT_HASH_TABLE_LOOKUP)(
    _In_ ULONG Key
    );
/*++

Routine Description:

    Looks up given key in a compiled perfect hash table and returns the value
    present.  If no insertion has taken place for this key, this routine
    guarantees to return 0 as the value.

    N.B. If the given key did not appear in the original set the hash table was
         created from, the behavior of this routine is undefined.  (In practice, the
         value returned will be the value for some other key in the table that
         hashes to the same location -- or potentially an empty slot in the
         table.)

Arguments:

    Key - Supplies the key to look up.

Return Value:

    The value at the given location.

--*/
typedef COMPILED_PERFECT_HASH_TABLE_LOOKUP *PCOMPILED_PERFECT_HASH_TABLE_LOOKUP;


typedef
ULONG
(CPHAPI COMPILED_PERFECT_HASH_TABLE_INSERT)(
    _In_ ULONG Key,
    _In_ ULONG Value
    );
/*++

Routine Description:

    Inserts value at key into a compiled hash table, and returns the previously
    set value (which will be 0 if no prior insert occurred).

    N.B. If the given key did not appear in the original set the hash table was
         created from, the behavior of this routine is undefined.  (In practice, the
         key will hash to either an existing key's location or an empty slot, so
         there is potential to corrupt the table in the sense that previously
         inserted values will be trampled over.)

Arguments:

    Key - Supplies the key for which the value will be inserted.

    Value - Supplies the value to insert.

Return Value:

    Previous value at the relevant table location prior to this insertion.

--*/
typedef COMPILED_PERFECT_HASH_TABLE_INSERT *PCOMPILED_PERFECT_HASH_TABLE_INSERT;


typedef
ULONG
(CPHAPI COMPILED_PERFECT_HASH_TABLE_DELETE)(
    _In_ ULONG Key
    );
/*++

Routine Description:

    Deletes a key from a perfect hash table, optionally returning the value
    prior to deletion back to the caller.  Deletion simply clears the value
    associated with the key, and thus, is a simple O(1) operation.  Deleting
    a key that has not yet been inserted has no effect other than potentially
    returning 0 as the previous value.  That is, a caller can safely issue
    deletes of keys regardless of whether or not said keys were inserted first.

    N.B. If the given key did not appear in the original set the hash table was
         created from, the behavior of this routine is undefined.  (In practice, the
         key will hash to either an existing key's location or an empty slot, so
         there is potential to corrupt the table in the sense that a previously
         inserted value for an unrelated, valid key will be cleared.)

Arguments:

    Key - Supplies the key to delete.

Return Value:

    Previous value at the given key's location prior to deletion.  If no prior
    insertion, the previous value is guaranteed to be 0.

--*/
typedef COMPILED_PERFECT_HASH_TABLE_DELETE
      *PCOMPILED_PERFECT_HASH_TABLE_DELETE;

//
// Define the vtbl structure encapsulating the compiled perfect hash table's
// function pointers.
//

typedef struct _COMPILED_PERFECT_HASH_TABLE_VTBL {
    COMPILED_PERFECT_HASH_TABLE_INDEX  Index;
    COMPILED_PERFECT_HASH_TABLE_LOOKUP Lookup;
    COMPILED_PERFECT_HASH_TABLE_INSERT Insert;
    COMPILED_PERFECT_HASH_TABLE_DELETE Delete;
} COMPILED_PERFECT_HASH_TABLE_VTBL;
typedef COMPILED_PERFECT_HASH_TABLE_VTBL *PCOMPILED_PERFECT_HASH_TABLE_VTBL;

typedef struct _Struct_size_bytes_(SizeOfStruct) _COMPILED_PERFECT_HASH_TABLE {

    COMPILED_PERFECT_HASH_TABLE_VTBL Vtbl;

    //
    // Size of the structure, in bytes.
    //

    _Field_range_(==, sizeof(struct _COMPILED_PERFECT_HASH_TABLE))
        ULONG SizeOfStruct;

    //
    // Size of an individual key element, in bytes.
    //

    ULONG KeySizeInBytes;

    //
    // Algorithm that was used.
    //

    ULONG AlgorithmId;

    //
    // Hash function that was used.
    //

    ULONG HashFunctionId;

    //
    // Masking type.
    //

    ULONG MaskFunctionId;

    //
    // Padding.
    //

    ULONG Padding;

    //
    // Number of keys in the input set.  This is used to size an appropriate
    // array for storing values.
    //

    ULARGE_INTEGER NumberOfKeys;

    //
    // Final number of elements in the underlying table.  This will vary
    // depending on how the graph was created.  If modulus masking is in use,
    // this will reflect the number of keys (unless a custom table size was
    // requested during creation).  Otherwise, this will be the number of keys
    // rounded up to the next power of 2.  (That is, take the number of keys,
    // round up to a power of 2, then round that up to the next power of 2.)
    //

    ULARGE_INTEGER NumberOfTableElements;

    //
    // Hash and index sizes and masks.
    //

    ULONG HashSize;
    ULONG IndexSize;

    ULONG HashMask;
    ULONG IndexMask;

    //
    // Base addresses of the table data and values array.
    //

    union {
        PVOID DataBaseAddress;
        PULONG Data;
    };

    union {
        PVOID ValuesBaseAddress;
        PULONG Values;
    };

    //
    // Seed data.
    //

    ULONG NumberOfSeeds;

    union {
        ULONG Seed1;
        ULONG FirstSeed;
    };

    ULONG Seed2;
    ULONG Seed3;

    union {
        ULONG Seed4;
        ULONG LastSeed;
    };

} COMPILED_PERFECT_HASH_TABLE;
typedef COMPILED_PERFECT_HASH_TABLE *PCOMPILED_PERFECT_HASH_TABLE;

typedef
_Success_(return != 0)
BOOLEAN
(CPHAPI GET_COMPILED_PERFECT_HASH_TABLE)(
    _In_ _Field_range_(==, sizeof(Table)) ULONG SizeOfTable,
    _Out_writes_bytes_(SizeOfTable) PCOMPILED_PERFECT_HASH_TABLE Table
    );
/*++

Routine Description:

    Obtains a compiled perfect hash table structure for a given module.

Arguments:

    SizeOfTable - Supplies the size, in bytes, of the Table structure.

    Table - Supplies a pointer to a COMPILED_PERFECT_HASH_TABLE structure for
        which the table instance will be copied.

Return Value:

    TRUE on success, FALSE on failure.  The only possible way for this routine
    to fail is if SizeOfTable isn't correct.

--*/
typedef GET_COMPILED_PERFECT_HASH_TABLE *PGET_COMPILED_PERFECT_HASH_TABLE;

#ifdef __cplusplus
} // extern "C"
#endif

// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :
