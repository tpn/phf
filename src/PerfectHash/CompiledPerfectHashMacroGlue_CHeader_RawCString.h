//
// Auto-generated.
//

DECLSPEC_ALIGN(16)
const CHAR CompiledPerfectHashMacroGlueCHeaderRawCStr[] =
    "\n"
    "//\n"
    "// Begin CompiledPerfectHashMacroGlue.h.\n"
    "//\n"
    "\n"
    "\n"
    "#define CPH_SEED1(U) U##_SEED1\n"
    "#define CPH_SEED2(U) U##_SEED2\n"
    "#define CPH_SEED3(U) U##_SEED3\n"
    "#define CPH_SEED4(U) U##_SEED4\n"
    "\n"
    "#define CPH_SEED1_BYTE1(U) U##_SEED1_BYTE1\n"
    "#define CPH_SEED1_BYTE2(U) U##_SEED1_BYTE2\n"
    "#define CPH_SEED1_BYTE3(U) U##_SEED1_BYTE3\n"
    "#define CPH_SEED1_BYTE4(U) U##_SEED1_BYTE4\n"
    "#define CPH_SEED1_WORD1(U) U##_SEED1_WORD1\n"
    "#define CPH_SEED1_WORD2(U) U##_SEED1_WORD2\n"
    "\n"
    "#define CPH_SEED1_BYTE1(U) U##_SEED1_BYTE1\n"
    "#define CPH_SEED1_BYTE2(U) U##_SEED1_BYTE2\n"
    "#define CPH_SEED1_BYTE3(U) U##_SEED1_BYTE3\n"
    "#define CPH_SEED1_BYTE4(U) U##_SEED1_BYTE4\n"
    "#define CPH_SEED1_WORD1(U) U##_SEED1_WORD1\n"
    "#define CPH_SEED1_WORD2(U) U##_SEED1_WORD2\n"
    "\n"
    "#define CPH_SEED2_BYTE1(U) U##_SEED2_BYTE1\n"
    "#define CPH_SEED2_BYTE2(U) U##_SEED2_BYTE2\n"
    "#define CPH_SEED2_BYTE3(U) U##_SEED2_BYTE3\n"
    "#define CPH_SEED2_BYTE4(U) U##_SEED2_BYTE4\n"
    "#define CPH_SEED2_WORD1(U) U##_SEED2_WORD1\n"
    "#define CPH_SEED2_WORD2(U) U##_SEED2_WORD2\n"
    "\n"
    "#define CPH_SEED3_BYTE1(U) U##_SEED3_BYTE1\n"
    "#define CPH_SEED3_BYTE2(U) U##_SEED3_BYTE2\n"
    "#define CPH_SEED3_BYTE3(U) U##_SEED3_BYTE3\n"
    "#define CPH_SEED3_BYTE4(U) U##_SEED3_BYTE4\n"
    "#define CPH_SEED3_WORD1(U) U##_SEED3_WORD1\n"
    "#define CPH_SEED3_WORD2(U) U##_SEED3_WORD2\n"
    "\n"
    "#define CPH_SEED4_BYTE1(U) U##_SEED4_BYTE1\n"
    "#define CPH_SEED4_BYTE2(U) U##_SEED4_BYTE2\n"
    "#define CPH_SEED4_BYTE3(U) U##_SEED4_BYTE3\n"
    "#define CPH_SEED4_BYTE4(U) U##_SEED4_BYTE4\n"
    "#define CPH_SEED4_WORD1(U) U##_SEED4_WORD1\n"
    "#define CPH_SEED4_WORD2(U) U##_SEED4_WORD2\n"
    "\n"
    "#define CPH_HASH_MASK(U) U##_HASH_MASK\n"
    "#define CPH_INDEX_MASK(U) U##_INDEX_MASK\n"
    "\n"
    "#define CPH_TABLE_DATA(T) T##_TableData\n"
    "#define CPH_TABLE_VALUES(T) T##_TableValues\n"
    "#define CPH_KEYS(T) T##_Keys\n"
    "#define CPH_NUMBER_OF_KEYS(U) U##_NUMBER_OF_KEYS\n"
    "\n"
    "#define CPH_DOWNSIZE_KEY(U) U##_DOWNSIZE_KEY\n"
    "#define CPH_ROTATE_KEY_LEFT(U) U##_ROTATE_KEY_LEFT\n"
    "#define CPH_ROTATE_KEY_RIGHT(U) U##_ROTATE_KEY_RIGHT\n"
    "\n"
    "#define CPH_INDEX_ROUTINE_NAME(T) CompiledPerfectHash_##T##_Index\n"
    "#define CPH_LOOKUP_ROUTINE_NAME(T) CompiledPerfectHash_##T##_Lookup\n"
    "#define CPH_INSERT_ROUTINE_NAME(T) CompiledPerfectHash_##T##_Insert\n"
    "#define CPH_DELETE_ROUTINE_NAME(T) CompiledPerfectHash_##T##_Delete\n"
    "\n"
    "#define CPH_INDEX_INLINE_ROUTINE_NAME(T) CompiledPerfectHash_##T##_IndexInline\n"
    "#define CPH_LOOKUP_INLINE_ROUTINE_NAME(T) CompiledPerfectHash_##T##_LookupInline\n"
    "#define CPH_INSERT_INLINE_ROUTINE_NAME(T) CompiledPerfectHash_##T##_InsertInline\n"
    "#define CPH_DELETE_INLINE_ROUTINE_NAME(T) CompiledPerfectHash_##T##_DeleteInline\n"
    "\n"
    "////////////////////////////////////////////////////////////////////////////////\n"
    "// Index\n"
    "////////////////////////////////////////////////////////////////////////////////\n"
    "\n"
    "//\n"
    "// Normal\n"
    "//\n"
    "\n"
    "#define CPH_INDEX_ROUTINE_HEADER(T)      \\\n"
    "CPHAPI COMPILED_PERFECT_HASH_TABLE_INDEX \\\n"
    "    CompiledPerfectHash_##T##_Index;     \\\n"
    "                                         \\\n"
    "_Use_decl_annotations_                   \\\n"
    "CPHINDEX                                 \\\n"
    "CompiledPerfectHash_##T##_Index(         \\\n"
    "    CPHKEY Key                           \\\n"
    "    )\n"
    "\n"
    "//\n"
    "// Inline\n"
    "//\n"
    "\n"
    "#define CPH_INDEX_INLINE_ROUTINE_HEADER(T) \\\n"
    "FORCEINLINE                                \\\n"
    "CPHINDEX                                   \\\n"
    "CompiledPerfectHash_##T##_IndexInline(     \\\n"
    "    CPHKEY Key                             \\\n"
    "    )\n"
    "\n"
    "////////////////////////////////////////////////////////////////////////////////\n"
    "// Lookup\n"
    "////////////////////////////////////////////////////////////////////////////////\n"
    "\n"
    "//\n"
    "// Normal\n"
    "//\n"
    "\n"
    "#define CPH_LOOKUP_ROUTINE_HEADER(T)      \\\n"
    "CPHAPI COMPILED_PERFECT_HASH_TABLE_LOOKUP \\\n"
    "    CompiledPerfectHash_##T##_Lookup;     \\\n"
    "                                          \\\n"
    "_Use_decl_annotations_                    \\\n"
    "CPHVALUE                                  \\\n"
    "CompiledPerfectHash_##T##_Lookup(         \\\n"
    "    CPHKEY Key                            \\\n"
    "    )\n"
    "\n"
    "//\n"
    "// Inline\n"
    "//\n"
    "\n"
    "#define CPH_LOOKUP_INLINE_ROUTINE_HEADER(T) \\\n"
    "FORCEINLINE                                 \\\n"
    "CPHVALUE                                    \\\n"
    "CompiledPerfectHash_##T##_LookupInline(     \\\n"
    "    CPHKEY Key                              \\\n"
    "    )\n"
    "\n"
    "////////////////////////////////////////////////////////////////////////////////\n"
    "// Insert\n"
    "////////////////////////////////////////////////////////////////////////////////\n"
    "\n"
    "//\n"
    "// Normal\n"
    "//\n"
    "\n"
    "#define CPH_INSERT_ROUTINE_HEADER(T)      \\\n"
    "CPHAPI COMPILED_PERFECT_HASH_TABLE_INSERT \\\n"
    "    CompiledPerfectHash_##T##_Insert;     \\\n"
    "                                          \\\n"
    "_Use_decl_annotations_                    \\\n"
    "CPHVALUE                                  \\\n"
    "CompiledPerfectHash_##T##_Insert(         \\\n"
    "    CPHKEY Key,                           \\\n"
    "    CPHVALUE Value                        \\\n"
    "    )\n"
    "\n"
    "//\n"
    "// Inline\n"
    "//\n"
    "\n"
    "#define CPH_INSERT_INLINE_ROUTINE_HEADER(T) \\\n"
    "FORCEINLINE                                 \\\n"
    "CPHVALUE                                    \\\n"
    "CompiledPerfectHash_##T##_InsertInline(     \\\n"
    "    CPHKEY Key,                             \\\n"
    "    CPHVALUE Value                          \\\n"
    "    )\n"
    "\n"
    "\n"
    "////////////////////////////////////////////////////////////////////////////////\n"
    "// Delete\n"
    "////////////////////////////////////////////////////////////////////////////////\n"
    "\n"
    "//\n"
    "// Normal\n"
    "//\n"
    "\n"
    "#define CPH_DELETE_ROUTINE_HEADER(T)      \\\n"
    "CPHAPI COMPILED_PERFECT_HASH_TABLE_DELETE \\\n"
    "    CompiledPerfectHash_##T##_Delete;     \\\n"
    "                                          \\\n"
    "_Use_decl_annotations_                    \\\n"
    "CPHVALUE                                  \\\n"
    "CompiledPerfectHash_##T##_Delete(         \\\n"
    "    CPHKEY Key                            \\\n"
    "    )\n"
    "\n"
    "//\n"
    "// Inline\n"
    "//\n"
    "\n"
    "#define CPH_DELETE_INLINE_ROUTINE_HEADER(T) \\\n"
    "FORCEINLINE                                 \\\n"
    "CPHVALUE                                    \\\n"
    "CompiledPerfectHash_##T##_DeleteInline(     \\\n"
    "    CPHKEY Key                              \\\n"
    "    )\n"
    "\n"
    "\n"
    "////////////////////////////////////////////////////////////////////////////////\n"
    "// Test and Benchmarking\n"
    "////////////////////////////////////////////////////////////////////////////////\n"
    "\n"
    "#define CPH_TEST_CPH_ROUTINE_NAME(T) TestCompiledPerfectHashTable_##T\n"
    "#define CPH_BENCHMARK_FULL_CPH_ROUTINE_NAME(T) BenchmarkFullCompiledPerfectHashTable_##T\n"
    "#define CPH_BENCHMARK_INDEX_CPH_ROUTINE_NAME(T) BenchmarkIndexCompiledPerfectHashTable_##T\n"
    "\n"
    "#define TEST_CPH_ROUTINE_HEADER(T)    \\\n"
    "TEST_COMPILED_PERFECT_HASH_TABLE      \\\n"
    "    TestCompiledPerfectHashTable_##T; \\\n"
    "                                      \\\n"
    "_Use_decl_annotations_                \\\n"
    "ULONG                                 \\\n"
    "TestCompiledPerfectHashTable_##T##(   \\\n"
    "    BOOLEAN DebugBreakOnFailure       \\\n"
    "    )\n"
    "\n"
    "#define BENCHMARK_FULL_CPH_ROUTINE_HEADER(T)   \\\n"
    "BENCHMARK_FULL_COMPILED_PERFECT_HASH_TABLE     \\\n"
    "    BenchmarkFullCompiledPerfectHashTable_##T; \\\n"
    "                                               \\\n"
    "_Use_decl_annotations_                         \\\n"
    "ULONG                                          \\\n"
    "BenchmarkFullCompiledPerfectHashTable_##T##(   \\\n"
    "    ULONG Seconds                              \\\n"
    "    )\n"
    "\n"
    "#define BENCHMARK_INDEX_CPH_ROUTINE_HEADER(T)   \\\n"
    "BENCHMARK_INDEX_COMPILED_PERFECT_HASH_TABLE     \\\n"
    "    BenchmarkIndexCompiledPerfectHashTable_##T; \\\n"
    "                                                \\\n"
    "_Use_decl_annotations_                          \\\n"
    "ULONG                                           \\\n"
    "BenchmarkIndexCompiledPerfectHashTable_##T##(   \\\n"
    "    ULONG Seconds                               \\\n"
    "    )\n"
    "\n"
    "\n"
    "////////////////////////////////////////////////////////////////////////////////\n"
    "// End of routine headers.\n"
    "////////////////////////////////////////////////////////////////////////////////\n"
    "\n"
    "#define CPH_DEFINE_TABLE_ROUTINES(T)                                        \\\n"
    "CPHAPI COMPILED_PERFECT_HASH_TABLE_INDEX CompiledPerfectHash_##T##_Index;   \\\n"
    "CPHAPI COMPILED_PERFECT_HASH_TABLE_LOOKUP CompiledPerfectHash_##T##_Lookup; \\\n"
    "CPHAPI COMPILED_PERFECT_HASH_TABLE_INSERT CompiledPerfectHash_##T##_Insert; \\\n"
    "CPHAPI COMPILED_PERFECT_HASH_TABLE_DELETE CompiledPerfectHash_##T##_Delete\n"
    "\n"
    "#define CPH_DEFINE_TEST_AND_BENCHMARKING_ROUTINES(T) \\\n"
    "extern TEST_COMPILED_PERFECT_HASH_TABLE              \\\n"
    "    TestCompiledPerfectHashTable_##T;                \\\n"
    "                                                     \\\n"
    "extern BENCHMARK_FULL_COMPILED_PERFECT_HASH_TABLE    \\\n"
    "    BenchmarkFullCompiledPerfectHashTable_##T;       \\\n"
    "                                                     \\\n"
    "extern BENCHMARK_INDEX_COMPILED_PERFECT_HASH_TABLE   \\\n"
    "    BenchmarkIndexCompiledPerfectHashTable_##T\n"
    "\n"
    "#define CPH_INDEX_ROUTINE(T) CPH_INDEX_ROUTINE_NAME(T)\n"
    "#define CPH_LOOKUP_ROUTINE(T) CPH_LOOKUP_ROUTINE_NAME(T)\n"
    "#define CPH_INSERT_ROUTINE(T) CPH_INSERT_ROUTINE_NAME(T)\n"
    "#define CPH_DELETE_ROUTINE(T) CPH_DELETE_ROUTINE_NAME(T)\n"
    "\n"
    "#define CPH_INDEX_INLINE_ROUTINE(T) CPH_INDEX_INLINE_ROUTINE_NAME(T)\n"
    "#define CPH_LOOKUP_INLINE_ROUTINE(T) CPH_LOOKUP_INLINE_ROUTINE_NAME(T)\n"
    "#define CPH_INSERT_INLINE_ROUTINE(T) CPH_INSERT_INLINE_ROUTINE_NAME(T)\n"
    "#define CPH_DELETE_INLINE_ROUTINE(T) CPH_DELETE_INLINE_ROUTINE_NAME(T)\n"
    "\n"
    "#define EXPAND_SEED1(U) CPH_SEED1(U)\n"
    "#define EXPAND_SEED2(U) CPH_SEED2(U)\n"
    "#define EXPAND_SEED3(U) CPH_SEED3(U)\n"
    "#define EXPAND_SEED4(U) CPH_SEED4(U)\n"
    "\n"
    "#define EXPAND_SEED1_BYTE1(U) CPH_SEED1_BYTE1(U)\n"
    "#define EXPAND_SEED1_BYTE2(U) CPH_SEED1_BYTE2(U)\n"
    "#define EXPAND_SEED1_BYTE3(U) CPH_SEED1_BYTE3(U)\n"
    "#define EXPAND_SEED1_BYTE4(U) CPH_SEED1_BYTE4(U)\n"
    "#define EXPAND_SEED1_WORD1(U) CPH_SEED1_WORD1(U)\n"
    "#define EXPAND_SEED1_WORD2(U) CPH_SEED1_WORD2(U)\n"
    "\n"
    "#define EXPAND_SEED2_BYTE1(U) CPH_SEED2_BYTE1(U)\n"
    "#define EXPAND_SEED2_BYTE2(U) CPH_SEED2_BYTE2(U)\n"
    "#define EXPAND_SEED2_BYTE3(U) CPH_SEED2_BYTE3(U)\n"
    "#define EXPAND_SEED2_BYTE4(U) CPH_SEED2_BYTE4(U)\n"
    "#define EXPAND_SEED2_WORD1(U) CPH_SEED2_WORD1(U)\n"
    "#define EXPAND_SEED2_WORD2(U) CPH_SEED2_WORD2(U)\n"
    "\n"
    "#define EXPAND_SEED3_BYTE1(U) CPH_SEED3_BYTE1(U)\n"
    "#define EXPAND_SEED3_BYTE2(U) CPH_SEED3_BYTE2(U)\n"
    "#define EXPAND_SEED3_BYTE3(U) CPH_SEED3_BYTE3(U)\n"
    "#define EXPAND_SEED3_BYTE4(U) CPH_SEED3_BYTE4(U)\n"
    "#define EXPAND_SEED3_WORD1(U) CPH_SEED3_WORD1(U)\n"
    "#define EXPAND_SEED3_WORD2(U) CPH_SEED3_WORD2(U)\n"
    "\n"
    "#define EXPAND_SEED4_BYTE1(U) CPH_SEED4_BYTE1(U)\n"
    "#define EXPAND_SEED4_BYTE2(U) CPH_SEED4_BYTE2(U)\n"
    "#define EXPAND_SEED4_BYTE3(U) CPH_SEED4_BYTE3(U)\n"
    "#define EXPAND_SEED4_BYTE4(U) CPH_SEED4_BYTE4(U)\n"
    "#define EXPAND_SEED4_WORD1(U) CPH_SEED4_WORD1(U)\n"
    "#define EXPAND_SEED4_WORD2(U) CPH_SEED4_WORD2(U)\n"
    "\n"
    "#define EXPAND_HASH_MASK(U) CPH_HASH_MASK(U)\n"
    "#define EXPAND_INDEX_MASK(U) CPH_INDEX_MASK(U)\n"
    "#define EXPAND_TABLE_DATA(T) CPH_TABLE_DATA(T)\n"
    "#define EXPAND_TABLE_VALUES(T) CPH_TABLE_VALUES(T)\n"
    "#define EXPAND_KEYS(T) CPH_KEYS(T)\n"
    "#define EXPAND_NUMBER_OF_KEYS(U) CPH_NUMBER_OF_KEYS(U)\n"
    "\n"
    "#define EXPAND_DOWNSIZE_KEY(U) CPH_DOWNSIZE_KEY(U)\n"
    "#define EXPAND_ROTATE_KEY_LEFT(U) CPH_ROTATE_KEY_LEFT(U)\n"
    "#define EXPAND_ROTATE_KEY_RIGHT(U) CPH_ROTATE_KEY_RIGHT(U)\n"
    "\n"
    "#define EXPAND_INDEX_ROUTINE(T) CPH_INDEX_ROUTINE(T)\n"
    "#define EXPAND_LOOKUP_ROUTINE(T) CPH_LOOKUP_ROUTINE(T)\n"
    "#define EXPAND_INSERT_ROUTINE(T) CPH_INSERT_ROUTINE(T)\n"
    "#define EXPAND_DELETE_ROUTINE(T) CPH_DELETE_ROUTINE(T)\n"
    "\n"
    "#define EXPAND_INDEX_INLINE_ROUTINE(T) CPH_INDEX_INLINE_ROUTINE(T)\n"
    "#define EXPAND_LOOKUP_INLINE_ROUTINE(T) CPH_LOOKUP_INLINE_ROUTINE(T)\n"
    "#define EXPAND_INSERT_INLINE_ROUTINE(T) CPH_INSERT_INLINE_ROUTINE(T)\n"
    "#define EXPAND_DELETE_INLINE_ROUTINE(T) CPH_DELETE_INLINE_ROUTINE(T)\n"
    "\n"
    "#define EXPAND_INDEX_ROUTINE_HEADER(T) CPH_INDEX_ROUTINE_HEADER(T)\n"
    "#define EXPAND_LOOKUP_ROUTINE_HEADER(T) CPH_LOOKUP_ROUTINE_HEADER(T)\n"
    "#define EXPAND_INSERT_ROUTINE_HEADER(T) CPH_INSERT_ROUTINE_HEADER(T)\n"
    "#define EXPAND_DELETE_ROUTINE_HEADER(T) CPH_DELETE_ROUTINE_HEADER(T)\n"
    "\n"
    "#define EXPAND_INDEX_INLINE_ROUTINE_HEADER(T) CPH_INDEX_INLINE_ROUTINE_HEADER(T)\n"
    "#define EXPAND_LOOKUP_INLINE_ROUTINE_HEADER(T) CPH_LOOKUP_INLINE_ROUTINE_HEADER(T)\n"
    "#define EXPAND_INSERT_INLINE_ROUTINE_HEADER(T) CPH_INSERT_INLINE_ROUTINE_HEADER(T)\n"
    "#define EXPAND_DELETE_INLINE_ROUTINE_HEADER(T) CPH_DELETE_INLINE_ROUTINE_HEADER(T)\n"
    "\n"
    "#define EXPAND_TEST_CPH_ROUTINE_HEADER(T) TEST_CPH_ROUTINE_HEADER(T)\n"
    "#define EXPAND_BENCHMARK_FULL_CPH_ROUTINE_HEADER(T) BENCHMARK_FULL_CPH_ROUTINE_HEADER(T)\n"
    "#define EXPAND_BENCHMARK_INDEX_CPH_ROUTINE_HEADER(T) BENCHMARK_INDEX_CPH_ROUTINE_HEADER(T)\n"
    "\n"
    "#define EXPAND_TEST_CPH_ROUTINE_NAME(T) CPH_TEST_CPH_ROUTINE_NAME(T)\n"
    "#define EXPAND_BENCHMARK_FULL_CPH_ROUTINE_NAME(T) CPH_BENCHMARK_FULL_CPH_ROUTINE_NAME(T)\n"
    "#define EXPAND_BENCHMARK_INDEX_CPH_ROUTINE_NAME(T) CPH_BENCHMARK_INDEX_CPH_ROUTINE_NAME(T)\n"
    "\n"
    "#define EXPAND_DEFINE_TABLE_ROUTINES(T) CPH_DEFINE_TABLE_ROUTINES(T)\n"
    "#define EXPAND_DEFINE_TEST_AND_BENCHMARKING_ROUTINES(T) CPH_DEFINE_TEST_AND_BENCHMARKING_ROUTINES(T)\n"
    "\n"
    "#define SEED1 EXPAND_SEED1(CPH_TABLENAME_UPPER)\n"
    "#define SEED2 EXPAND_SEED2(CPH_TABLENAME_UPPER)\n"
    "#define SEED3 EXPAND_SEED3(CPH_TABLENAME_UPPER)\n"
    "#define SEED4 EXPAND_SEED4(CPH_TABLENAME_UPPER)\n"
    "\n"
    "#define SEED1_BYTE1 EXPAND_SEED1_BYTE1(CPH_TABLENAME_UPPER)\n"
    "#define SEED1_BYTE2 EXPAND_SEED1_BYTE2(CPH_TABLENAME_UPPER)\n"
    "#define SEED1_BYTE3 EXPAND_SEED1_BYTE3(CPH_TABLENAME_UPPER)\n"
    "#define SEED1_BYTE4 EXPAND_SEED1_BYTE4(CPH_TABLENAME_UPPER)\n"
    "#define SEED1_WORD1 EXPAND_SEED1_WORD1(CPH_TABLENAME_UPPER)\n"
    "#define SEED1_WORD2 EXPAND_SEED1_WORD2(CPH_TABLENAME_UPPER)\n"
    "\n"
    "#define SEED2_BYTE1 EXPAND_SEED2_BYTE1(CPH_TABLENAME_UPPER)\n"
    "#define SEED2_BYTE2 EXPAND_SEED2_BYTE2(CPH_TABLENAME_UPPER)\n"
    "#define SEED2_BYTE3 EXPAND_SEED2_BYTE3(CPH_TABLENAME_UPPER)\n"
    "#define SEED2_BYTE4 EXPAND_SEED2_BYTE4(CPH_TABLENAME_UPPER)\n"
    "#define SEED2_WORD1 EXPAND_SEED2_WORD1(CPH_TABLENAME_UPPER)\n"
    "#define SEED2_WORD2 EXPAND_SEED2_WORD2(CPH_TABLENAME_UPPER)\n"
    "\n"
    "#define SEED3_BYTE1 EXPAND_SEED3_BYTE1(CPH_TABLENAME_UPPER)\n"
    "#define SEED3_BYTE2 EXPAND_SEED3_BYTE2(CPH_TABLENAME_UPPER)\n"
    "#define SEED3_BYTE3 EXPAND_SEED3_BYTE3(CPH_TABLENAME_UPPER)\n"
    "#define SEED3_BYTE4 EXPAND_SEED3_BYTE4(CPH_TABLENAME_UPPER)\n"
    "#define SEED3_WORD1 EXPAND_SEED3_WORD1(CPH_TABLENAME_UPPER)\n"
    "#define SEED3_WORD2 EXPAND_SEED3_WORD2(CPH_TABLENAME_UPPER)\n"
    "\n"
    "#define SEED4_BYTE1 EXPAND_SEED4_BYTE1(CPH_TABLENAME_UPPER)\n"
    "#define SEED4_BYTE2 EXPAND_SEED4_BYTE2(CPH_TABLENAME_UPPER)\n"
    "#define SEED4_BYTE3 EXPAND_SEED4_BYTE3(CPH_TABLENAME_UPPER)\n"
    "#define SEED4_BYTE4 EXPAND_SEED4_BYTE4(CPH_TABLENAME_UPPER)\n"
    "#define SEED4_WORD1 EXPAND_SEED4_WORD1(CPH_TABLENAME_UPPER)\n"
    "#define SEED4_WORD2 EXPAND_SEED4_WORD2(CPH_TABLENAME_UPPER)\n"
    "\n"
    "#define HASH_MASK EXPAND_HASH_MASK(CPH_TABLENAME_UPPER)\n"
    "#define INDEX_MASK EXPAND_INDEX_MASK(CPH_TABLENAME_UPPER)\n"
    "#define TABLE_DATA EXPAND_TABLE_DATA(CPH_TABLENAME)\n"
    "#define TABLE_VALUES EXPAND_TABLE_VALUES(CPH_TABLENAME)\n"
    "#define KEYS EXPAND_KEYS(CPH_TABLENAME)\n"
    "#define NUMBER_OF_KEYS EXPAND_NUMBER_OF_KEYS(CPH_TABLENAME_UPPER)\n"
    "#define DOWNSIZE_KEY(K) EXPAND_DOWNSIZE_KEY(CPH_TABLENAME_UPPER)(K)\n"
    "#define ROTATE_KEY_LEFT EXPAND_ROTATE_KEY_LEFT(CPH_TABLENAME_UPPER)\n"
    "#define ROTATE_KEY_RIGHT EXPAND_ROTATE_KEY_RIGHT(CPH_TABLENAME_UPPER)\n"
    "\n"
    "#define DECLARE_INDEX_ROUTINE_HEADER() EXPAND_INDEX_ROUTINE_HEADER(CPH_TABLENAME)\n"
    "#define DECLARE_LOOKUP_ROUTINE_HEADER() EXPAND_LOOKUP_ROUTINE_HEADER(CPH_TABLENAME)\n"
    "#define DECLARE_INSERT_ROUTINE_HEADER() EXPAND_INSERT_ROUTINE_HEADER(CPH_TABLENAME)\n"
    "#define DECLARE_DELETE_ROUTINE_HEADER() EXPAND_DELETE_ROUTINE_HEADER(CPH_TABLENAME)\n"
    "\n"
    "#define DECLARE_INDEX_INLINE_ROUTINE_HEADER() EXPAND_INDEX_INLINE_ROUTINE_HEADER(CPH_TABLENAME)\n"
    "#define DECLARE_LOOKUP_INLINE_ROUTINE_HEADER() EXPAND_LOOKUP_INLINE_ROUTINE_HEADER(CPH_TABLENAME)\n"
    "#define DECLARE_INSERT_INLINE_ROUTINE_HEADER() EXPAND_INSERT_INLINE_ROUTINE_HEADER(CPH_TABLENAME)\n"
    "#define DECLARE_DELETE_INLINE_ROUTINE_HEADER() EXPAND_DELETE_INLINE_ROUTINE_HEADER(CPH_TABLENAME)\n"
    "\n"
    "#define INDEX_INLINE_ROUTINE EXPAND_INDEX_INLINE_ROUTINE(CPH_TABLENAME)\n"
    "#define LOOKUP_INLINE_ROUTINE EXPAND_LOOKUP_INLINE_ROUTINE(CPH_TABLENAME)\n"
    "#define INSERT_INLINE_ROUTINE EXPAND_INSERT_INLINE_ROUTINE(CPH_TABLENAME)\n"
    "#define DELETE_INLINE_ROUTINE EXPAND_DELETE_INLINE_ROUTINE(CPH_TABLENAME)\n"
    "\n"
    "#define TEST_CPH_ROUTINE EXPAND_TEST_CPH_ROUTINE_NAME(CPH_TABLENAME)\n"
    "#define BENCHMARK_FULL_CPH_ROUTINE EXPAND_BENCHMARK_FULL_CPH_ROUTINE_NAME(CPH_TABLENAME)\n"
    "#define BENCHMARK_INDEX_CPH_ROUTINE EXPAND_BENCHMARK_INDEX_CPH_ROUTINE_NAME(CPH_TABLENAME)\n"
    "\n"
    "#define DECLARE_TEST_CPH_ROUTINE() EXPAND_TEST_CPH_ROUTINE_HEADER(CPH_TABLENAME)\n"
    "#define DECLARE_BENCHMARK_FULL_CPH_ROUTINE() EXPAND_BENCHMARK_FULL_CPH_ROUTINE_HEADER(CPH_TABLENAME)\n"
    "#define DECLARE_BENCHMARK_INDEX_CPH_ROUTINE() EXPAND_BENCHMARK_INDEX_CPH_ROUTINE_HEADER(CPH_TABLENAME)\n"
    "\n"
    "#define DEFINE_TABLE_ROUTINES() EXPAND_DEFINE_TABLE_ROUTINES(CPH_TABLENAME)\n"
    "#define DEFINE_TEST_AND_BENCHMARKING_ROUTINES() EXPAND_DEFINE_TEST_AND_BENCHMARKING_ROUTINES(CPH_TABLENAME)\n"
    "\n"
    "\n"
    "//\n"
    "// End CompiledPerfectHashMacroGlue.h.\n"
    "//\n"
    "\n"
;

const STRING CompiledPerfectHashMacroGlueCHeaderRawCString = {
    sizeof(CompiledPerfectHashMacroGlueCHeaderRawCStr) - sizeof(CHAR),
    sizeof(CompiledPerfectHashMacroGlueCHeaderRawCStr),
#ifdef _WIN64
    0,
#endif
    (PCHAR)&CompiledPerfectHashMacroGlueCHeaderRawCStr,
};

#ifndef RawCString
#define RawCString (&CompiledPerfectHashMacroGlueCHeaderRawCString)
#endif
