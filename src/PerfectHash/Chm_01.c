/*++

Copyright (c) 2018 Trent Nelson <trent@trent.me>

Module Name:

    Chm_01.c

Abstract:

    This module implements the CHM perfect hash table algorithm.  As a time
    saving measure, this module contains *everything* pertaining to the CHM
    implementation, including more general structures like hypergraphs etc.

    The general structures and functions can be moved out into their own
    modules at a later date if we author other algorithms that wish to use them.

    N.B. This implementation attempts to mirror the chm.c implementation as
         best it can, including but not limited to the underlying algorithm
         approach and function names.  This will be used as the baseline for
         evaluating the performance of subsequent revisions.

--*/

#include "stdafx.h"
#include "Chm_01.h"

_Use_decl_annotations_
BOOLEAN
CreatePerfectHashTableImplChm01(
    PPERFECT_HASH_TABLE Table
    )
/*++

Routine Description:

    Attempts to create a perfect hash table using the CHM algorithm and a
    2-part random hypergraph.

Arguments:

    Table - Supplies a pointer to a partially-initialized PERFECT_HASH_TABLE
        structure.

Return Value:

    TRUE on success, FALSE on failure.

--*/
{
    PRTL Rtl;
    USHORT Index;
    PULONG Keys;
    PGRAPH Graph;
    PBYTE Buffer;
    BOOLEAN Success;
    USHORT PageSize;
    USHORT PageShift;
    ULONG_PTR LastPage;
    ULONG_PTR ThisPage;
    BYTE NumberOfEvents;
    PVOID BaseAddress = NULL;
    ULONG WaitResult;
    GRAPH_INFO Info;
    PBYTE Unusable;
    ULONG NumberOfKeys;
    BOOLEAN CaughtException;
    PALLOCATOR Allocator;
    HANDLE ProcessHandle = NULL;
    PHANDLE Event;
    HRESULT Result;
    USHORT NumberOfGraphs;
    USHORT NumberOfGuardPages;
    ULONG NumberOfPagesPerGraph;
    ULONG TotalNumberOfPages;
    USHORT NumberOfBitmaps;
    PGRAPH_DIMENSIONS Dim;
    PSLIST_ENTRY ListEntry;
    SYSTEM_INFO SystemInfo;
    FILE_WORK_ITEM SaveFile;
    FILE_WORK_ITEM PrepareFile;
    PGRAPH_INFO_ON_DISK OnDiskInfo;
    PTABLE_INFO_ON_DISK_HEADER OnDiskHeader;
    ULONGLONG Closest;
    ULONGLONG LastClosest;
    ULONGLONG NextSizeInBytes;
    ULONGLONG PrevSizeInBytes;
    ULONGLONG FirstSizeInBytes;
    ULONGLONG EdgesSizeInBytes;
    ULONGLONG ValuesSizeInBytes;
    ULONGLONG AssignedSizeInBytes;
    ULONGLONG TotalBufferSizeInBytes;
    ULONGLONG UsableBufferSizeInBytesPerBuffer;
    ULONGLONG ExpectedTotalBufferSizeInBytes;
    ULONGLONG ExpectedUsableBufferSizeInBytesPerBuffer;
    ULONGLONG GraphSizeInBytesIncludingGuardPage;
    PERFECT_HASH_MASK_FUNCTION_ID MaskFunctionId;
    ULARGE_INTEGER AllocSize;
    ULARGE_INTEGER NumberOfEdges;
    ULARGE_INTEGER NumberOfVertices;
    ULARGE_INTEGER TotalNumberOfEdges;
    ULARGE_INTEGER DeletedEdgesBitmapBufferSizeInBytes;
    ULARGE_INTEGER VisitedVerticesBitmapBufferSizeInBytes;
    ULARGE_INTEGER AssignedBitmapBufferSizeInBytes;
    ULARGE_INTEGER IndexBitmapBufferSizeInBytes;
    PPERFECT_HASH_CONTEXT Context = Table->Context;
    HANDLE Events[5];

    //
    // Validate arguments.
    //

    if (!ARGUMENT_PRESENT(Table)) {
        return FALSE;
    }

    //
    // The following label is jumped to by code later in this routine when we
    // detect that we've exceeded a plausible number of attempts at finding a
    // graph solution with the given number of vertices, and have bumped up
    // the vertex count (by adjusting Table->RequestedNumberOfElements) and
    // want to try again.
    //

RetryWithLargerTableSize:

    //
    // Initialize aliases.
    //

    Rtl = Table->Rtl;
    Keys = (PULONG)Table->Keys->BaseAddress;
    Allocator = Table->Allocator;
    Context = Table->Context;
    MaskFunctionId = Context->MaskFunctionId;
    ProcessHandle = GetCurrentProcess();

    //
    // If no threshold has been set, use the default.
    //

    if (!Context->ResizeTableThreshold) {
        Context->ResizeTableThreshold = GRAPH_SOLVING_ATTEMPTS_THRESHOLD;
        Context->ResizeLimit = GRAPH_SOLVING_RESIZE_TABLE_LIMIT;
    }

    //
    // Explicitly reset all events.  This ensures everything is back in the
    // starting state if we happen to be attempting to solve the graph after
    // a resize event.
    //

    Event = (PHANDLE)&Context->FirstEvent;
    NumberOfEvents = GetNumberOfContextEvents(Context);

    for (Index = 0; Index < NumberOfEvents; Index++, Event++) {

        if (!ResetEvent(*Event)) {
            SYS_ERROR(ResetEvent);
            goto Error;
        }
    }

    //
    // The number of edges in our graph is equal to the number of keys in the
    // input data set if modulus masking is in use.  It will be rounded up to
    // a power of 2 otherwise.
    //

    NumberOfEdges.QuadPart = Table->Keys->NumberOfElements.QuadPart;

    //
    // Sanity check we're under MAX_ULONG.
    //

    ASSERT(!NumberOfEdges.HighPart);

    NumberOfKeys = NumberOfEdges.LowPart;

    //
    // Determine the number of vertices.  If we've reached here due to a resize
    // event, Table->RequestedNumberOfTableElements will be non-zero, and takes
    // precedence.  Otherwise, determine the vertices heuristically.
    //

    if (Table->RequestedNumberOfTableElements.QuadPart) {

        NumberOfVertices.QuadPart = (
            Table->RequestedNumberOfTableElements.QuadPart
        );

        if (IsModulusMasking(MaskFunctionId)) {

            //
            // Nothing more to do with modulus masking; we'll verify the number
            // of vertices below.
            //

            NOTHING;

        } else {

            //
            // For non-modulus masking, make sure the number of vertices are
            // rounded up to a power of 2.  The number of edges will be rounded
            // up to a power of 2 from the number of keys.
            //

            NumberOfVertices.QuadPart = (
                RoundUpPowerOf2(NumberOfVertices.LowPart)
            );

            NumberOfEdges.QuadPart = (
                RoundUpPowerOf2(NumberOfEdges.LowPart)
            );

        }

    } else {

        //
        // No table size was requested, so we need to determine how many
        // vertices to use heuristically.  The main factor is what type of
        // masking has been requested.  The chm.c implementation, which is
        // modulus based, uses a size multiplier (c) of 2.09, and calculates
        // the final size via ceil(nedges * (double)2.09).  We can avoid the
        // need for doubles and linking with a math library (to get ceil())
        // and just use ~2.25, which we can calculate by adding the result
        // of right shifting the number of edges by 1 to the result of left
        // shifting said edge count by 2 (simulating multiplication by 0.25).
        //
        // If we're dealing with modulus masking, this will be the exact number
        // of vertices used.  For other types of masking, we need the edges size
        // to be a power of 2, and the vertices size to be the next power of 2.
        //

        if (IsModulusMasking(MaskFunctionId)) {

            NumberOfVertices.QuadPart = NumberOfEdges.QuadPart << 1ULL;
            NumberOfVertices.QuadPart += NumberOfEdges.QuadPart >> 2ULL;

        } else {

            //
            // Round up the edges to a power of 2.
            //

            NumberOfEdges.QuadPart = RoundUpPowerOf2(NumberOfEdges.LowPart);

            //
            // Make sure we haven't overflowed.
            //

            ASSERT(!NumberOfEdges.HighPart);

            //
            // For the number of vertices, round the number of edges up to the
            // next power of 2.
            //

            NumberOfVertices.QuadPart = (
                RoundUpNextPowerOf2(NumberOfEdges.LowPart)
            );

        }
    }

    //
    // Another sanity check we haven't exceeded MAX_ULONG.
    //

    ASSERT(!NumberOfVertices.HighPart);

    //
    // The r-graph (r = 2) nature of this implementation results in various
    // arrays having twice the number of elements indicated by the edge count.
    // Capture this number now, as we need it in various size calculations.
    //

    TotalNumberOfEdges.QuadPart = NumberOfEdges.QuadPart;
    TotalNumberOfEdges.QuadPart <<= 1ULL;

    //
    // Another overflow sanity check.
    //

    ASSERT(!TotalNumberOfEdges.HighPart);

    //
    // Make sure vertices > edges.
    //

    ASSERT(NumberOfVertices.QuadPart > NumberOfEdges.QuadPart);

    //
    // Calculate the size required for the DeletedEdges bitmap buffer.  One
    // bit is used per TotalNumberOfEdges.  Convert the bits into bytes by
    // shifting right 3 (dividing by 8) then align it up to a 16 byte boundary.
    // We add 1 before shifting to account 1-based bitmaps vs 0-based indices.
    //

    DeletedEdgesBitmapBufferSizeInBytes.QuadPart = (
        ALIGN_UP(((ALIGN_UP(TotalNumberOfEdges.QuadPart + 1, 8)) >> 3), 16)
    );

    ASSERT(!DeletedEdgesBitmapBufferSizeInBytes.HighPart);

    //
    // Calculate the size required for the VisitedVertices bitmap buffer.  One
    // bit is used per NumberOfVertices.  Convert the bits into bytes by
    // shifting right 3 (dividing by 8) then align it up to a 16 byte boundary.
    // We add 1 before shifting to account 1-based bitmaps vs 0-based indices.
    //

    VisitedVerticesBitmapBufferSizeInBytes.QuadPart = (
        ALIGN_UP(((ALIGN_UP(NumberOfVertices.QuadPart + 1, 8)) >> 3), 16)
    );

    ASSERT(!VisitedVerticesBitmapBufferSizeInBytes.HighPart);

    //
    // Calculate the size required for the AssignedBitmap bitmap buffer.  One
    // bit is used per NumberOfVertices.  Convert the bits into bytes by shifting
    // right 3 (dividing by 8) then align it up to a 16 byte boundary.
    // We add 1 before shifting to account 1-based bitmaps vs 0-based indices.
    //

    AssignedBitmapBufferSizeInBytes.QuadPart = (
        ALIGN_UP(((ALIGN_UP(NumberOfVertices.QuadPart + 1, 8)) >> 3), 16)
    );

    ASSERT(!AssignedBitmapBufferSizeInBytes.HighPart);

    //
    // Calculate the size required for the IndexBitmap bitmap buffer.  One
    // bit is used per NumberOfVertices.  Convert the bits into bytes by shifting
    // right 3 (dividing by 8) then align it up to a 16 byte boundary.
    // We add 1 before shifting to account 1-based bitmaps vs 0-based indices.
    //

    IndexBitmapBufferSizeInBytes.QuadPart = (
        ALIGN_UP(((ALIGN_UP(NumberOfVertices.QuadPart + 1, 8)) >> 3), 16)
    );

    ASSERT(!IndexBitmapBufferSizeInBytes.HighPart);

    //
    // Calculate the sizes required for each of the arrays.  We collect them
    // into independent variables as it makes carving up the allocated buffer
    // easier down the track.
    //

    EdgesSizeInBytes = (
        ALIGN_UP_POINTER(sizeof(*Graph->Edges) * TotalNumberOfEdges.QuadPart)
    );

    NextSizeInBytes = (
        ALIGN_UP_POINTER(sizeof(*Graph->Next) * TotalNumberOfEdges.QuadPart)
    );

    FirstSizeInBytes = (
        ALIGN_UP_POINTER(sizeof(*Graph->First) * NumberOfVertices.QuadPart)
    );

    PrevSizeInBytes = (
        ALIGN_UP_POINTER(sizeof(*Graph->Prev) * TotalNumberOfEdges.QuadPart)
    );

    AssignedSizeInBytes = (
        ALIGN_UP_POINTER(sizeof(*Graph->Assigned) * NumberOfVertices.QuadPart)
    );

    //
    // Calculate the size required for the values array.  This is used as part
    // of verification, where we essentially do Insert(Key, Key) in combination
    // with bitmap tracking of assigned indices, which allows us to detect if
    // there are any colliding indices, and if so, what was the previous key
    // that mapped to the same index.
    //

    ValuesSizeInBytes = (
        ALIGN_UP_POINTER(sizeof(*Graph->Values) * NumberOfVertices.QuadPart)
    );

    //
    // Calculate the total size required for the underlying graph, such that
    // we can allocate memory via a single call to the allocator.
    //

    AllocSize.QuadPart = ALIGN_UP_POINTER(

        //
        // Account for the size of the graph structure.
        //

        sizeof(GRAPH) +

        //
        // Account for the size of the Graph->Edges array, which is double
        // sized.
        //

        EdgesSizeInBytes +

        //
        // Account for the size of the Graph->Next array; also double sized.
        //

        NextSizeInBytes +

        //
        // Account for the size of the Graph->First array.  This is sized
        // proportional to the number of vertices.
        //

        FirstSizeInBytes +

        //
        // Account for the size of the Graph->Prev array, also double sized.
        //

        PrevSizeInBytes +

        //
        // Account for Graph->Assigned array of vertices.
        //

        AssignedSizeInBytes +

        //
        // Account for the Table->Values array of values for the perfect hash
        // table, indexed via the result of the table's Index() method.
        //

        ValuesSizeInBytes +

        //
        // Account for the size of the bitmap buffer for Graph->DeletedEdges.
        //

        DeletedEdgesBitmapBufferSizeInBytes.QuadPart +

        //
        // Account for the size of the bitmap buffer for Graph->VisitedVertices.
        //

        VisitedVerticesBitmapBufferSizeInBytes.QuadPart +

        //
        // Account for the size of the bitmap buffer for Graph->AssignedBitmap.
        //

        AssignedBitmapBufferSizeInBytes.QuadPart +

        //
        // Account for the size of the bitmap buffer for Graph->IndexBitmap.
        //

        IndexBitmapBufferSizeInBytes.QuadPart +

        //
        // Keep a dummy 0 at the end such that the last item above can use an
        // addition sign at the end of it, which minimizes the diff churn when
        // adding a new size element.
        //

        0

    );

    //
    // Capture the number of bitmaps here, where it's close to the lines above
    // that indicate how many bitmaps we're dealing with.  The number of bitmaps
    // accounted for above should match this number.  Visually confirm this any
    // time a new bitmap buffer is accounted for.
    //
    // N.B. We ASSERT() in InitializeGraph() if we detect a mismatch between
    //      Info->NumberOfBitmaps and a local counter incremented each time
    //      we initialize a bitmap.
    //

    NumberOfBitmaps = 4;

    //
    // Sanity check the size hasn't overflowed.
    //

    ASSERT(!AllocSize.HighPart);

    //
    // Calculate the number of pages required by each graph, then extrapolate
    // the number of guard pages and total number of pages.  We currently use
    // 4KB for the page size (i.e. we're not using large pages).
    //

    PageSize = PAGE_SIZE;
    PageShift = (USHORT)TrailingZeros(PageSize);
    NumberOfGraphs = (USHORT)Context->MaximumConcurrency;
    NumberOfPagesPerGraph = BYTES_TO_PAGES(AllocSize.LowPart);
    NumberOfGuardPages = (USHORT)Context->MaximumConcurrency;
    TotalNumberOfPages = (
        (NumberOfGraphs * NumberOfPagesPerGraph) +
        NumberOfGuardPages
    );
    GraphSizeInBytesIncludingGuardPage = (
        (ULONGLONG)PageSize +
        ((ULONGLONG)NumberOfPagesPerGraph * (ULONGLONG)PageSize)
    );

    //
    // Create multiple buffers separated by guard pages using a single call
    // to VirtualAllocEx().
    //

    Result = Rtl->Vtbl->CreateMultipleBuffers(Rtl,
                                              &ProcessHandle,
                                              PageSize,
                                              NumberOfGraphs,
                                              NumberOfPagesPerGraph,
                                              NULL,
                                              NULL,
                                              &UsableBufferSizeInBytesPerBuffer,
                                              &TotalBufferSizeInBytes,
                                              &BaseAddress);

    if (FAILED(Result)) {
        goto Error;
    }

    //
    // N.B. Subsequent errors must 'goto Error' at this point to ensure our
    //      cleanup logic kicks in.
    //

    //
    // Assert the sizes returned by the buffer allocation match what we're
    // expecting.
    //

    ExpectedTotalBufferSizeInBytes = (
        (ULONGLONG)TotalNumberOfPages *
        (ULONGLONG)PageSize
    );

    ExpectedUsableBufferSizeInBytesPerBuffer = (
        (ULONGLONG)NumberOfPagesPerGraph *
        (ULONGLONG)PageSize
    );

    ASSERT(TotalBufferSizeInBytes == ExpectedTotalBufferSizeInBytes);
    ASSERT(UsableBufferSizeInBytesPerBuffer ==
           ExpectedUsableBufferSizeInBytesPerBuffer);

    //
    // Initialize the GRAPH_INFO structure with all the sizes captured earlier.
    // (We zero it first just to ensure any of the padding fields are cleared.)
    //

    ZeroStruct(Info);

    Info.PageSize = PageSize;
    Info.AllocSize = AllocSize.QuadPart;
    Info.Context = Context;
    Info.BaseAddress = BaseAddress;
    Info.NumberOfPagesPerGraph = NumberOfPagesPerGraph;
    Info.NumberOfGraphs = NumberOfGraphs;
    Info.NumberOfBitmaps = NumberOfBitmaps;
    Info.SizeOfGraphStruct = sizeof(GRAPH);
    Info.EdgesSizeInBytes = EdgesSizeInBytes;
    Info.NextSizeInBytes = NextSizeInBytes;
    Info.FirstSizeInBytes = FirstSizeInBytes;
    Info.PrevSizeInBytes = PrevSizeInBytes;
    Info.AssignedSizeInBytes = AssignedSizeInBytes;
    Info.ValuesSizeInBytes = ValuesSizeInBytes;
    Info.AllocSize = AllocSize.QuadPart;
    Info.FinalSize = UsableBufferSizeInBytesPerBuffer;

    Info.DeletedEdgesBitmapBufferSizeInBytes = (
        DeletedEdgesBitmapBufferSizeInBytes.QuadPart
    );

    Info.VisitedVerticesBitmapBufferSizeInBytes = (
        VisitedVerticesBitmapBufferSizeInBytes.QuadPart
    );

    Info.AssignedBitmapBufferSizeInBytes = (
        AssignedBitmapBufferSizeInBytes.QuadPart
    );

    Info.IndexBitmapBufferSizeInBytes = (
        IndexBitmapBufferSizeInBytes.QuadPart
    );

    //
    // Capture the system allocation granularity.  This is used to align the
    // backing memory maps used for the table array.
    //

    GetSystemInfo(&SystemInfo);
    Info.AllocationGranularity = SystemInfo.dwAllocationGranularity;

    //
    // Copy the dimensions over.
    //

    Dim = &Info.Dimensions;
    Dim->NumberOfEdges = NumberOfEdges.LowPart;
    Dim->TotalNumberOfEdges = TotalNumberOfEdges.LowPart;
    Dim->NumberOfVertices = NumberOfVertices.LowPart;

    Dim->NumberOfEdgesPowerOf2Exponent = (BYTE)(
        TrailingZeros64(RoundUpPowerOf2(NumberOfEdges.LowPart))
    );

    Dim->NumberOfEdgesNextPowerOf2Exponent = (BYTE)(
        TrailingZeros64(RoundUpNextPowerOf2(NumberOfEdges.LowPart))
    );

    Dim->NumberOfVerticesPowerOf2Exponent = (BYTE)(
        TrailingZeros64(RoundUpPowerOf2(NumberOfVertices.LowPart))
    );

    Dim->NumberOfVerticesNextPowerOf2Exponent = (BYTE)(
        TrailingZeros64(RoundUpNextPowerOf2(NumberOfVertices.LowPart))
    );

    //
    // If non-modulus masking is active, initialize the edge and vertex masks.
    //

    if (!IsModulusMasking(MaskFunctionId)) {

        Info.EdgeMask = NumberOfEdges.LowPart - 1;
        Info.VertexMask = NumberOfVertices.LowPart - 1;

        //
        // Sanity check our masks are correct: their popcnts should match the
        // exponent value identified above whilst filling out the dimensions
        // structure.
        //

        ASSERT(PopulationCount32(Info.EdgeMask) ==
               Dim->NumberOfEdgesPowerOf2Exponent);

        ASSERT(PopulationCount32(Info.VertexMask) ==
               Dim->NumberOfVerticesPowerOf2Exponent);

    }

    //
    // Set the Modulus, Size, Shift, Mask and Fold fields of the table, such
    // that the Hash and Mask vtbl functions operate correctly.
    //
    // N.B. Shift, Mask and Fold are meaningless for modulus masking.
    //
    // N.B. If you change these fields, you'll probably need to change something
    //      in LoadPerfectHashTableImplChm01() too.
    //

    Table->HashModulus = NumberOfVertices.LowPart;
    Table->IndexModulus = NumberOfEdges.LowPart;
    Table->HashSize = NumberOfVertices.LowPart;
    Table->IndexSize = NumberOfEdges.LowPart;
    Table->HashShift = TrailingZeros(Table->HashSize);
    Table->IndexShift = TrailingZeros(Table->IndexSize);
    Table->HashMask = (Table->HashSize - 1);
    Table->IndexMask = (Table->IndexSize - 1);
    Table->HashFold = Table->HashShift >> 3;
    Table->IndexFold = Table->IndexShift >> 3;

    //
    // Save the on-disk representation of the graph information.  This is a
    // smaller subset of data needed in order to load a previously-solved
    // graph as a perfect hash table.  The data resides in an NTFS stream named
    // :Info off the main perfect hash table file.  It will have been mapped for
    // us already at Table->InfoStreamBaseAddress.
    //

    OnDiskInfo = (PGRAPH_INFO_ON_DISK)Table->InfoStreamBaseAddress;
    ASSERT(OnDiskInfo);
    OnDiskHeader = &OnDiskInfo->Header;
    OnDiskHeader->Magic.LowPart = TABLE_INFO_ON_DISK_MAGIC_LOWPART;
    OnDiskHeader->Magic.HighPart = TABLE_INFO_ON_DISK_MAGIC_HIGHPART;
    OnDiskHeader->SizeOfStruct = sizeof(*OnDiskInfo);
    OnDiskHeader->Flags.AsULong = 0;
    OnDiskHeader->Concurrency = Context->MaximumConcurrency;
    OnDiskHeader->AlgorithmId = Context->AlgorithmId;
    OnDiskHeader->MaskFunctionId = Context->MaskFunctionId;
    OnDiskHeader->HashFunctionId = Context->HashFunctionId;
    OnDiskHeader->KeySizeInBytes = sizeof(ULONG);
    OnDiskHeader->HashSize = Table->HashSize;
    OnDiskHeader->IndexSize = Table->IndexSize;
    OnDiskHeader->HashShift = Table->HashShift;
    OnDiskHeader->IndexShift = Table->IndexShift;
    OnDiskHeader->HashMask = Table->HashMask;
    OnDiskHeader->IndexMask = Table->IndexMask;
    OnDiskHeader->HashFold = Table->HashFold;
    OnDiskHeader->IndexFold = Table->IndexFold;
    OnDiskHeader->HashModulus = Table->HashModulus;
    OnDiskHeader->IndexModulus = Table->IndexModulus;
    OnDiskHeader->NumberOfKeys.QuadPart = (
        Table->Keys->NumberOfElements.QuadPart
    );
    OnDiskHeader->NumberOfSeeds = ((
        FIELD_OFFSET(GRAPH, LastSeed) -
        FIELD_OFFSET(GRAPH, FirstSeed)
    ) / sizeof(ULONG)) + 1;

    //
    // This will change based on masking type and whether or not the caller
    // has provided a value for NumberOfTableElements.  For now, keep it as
    // the number of vertices.
    //

    OnDiskHeader->NumberOfTableElements.QuadPart = (
        NumberOfVertices.QuadPart
    );

    CopyMemory(&OnDiskInfo->Dimensions, Dim, sizeof(*Dim));

    //
    // Set the context's main work callback to our worker routine, and the algo
    // context to our graph info structure.
    //

    Context->MainWorkCallback = ProcessGraphCallbackChm01;
    Context->AlgorithmContext = &Info;

    //
    // Set the context's file work callback to our worker routine.
    //

    Context->FileWorkCallback = FileWorkCallbackChm01;

    //
    // Prepare the initial "file preparation" work callback.  This will extend
    // the backing file to the appropriate size.
    //

    ZeroStruct(PrepareFile);
    PrepareFile.FileWorkId = FileWorkPrepareId;
    InterlockedPushEntrySList(&Context->FileWorkListHead,
                              &PrepareFile.ListEntry);

    CONTEXT_START_TIMERS(PrepareFile);

    SubmitThreadpoolWork(Context->FileWork);

    //
    // Capture initial cycles as reported by __rdtsc() and the performance
    // counter.  The former is used to report a raw cycle count, the latter
    // is used to convert to microseconds reliably (i.e. unaffected by turbo
    // boosting).
    //

    QueryPerformanceFrequency(&Context->Frequency);

    CONTEXT_START_TIMERS(Solve);

    //
    // We're ready to create threadpool work for the graph.
    //

    Buffer = (PBYTE)BaseAddress;
    Unusable = Buffer;

    for (Index = 0; Index < NumberOfGraphs; Index++) {

        //
        // Invariant check: at the top of the loop, Buffer and Unusable should
        // point to the same address (which will be the base of the current
        // graph being processed).  Assert this invariant now.
        //

        ASSERT(Buffer == Unusable);

        //
        // Carve out the graph pointer, and bump the unusable pointer past the
        // graph's pages, such that it points to the first byte of the guard
        // page.
        //

        Graph = (PGRAPH)Buffer;
        Unusable = Buffer + UsableBufferSizeInBytesPerBuffer;

        //
        // Sanity check the page alignment logic.  If we subtract 1 byte from
        // Unusable, it should reside on a different page.  Additionally, the
        // two pages should be separated by at most a single page size.
        //

        ThisPage = ALIGN_DOWN(Unusable,   PageSize);
        LastPage = ALIGN_DOWN(Unusable-1, PageSize);
        ASSERT(LastPage < ThisPage);
        ASSERT((ThisPage - LastPage) == PageSize);

        //
        // Verify the guard page is working properly by wrapping an attempt to
        // write to it in a structured exception handler that will catch the
        // access violation trap.
        //
        // N.B. We only do this if we're not actively being debugged, as the
        //      traps get dispatched to the debugger engine first as part of
        //      the "first-pass" handling logic of the kernel.
        //

        if (!IsDebuggerPresent()) {

            CaughtException = FALSE;

            TRY_PROBE_MEMORY {

                *Unusable = 1;

            } CATCH_EXCEPTION_ACCESS_VIOLATION{

                CaughtException = TRUE;

            }

            ASSERT(CaughtException);
        }

        //
        // Guard page is working properly.  Push the graph onto the context's
        // main work list head and submit the corresponding threadpool work.
        //

        InterlockedPushEntrySList(&Context->MainWorkListHead,
                                  &Graph->ListEntry);
        SubmitThreadpoolWork(Context->MainWork);

        //
        // Advance the buffer past the graph size and guard page.  Copy the
        // same address to the Unusable variable as well, such that our top
        // of the loop invariants hold true.
        //

        Buffer += GraphSizeInBytesIncludingGuardPage;
        Unusable = Buffer;

        //
        // If our key set size is small and our maximum concurrency is large,
        // we may have already solved the graph, in which case, we can stop
        // submitting new solver attempts and just break out of the loop here.
        //

        if (!ShouldWeContinueTryingToSolveGraph(Context)) {
            break;
        }
    }

    //
    // Wait on the context's events.
    //

    Events[0] = Context->SucceededEvent;
    Events[1] = Context->CompletedEvent;
    Events[2] = Context->ShutdownEvent;
    Events[3] = Context->FailedEvent;
    Events[4] = Context->TryLargerTableSizeEvent;

    WaitResult = WaitForMultipleObjects(ARRAYSIZE(Events),
                                        Events,
                                        FALSE,
                                        INFINITE);

    //
    // If the wait result indicates the try larger table size event was set,
    // deal with that, first.
    //

    if (WaitResult == WAIT_OBJECT_0+4) {

        //
        // The number of attempts at solving this graph have exceeded the
        // threshold.  Set the shutdown event in order to trigger all worker
        // threads to abort their current attempts and wait on the main thread
        // work to complete.
        //

        SetEvent(Context->ShutdownEvent);
        WaitForThreadpoolWorkCallbacks(Context->MainWork, TRUE);

        //
        // Perform a blocking wait for the prepare file work to complete.  (It
        // would be highly unlikely that this event hasn't been set yet.)
        //

        WaitResult = WaitForSingleObject(Context->PreparedFileEvent, INFINITE);

        if (WaitResult != WAIT_OBJECT_0) {
            SYS_ERROR(WaitForSingleObject);
            goto Error;
        }

        //
        // There are no more threadpool callbacks running.  However, a thread
        // could have finished a solution between the time the try larger table
        // size event was set, and this point.  So, check the finished count
        // first.  If it indicates a solution, jump to that handler code.
        //

        if (Context->FinishedCount > 0) {
            goto FinishedSolution;
        }

        //
        // Destroy the existing buffer we allocated for this attempt.  We'll
        // need a new, larger one to accommodate the resize.
        //

        Result = Rtl->Vtbl->DestroyBuffer(Rtl, ProcessHandle, &BaseAddress);
        if (FAILED(Result)) {
            SYS_ERROR(VirtualFree);
            goto Error;
        }

        //
        // Increment the resize counter and update the total number of attempts
        // in the header.  Then, determine how close we came to solving the
        // graph, and store that in the header as well if it's the best so far
        // (or no previous version is present).
        //

        OnDiskHeader->NumberOfTableResizeEvents++;
        OnDiskHeader->TotalNumberOfAttemptsWithSmallerTableSizes += (
            Context->Attempts
        );

        Closest = NumberOfEdges.LowPart - Context->HighestDeletedEdgesCount;
        LastClosest = (
            OnDiskHeader->ClosestWeCameToSolvingGraphWithSmallerTableSizes
        );

        if (!LastClosest || Closest < LastClosest) {
            OnDiskHeader->ClosestWeCameToSolvingGraphWithSmallerTableSizes = (
                Closest
            );
        }

        //
        // If this is our first resize, capture the initial size we used.
        //

        if (!OnDiskHeader->InitialTableSize) {
            OnDiskHeader->InitialTableSize = NumberOfVertices.QuadPart;
        }

        //
        // Reset the remaining counters.
        //

        Context->Attempts = 0;
        Context->FailedAttempts = 0;
        Context->HighestDeletedEdgesCount = 0;

        //
        // Double the vertex count.  If we have overflowed max ULONG, abort.
        //

        Table->RequestedNumberOfTableElements.QuadPart = (
            NumberOfVertices.QuadPart
        );

        Table->RequestedNumberOfTableElements.QuadPart <<= 1ULL;

        if (Table->RequestedNumberOfTableElements.HighPart) {
            goto Error;
        }

        //
        // Unmap the existing mapping and close the section.
        //

        _Analysis_assume_(Table->BaseAddress != NULL);
        if (!UnmapViewOfFile(Table->BaseAddress)) {
            SYS_ERROR(UnmapViewOfFile);
            goto Error;
        }
        Table->BaseAddress = NULL;

        _Analysis_assume_(Table->MappingHandle != NULL);
        if (!CloseHandle(Table->MappingHandle)) {
            SYS_ERROR(CloseHandle);
            goto Error;
        }
        Table->MappingHandle = NULL;

        //
        // Jump back to the start and try again with a larger vertex count.
        //

        goto RetryWithLargerTableSize;
    }

    //
    // The wait result did not indicate a resize event.  Ignore the wait
    // result for now; determine if the graph solving was successful by the
    // finished count of the context.  We'll corroborate that with whatever
    // events have been signaled shortly.
    //

    Success = (Context->FinishedCount > 0);

    if (!Success) {

        BOOL CancelPending = TRUE;

        //
        // Invariant check: if no worker thread registered a solved graph (i.e.
        // Context->FinishedCount > 0), then verify that the shutdown event was
        // set.  If our WaitResult above indicates WAIT_OBJECT_2, we're done.
        // If not, verify explicitly.
        //

        if (WaitResult != WAIT_OBJECT_0+2) {

            //
            // Manually test that the shutdown event has been signaled.
            //

            WaitResult = WaitForSingleObject(Context->ShutdownEvent, 0);

            if (WaitResult != WAIT_OBJECT_0) {
                SYS_ERROR(WaitForSingleObject);
                goto Error;
            }
        }

        //
        // Wait for the main thread work group members.  This will block until
        // all the worker threads have returned.  We need to put this in place
        // prior to jumping to the End: label as that step will destroy the
        // buffer we allocated earlier for the parallel graphs, which we mustn't
        // do if any threads are still working.
        //

        WaitForThreadpoolWorkCallbacks(Context->MainWork, CancelPending);

        //
        // Perform the same operation for the file work threadpool.  Note that
        // the only work we've dispatched to this pool at this point is the
        // initial file preparation work.
        //

        WaitForThreadpoolWorkCallbacks(Context->FileWork, CancelPending);

        goto End;
    }

    //
    // Pop the winning graph off the finished list head.
    //

FinishedSolution:

    ListEntry = InterlockedPopEntrySList(&Context->FinishedWorkListHead);
    ASSERT(ListEntry);

    Graph = CONTAINING_RECORD(ListEntry, GRAPH, ListEntry);

    //
    // Note this graph as the one solved to the context.  This is used by the
    // save file work callback we dispatch below.
    //

    Context->SolvedContext = Graph;

    //
    // Graphs always pass verification in normal circumstances.  The only time
    // they don't is if there's an internal bug in our code.  So, knowing that
    // the graph is probably correct, we can dispatch the file work required to
    // save it to disk to the file work threadpool whilst we verify it has been
    // solved correctly.
    //

    ZeroStruct(SaveFile);
    SaveFile.FileWorkId = FileWorkSaveId;

    //
    // Before we dispatch the save file work, make sure the preparation has
    // completed.
    //

    WaitResult = WaitForSingleObject(Context->PreparedFileEvent, INFINITE);
    if (WaitResult != WAIT_OBJECT_0) {
        SYS_ERROR(WaitForSingleObject);
        goto Error;
    }

    if (Context->FileWorkErrors > 0) {
        goto Error;
    }

    //
    // Push this work item to the file work list head and submit the threadpool
    // work for it.
    //

    CONTEXT_START_TIMERS(SaveFile);

    InterlockedPushEntrySList(&Context->FileWorkListHead, &SaveFile.ListEntry);
    SubmitThreadpoolWork(Context->FileWork);

    //
    // Capture another round of cycles and performance counter values, then
    // continue with verification of the solution.
    //

    CONTEXT_START_TIMERS(Verify);

    Success = VerifySolvedGraph(Graph);

    CONTEXT_END_TIMERS(Verify);

    //
    // Set the verified event (regardless of whether or not we succeeded in
    // verification).  The save file work will be waiting upon it in order to
    // write the final timing details to the on-disk header.
    //

    if (!SetEvent(Context->VerifiedEvent)) {
        SYS_ERROR(SetEvent);
        goto Error;
    }

    if (!Success) {
        goto Error;
    }

    //
    // Wait on the saved file event before returning.
    //

    WaitResult = WaitForSingleObject(Context->SavedFileEvent, INFINITE);
    if (WaitResult != WAIT_OBJECT_0) {
        SYS_ERROR(WaitForSingleObject);
        goto Error;
    }

    if (Context->FileWorkErrors > 0) {
        goto Error;
    }

    //
    // We're done, indicate success and finish up.
    //

    Success = TRUE;
    goto End;

Error:

    Success = FALSE;

    //
    // Intentional follow-on to End.
    //

End:

    //
    // Destroy the buffer we created earlier.
    //
    // N.B. Although we used Rtl->CreateMultipleBuffers(), we can still free
    //      the underlying buffer via Rtl->DestroyBuffer(), as only a single
    //      VirtualAllocEx() call was dispatched for the entire buffer.
    //

    if (BaseAddress && ProcessHandle) {
        Result = Rtl->Vtbl->DestroyBuffer(Rtl, ProcessHandle, &BaseAddress);
        if (FAILED(Result)) {
            SYS_ERROR(VirtualFree);
            Success = FALSE;
        }
    }

    //
    // Explicitly reset all events before returning.
    //

    Event = (PHANDLE)&Context->FirstEvent;
    NumberOfEvents = GetNumberOfContextEvents(Context);

    for (Index = 0; Index < NumberOfEvents; Index++, Event++) {

        if (!ResetEvent(*Event)) {
            SYS_ERROR(ResetEvent);
            Success = FALSE;
        }
    }

    return Success;
}

