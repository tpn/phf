/*++

Copyright (c) 2018 Trent Nelson <trent@trent.me>

Module Name:

    ExtractArg.c

Abstract:

    This module implements the various argument extraction routines.

--*/

#include "stdafx.h"

//
// Helper macro for defining local UNICODE_STRING structures.
//

#ifndef RCS
#define RCS RTL_CONSTANT_STRING
#endif

#define DECL_ARG(Name) const UNICODE_STRING Name = RCS(L#Name)

//
// Helper macro for just the Rtl->RtlEqualUnicodeString() comparison.
//

#define IS_EQUAL(Name) Rtl->RtlEqualUnicodeString(Argument, &Name, TRUE)

//
// Helper macro for Rtl->RtlPrefixUnicodeString().  This is used instead of the
// IS_EQUAL() macro above when the argument may contain an equal sign, e.g.
//
//      --TryLargePagesForKeysData vs --AttemptsBeforeTableResize=100
//

#define IS_PREFIX(Name) Rtl->RtlPrefixUnicodeString(&Name, Argument, TRUE)

//
// Helper macro for toggling the given flag if the current argument matches
// the given UNICODE_STRING.
//

#define SET_FLAG_AND_RETURN_IF_EQUAL(Name) \
    if (IS_EQUAL(Name)) {                  \
        Flags->##Name = TRUE;              \
        return S_OK;                       \
    }

//
// Method implementations.
//

TRY_EXTRACT_ARG_CONTEXT_BULK_CREATE_FLAGS TryExtractArgContextBulkCreateFlags;

_Use_decl_annotations_
HRESULT
TryExtractArgContextBulkCreateFlags(
    PRTL Rtl,
    PALLOCATOR Allocator,
    PCUNICODE_STRING Argument,
    PPERFECT_HASH_CONTEXT_BULK_CREATE_FLAGS Flags
    )
{
    DECL_ARG(SkipTestAfterCreate);
    DECL_ARG(Compile);

    UNREFERENCED_PARAMETER(Allocator);

    SET_FLAG_AND_RETURN_IF_EQUAL(SkipTestAfterCreate);
    SET_FLAG_AND_RETURN_IF_EQUAL(Compile);

    return S_FALSE;
}


TRY_EXTRACT_ARG_CONTEXT_TABLE_CREATE_FLAGS TryExtractArgContextTableCreateFlags;

_Use_decl_annotations_
HRESULT
TryExtractArgContextTableCreateFlags(
    PRTL Rtl,
    PALLOCATOR Allocator,
    PCUNICODE_STRING Argument,
    PPERFECT_HASH_CONTEXT_TABLE_CREATE_FLAGS Flags
    )
{
    DECL_ARG(SkipTestAfterCreate);
    DECL_ARG(Compile);

    UNREFERENCED_PARAMETER(Allocator);

    SET_FLAG_AND_RETURN_IF_EQUAL(SkipTestAfterCreate);
    SET_FLAG_AND_RETURN_IF_EQUAL(Compile);

    return S_FALSE;
}


TRY_EXTRACT_ARG_KEYS_LOAD_FLAGS TryExtractArgKeysLoadFlags;

_Use_decl_annotations_
HRESULT
TryExtractArgKeysLoadFlags(
    PRTL Rtl,
    PALLOCATOR Allocator,
    PCUNICODE_STRING Argument,
    PPERFECT_HASH_KEYS_LOAD_FLAGS Flags
    )
{
    DECL_ARG(TryLargePagesForKeysData);
    DECL_ARG(SkipKeysVerification);

    UNREFERENCED_PARAMETER(Allocator);

    SET_FLAG_AND_RETURN_IF_EQUAL(TryLargePagesForKeysData);
    SET_FLAG_AND_RETURN_IF_EQUAL(SkipKeysVerification);

    return S_FALSE;
}


TRY_EXTRACT_ARG_TABLE_CREATE_FLAGS TryExtractArgTableCreateFlags;

