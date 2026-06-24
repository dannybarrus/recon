#pragma once

namespace recon::algorithms {

// A hand-rolled implementation of the standard library's strchr() --
// find the first occurrence of character c in string s, or nullptr if
// it is not present.
//
// Renamed from the original's "mystrchr" to customStrchr -- the original
// name still shadowed the standard library function it was reimplementing
// once <cstring> was included in the same translation unit, which is its
// own minor footgun worth avoiding.
//
// BUG FOUND AND FIXED: the original's while-loop condition incremented
// the pointer via *s++ and then checked the NEW position in the loop
// body. That meant the very first character of the string was advanced
// past before ever being compared against the target -- a match at
// index 0 was always missed entirely.
//
//   mystrchr("abc", 'a') incorrectly returned nullptr.
//   The real strchr("abc", 'a') correctly returns a pointer to index 0.
//
// This went unnoticed because the original's own test case searched for
// 't' in "This is a test" -- a string where the target character never
// happens to be the first one, so the bug had nothing to expose it.
const char* customStrchr(const char* s, int c);

}  // namespace recon::algorithms
