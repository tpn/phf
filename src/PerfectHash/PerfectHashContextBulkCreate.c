/*++

Copyright (c) 2018 Trent Nelson <trent@trent.me>

Module Name:

    PerfectHashContextBulkCreate.c

Abstract:

    This module implements the bulk-create routine for the perfect hash library.

    N.B. This component is a work in progress.  It is based off the self-test
         component.

--*/

#include "stdafx.h"


#define PH_ERROR_EX(Name, Result, ...) \
    PH_ERROR(Name, Result)

#define PH_KEYS_ERROR(Name, Result) \
    PH_ERROR(Name, Result)

#define PH_TABLE_ERROR(Name, Result) \
    PH_ERROR(Name, Result)

#if 0
#define PH_ERROR_EX(Name, Result, ...)     \
    PerfectHashPrintErrorEx(#Name,         \
                            __FILE__,      \
                            __FUNCTION__,  \
                            __LINE__,      \
                            (ULONG)Result, \
                            __VA_ARGS__)
#endif

//
// Method implementations.
//

PERFECT_HASH_CONTEXT_BULK_CREATE PerfectHashContextBulkCreate;

_Use_decl_annotations_
HRESULT
PerfectHashContextBulkCreate(
    PPERFECT_HASH_CONTEXT Context,
    PCUNICODE_STRING KeysDirectory,
    PCUNICODE_STRING BaseOutputDirectory,
    PERFECT_HASH_ALGORITHM_ID AlgorithmId,
    PERFECT_HASH_HASH_FUNCTION_ID HashFunctionId,
    PERFECT_HASH_MASK_FUNCTION_ID MaskFunctionId,
    PPERFECT_HASH_CONTEXT_BULK_CREATE_FLAGS ContextBulkCreateFlagsPointer,
    PPERFECT_HASH_KEYS_LOAD_FLAGS KeysLoadFlagsPointer,
    PPERFECT_HASH_TABLE_CREATE_FLAGS TableCreateFlagsPointer,
    PPERFECT_HASH_TABLE_COMPILE_FLAGS TableCompileFlagsPointer,
    ULONG NumberOfTableCreateParameters,
    PPERFECT_HASH_TABLE_CREATE_PARAMETER TableCreateParameters
    )
