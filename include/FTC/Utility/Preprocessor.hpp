// Preprocessor Helpers
// 
// Provides preprocessor utilty macros for token contatenation, stringify, etc.

#pragma once


/// Make a token into a raw string literal
#define PP_TOSTRING(x)      PP_TOSTRING_IMPL(x)
#define PP_TOSTRING_IMPL(x) #x

/// Make a list of token into a raw string literal
#define PP_LIST_TOSTRING(...)      PP_LIST_TOSTRING_IMPL(__VA_ARGS__)
#define PP_LIST_TOSTRING_IMPL(...) #__VA_ARGS__

/// Concatenate two token into one token
#define PP_CONCAT(x, y)      PP_CONCAT_IMPL(x, y)
#define PP_CONCAT_IMPL(x, y) x ## y

/// Concatenate the first token with the rest of tokens
#define PP_CONCAT_FIRST(x, ...)      PP_CONCAT_FIRST_IMPL(x, __VA_ARGS__)
#define PP_CONCAT_FIRST_IMPL(x, ...) x ## __VA_ARGS__