_Use_decl_annotations_
BOOLEAN
LoadPerfectHashTableImplChm01(
    PPERFECT_HASH_TABLE Table
    )
/*++

Routine Description:

    Loads a previously created perfect hash table.

Arguments:

    Table - Supplies a pointer to a partially-initialized PERFECT_HASH_TABLE
        structure.

Return Value:

    TRUE on success, FALSE on failure.

--*/
{
    PTABLE_INFO_ON_DISK_HEADER Header;

    Header = Table->Header;

    Table->HashSize = Header->HashSize;
    Table->IndexSize = Header->IndexSize;
    Table->HashShift = Header->HashShift;
    Table->IndexShift = Header->IndexShift;
    Table->HashMask = Header->HashMask;
    Table->IndexMask = Header->IndexMask;
    Table->HashFold = Header->HashFold;
    Table->IndexFold = Header->IndexFold;
    Table->HashModulus = Header->HashModulus;
    Table->IndexModulus = Header->IndexModulus;

    return TRUE;
}

//
// The entry point into the actual per-thread solving attempts is the following
// routine.
//

_Use_decl_annotations_
VOID
ProcessGraphCallbackChm01(
    PTP_CALLBACK_INSTANCE Instance,
    PPERFECT_HASH_CONTEXT Context,
    PSLIST_ENTRY ListEntry
    )