/*++

Routine Description:

    Attempts to create perfect hash tables for all key files in a directory.

Arguments:

    Context - Supplies an instance of PERFECT_HASH_CONTEXT.

    KeysDirectory - Supplies a pointer to a UNICODE_STRING structure that
        represents a fully-qualified path of the keys directory.

    BaseOutputDirectory - Supplies a pointer to a UNICODE_STRING structure that
        represents a fully-qualified path of the directory where the perfect
        hash table files generated as part of this routine will be saved.

    AlgorithmId - Supplies the algorithm to use.

    MaskFunctionId - Supplies the type of masking to use.

    HashFunctionId - Supplies the hash function to use.

    ContextBulkCreateFlags - Optionally supplies a pointer to a bulk-create flags
        structure that can be used to customize bulk-create behavior.

    KeysLoadFlags - Optionally supplies a pointer to a key loading flags
        structure that can be used to customize key loading behavior.

    TableCreateFlags - Optionally supplies a pointer to a table create flags
        structure that can be used to customize table creation behavior.

    TableCompileFlags - Optionally supplies a pointer to a compile table flags
        structure that can be used to customize table compilation behavior.

    NumberOfTableCreateParameters - Optionally supplies the number of elements
        in the TableCreateParameters array.

    TableCreateParameters - Optionally supplies an array of additional
        parameters that can be used to further customize table creation
        behavior.

Return Value:

    S_OK - Success.

    The following error codes may also be returned.  Note that this list is not
    guaranteed to be exhaustive; that is, error codes other than the ones listed
    below may also be returned.

    E_POINTER - One or more mandatory parameters were NULL pointers.

    E_INVALIDARG - KeysDirectory or BaseOutputDirectory were invalid.

    PH_E_INVALID_ALGORITHM_ID - Invalid algorithm ID.

    PH_E_INVALID_HASH_FUNCTION_ID - Invalid hash function ID.

    PH_E_INVALID_MASK_FUNCTION_ID - Invalid mask function ID.

    PH_E_INVALID_MAXIMUM_CONCURRENCY - Invalid maximum concurrency.

    PH_E_INVALID_KEYS_LOAD_FLAGS - Invalid keys load flags.

    PH_E_INVALID_TABLE_CREATE_FLAGS - Invalid table create flags.

    PH_E_INVALID_TABLE_LOAD_FLAGS - Invalid table load flags.

    PH_E_INVALID_TABLE_COMPILE_FLAGS - Invalid table compile flags.

    PH_E_INVALID_CONTEXT_BULK_CREATE_FLAGS - Invalid context bulk create flags.

    PH_E_NO_KEYS_FOUND_IN_DIRECTORY - No keys found in directory.

--*/
{
    PRTL Rtl;
    PWSTR Dest;
    PWSTR Source;
    ULONG LastError;
    USHORT Length;
    USHORT BaseLength;
    USHORT NumberOfPages;
    ULONG Count = 0;
    ULONG ReferenceCount;
    BOOLEAN Failed;
    BOOLEAN Terminate;
    HRESULT Result;
    PCHAR Buffer;
    PCHAR BaseBuffer = NULL;
    PCHAR Output;
    PCHAR OutputBuffer = NULL;
    PALLOCATOR Allocator;
    PVOID KeysBaseAddress;
    ULARGE_INTEGER NumberOfKeys;
    HANDLE FindHandle = NULL;
    HANDLE OutputHandle = NULL;
    HANDLE ProcessHandle = NULL;
    ULONG Failures;
    ULONGLONG BufferSize;
    ULONGLONG OutputBufferSize;
    LONG_INTEGER AllocSize;
    ULONG BytesWritten = 0;
    WIN32_FIND_DATAW FindData;
    UNICODE_STRING WildcardPath;
    UNICODE_STRING KeysPathString;
    PPERFECT_HASH_KEYS Keys;
    PPERFECT_HASH_TABLE Table;
    PPERFECT_HASH_FILE TableFile = NULL;
    PPERFECT_HASH_PATH TablePath = NULL;
    PPERFECT_HASH_DIRECTORY BaseOutputDir = NULL;
    PERFECT_HASH_KEYS_FLAGS KeysFlags;
    PERFECT_HASH_KEYS_BITMAP KeysBitmap;
    PCUNICODE_STRING Suffix = &KeysWildcardSuffix;
    PERFECT_HASH_CONTEXT_BULK_CREATE_FLAGS ContextBulkCreateFlags;
    PERFECT_HASH_KEYS_LOAD_FLAGS KeysLoadFlags;
    PERFECT_HASH_TABLE_CREATE_FLAGS TableCreateFlags;
    PERFECT_HASH_TABLE_COMPILE_FLAGS TableCompileFlags;

    //
    // Validate arguments.
    //

    if (!ARGUMENT_PRESENT(Context)) {
        return E_POINTER;
    } else {
        Rtl = Context->Rtl;
        Allocator = Context->Allocator;
    }

    VALIDATE_FLAGS(ContextBulkCreate, CONTEXT_BULK_CREATE);
    VALIDATE_FLAGS(KeysLoad, KEYS_LOAD);
    VALIDATE_FLAGS(TableCreate, TABLE_CREATE);
    VALIDATE_FLAGS(TableCompile, TABLE_COMPILE);

    if (!ARGUMENT_PRESENT(KeysDirectory)) {
        return E_POINTER;
    } else if (!IsValidMinimumDirectoryUnicodeString(KeysDirectory)) {
        return E_INVALIDARG;
    }

    if (!ARGUMENT_PRESENT(BaseOutputDirectory)) {
        return E_POINTER;
    } else if (!IsValidMinimumDirectoryUnicodeString(BaseOutputDirectory)) {
        return E_INVALIDARG;
    }
    else {
        Result = Context->Vtbl->SetBaseOutputDirectory(Context,
                                                       BaseOutputDirectory);
        if (FAILED(Result)) {
            PH_ERROR(PerfectHashContextSetBaseOutputDirectory, Result);
            return Result;
        }

        Result = Context->Vtbl->GetBaseOutputDirectory(Context, &BaseOutputDir);
        if (FAILED(Result)) {
            PH_ERROR(PerfectHashContextGetBaseOutputDirectory, Result);
            return Result;
        }
        RELEASE(BaseOutputDir);
    }

    if (!IsValidPerfectHashAlgorithmId(AlgorithmId)) {
        return E_INVALIDARG;
    }

    if (!IsValidPerfectHashHashFunctionId(HashFunctionId)) {
        return E_INVALIDARG;
    }

    if (!IsValidPerfectHashMaskFunctionId(MaskFunctionId)) {
        return E_INVALIDARG;
    }

    //
    // Arguments have been validated, proceed.
    //

    SetContextBulkCreate(Context);

    //
    // Create a buffer we can use for stdout, using a very generous buffer size.
    //

    NumberOfPages = 10;
    ProcessHandle = GetCurrentProcess();

    Result = Rtl->Vtbl->CreateBuffer(Rtl,
                                     &ProcessHandle,
                                     NumberOfPages,
                                     NULL,
                                     &OutputBufferSize,
                                     &OutputBuffer);

    if (FAILED(Result)) {
        Result = E_OUTOFMEMORY;
        goto Error;
    }

    Output = OutputBuffer;

    //
    // Create a buffer we can use for temporary path construction.  We want it
    // to be MAX_USHORT in size, so (1 << 16) >> PAGE_SHIFT converts this into
    // the number of pages we need.
    //

    NumberOfPages = (1 << 16) >> PAGE_SHIFT;

    Result = Rtl->Vtbl->CreateBuffer(Rtl,
                                     &ProcessHandle,
                                     NumberOfPages,
                                     NULL,
                                     &BufferSize,
                                     &BaseBuffer);

    if (FAILED(Result)) {
        HRESULT Result2;
        SYS_ERROR(VirtualAlloc);
        Result2 = Rtl->Vtbl->DestroyBuffer(Rtl,
                                          ProcessHandle,
                                          &OutputBuffer);
        if (FAILED(Result2)) {
            SYS_ERROR(VirtualFree);
        }
        goto Error;
    }

    Buffer = BaseBuffer;

    //
    // Get a reference to the stdout handle.
    //

    if (!Context->OutputHandle) {
        Context->OutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        if (!Context->OutputHandle) {
            SYS_ERROR(GetStdHandle);
            goto Error;
        }
    }

    OutputHandle = Context->OutputHandle;

    //
    // Calculate the size required for a new concatenated wide string buffer
    // that combines the test data directory with the "*.keys" suffix.  The
    // 2 * sizeof(*Dest) accounts for the joining slash and trailing NULL.
    //

    AllocSize.LongPart = KeysDirectory->Length;
    AllocSize.LongPart += Suffix->Length + (2 * sizeof(*Dest));

    //
    // Sanity check we haven't overflowed.
    //

    if (AllocSize.HighPart) {
        Result = PH_E_STRING_BUFFER_OVERFLOW;
        PH_ERROR(PerfectHashContextBulkCreate_AllocSize, Result);
        goto Error;
    }

    WildcardPath.Buffer = (PWSTR)Buffer;

    if (!WildcardPath.Buffer) {
        goto Error;
    }

    //
    // Copy incoming keys directory name.
    //

    Length = KeysDirectory->Length;
    CopyInline(WildcardPath.Buffer,
               KeysDirectory->Buffer,
               Length);

    //
    // Advance our Dest pointer to the end of the directory name, write a
    // slash, then copy the suffix over.
    //

    Dest = (PWSTR)RtlOffsetToPointer(WildcardPath.Buffer, Length);
    *Dest++ = L'\\';
    CopyInline(Dest, Suffix->Buffer, Suffix->Length);

    //
    // Wire up the search path length and maximum length variables.  The max
    // length will be our AllocSize, length will be this value minus 2 to
    // account for the trailing NULL.
    //

    WildcardPath.MaximumLength = AllocSize.LowPart;
    WildcardPath.Length = AllocSize.LowPart - sizeof(*Dest);
    ASSERT(WildcardPath.Buffer[WildcardPath.Length] == L'\0');

    //
    // Advance the buffer past this string allocation, up to the next 16-byte
    // boundary.
    //

    Buffer = (PSTR)(
        RtlOffsetToPointer(
            Buffer,
            ALIGN_UP(WildcardPath.MaximumLength, 16)
        )
    );

    //
    // Create a find handle for the <keys dir>\*.keys search pattern we
    // created.
    //

    FindHandle = FindFirstFileW(WildcardPath.Buffer, &FindData);

    if (!IsValidHandle(FindHandle)) {

        //
        // Check to see if we failed because there were no files matching the
        // wildcard *.keys in the test directory.  In this case, GetLastError()
        // will report ERROR_FILE_NOT_FOUND.
        //

        FindHandle = NULL;
        LastError = GetLastError();

        if (LastError == ERROR_FILE_NOT_FOUND) {
            Result = PH_E_NO_KEYS_FOUND_IN_DIRECTORY;
            PH_MESSAGE(Result);
        } else {
            SYS_ERROR(FindFirstFileW);
            Result = PH_E_SYSTEM_CALL_FAILED;
        }

        goto Error;
    }

    //
    // Initialize the fully-qualified keys path.
    //

    KeysPathString.Buffer = (PWSTR)Buffer;
    CopyInline(KeysPathString.Buffer, KeysDirectory->Buffer, Length);

    //
    // Advance our Dest pointer to the end of the directory name, then write
    // a slash.
    //

    Dest = (PWSTR)RtlOffsetToPointer(KeysPathString.Buffer, Length);
    *Dest++ = L'\\';

    //
    // Update the length to account for the slash we just wrote, then make a
    // copy of it in the variable BaseLength.
    //

    Length += sizeof(*Dest);
    BaseLength = Length;

    //
    // Initialize local variables and then begin the main loop.
    //

    Failures = 0;
    Terminate = FALSE;
    ZeroStruct(KeysBitmap);

    Table = NULL;
    KeysBaseAddress = NULL;
    NumberOfKeys.QuadPart = 0;

    //
    // We're not using the tables after we create them, so toggle the relevant
    // table create flag explicitly.
    //

    TableCreateFlags.CreateOnly = TRUE;

    do {

        Count++;

        //
        // Clear the failure flag at the top of every loop invocation.
        //

        Failed = FALSE;

#if 0
        WIDE_OUTPUT_RAW(Output, L"Processing key file: ");
        WIDE_OUTPUT_WCSTR(Output, (PCWSZ)FindData.cFileName);
        WIDE_OUTPUT_LF(Output);
        WIDE_OUTPUT_FLUSH();
#endif

        //
        // Copy the filename over to the fully-qualified keys path.
        //

        Dest = (PWSTR)RtlOffsetToPointer(KeysPathString.Buffer, BaseLength);
        Source = (PWSTR)FindData.cFileName;

        while (*Source) {
            *Dest++ = *Source++;
        }
        *Dest = L'\0';

        Length = (USHORT)RtlPointerToOffset(KeysPathString.Buffer, Dest);
        KeysPathString.Length = Length;
        KeysPathString.MaximumLength = Length + sizeof(*Dest);
        ASSERT(KeysPathString.Buffer[KeysPathString.Length >> 1] == L'\0');
        ASSERT(&KeysPathString.Buffer[KeysPathString.Length >> 1] == Dest);

        Result = Context->Vtbl->CreateInstance(Context,
                                               NULL,
                                               &IID_PERFECT_HASH_KEYS,
                                               &Keys);

        if (FAILED(Result)) {
            PH_ERROR(PerfectHashKeysCreateInstance, Result);
            Failures++;
            break;
        }

        Result = Keys->Vtbl->Load(Keys,
                                  &KeysLoadFlags,
                                  &KeysPathString,
                                  sizeof(ULONG));

        if (FAILED(Result)) {
            PH_KEYS_ERROR(PerfectHashKeysLoad, Result);
            Failures++;
            Terminate = TRUE;
            goto ReleaseKeys;
        }

        Result = Keys->Vtbl->GetFlags(Keys,
                                      sizeof(KeysFlags),
                                      &KeysFlags);

        if (FAILED(Result)) {
            PH_KEYS_ERROR(PerfectHashKeysGetFlags, Result);
            Failures++;
            Terminate = TRUE;
            goto ReleaseKeys;
        }

        Result = Keys->Vtbl->GetAddress(Keys,
                                        &KeysBaseAddress,
                                        &NumberOfKeys);

        if (FAILED(Result)) {
            PH_KEYS_ERROR(PerfectHashKeysGetAddress, Result);
            Failures++;
            Terminate = TRUE;
            goto ReleaseKeys;
        }

        //
        // Keys were loaded successfully.  Proceed with table creation.
        //

        ASSERT(Table == NULL);

        Result = Context->Vtbl->CreateInstance(Context,
                                               NULL,
                                               &IID_PERFECT_HASH_TABLE,
                                               &Table);

        if (FAILED(Result)) {
            PH_ERROR(PerfectHashTableCreateInstance, Result);
            Failures++;
            Terminate = TRUE;
            goto ReleaseKeys;
        }

        Result = Table->Vtbl->Create(Table,
                                     Context,
                                     AlgorithmId,
                                     MaskFunctionId,
                                     HashFunctionId,
                                     Keys,
                                     &TableCreateFlags,
                                     NumberOfTableCreateParameters,
                                     TableCreateParameters);

        if (FAILED(Result)) {
            PH_KEYS_ERROR(PerfectHashTableCreate, Result);
            Failed = TRUE;
            Failures++;
            goto ReleaseTable;
        } else if (Result != S_OK) {

            //
            // Todo.
            //

            CROSS();
            goto ReleaseTable;

        } else {
            DOT();
        }


        //
        // Disable compilation at the moment as it adds an extra ~6-10 seconds
        // per iteration.
        //

#if 0

        //
        // Compile the table.
        //

        Result = Table->Vtbl->Compile(Table,
                                      &TableCompileFlags,
                                      CpuArchId);

        if (FAILED(Result)) {
            PH_TABLE_ERROR(PerfectHashTableCompile, Result);
            Failures++;
            Failed = TRUE;
            goto ReleaseTable;
        }

#endif

    ReleaseTable:

        //
        // Release the table.
        //

        ReferenceCount = Table->Vtbl->Release(Table);
        Table = NULL;

        if (ReferenceCount != 0) {
            PH_RAISE(PH_E_INVARIANT_CHECK_FAILED);
        }

        //
        // Release the table path and file.
        //

        RELEASE(TablePath);
        RELEASE(TableFile);

        //
        // Intentional follow-on to ReleaseKeys.
        //

    ReleaseKeys:

        RELEASE(Keys);

        DOT();

        if (Terminate) {
            break;
        }

    } while (FindNextFile(FindHandle, &FindData));

    //
    // Bulk create complete!
    //

    NEWLINE();

    if (!Failures && !Terminate) {
        Result = S_OK;
        goto End;
    }

    //
    // Intentional follow-on to Error.
    //

Error:

    if (Result == S_OK) {
        Result = E_UNEXPECTED;
    }

    //
    // Intentional follow-on to End.
    //

End:

    if (OutputBuffer) {
        Result = Rtl->Vtbl->DestroyBuffer(Rtl,
                                          ProcessHandle,
                                          &OutputBuffer);
        if (FAILED(Result)) {
            SYS_ERROR(VirtualFree);
            Result = PH_E_SYSTEM_CALL_FAILED;
        }
        Output = NULL;
    }

    if (BaseBuffer) {
        Result = Rtl->Vtbl->DestroyBuffer(Rtl,
                                          ProcessHandle,
                                          &BaseBuffer);
        if (FAILED(Result)) {
            SYS_ERROR(VirtualFree);
            Result = PH_E_SYSTEM_CALL_FAILED;
        }
        BaseBuffer = NULL;
    }

    if (FindHandle) {
        if (!FindClose(FindHandle)) {
            SYS_ERROR(FindClose);
            Result = PH_E_SYSTEM_CALL_FAILED;
        }
        FindHandle = NULL;
    }

    ClearContextBulkCreate(Context);

    return Result;
}

