#pragma once

namespace recon::algorithms {

// Matches text against a pattern containing '*' wildcards, where '*'
// matches any sequence of characters (including an empty one).
//
// THE BIG BUG (this is the most important fix in this folder): the
// original validated the literal text BEFORE the first wildcard, then
// -- due to a pointer-arithmetic quirk -- never validated anything
// after the first wildcard at all. Once the first segment matched
// anywhere in the text, the function returned true immediately.
//
//   ExpressionMatches("This is a test string", "This*ZZZNOTPRESENT")
//     original: true   (WRONG -- "ZZZNOTPRESENT" appears nowhere)
//     fixed:    false
//
// Worse: every one of the original's own 8 test cases in main() passed
// despite this bug, purely because none of them were adversarial enough
// to probe it. "A*string" against text with no "A" failed for an
// unrelated reason (the literal "A" segment itself was never found,
// since the text only has lowercase 'a') -- never actually exercising
// whether "string" was checked after the wildcard. Passing tests do not
// prove correctness if the tests never attack the actual weak point.
//
// Rewritten as a straightforward recursive matcher: at each position,
// either the next pattern character is a wildcard (try consuming
// zero-or-more characters of the text, trying each possibility) or it
// must match the next text character exactly.
bool matchesPattern(const char* text, const char* pattern);

}  // namespace recon::algorithms