/*++

Routine Description:

    This routine is the callback entry point for graph solving threads.  It
    will enter an infinite loop attempting to solve the graph; terminating
    only when the graph is solved or we detect another thread has solved it.

Arguments:

    Instance - Supplies a pointer to the callback instance for this invocation.

    Context - Supplies a pointer to the active context for the graph solving.

    ListEntry - Supplies a pointer to the list entry that was popped off the
        context's main work interlocked singly-linked list head.  The list
        entry will be the address of Graph->ListEntry, and thus, the Graph
        address can be obtained via the following CONTAINING_RECORD() construct:

            Graph = CONTAINING_RECORD(ListEntry, GRAPH, ListEntry);


Return Value:

    None.

--*/
{
    PRTL Rtl;
    PGRAPH Graph;
    ULONG Attempt = 0;
    PGRAPH_INFO Info;
    PRTL_FILL_PAGES FillPages;

    UNREFERENCED_PARAMETER(Instance);

    //
    // Resolve the graph base address from the list entry.  Nothing will be
    // filled in initially.
    //

    Graph = CONTAINING_RECORD(ListEntry, GRAPH, ListEntry);

    //
    // Resolve aliases.
    //

    Rtl = Context->Rtl;
    FillPages = Rtl->Vtbl->FillPages;

    //
    // The graph info structure will be stashed in the algo context field.
    //

    Info = (PGRAPH_INFO)Context->AlgorithmContext;

    //
    // Begin the solving loop.  InitializeGraph() generates new seed data,
    // so each loop iteration will be attempting to solve the graph uniquely.
    //

    while (ShouldWeContinueTryingToSolveGraph(Context)) {

        InitializeGraph(Info, Graph);

        Graph->ThreadAttempt = ++Attempt;

        if (SolveGraph(Graph)) {

            //
            // Hey, we were the ones to solve it, great!
            //

            break;
        }

        //
        // Our attempt at solving failed.  Zero all pages associated with the
        // graph and then try again with new seed data.
        //

        FillPages(Rtl, (PCHAR)Graph, 0, Info->NumberOfPagesPerGraph);

    }

    return;
}

_Use_decl_annotations_
VOID
FileWorkCallbackChm01(
    PTP_CALLBACK_INSTANCE Instance,
    PPERFECT_HASH_CONTEXT Context,
    PSLIST_ENTRY ListEntry
    )
/*++

Routine Description:

    This routine is the callback entry point for file-oriented work we want
    to perform in the file work threadpool context.

Arguments:

    Instance - Supplies a pointer to the callback instance for this invocation.

    Context - Supplies a pointer to the active context for the graph solving.

    ListEntry - Supplies a pointer to the list entry that was popped off the
        context's file work interlocked singly-linked list head.

Return Value:

    None.

--*/
{
    PRTL Rtl;
    HANDLE SavedEvent;
    HANDLE PreparedEvent;
    HANDLE SetOnReturnEvent;
    PGRAPH_INFO Info;
    PFILE_WORK_ITEM Item;
    PGRAPH_INFO_ON_DISK OnDiskInfo;

    //
    // Initialize aliases.
    //

    Rtl = Context->Rtl;
    Info = (PGRAPH_INFO)Context->AlgorithmContext;
    SavedEvent = Context->SavedFileEvent;
    PreparedEvent = Context->PreparedFileEvent;
    OnDiskInfo = (PGRAPH_INFO_ON_DISK)Context->Table->InfoStreamBaseAddress;

    //
    // Resolve the work item base address from the list entry.
    //

    Item = CONTAINING_RECORD(ListEntry, FILE_WORK_ITEM, ListEntry);

    ASSERT(IsValidFileWorkId(Item->FileWorkId));

    switch (Item->FileWorkId) {

        case FileWorkPrepareId: {

            PVOID BaseAddress;
            HANDLE MappingHandle;
            PPERFECT_HASH_TABLE Table;
            ULARGE_INTEGER SectorAlignedSize;

            //
            // Indicate we've completed the preparation work when this callback
            // completes.
            //

            SetOnReturnEvent = PreparedEvent;

            //
            // We need to extend the file to accommodate for the solved graph.
            //

            SectorAlignedSize.QuadPart = ALIGN_UP(Info->AssignedSizeInBytes,
                                                  Info->AllocationGranularity);

            Table = Context->Table;

            //
            // Create the file mapping for the sector-aligned size.  This will
            // extend the underlying file size accordingly.
            //

            MappingHandle = CreateFileMappingW(Table->FileHandle,
                                               NULL,
                                               PAGE_READWRITE,
                                               SectorAlignedSize.HighPart,
                                               SectorAlignedSize.LowPart,
                                               NULL);

            Table->MappingHandle = MappingHandle;

            if (!MappingHandle || MappingHandle == INVALID_HANDLE_VALUE) {
                SYS_ERROR(CreateFileMappingW);
                goto Error;
            }

            BaseAddress = MapViewOfFile(MappingHandle,
                                        FILE_MAP_READ | FILE_MAP_WRITE,
                                        0,
                                        0,
                                        SectorAlignedSize.QuadPart);

            Table->BaseAddress = BaseAddress;

            if (!BaseAddress) {
                SYS_ERROR(MapViewOfFile);
                goto Error;
            }

            CONTEXT_END_TIMERS(PrepareFile);

            //
            // We've successfully mapped an area of sufficient space to store
            // the underlying table array if a perfect hash table solution is
            // found.  Nothing more to do.
            //

            break;
        }

        case FileWorkSaveId: {

            BOOL Success;
            PULONG Dest;
            PULONG Source;
            PGRAPH Graph;
            ULONG WaitResult;
            ULONGLONG SizeInBytes;
            LARGE_INTEGER EndOfFile;
            PPERFECT_HASH_TABLE Table;
            PTABLE_INFO_ON_DISK_HEADER Header;

            //
            // Indicate the save event has completed upon return of this
            // callback.
            //

            SetOnReturnEvent = SavedEvent;

            //
            // Initialize aliases.
            //

            Table = Context->Table;
            Dest = (PULONG)Table->BaseAddress;
            Graph = (PGRAPH)Context->SolvedContext;
            Source = Graph->Assigned;
            Header = Table->Header;

            SizeInBytes = (
                Header->NumberOfTableElements.QuadPart *
                Header->KeySizeInBytes
            );

            //
            // The graph has been solved.  Copy the array of assigned values
            // to the mapped area we prepared earlier (above).
            //

            CopyMemory(Dest, Source, SizeInBytes);

            //
            // Save the seed values used by this graph.  (Everything else in
            // the on-disk info representation was saved earlier.)
            //

            Header->Seed1 = Graph->Seed1;
            Header->Seed2 = Graph->Seed2;
            Header->Seed3 = Graph->Seed3;
            Header->Seed4 = Graph->Seed4;

            //
            // Kick off a flush file buffers now before we wait on the verified
            // event.  The flush will be a blocking call.  The wait on verified
            // will be blocking if the event isn't signaled.  So, we may as well
            // get some useful blocking work done, before potentially going into
            // another wait state where we're not doing anything useful.
            //

            if (!FlushFileBuffers(Table->FileHandle)) {
                SYS_ERROR(FlushFileBuffers);
                goto Error;
            }

            //
            // Stop the save file timer here, after flushing the file buffers,
            // but before we potentially wait on the verified state.
            //

            CONTEXT_END_TIMERS(SaveFile);

            //
            // Wait on the verification complete event.  This is done in the
            // main thread straight after it dispatches our file work callback
            // (that ended up here).  We need to block on this event as we want
            // to save the timings for verification to the header.
            //

            WaitResult = WaitForSingleObject(Context->VerifiedEvent, INFINITE);
            if (WaitResult != WAIT_OBJECT_0) {
                SYS_ERROR(WaitForSingleObject);
                goto Error;
            }

            //
            // When we mapped the array in the work item above, we used a size
            // that was aligned with the system allocation granularity.  We now
            // want to set the end of file explicitly to the exact size of the
            // underlying array.  To do this, we unmap the view, delete the
            // section, set the file pointer to where we want, set the end of
            // file (which will apply the file pointer position as EOF), then
            // close the file handle.
            //

            if (!UnmapViewOfFile(Table->BaseAddress)) {
                SYS_ERROR(UnmapViewOfFile);
                goto Error;
            }
            Table->BaseAddress = NULL;

            if (!CloseHandle(Table->MappingHandle)) {
                SYS_ERROR(UnmapViewOfFile);
                goto Error;
            }
            Table->MappingHandle = NULL;

            EndOfFile.QuadPart = SizeInBytes;

            Success = SetFilePointerEx(Table->FileHandle,
                                       EndOfFile,
                                       NULL,
                                       FILE_BEGIN);

            if (!Success) {
                SYS_ERROR(SetFilePointerEx);
                goto Error;
            }

            if (!SetEndOfFile(Table->FileHandle)) {
                SYS_ERROR(SetEndOfFile);
                goto Error;
            }

            if (!CloseHandle(Table->FileHandle)) {
                SYS_ERROR(CloseHandle);
                goto Error;
            }

            Table->FileHandle = NULL;

            //
            // Stop the save file timers, then copy all the timer values into
            // the header (before closing the :Info stream).
            //

            CONTEXT_SAVE_TIMERS_TO_HEADER(Solve);
            CONTEXT_SAVE_TIMERS_TO_HEADER(Verify);
            CONTEXT_SAVE_TIMERS_TO_HEADER(PrepareFile);
            CONTEXT_SAVE_TIMERS_TO_HEADER(SaveFile);

            //
            // Save the number of attempts and number of finished solutions.
            //

            Header->NumberOfAttempts = Context->Attempts;
            Header->NumberOfFailedAttempts = Context->FailedAttempts;
            Header->NumberOfSolutionsFound = Context->FinishedCount;

            //
            // Finalize the :Info stream the same way we handled the backing
            // file above; unmap, delete section, set file pointer, set eof,
            // close file.
            //

            if (!UnmapViewOfFile(Table->InfoStreamBaseAddress)) {
                SYS_ERROR(UnmapViewOfFile);
                goto Error;
            }
            Table->InfoStreamBaseAddress = NULL;

            if (!CloseHandle(Table->InfoStreamMappingHandle)) {
                SYS_ERROR(CloseHandle);
                goto Error;
            }
            Table->InfoStreamMappingHandle = NULL;

            //
            // The file size for the :Info stream will be the size of our
            // on-disk info structure.
            //

            EndOfFile.QuadPart = sizeof(*OnDiskInfo);

            Success = SetFilePointerEx(Table->InfoStreamFileHandle,
                                       EndOfFile,
                                       NULL,
                                       FILE_BEGIN);

            if (!Success) {
                SYS_ERROR(SetFilePointerEx);
                goto Error;
            }

            if (!SetEndOfFile(Table->InfoStreamFileHandle)) {
                SYS_ERROR(SetEndOfFile);
                goto Error;
            }

            if (!CloseHandle(Table->InfoStreamFileHandle)) {
                SYS_ERROR(CloseHandle);
                goto Error;
            }

            Table->InfoStreamFileHandle = NULL;

            break;
        }

        default:

            //
            // Should never get here.
            //

            ASSERT(FALSE);
            return;

    }

    //
    // We're done, jump to the end.
    //

    goto End;

Error:

    InterlockedIncrement((PLONG)&Context->FileWorkErrors);
    Context->FileWorkLastError = GetLastError();

    //
    // Intentional follow-on to End.
    //

End:

    //
    // Register the relevant event to be set when this threadpool callback
    // returns, then return.
    //

    SetEventWhenCallbackReturns(Instance, SetOnReturnEvent);

    return;
}