_Use_decl_annotations_
HRESULT
TryExtractArgTableCreateFlags(
    PRTL Rtl,
    PALLOCATOR Allocator,
    PCUNICODE_STRING Argument,
    PPERFECT_HASH_TABLE_CREATE_FLAGS Flags
    )
{
    DECL_ARG(FirstGraphWins);
    DECL_ARG(FindBestGraph);
    DECL_ARG(SkipGraphVerification);
    DECL_ARG(CreateOnly);
    DECL_ARG(TryLargePagesForTableData);
    DECL_ARG(TryLargePagesForValuesArray);
    DECL_ARG(IgnorePreviousTableSize);
    DECL_ARG(IncludeNumberOfTableResizeEventsInOutputPath);
    DECL_ARG(IncludeNumberOfTableElementsInOutputPath);
    DECL_ARG(NoFileIo);
    DECL_ARG(Silent);
    DECL_ARG(Paranoid);
    DECL_ARG(SkipMemoryCoverageInFirstGraphWinsMode);
    DECL_ARG(TryLargePagesForGraphTableData);
    DECL_ARG(TryLargePagesForGraphEdgeAndVertexArrays);

    UNREFERENCED_PARAMETER(Allocator);

    //
    // FirstGraphWins isn't actually a flag as it's the default behavior.
    // However, it's explicitly referenced in the usage string, so, add a
    // test for it that simply returns S_OK.
    //

    if (IS_EQUAL(FirstGraphWins)) {
        return S_OK;
    }

    //
    // Continue with additional flag extraction.
    //

    SET_FLAG_AND_RETURN_IF_EQUAL(FindBestGraph);
    SET_FLAG_AND_RETURN_IF_EQUAL(SkipGraphVerification);
    SET_FLAG_AND_RETURN_IF_EQUAL(CreateOnly);
    SET_FLAG_AND_RETURN_IF_EQUAL(TryLargePagesForTableData);
    SET_FLAG_AND_RETURN_IF_EQUAL(TryLargePagesForValuesArray);
    SET_FLAG_AND_RETURN_IF_EQUAL(IgnorePreviousTableSize);
    SET_FLAG_AND_RETURN_IF_EQUAL(IncludeNumberOfTableResizeEventsInOutputPath);
    SET_FLAG_AND_RETURN_IF_EQUAL(IncludeNumberOfTableElementsInOutputPath);
    SET_FLAG_AND_RETURN_IF_EQUAL(NoFileIo);
    SET_FLAG_AND_RETURN_IF_EQUAL(Silent);
    SET_FLAG_AND_RETURN_IF_EQUAL(Paranoid);
    SET_FLAG_AND_RETURN_IF_EQUAL(SkipMemoryCoverageInFirstGraphWinsMode);
    SET_FLAG_AND_RETURN_IF_EQUAL(TryLargePagesForGraphTableData);
    SET_FLAG_AND_RETURN_IF_EQUAL(TryLargePagesForGraphEdgeAndVertexArrays);

    return S_FALSE;
}


TRY_EXTRACT_ARG_TABLE_LOAD_FLAGS TryExtractArgTableLoadFlags;

_Use_decl_annotations_
HRESULT
TryExtractArgTableLoadFlags(
    PRTL Rtl,
    PALLOCATOR Allocator,
    PCUNICODE_STRING Argument,
    PPERFECT_HASH_TABLE_LOAD_FLAGS Flags
    )
{
    DECL_ARG(TryLargePagesForTableData);
    DECL_ARG(TryLargePagesForValuesArray);

    UNREFERENCED_PARAMETER(Allocator);

    SET_FLAG_AND_RETURN_IF_EQUAL(TryLargePagesForTableData);
    SET_FLAG_AND_RETURN_IF_EQUAL(TryLargePagesForValuesArray);

    return S_FALSE;
}


TRY_EXTRACT_ARG_TABLE_COMPILE_FLAGS TryExtractArgTableCompileFlags;

_Use_decl_annotations_
HRESULT
TryExtractArgTableCompileFlags(
    PRTL Rtl,
    PALLOCATOR Allocator,
    PCUNICODE_STRING Argument,
    PPERFECT_HASH_TABLE_COMPILE_FLAGS Flags
    )
{
    UNREFERENCED_PARAMETER(Rtl);
    UNREFERENCED_PARAMETER(Allocator);
    DBG_UNREFERENCED_PARAMETER(Argument);
    DBG_UNREFERENCED_PARAMETER(Flags);

    return S_FALSE;
}

