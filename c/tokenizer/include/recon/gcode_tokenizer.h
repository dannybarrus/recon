#pragma once

#include <stddef.h>

// A zero-copy lexer for a single line of G-code -- the line-based
// command language 3D printers, CNC mills, and similar motion
// controllers consume, made of "words" like G1, X10.5, Y-20.3, F1500.
//
// ─── What "zero-copy" actually means here ──────────────────────────────────
//
// Every token is a {start, length} pair pointing directly INTO the
// caller's original line buffer -- nothing is ever copied into a new
// string, and nothing is ever malloc'd. This matters in exactly the
// place this is used in real life: a motion controller parsing
// incoming G-code, often many lines per second, on hardware where
// allocation overhead is wasteful at best and unavailable at worst.
//
// Pulling a token's NUMERIC value out also stays zero-copy, which is
// the less obvious half of this: strtod() doesn't need an explicit
// "end of number" boundary passed to it -- given any pointer into a
// null-terminated string, it parses for as long as the text in front
// of it still looks like a valid number, and stops naturally the
// moment it doesn't (at whitespace, a comment marker, or the line's
// own null terminator). So gcode_word_value() can call strtod()
// directly on a pointer into the MIDDLE of the original line, with no
// substring ever extracted -- the only real requirement is that the
// original line is null-terminated somewhere after every token, which
// any normal line-based input naturally is.
//
// ─── Two comment styles, two different behaviors ───────────────────────────
//
// A `;` comment runs to the end of the line -- nothing after it is
// parsed at all. A `(parenthesized)` comment is INLINE: it ends at its
// own closing `)` (or at the end of the line, if unterminated), and
// parsing continues normally afterward. Real G-code dialects use both,
// for different purposes, and they are not interchangeable.

// All public symbols here are prefixed with GCode -- plain C has no
// namespaces, and a genuinely real collision surfaced during this
// repo's own development: Windows' own winnt.h typedefs an enum named
// TokenType for an unrelated Win32 security API
// (_TOKEN_INFORMATION_CLASS). Any file that includes both <windows.h>
// (even transitively, via recon_thread.h) and an UNPREFIXED TokenType
// from this header fails to compile with a redefinition error -- which
// is exactly what happened the first time errors/demo/main.c included
// both in the same translation unit. Prefixing avoids this whole class
// of problem, the same reason ringbuffer's macro-generated types use an
// IntQueue_-style prefix and statemachine's generated enum uses
// Printer_.

typedef enum {
    GCODE_TOKEN_WORD,     // a letter immediately followed by a number, e.g. "X10.5"
    GCODE_TOKEN_COMMENT,  // a ; comment (runs to end of line) or a (parenthesized) one
} GCodeTokenType;

typedef struct {
    GCodeTokenType type;
    const char* start;  // points into the CALLER's original line buffer -- never copied
    size_t length;
    char letter;        // valid only when type == GCODE_TOKEN_WORD: the command letter, e.g. 'G', 'X'
} GCodeToken;

typedef struct {
    int success;
    size_t token_count;
    const char* error_message;  // NULL if success
    size_t error_position;      // byte offset into the line where parsing failed; valid only if !success
} GCodeTokenizeResult;

// Tokenizes a single null-terminated line into `tokens`, writing at
// most `max_tokens` entries. On success, error_message is NULL and
// token_count reflects how many tokens were actually found (which may
// be 0, for a blank or comment-only line). On failure (malformed
// input, or more tokens than max_tokens could hold), success is 0 and
// error_message/error_position describe what went wrong and where.
GCodeTokenizeResult gcode_tokenize_line(const char* line, GCodeToken* tokens, size_t max_tokens);

// Parses a GCODE_TOKEN_WORD's numeric portion (everything after the letter)
// directly from the original buffer via strtod -- no copying. Behavior
// is undefined if called on a token that isn't GCODE_TOKEN_WORD.
double gcode_word_value(const GCodeToken* word_token);