SHOULD_WE_CONTINUE_TRYING_TO_SOLVE_GRAPH ShouldWeContinueTryingToSolveGraph;

BOOLEAN
ShouldWeContinueTryingToSolveGraph(
    PPERFECT_HASH_CONTEXT Context
    )
{
    ULONG WaitResult;
    HANDLE Events[4];
    USHORT NumberOfEvents = ARRAYSIZE(Events);

    Events[0] = Context->ShutdownEvent;
    Events[1] = Context->SucceededEvent;
    Events[2] = Context->FailedEvent;
    Events[3] = Context->CompletedEvent;

    //
    // Fast-path exit: if the finished count is not 0, then someone has already
    // solved the solution, and we don't need to wait on any of the events.
    //

    if (Context->FinishedCount > 0) {
        return FALSE;
    }

    //
    // N.B. We should probably switch this to simply use volatile field of the
    //      context structure to indicate whether or not the context is active.
    //      WaitForMultipleObjects() on four events seems a bit... excessive.
    //

    WaitResult = WaitForMultipleObjects(NumberOfEvents,
                                        Events,
                                        FALSE,
                                        0);

    //
    // The only situation where we continue attempting to solve the graph is
    // if the result from the wait is WAIT_TIMEOUT, which indicates none of
    // the events have been set.  We treat any other situation as an indication
    // to stop processing.  (This includes wait failures and abandonment.)
    //

    return (WaitResult == WAIT_TIMEOUT ? TRUE : FALSE);
}

_Use_decl_annotations_
HRESULT
PerfectHashTableIndexImplChm01(
    PPERFECT_HASH_TABLE Table,
    ULONG Key,
    PULONG Index
    )
/*++

Routine Description:

    Looks up given key in a perfect hash table and returns its index.

    N.B. If Key did not appear in the original set the hash table was created
         from, the behavior of this routine is undefined.  (In practice, the
         key will hash to either an existing key's location or an empty slot,
         so there is potential for returning a non-unique index.)

Arguments:

    Table - Supplies a pointer to the table for which the key lookup is to be
        performed.

    Key - Supplies the key to look up.

    Index - Receives the index associated with this key.  The index will be
        between 0 and Table->HashSize-1, and can be safely used to offset
        directly into an appropriately sized array (e.g. Table->Values[]).

Return Value:

    S_OK on success, E_FAIL if the underlying hash function returned a failure.
    This will happen if the two hash values for a key happen to be identical.
    It shouldn't happen once a perfect graph has been created (i.e. it only
    happens when attempting to solve the graph).  The Index parameter will
    be cleared in the case of E_FAIL.

--*/
{
    ULONG Masked;
    ULONG Vertex1;
    ULONG Vertex2;
    ULONG MaskedLow;
    ULONG MaskedHigh;
    PULONG Assigned;
    ULONGLONG Combined;
    ULARGE_INTEGER Hash;

    //
    // Hash the incoming key into the 64-bit representation, which is two
    // 32-bit ULONGs in disguise, each one driven by a separate seed value.
    //

    if (FAILED(Table->Vtbl->Hash(Table, Key, &Hash.QuadPart))) {
        goto Error;
    }

    //
    // Mask each hash value such that it falls within the confines of the
    // number of vertices.  That is, make sure the value is between 0 and
    // Table->NumberOfVertices-1.
    //

    if (FAILED(Table->Vtbl->MaskHash(Table, Hash.LowPart, &MaskedLow))) {
        goto Error;
    }

    if (FAILED(Table->Vtbl->MaskHash(Table, Hash.HighPart, &MaskedHigh))) {
        goto Error;
    }

    //
    // Obtain the corresponding vertex values for the masked high and low hash
    // values.  These are derived from the "assigned" array that we construct
    // during the creation routine's assignment step (GraphAssign()).
    //

    Assigned = Table->Data;

    Vertex1 = Assigned[MaskedLow];
    Vertex2 = Assigned[MaskedHigh];

    //
    // Combine the two values, then perform the index masking operation, such
    // that our final index into the array falls within the confines of the
    // number of edges, or keys, in the table.  That is, make sure the index
    // value is between 0 and Table->Keys->NumberOfElements-1.
    //

    Combined = (ULONGLONG)Vertex1 + (ULONGLONG)Vertex2;

    if (FAILED(Table->Vtbl->MaskIndex(Table, Combined, &Masked))) {
        goto Error;
    }

    //
    // Update the caller's pointer and return success.  The resulting index
    // value represents the array offset index for this given key in the
    // underlying table, and is guaranteed to be unique amongst the original
    // keys in the input set.
    //

    *Index = Masked;
    return S_OK;

Error:

    //
    // Clear the caller's pointer and return failure.  We should only hit this
    // point if the caller supplies a key that both: a) wasn't in the original
    // input set, and b) happens to result in a hash value where both the high
    // part and low part are identical, which is rare, but not impossible.
    //

    *Index = 0;
    return E_FAIL;
}

_Use_decl_annotations_
HRESULT
PerfectHashTableFastIndexImplChm01Crc32RotateHashAndMask(
    PPERFECT_HASH_TABLE Table,
    ULONG Key,
    PULONG Index
    )
/*++

Routine Description:

    Looks up given key in a perfect hash table and returns its index.  This
    is a fast version of the normal Index() routine that inlines the Crc32Rotate
    hash function and AND masking.

    N.B. If Key did not appear in the original set the hash table was created
         from, the behavior of this routine is undefined.  (In practice, the
         key will hash to either an existing key's location or an empty slot,
         so there is potential for returning a non-unique index.)

Arguments:

    Table - Supplies a pointer to the table for which the key lookup is to be
        performed.

    Key - Supplies the key to look up.

    Index - Receives the index associated with this key.  The index will be
        between 0 and Table->HashSize-1, and can be safely used to offset
        directly into an appropriately sized array (e.g. Table->Values[]).

Return Value:

    S_OK on success, E_FAIL if the underlying hash function returned a failure.
    This will happen if the two hash values for a key happen to be identical.
    It shouldn't happen once a perfect graph has been created (i.e. it only
    happens when attempting to solve the graph).  The Index parameter will
    be cleared in the case of E_FAIL.

--*/
{
    ULONG A;
    ULONG B;
    ULONG C;
    ULONG D;
    ULONG Seed1;
    ULONG Seed2;
    ULONG Seed3;
    ULONG Input;
    PULONG Seeds;
    ULONG Masked;
    ULONG Vertex1;
    ULONG Vertex2;
    PULONG Assigned;
    ULONG MaskedLow;
    ULONG MaskedHigh;
    ULONGLONG Combined;

    //IACA_VC_START();

    //
    // Initialize aliases.
    //

    Seeds = &Table->Header->FirstSeed;
    Seed1 = Seeds[0];
    Seed2 = Seeds[1];
    Seed3 = Seeds[2];
    Input = Key;
    Assigned = Table->Data;

    //
    // Calculate the individual hash parts.
    //

    A = _mm_crc32_u32(Seed1, Input);
    B = _mm_crc32_u32(Seed2, _rotl(Input, 15));
    C = Seed3 ^ Input;
    D = _mm_crc32_u32(B, C);

    //IACA_VC_END();

    Vertex1 = A;
    Vertex2 = D;

    if (Vertex1 == Vertex2) {
        goto Error;
    }

    //
    // Mask each hash value such that it falls within the confines of the
    // number of vertices.
    //

    MaskedLow = Vertex1 & Table->HashMask;
    MaskedHigh = Vertex2 & Table->HashMask;

    //
    // Obtain the corresponding vertex values for the masked high and low hash
    // values.  These are derived from the "assigned" array that we construct
    // during the creation routine's assignment step (GraphAssign()).
    //

    Vertex1 = Assigned[MaskedLow];
    Vertex2 = Assigned[MaskedHigh];

    //
    // Combine the two values, then perform the index masking operation, such
    // that our final index into the array falls within the confines of the
    // number of edges, or keys, in the table.  That is, make sure the index
    // value is between 0 and Table->Keys->NumberOfElements-1.
    //

    Combined = (ULONGLONG)Vertex1 + (ULONGLONG)Vertex2;

    Masked = Combined & Table->IndexMask;

    //
    // Update the caller's pointer and return success.  The resulting index
    // value represents the array offset index for this given key in the
    // underlying table, and is guaranteed to be unique amongst the original
    // keys in the input set.
    //

    *Index = Masked;

    //IACA_VC_END();

    return S_OK;

Error:

    //
    // Clear the caller's pointer and return failure.  We should only hit this
    // point if the caller supplies a key that both: a) wasn't in the original
    // input set, and b) happens to result in a hash value where both the high
    // part and low part are identical, which is rare, but not impossible.
    //

    *Index = 0;
    return E_FAIL;
}

_Use_decl_annotations_
HRESULT
PerfectHashTableFastIndexImplChm01JenkinsHashAndMask(
    PPERFECT_HASH_TABLE Table,
    ULONG Key,
    PULONG Index
    )
/*++

Routine Description:

    Looks up given key in a perfect hash table and returns its index.  This
    is a fast version of the normal Index() routine that inlines the Jenkins
    hash function and AND masking.

    N.B. If Key did not appear in the original set the hash table was created
         from, the behavior of this routine is undefined.  (In practice, the
         key will hash to either an existing key's location or an empty slot,
         so there is potential for returning a non-unique index.)

Arguments:

    Table - Supplies a pointer to the table for which the key lookup is to be
        performed.

    Key - Supplies the key to look up.

    Index - Receives the index associated with this key.  The index will be
        between 0 and Table->HashSize-1, and can be safely used to offset
        directly into an appropriately sized array (e.g. Table->Values[]).

Return Value:

    S_OK on success, E_FAIL if the underlying hash function returned a failure.
    This will happen if the two hash values for a key happen to be identical.
    It shouldn't happen once a perfect graph has been created (i.e. it only
    happens when attempting to solve the graph).  The Index parameter will
    be cleared in the case of E_FAIL.

--*/
{
    ULONG A;
    ULONG B;
    ULONG C;
    ULONG D;
    ULONG E;
    ULONG F;
    PBYTE Byte;
    ULONG Seed1;
    ULONG Seed2;
    ULONG Input;
    PULONG Seeds;
    ULONG Masked;
    ULONG Vertex1;
    ULONG Vertex2;
    PULONG Assigned;
    ULONG MaskedLow;
    ULONG MaskedHigh;
    ULONGLONG Combined;

    //
    // Initialize aliases.
    //

    //IACA_VC_START();

    Seeds = &Table->Header->FirstSeed;
    Seed1 = Seeds[0];
    Seed2 = Seeds[1];
    Input = Key;

    Byte = (PBYTE)&Input;

    //
    // Generate the first hash.
    //

    A = B = 0x9e3779b9;
    C = Seed1;

    A += (((ULONG)Byte[3]) << 24);
    A += (((ULONG)Byte[2]) << 16);
    A += (((ULONG)Byte[1]) <<  8);
    A += ((ULONG)Byte[0]);

    A -= B; A -= C; A ^= (C >> 13);
    B -= C; B -= A; B ^= (A <<  8);
    C -= A; C -= B; C ^= (B >> 13);
    A -= B; A -= C; A ^= (C >> 12);
    B -= C; B -= A; B ^= (A << 16);
    C -= A; C -= B; C ^= (B >>  5);
    A -= B; A -= C; A ^= (C >>  3);
    B -= C; B -= A; B ^= (A << 10);
    C -= A; C -= B; C ^= (B >> 15);

    Vertex1 = C;

    //
    // Generate the second hash.
    //

    D = E = 0x9e3779b9;
    F = Seed2;

    D += (((ULONG)Byte[3]) << 24);
    D += (((ULONG)Byte[2]) << 16);
    D += (((ULONG)Byte[1]) <<  8);
    D += ((ULONG)Byte[0]);

    D -= E; D -= F; D ^= (F >> 13);
    E -= F; E -= D; E ^= (D <<  8);
    F -= D; F -= E; F ^= (E >> 13);
    D -= E; D -= F; D ^= (F >> 12);
    E -= F; E -= D; E ^= (D << 16);
    F -= D; F -= E; F ^= (E >>  5);
    D -= E; D -= F; D ^= (F >>  3);
    E -= F; E -= D; E ^= (D << 10);
    F -= D; F -= E; F ^= (E >> 15);

    //IACA_VC_END();

    Vertex2 = F;

    if (Vertex1 == Vertex2) {
        goto Error;
    }

    //
    // Mask each hash value such that it falls within the confines of the
    // number of vertices.
    //

    MaskedLow = Vertex1 & Table->HashMask;
    MaskedHigh = Vertex2 & Table->HashMask;

    //
    // Obtain the corresponding vertex values for the masked high and low hash
    // values.  These are derived from the "assigned" array that we construct
    // during the creation routine's assignment step (GraphAssign()).
    //

    Assigned = Table->Data;

    Vertex1 = Assigned[MaskedLow];
    Vertex2 = Assigned[MaskedHigh];

    //
    // Combine the two values, then perform the index masking operation, such
    // that our final index into the array falls within the confines of the
    // number of edges, or keys, in the table.  That is, make sure the index
    // value is between 0 and Table->Keys->NumberOfElements-1.
    //

    Combined = (ULONGLONG)Vertex1 + (ULONGLONG)Vertex2;

    Masked = Combined & Table->IndexMask;

    //
    // Update the caller's pointer and return success.  The resulting index
    // value represents the array offset index for this given key in the
    // underlying table, and is guaranteed to be unique amongst the original
    // keys in the input set.
    //

    *Index = Masked;

    //IACA_VC_END();

    return S_OK;

Error:

    //
    // Clear the caller's pointer and return failure.  We should only hit this
    // point if the caller supplies a key that both: a) wasn't in the original
    // input set, and b) happens to result in a hash value where both the high
    // part and low part are identical, which is rare, but not impossible.
    //

    *Index = 0;
    return E_FAIL;
}

PERFECT_HASH_TABLE_INDEX PerfectHashTableFastIndexImplChm01JenkinsHashModMask;

_Use_decl_annotations_
HRESULT
PerfectHashTableFastIndexImplChm01JenkinsHashModMask(
    PPERFECT_HASH_TABLE Table,
    ULONG Key,
    PULONG Index
    )
/*++

Routine Description:

    Looks up given key in a perfect hash table and returns its index.

    N.B. This version is based off the Jenkins hash function and modulus
         masking.  As we don't use modulus masking at all, it's not intended
         to be used in reality.  However, it's useful to feed to IACA to see
         the impact of the modulus operation.

Arguments:

    Table - Supplies a pointer to the table for which the key lookup is to be
        performed.

    Key - Supplies the key to look up.

    Index - Receives the index associated with this key.  The index will be
        between 0 and Table->HashSize-1, and can be safely used to offset
        directly into an appropriately sized array (e.g. Table->Values[]).

Return Value:

    S_OK on success, E_FAIL if the underlying hash function returned a failure.
    This will happen if the two hash values for a key happen to be identical.
    It shouldn't happen once a perfect graph has been created (i.e. it only
    happens when attempting to solve the graph).  The Index parameter will
    be cleared in the case of E_FAIL.

--*/
{
    ULONG A;
    ULONG B;
    ULONG C;
    ULONG D;
    ULONG E;
    ULONG F;
    PBYTE Byte;
    ULONG Seed1;
    ULONG Seed2;
    ULONG Input;
    PULONG Seeds;
    ULONG Masked;
    ULONG Vertex1;
    ULONG Vertex2;
    PULONG Assigned;
    ULONG MaskedLow;
    ULONG MaskedHigh;
    ULONGLONG Combined;

    //
    // Initialize aliases.
    //

    //IACA_VC_START();

    Seeds = &Table->Header->FirstSeed;
    Seed1 = Seeds[0];
    Seed2 = Seeds[1];
    Input = Key;

    Byte = (PBYTE)&Input;

    //
    // Generate the first hash.
    //

    A = B = 0x9e3779b9;
    C = Seed1;

    A += (((ULONG)Byte[3]) << 24);
    A += (((ULONG)Byte[2]) << 16);
    A += (((ULONG)Byte[1]) <<  8);
    A += ((ULONG)Byte[0]);

    A -= B; A -= C; A ^= (C >> 13);
    B -= C; B -= A; B ^= (A <<  8);
    C -= A; C -= B; C ^= (B >> 13);
    A -= B; A -= C; A ^= (C >> 12);
    B -= C; B -= A; B ^= (A << 16);
    C -= A; C -= B; C ^= (B >>  5);
    A -= B; A -= C; A ^= (C >>  3);
    B -= C; B -= A; B ^= (A << 10);
    C -= A; C -= B; C ^= (B >> 15);

    Vertex1 = C;

    //
    // Generate the second hash.
    //

    D = E = 0x9e3779b9;
    F = Seed2;

    D += (((ULONG)Byte[3]) << 24);
    D += (((ULONG)Byte[2]) << 16);
    D += (((ULONG)Byte[1]) <<  8);
    D += ((ULONG)Byte[0]);

    D -= E; D -= F; D ^= (F >> 13);
    E -= F; E -= D; E ^= (D <<  8);
    F -= D; F -= E; F ^= (E >> 13);
    D -= E; D -= F; D ^= (F >> 12);
    E -= F; E -= D; E ^= (D << 16);
    F -= D; F -= E; F ^= (E >>  5);
    D -= E; D -= F; D ^= (F >>  3);
    E -= F; E -= D; E ^= (D << 10);
    F -= D; F -= E; F ^= (E >> 15);

    //IACA_VC_END();

    Vertex2 = F;

    if (Vertex1 == Vertex2) {
        goto Error;
    }

    //
    // Mask each hash value such that it falls within the confines of the
    // number of vertices.
    //

    MaskedLow = Vertex1 % Table->HashModulus;
    MaskedHigh = Vertex2 % Table->HashModulus;

    //
    // Obtain the corresponding vertex values for the masked high and low hash
    // values.  These are derived from the "assigned" array that we construct
    // during the creation routine's assignment step (GraphAssign()).
    //

    Assigned = Table->Data;

    Vertex1 = Assigned[MaskedLow];
    Vertex2 = Assigned[MaskedHigh];

    //
    // Combine the two values, then perform the index masking operation, such
    // that our final index into the array falls within the confines of the
    // number of edges, or keys, in the table.  That is, make sure the index
    // value is between 0 and Table->Keys->NumberOfElements-1.
    //

    Combined = (ULONGLONG)Vertex1 + (ULONGLONG)Vertex2;

    Masked = Combined % Table->IndexModulus;

    //
    // Update the caller's pointer and return success.  The resulting index
    // value represents the array offset index for this given key in the
    // underlying table, and is guaranteed to be unique amongst the original
    // keys in the input set.
    //

    *Index = Masked;

    //IACA_VC_END();

    return S_OK;

Error:

    //
    // Clear the caller's pointer and return failure.  We should only hit this
    // point if the caller supplies a key that both: a) wasn't in the original
    // input set, and b) happens to result in a hash value where both the high
    // part and low part are identical, which is rare, but not impossible.
    //

    *Index = 0;
    return E_FAIL;
}

// vim:set ts=8 sw=4 sts=4 tw=80 expandtab                                     :