typedef enum _EXTRACT_VALUE_ARRAY_STATE {
    LookingForValue,
    LookingForComma,
} EXTRACT_VALUE_ARRAY_STATE;

TRY_EXTRACT_VALUE_ARRAY TryExtractValueArray;

_Use_decl_annotations_
HRESULT
TryExtractValueArray(
    PRTL Rtl,
    PALLOCATOR Allocator,
    PCUNICODE_STRING InputString,
    PPERFECT_HASH_TABLE_CREATE_PARAMETER Param,
    BOOLEAN EnsureSortedAndUnique
    )
{
    USHORT Index;
    USHORT NumberOfInputStringChars;
    ULONG Commas = 0;
    ULONG NumberOfValues;
    ULONG NumberOfValidValues;
    PWCHAR Wide;
    PWCHAR ValueStart = NULL;
    PULONG Value;
    PULONG Values = NULL;
    UNICODE_STRING ValueString;
    EXTRACT_VALUE_ARRAY_STATE State;
    BOOLEAN IsLastChar;
    NTSTATUS Status;
    HRESULT Result = S_OK;
    PVALUE_ARRAY ValueArray;
    PRTL_UNICODE_STRING_TO_INTEGER RtlUnicodeStringToInteger;

    //
    // Initialize aliases.
    //

    RtlUnicodeStringToInteger = Rtl->RtlUnicodeStringToInteger;

    //
    // Loop through the string and count the number of commas we see.
    //

    Wide = InputString->Buffer;
    NumberOfInputStringChars = InputString->Length >> 1;

    for (Index = 0; Index < NumberOfInputStringChars; Index++, Wide++) {
        if (*Wide == L',') {
            Commas++;
        }
    }

    if (Commas == 0) {
        return S_FALSE;
    }

    //
    // Allocate memory to contain an array of ULONGs whose length matches the
    // number of commas we saw, plus 1.
    //

    NumberOfValues = Commas + 1;
    Value = Values = Allocator->Vtbl->Calloc(Allocator,
                                             NumberOfValues,
                                             sizeof(ULONG));
    if (!Values) {
        return E_OUTOFMEMORY;
    }

    State = LookingForValue;
    NumberOfValidValues = 0;
    ValueStart = NULL;

    Wide = InputString->Buffer;

    for (Index = 0; Index < NumberOfInputStringChars; Index++, Wide++) {

        IsLastChar = (Index == (NumberOfInputStringChars - 1));

        if (IsLastChar && State == LookingForComma &&
            *Wide != L',' && ValueStart != NULL) {
            Wide++;
            goto ProcessValue;
        }

        if (State == LookingForValue) {

            ASSERT(ValueStart == NULL);

            if (*Wide != L',') {
                ValueStart = Wide;
                State = LookingForComma;
            }

            continue;

        } else {

            ASSERT(State == LookingForComma);
            ASSERT(ValueStart != NULL);

            if (*Wide != L',') {
                continue;
            }

ProcessValue:

            ValueString.Buffer = ValueStart;
            ValueString.Length = (USHORT)RtlPointerToOffset(ValueStart, Wide);
            ValueString.MaximumLength = ValueString.Length;

            Status = RtlUnicodeStringToInteger(&ValueString, 0, Value);
            if (!SUCCEEDED(Status)) {
                Result = E_FAIL;
                goto Error;
            }

            if (EnsureSortedAndUnique && Value != Values) {
                ULONG Previous;
                ULONG This;

                Previous = *(Value - 1);
                This = *Value;

                if (Previous > This) {
                    Result = PH_E_NOT_SORTED;
                    goto Error;
                } else if (Previous == This) {
                    Result = PH_E_DUPLICATE_DETECTED;
                    goto Error;
                }
            }

            Value++;
            ValueStart = NULL;
            NumberOfValidValues++;
            State = LookingForValue;

            continue;
        }
    }

    if (NumberOfValidValues == 0) {
        Result = E_FAIL;
        goto Error;
    } else if (NumberOfValidValues != NumberOfValues) {
        Result = PH_E_INVARIANT_CHECK_FAILED;
        PH_ERROR(TryExtractValueArray_NumValidValuesMismatch, Result);
        goto Error;
    }

    //
    // We've successfully extracted the array.  Update the parameter and return
    // success.
    //

    ValueArray = &Param->AsValueArray;
    ValueArray->Values = Values;
    ValueArray->NumberOfValues = NumberOfValidValues;

    goto End;

Error:

    if (Result == S_OK) {
        Result = E_UNEXPECTED;
    }

    if (Values) {
        Allocator->Vtbl->FreePointer(Allocator, &Values);
    }

    //
    // Intentional follow-on to End.
    //

End:

    return Result;
}

FORCEINLINE
VOID
MaybeDeallocateTableCreateParameter(
    _In_ PALLOCATOR Allocator,
    _In_ PPERFECT_HASH_TABLE_CREATE_PARAMETER Param
    )
{
    if (DoesTableCreateParameterRequireDeallocation(Param->Id)) {
        Allocator->Vtbl->FreePointer(Allocator,
                                     (PVOID *)&Param->AsVoidPointer);
    }
}

//
// Processing table create parameters is more involved than the flag-oriented
// routines above as we need to allocate or reallocate the parameters array
// when we find a match.
//

TRY_EXTRACT_ARG_TABLE_CREATE_PARAMETERS TryExtractArgTableCreateParameters;

_Use_decl_annotations_
HRESULT
TryExtractArgTableCreateParameters(
    PRTL Rtl,
    PALLOCATOR Allocator,
    PCUNICODE_STRING Argument,
    PULONG NumberOfTableCreateParametersPointer,
    PPERFECT_HASH_TABLE_CREATE_PARAMETER *TableCreateParametersPointer
    )
{
    USHORT Count;
    USHORT Index;
    ULONG Value = 0;
    PWSTR Source;
    BOOLEAN Found = FALSE;
    BOOLEAN ValueIsInteger = FALSE;
    HRESULT Result = S_FALSE;
    UNICODE_STRING Temp = { 0 };
    PUNICODE_STRING ValueString;
    ULONG NumberOfTableCreateParameters;
    PERFECT_HASH_TABLE_CREATE_PARAMETER LocalParam;
    PPERFECT_HASH_TABLE_CREATE_PARAMETER Param;
    PPERFECT_HASH_TABLE_CREATE_PARAMETER TableCreateParameters;
    PPERFECT_HASH_TABLE_CREATE_PARAMETER NewTableCreateParameters;

    DECL_ARG(AttemptsBeforeTableResize);
    DECL_ARG(MaxNumberOfTableResizes);
    DECL_ARG(BestCoverageAttempts);
    DECL_ARG(BestCoverageType);
    DECL_ARG(KeysSubset);
    DECL_ARG(MainWorkThreadpoolPriority);
    DECL_ARG(FileWorkThreadpoolPriority);
    DECL_ARG(High);
    DECL_ARG(Normal);
    DECL_ARG(Low);

#define EXPAND_AS_DECL_ARG(Name, Comparison, Comparator) \
    DECL_ARG(Comparison##Name);

    BEST_COVERAGE_TYPE_TABLE_ENTRY(EXPAND_AS_DECL_ARG)

    ZeroStructInline(LocalParam);

    NumberOfTableCreateParameters = *NumberOfTableCreateParametersPointer;
    TableCreateParameters = *TableCreateParametersPointer;

    //
    // Invariant check: if number of table create parameters is 0, the array
    // pointer should be null, and vice versa.
    //

    if (NumberOfTableCreateParameters == 0) {
        if (TableCreateParameters != NULL) {
            PH_RAISE(PH_E_INVARIANT_CHECK_FAILED);
        }
    } else {
        if (TableCreateParameters == NULL) {
            PH_RAISE(PH_E_INVARIANT_CHECK_FAILED);
        }
    }

    //
    // Find where the equals sign occurs, if at all.
    //

    Source = Argument->Buffer;
    Count = Argument->Length >> 1;

    for (Index = 0; Index < Count; Index++) {
        if (*Source == L'=') {
            Source++;
            Found = TRUE;
            break;
        }
        Source++;
    }

    if (!Found) {
        return S_FALSE;
    }

    ASSERT(*(Source - 1) == L'=');
    ASSERT(Argument->Buffer[Index] == L'=');

    //
    // Wire up the ValueString variable to point immediately after the equals
    // sign.  The +1 in (Index + 1) accounts for (subtracts) the NULL.
    //

    ValueString = &Temp;
    ValueString->Buffer = Source;
    ValueString->Length = Argument->Length - ((Index + 1) << 1);
    ValueString->MaximumLength = ValueString->Length + 2;

    ASSERT(ValueString->Buffer[ValueString->Length >> 1] == L'\0');

    //
    // Attempt to convert the value string into an integer representation.
    //

    Result = Rtl->RtlUnicodeStringToInteger(ValueString, 10, &Value);
    if (SUCCEEDED(Result)) {
        ValueIsInteger = TRUE;
    }

#define SET_PARAM_ID(Name)                         \
    LocalParam.Id = TableCreateParameter##Name##Id

#define ADD_PARAM_IF_PREFIX_AND_VALUE_IS_INTEGER(Name) \
    if (IS_PREFIX(Name) && ValueIsInteger) {           \
        SET_PARAM_ID(Name);                            \
        LocalParam.AsULong = Value;                    \
        goto AddParam;                                 \
    }

    ADD_PARAM_IF_PREFIX_AND_VALUE_IS_INTEGER(AttemptsBeforeTableResize);

    ADD_PARAM_IF_PREFIX_AND_VALUE_IS_INTEGER(MaxNumberOfTableResizes);

    ADD_PARAM_IF_PREFIX_AND_VALUE_IS_INTEGER(BestCoverageAttempts);

#define IS_VALUE_EQUAL(ValueName) \
    Rtl->RtlEqualUnicodeString(ValueString, &ValueName, TRUE)

#define ADD_PARAM_IF_PREFIX_AND_VALUE_EQUAL(Name, ValueName) \
    if (IS_PREFIX(Name) && IS_VALUE_EQUAL(ValueName)) {      \
        SET_PARAM_ID(Name);                                  \
        LocalParam.AsULongLong = Name##ValueName##Id;        \
        goto AddParam;                                       \
    }

#define EXPAND_AS_ADD_PARAM(Name, Comparison, Comparator)  \
    ADD_PARAM_IF_PREFIX_AND_VALUE_EQUAL(BestCoverageType,  \
                                        Comparison##Name);

    BEST_COVERAGE_TYPE_TABLE_ENTRY(EXPAND_AS_ADD_PARAM);

#define ADD_PARAM_IF_PREFIX_AND_VALUE_IS_CSV_OF_ASCENDING_INTEGERS(Name,  \
                                                                   Upper) \
    if (IS_PREFIX(Name)) {                                                \
        Result = TryExtractValueArray(Rtl,                                \
                                      Allocator,                          \
                                      ValueString,                        \
                                      &LocalParam,                        \
                                      TRUE);                              \
                                                                          \
        if (Result == S_OK) {                                             \
            SET_PARAM_ID(Name);                                           \
            goto AddParam;                                                \
        } else {                                                          \
            if (Result == PH_E_NOT_SORTED) {                              \
                Result = PH_E_##Upper##_NOT_SORTED;                       \
            } else if (Result == PH_E_DUPLICATE_DETECTED) {               \
                Result = PH_E_DUPLICATE_VALUE_DETECTED_IN_##Upper;        \
            } else if (Result != E_OUTOFMEMORY) {                         \
                Result = PH_E_INVALID_##Upper;                            \
            }                                                             \
            goto Error;                                                   \
        }                                                                 \
    }

    ADD_PARAM_IF_PREFIX_AND_VALUE_IS_CSV_OF_ASCENDING_INTEGERS(KeysSubset,
                                                               KEYS_SUBSET);

#define ADD_PARAM_IF_PREFIX_AND_VALUE_IS_TP_PRIORITY(Name, Upper)          \
    if (IS_PREFIX(Name##ThreadpoolPriority)) {                             \
        if (IS_VALUE_EQUAL(High)) {                                        \
            SET_PARAM_ID(Name##ThreadpoolPriority);                        \
            LocalParam.AsTpCallbackPriority = TP_CALLBACK_PRIORITY_HIGH;   \
            goto AddParam;                                                 \
        } else if (IS_VALUE_EQUAL(Normal)) {                               \
            SET_PARAM_ID(Name##ThreadpoolPriority);                        \
            LocalParam.AsTpCallbackPriority = TP_CALLBACK_PRIORITY_NORMAL; \
            goto AddParam;                                                 \
        } else if (IS_VALUE_EQUAL(Low)) {                                  \
            SET_PARAM_ID(Name##ThreadpoolPriority);                        \
            LocalParam.AsTpCallbackPriority = TP_CALLBACK_PRIORITY_LOW;    \
            goto AddParam;                                                 \
        } else {                                                           \
            Result = PH_E_INVALID_##Upper##_THREADPOOL_PRIORITY;           \
            goto Error;                                                    \
        }                                                                  \
    }

    ADD_PARAM_IF_PREFIX_AND_VALUE_IS_TP_PRIORITY(MainWork, MAIN_WORK);
    ADD_PARAM_IF_PREFIX_AND_VALUE_IS_TP_PRIORITY(FileWork, FILE_WORK);

    //
    // No more table create parameters to look for, finish up.
    //

    goto End;

AddParam:

    //
    // If there are already table create parameters, we may have already seen
    // a parameter with the same ID.  Loop through all the existing ones and
    // see if we can find a match.  If so, update the value directly and return;
    // avoiding the allocation/reallocation logic altogether.
    //

    Param = TableCreateParameters;
    for (Index = 0; Index < NumberOfTableCreateParameters; Index++) {
        if (Param->Id == LocalParam.Id) {

            //
            // Make sure we potentially deallocate the existing param if
            // applicable.
            //

            MaybeDeallocateTableCreateParameter(Allocator, Param);

            Param->AsULongLong = LocalParam.AsULongLong;
            return S_OK;
        }
    }

    if (NumberOfTableCreateParameters == 0) {

        NewTableCreateParameters = (
            Allocator->Vtbl->Calloc(
                Allocator,
                1,
                sizeof(*TableCreateParameters)
            )
        );

    } else {

        NewTableCreateParameters = (
            Allocator->Vtbl->ReCalloc(
                Allocator,
                TableCreateParameters,
                (ULONG_PTR)NumberOfTableCreateParameters + 1,
                sizeof(*TableCreateParameters)
            )
        );

    }

    if (!NewTableCreateParameters) {
        Result = E_OUTOFMEMORY;
        goto Error;
    }

    //
    // Write our new parameter to the end of the array.
    //

    Param = &NewTableCreateParameters[NumberOfTableCreateParameters];
    CopyMemory(Param, &LocalParam, sizeof(*Param));

    //
    // Update the caller's pointers and finish up.
    //

    *NumberOfTableCreateParametersPointer = NumberOfTableCreateParameters + 1;
    *TableCreateParametersPointer = NewTableCreateParameters;

    Result = S_OK;
    goto End;

Error:

    if (Result == S_FALSE) {
        Result = E_UNEXPECTED;
    }

    //
    // Intentional follow-on to End.
    //

End:

    return Result;
}


DESTROY_TABLE_CREATE_PARAMETERS DestroyTableCreateParameters;

_Use_decl_annotations_
HRESULT
DestroyTableCreateParameters(
    PALLOCATOR Allocator,
    ULONG NumberOfTableCreateParameters,
    PPERFECT_HASH_TABLE_CREATE_PARAMETER *TableCreateParametersPointer
    )
{
    ULONG Index;
    PPERFECT_HASH_TABLE_CREATE_PARAMETER Param;
    PPERFECT_HASH_TABLE_CREATE_PARAMETER Params;

    Param = Params = *TableCreateParametersPointer;

    for (Index = 0; Index < NumberOfTableCreateParameters; Index++, Param++) {
        MaybeDeallocateTableCreateParameter(Allocator, Param);
    }

    Allocator->Vtbl->FreePointer(Allocator, TableCreateParametersPointer);

    return S_OK;
}

// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :
