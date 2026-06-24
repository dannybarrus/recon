# Depends on recon_allocator (from ../allocator) for the Arena type
# backing each interned string's storage -- genuine cross-topic reuse,
# not a re-derived copy of the same allocator.

add_library(recon_hashtable
  src/intern_table.c
)
target_include_directories(recon_hashtable PUBLIC include)
target_link_libraries(recon_hashtable PUBLIC recon_allocator)

add_executable(hashtable_demo demo/main.c)
target_link_libraries(hashtable_demo PRIVATE recon_hashtable)
target_include_directories(hashtable_demo PRIVATE ${CMAKE_SOURCE_DIR}/common/include)

add_executable(hashtable_test test/hashtable_test.c)
target_link_libraries(hashtable_test PRIVATE recon_hashtable unity)
unity_discover_tests(hashtable_test ${CMAKE_CURRENT_SOURCE_DIR}/test/hashtable_test.c)