//
// Helper macros for argument extraction.
//

#define GET_LENGTH(Name) (USHORT)wcslen(Name->Buffer) << (USHORT)1
#define GET_MAX_LENGTH(Name) Name->Length + 2

#define VALIDATE_ID(Name, Upper)                                      \
    if (FAILED(Rtl->RtlUnicodeStringToInteger(String,                 \
                                              10,                     \
                                              (PULONG)##Name##Id))) { \
        return PH_E_INVALID_##Upper##_ID;                             \
    } else if (!IsValidPerfectHash##Name##Id(*##Name##Id)) {          \
        return PH_E_INVALID_##Upper##_ID;                             \
    }



PERFECT_HASH_CONTEXT_EXTRACT_BULK_CREATE_ARGS_FROM_ARGVW
    PerfectHashContextExtractBulkCreateArgsFromArgvW;

_Use_decl_annotations_
HRESULT
PerfectHashContextExtractBulkCreateArgsFromArgvW(
    PPERFECT_HASH_CONTEXT Context,
    ULONG NumberOfArguments,
    LPWSTR *ArgvW,
    PUNICODE_STRING KeysDirectory,
    PUNICODE_STRING BaseOutputDirectory,
    PPERFECT_HASH_ALGORITHM_ID AlgorithmId,
    PPERFECT_HASH_HASH_FUNCTION_ID HashFunctionId,
    PPERFECT_HASH_MASK_FUNCTION_ID MaskFunctionId,
    PULONG MaximumConcurrency,
    PPERFECT_HASH_CONTEXT_BULK_CREATE_FLAGS ContextBulkCreateFlags,
    PPERFECT_HASH_KEYS_LOAD_FLAGS KeysLoadFlags,
    PPERFECT_HASH_TABLE_CREATE_FLAGS TableCreateFlags,
    PPERFECT_HASH_TABLE_COMPILE_FLAGS TableCompileFlags,
    PULONG NumberOfTableCreateParameters,
    PPERFECT_HASH_TABLE_CREATE_PARAMETER *TableCreateParameters
    )
/*++

Routine Description:

    Extracts arguments for the bulk-create functionality from an argument vector
    array, typically obtained from a commandline invocation.

Arguments:

    Context - Supplies a pointer to the PERFECT_HASH_CONTEXT instance
        for which the arguments are to be extracted.

    NumberOfArguments - Supplies the number of elements in the ArgvW array.

    ArgvW - Supplies a pointer to an array of wide C string arguments.

    KeysDirectory - Supplies a pointer to a UNICODE_STRING structure that
        will be filled out with the keys directory.

    BaseOutputDirectory - Supplies a pointer to a UNICODE_STRING structure that
        will be filled out with the output directory.

    AlgorithmId - Supplies the address of a variable that will receive the
        algorithm ID.

    HashFunctionId - Supplies the address of a variable that will receive the
        hash function ID.

    MaskFunctionId - Supplies the address of a variable that will receive the
        mask function ID.

    MaximumConcurrency - Supplies the address of a variable that will receive
        the maximum concurrency.

    BulkCreateFlags - Supplies the address of a variable that will receive the
        bulk-create flags.

    KeysLoadFlags - Supplies the address of a variable that will receive
        the keys load flags.

    TableCreateFlags - Supplies the address of a variable that will receive
        the table create flags.

    TableLoadFlags - Supplies the address of a variable that will receive the
        the load table flags.

    TableCompileFlags - Supplies the address of a variable that will receive
        the table compile flags.

    NumberOfTableCreateParameters - Supplies the address of a variable that will
        receive the number of elements in the TableCreateParameters array.

    TableCreateParameters - Supplies the address of a variable that will receive
        a pointer to an array of table create parameters.  If this is not NULL,
        the memory will be allocated via the context's allocator and the caller
        is responsible for freeing it.

Return Value:

    S_OK - Arguments extracted successfully.

    E_POINTER - One or more mandatory parameters were NULL pointers.

    PH_E_CONTEXT_BULK_CREATE_INVALID_NUM_ARGS - Invalid number of arguments.

    PH_E_INVALID_ALGORITHM_ID - Invalid algorithm ID.

    PH_E_INVALID_HASH_FUNCTION_ID - Invalid hash function ID.

    PH_E_INVALID_MASK_FUNCTION_ID - Invalid mask function ID.

    PH_E_INVALID_MAXIMUM_CONCURRENCY - Invalid maximum concurrency.

--*/
{
    PRTL Rtl;
    LPWSTR *ArgW;
    LPWSTR Arg;
    HRESULT Result = S_OK;
    ULONG CurrentArg = 1;
    PALLOCATOR Allocator;
    UNICODE_STRING Temp;
    PUNICODE_STRING String;
    BOOLEAN ValidNumberOfArguments;

    String = &Temp;

    //
    // Validate arguments.
    //

    if (!ARGUMENT_PRESENT(Context)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(ArgvW)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(KeysDirectory)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(BaseOutputDirectory)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(AlgorithmId)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(HashFunctionId)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(MaskFunctionId)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(MaximumConcurrency)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(ContextBulkCreateFlags)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(KeysLoadFlags)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(TableCreateFlags)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(TableCompileFlags)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(NumberOfTableCreateParameters)) {
        return E_POINTER;
    }

    if (!ARGUMENT_PRESENT(TableCreateParameters)) {
        return E_POINTER;
    }

    ValidNumberOfArguments = (
        NumberOfArguments >= 7
    );

    if (!ValidNumberOfArguments) {
        return PH_E_CONTEXT_BULK_CREATE_INVALID_NUM_ARGS;
    }

    //
    // Argument validation complete, continue.
    //

    ArgW = &ArgvW[1];
    Rtl = Context->Rtl;
    Allocator = Context->Allocator;

    //
    // Extract keys directory.
    //

    CurrentArg++;
    KeysDirectory->Buffer = *ArgW++;
    KeysDirectory->Length = GET_LENGTH(KeysDirectory);
    KeysDirectory->MaximumLength = GET_MAX_LENGTH(KeysDirectory);

    //
    // Extract base output directory.
    //

    CurrentArg++;
    BaseOutputDirectory->Buffer = *ArgW++;
    BaseOutputDirectory->Length = GET_LENGTH(BaseOutputDirectory);
    BaseOutputDirectory->MaximumLength = GET_MAX_LENGTH(BaseOutputDirectory);

    //
    // Extract algorithm ID.
    //

    CurrentArg++;
    String->Buffer = *ArgW++;
    String->Length = GET_LENGTH(String);
    String->MaximumLength = GET_MAX_LENGTH(String);
    VALIDATE_ID(Algorithm, ALGORITHM);

    //
    // Extract hash function ID.
    //

    CurrentArg++;
    String->Buffer = *ArgW++;
    String->Length = GET_LENGTH(String);
    String->MaximumLength = GET_MAX_LENGTH(String);
    VALIDATE_ID(HashFunction, HASH_FUNCTION);

    //
    // Extract mask function ID.
    //

    CurrentArg++;
    String->Buffer = *ArgW++;
    String->Length = GET_LENGTH(String);
    String->MaximumLength = GET_MAX_LENGTH(String);
    VALIDATE_ID(MaskFunction, MASK_FUNCTION);

    //
    // Extract maximum concurrency.
    //

    CurrentArg++;
    String->Buffer = *ArgW++;
    String->Length = GET_LENGTH(String);
    String->MaximumLength = GET_MAX_LENGTH(String);

    if (FAILED(Rtl->RtlUnicodeStringToInteger(String,
                                              10,
                                              MaximumConcurrency))) {
        return PH_E_INVALID_MAXIMUM_CONCURRENCY;
    }

    //
    // Zero all flags and table create parameters.
    //

    ContextBulkCreateFlags->AsULong = 0;
    KeysLoadFlags->AsULong = 0;
    TableCreateFlags->AsULong = 0;
    TableCompileFlags->AsULong = 0;
    *NumberOfTableCreateParameters = 0;
    *TableCreateParameters = NULL;

    for (; CurrentArg < NumberOfArguments; CurrentArg++, ArgW++) {

        String->Buffer = Arg = *ArgW;
        String->Length = GET_LENGTH(String);
        String->MaximumLength = GET_MAX_LENGTH(String);

        //
        // If the argument doesn't start with two dashes, ignore it.
        //

        if (String->Length <= (sizeof(L'-') + sizeof(L'-'))) {
            continue;
        }

        if (!(*Arg++ == L'-' && *Arg++ == L'-')) {
            continue;
        }

        //
        // Advance the buffer past the two dashes and update lengths
        // accordingly.
        //

        String->Buffer += 2;
        String->Length -= 4;
        String->MaximumLength -= 4;

        //
        // Try each argument extraction routine for this argument; if it
        // indicates an error, report it and break out of the loop.  If it
        // indicates it successfully extracted the argument (Result == S_OK),
        // continue onto the next argument.  Otherwise, verify it indicates
        // that no argument was extracted (S_FALSE), then try the next routine.
        //

#define TRY_EXTRACT_ARG(Name)                                                 \
    Result = TryExtractArg##Name##Flags(Rtl, Allocator, String, Name##Flags); \
    if (FAILED(Result)) {                                                     \
        PH_ERROR(ExtractBulkCreateArgs_TryExtractArg##Name##Flags, Result);   \
        break;                                                                \
    } else if (Result == S_OK) {                                              \
        continue;                                                             \
    } else {                                                                  \
        ASSERT(Result == S_FALSE);                                            \
    }

        TRY_EXTRACT_ARG(ContextBulkCreate);
        TRY_EXTRACT_ARG(KeysLoad);
        TRY_EXTRACT_ARG(TableCreate);
        TRY_EXTRACT_ARG(TableCompile);

        //
        // If we get here, none of the previous extraction routines claimed the
        // argument, so, provide the table create parameters extraction routine
        // an opportunity to run.
        //

        Result =
            TryExtractArgTableCreateParameters(Rtl,
                                               Allocator,
                                               String,
                                               NumberOfTableCreateParameters,
                                               TableCreateParameters);

        if (FAILED(Result)) {

            PH_ERROR(ExtractBulkCreateArgs_TryExtractTableCreateParams, Result);
            break;

        } else {

            //
            // Ignore anything not recognized for now.
            //

            continue;
        }
    }

    //
    // If we failed, free the table create parameters if they are non-null and
    // clear the corresponding number-of variable.
    //

    if (FAILED(Result) && TableCreateParameters) {
        Context->Allocator->Vtbl->FreePointer(Context->Allocator,
                                              (PVOID *)&TableCreateParameters);
        *NumberOfTableCreateParameters = 0;
    }

    return Result;
}

PERFECT_HASH_CONTEXT_BULK_CREATE_ARGVW PerfectHashContextBulkCreateArgvW;

_Use_decl_annotations_
HRESULT
PerfectHashContextBulkCreateArgvW(
    PPERFECT_HASH_CONTEXT Context,
    ULONG NumberOfArguments,
    LPWSTR *ArgvW
    )
/*++

Routine Description:

    Extracts arguments for the bulk-create functionality from an argument vector
    array and then invokes the context bulk-create functionality.

Arguments:

    Context - Supplies a pointer to the PERFECT_HASH_CONTEXT instance
        for which the arguments are to be extracted.

    NumberOfArguments - Supplies the number of elements in the ArgvW array.

    ArgvW - Supplies a pointer to an array of wide C string arguments.

Return Value:

    S_OK - Arguments extracted successfully.

    E_POINTER - One or more mandatory parameters were NULL pointers.

    PH_E_CONTEXT_BULK_CREATE_INVALID_NUM_ARGS - Invalid number of arguments.

    PH_E_INVALID_ALGORITHM_ID - Invalid algorithm ID.

    PH_E_INVALID_HASH_FUNCTION_ID - Invalid hash function ID.

    PH_E_INVALID_MASK_FUNCTION_ID - Invalid mask function ID.

    PH_E_INVALID_MAXIMUM_CONCURRENCY - Invalid maximum concurrency.

    PH_E_INVALID_KEYS_LOAD_FLAGS - Invalid keys load flags.

    PH_E_INVALID_CONTEXT_CREATE_TABLE_FLAGS - Invalid context create table
        flags.

    PH_E_INVALID_TABLE_LOAD_FLAGS - Invalid table load flags.

    PH_E_INVALID_TABLE_COMPILE_FLAGS - Invalid table compile flags.

--*/
{
    PRTL Rtl;
    HRESULT Result;
    UNICODE_STRING KeysDirectory = { 0 };
    UNICODE_STRING BaseOutputDirectory = { 0 };
    PERFECT_HASH_ALGORITHM_ID AlgorithmId = 0;
    PERFECT_HASH_HASH_FUNCTION_ID HashFunctionId = 0;
    PERFECT_HASH_MASK_FUNCTION_ID MaskFunctionId = 0;
    ULONG MaximumConcurrency = 0;
    PERFECT_HASH_CONTEXT_BULK_CREATE_FLAGS ContextBulkCreateFlags = { 0 };
    PERFECT_HASH_KEYS_LOAD_FLAGS KeysLoadFlags = { 0 };
    PERFECT_HASH_TABLE_CREATE_FLAGS TableCreateFlags = { 0 };
    PERFECT_HASH_TABLE_COMPILE_FLAGS TableCompileFlags = { 0 };
    PPERFECT_HASH_CONTEXT_EXTRACT_BULK_CREATE_ARGS_FROM_ARGVW
        ExtractBulkCreateArgs;
    ULONG NumberOfTableCreateParameters = 0;
    PPERFECT_HASH_TABLE_CREATE_PARAMETER TableCreateParameters = 0;

    Rtl = Context->Rtl;

    ExtractBulkCreateArgs = Context->Vtbl->ExtractBulkCreateArgsFromArgvW;
    Result = ExtractBulkCreateArgs(Context,
                                   NumberOfArguments,
                                   ArgvW,
                                   &KeysDirectory,
                                   &BaseOutputDirectory,
                                   &AlgorithmId,
                                   &HashFunctionId,
                                   &MaskFunctionId,
                                   &MaximumConcurrency,
                                   &ContextBulkCreateFlags,
                                   &KeysLoadFlags,
                                   &TableCreateFlags,
                                   &TableCompileFlags,
                                   &NumberOfTableCreateParameters,
                                   &TableCreateParameters);

    if (FAILED(Result)) {

        //
        // Todo: write the usage string.
        //

        return Result;
    }

    if (MaximumConcurrency > 0) {
        Result = Context->Vtbl->SetMaximumConcurrency(Context,
                                                      MaximumConcurrency);
        if (FAILED(Result)) {
            Result = PH_E_SET_MAXIMUM_CONCURRENCY_FAILED;
            PH_ERROR(PerfectHashContextContextBulkCreateArgvW, Result);
            return Result;
        }
    }

    Result = Context->Vtbl->BulkCreate(Context,
                                       &KeysDirectory,
                                       &BaseOutputDirectory,
                                       AlgorithmId,
                                       HashFunctionId,
                                       MaskFunctionId,
                                       &ContextBulkCreateFlags,
                                       &KeysLoadFlags,
                                       &TableCreateFlags,
                                       &TableCompileFlags,
                                       NumberOfTableCreateParameters,
                                       TableCreateParameters);

    if (FAILED(Result)) {

        //
        // There's is nothing we can do here.  We don't PH_ERROR() the return
        // code as BulkCreate() will have done that multiple times each time
        // the error bubbled back up the stack.
        //

        NOTHING;
    }

    if (TableCreateParameters) {
        Context->Allocator->Vtbl->FreePointer(Context->Allocator,
                                              (PVOID *)&TableCreateParameters);
    }

    return Result;
}

// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :