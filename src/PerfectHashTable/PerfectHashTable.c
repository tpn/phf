/*++

Copyright (c) 2018 Trent Nelson <trent@trent.me>

Module Name:

    PerfectHashTable.c

Abstract:

    This module implements the initialization and rundown routines for the
    PERFECT_HASH_TABLE instance.

--*/

#include "stdafx.h"

PERFECT_HASH_TABLE_INITIALIZE PerfectHashTableInitialize;

_Use_decl_annotations_
HRESULT
PerfectHashTableInitialize(
    PPERFECT_HASH_TABLE Table
    )
/*++

Routine Description:

    Initializes a perfect hash table.  This is a relatively simple method that
    just primes the COM scaffolding; the bulk of the work is done when either
    creating a new table (PerfectHashTableContext->Vtbl->CreateTable) or when
    loading an existing table (PerfectHashTable->Vtbl->Load).

Arguments:

    Table - Supplies a pointer to a PERFECT_HASH_TABLE structure for which
        initialization is to be performed.

Return Value:

    S_OK on success.  E_POINTER if Table is NULL.

--*/
{
    if (!ARGUMENT_PRESENT(Table)) {
        return E_POINTER;
    }

    Table->SizeOfStruct = sizeof(*Table);
    Table->Flags.AsULong = 0;
    Table->State.AsULong = 0;

    return S_OK;
}

PERFECT_HASH_TABLE_RUNDOWN PerfectHashTableRundown;

_Use_decl_annotations_
VOID
PerfectHashTableRundown(
    PPERFECT_HASH_TABLE Table
    )
/*++

Routine Description:

    Release all resources associated with a perfect hash table.

Arguments:

    Table - Supplies a pointer to a PERFECT_HASH_TABLE structure for which
        rundown is to be performed.

Return Value:

    None.

--*/
{
    //
    // Validate arguments.
    //

    if (!ARGUMENT_PRESENT(Table)) {
        return;
    }

    //
    // Sanity check structure size.
    //

    ASSERT(Table->SizeOfStruct == sizeof(*Table));

    //
    // Close resources associated with the :Info stream.
    //

    if (Table->InfoStreamBaseAddress) {
        if (!UnmapViewOfFile(Table->InfoStreamBaseAddress)) {
            SYS_ERROR(UnmapViewOfFile);
        }
        Table->InfoStreamBaseAddress = NULL;
    }

    if (Table->InfoStreamMappingHandle) {
        if (!CloseHandle(Table->InfoStreamMappingHandle)) {
            SYS_ERROR(CloseHandle);
        }
        Table->InfoStreamMappingHandle = NULL;
    }

    if (Table->InfoStreamFileHandle) {
        if (!CloseHandle(Table->InfoStreamFileHandle)) {
            SYS_ERROR(CloseHandle);
        }
        Table->InfoStreamFileHandle = NULL;
    }

    //
    // Clean up any resources that are still active.
    //

    if (Table->BaseAddress) {
        if (!UnmapViewOfFile(Table->BaseAddress)) {
            SYS_ERROR(UnmapViewOfFile);
        }
        Table->BaseAddress = NULL;
    }

    if (Table->MappingHandle) {
        if (!CloseHandle(Table->MappingHandle)) {
            SYS_ERROR(CloseHandle);
        }
        Table->MappingHandle = NULL;
    }

    if (Table->FileHandle) {
        if (!CloseHandle(Table->FileHandle)) {
            SYS_ERROR(CloseHandle);
        }
        Table->FileHandle = NULL;
    }

    //
    // Free the buffer created as part of key validation, if applicable.
    //

    if (Table->KeysBitmap.Buffer) {
        if (!VirtualFree(Table->KeysBitmap.Buffer, 0, MEM_RELEASE)) {
            SYS_ERROR(VirtualFree);
        }
        Table->KeysBitmap.Buffer = NULL;
        Table->KeysBitmap.SizeOfBitMap = 0;
    }

    //
    // Free the memory used for the values array, if applicable.
    //

    if (Table->ValuesBaseAddress) {
        if (!VirtualFree(Table->ValuesBaseAddress, 0, MEM_RELEASE)) {
            SYS_ERROR(VirtualFree);
        }
        Table->ValuesBaseAddress = NULL;
    }

    //
    // Release COM references, if applicable.
    //

    if (Table->Context) {
        Table->Context->Vtbl->Release(Table->Context);
        Table->Context = NULL;
    }

    if (Table->Keys) {
        Table->Keys->Vtbl->Release(Table->Keys);
        Table->Keys = NULL;
    }

    if (Table->Allocator) {
        Table->Allocator->Vtbl->Release(Table->Allocator);
        Table->Allocator = NULL;
    }

    if (Table->Rtl) {
        Table->Rtl->Vtbl->Release(Table->Rtl);
        Table->Rtl = NULL;
    }
}

// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :
