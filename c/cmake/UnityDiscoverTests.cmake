# unity_discover_tests(<target> <source_file>)
#
# Registers each Unity test in <source_file> as its own separate ctest
# entry -- the same end result gtest_discover_tests() gives the
# GoogleTest-based suites elsewhere in this repo (cpp/, where every
# test shows up as its own line in `ctest`, rather than the whole
# binary as one).
#
# Unity has no built-in equivalent of GoogleTest's --gtest_list_tests,
# so there is no way to ask an already-built test binary "what tests
# do you contain" the way gtest_discover_tests does. Instead, this
# scans the SOURCE FILE directly for entries in its test_cases[] table
# (see the convention established in ringbuffer/test/ringbuffer_test.c)
# and registers one add_test() per entry found, each one invoking
# <target> with that test's name as a command-line argument -- the
# test binary's own main() filters down to running just that one test
# when a name is passed, and runs everything when it isn't.
#
# CAVEAT, worth knowing: because this parses source TEXT at CMake
# CONFIGURE time, rather than by actually running the built binary,
# adding, renaming, or removing a test in the test_cases[] table
# requires a fresh `cmake -S . -B build` reconfigure before `ctest`
# will see the change -- a plain rebuild is not enough to update
# ctest's registered test list, even though running the test binary
# directly always reflects the current table immediately, with no
# reconfigure needed.
function(unity_discover_tests TARGET_NAME SOURCE_FILE)
  file(STRINGS "${SOURCE_FILE}" test_table_lines REGEX "\\{\"[a-zA-Z0-9_]+\",")
  foreach(line ${test_table_lines})
    string(REGEX MATCH "\"([a-zA-Z0-9_]+)\"" _ "${line}")
    set(test_name "${CMAKE_MATCH_1}")
    add_test(NAME ${test_name} COMMAND ${TARGET_NAME} ${test_name})
  endforeach()
endfunction()
