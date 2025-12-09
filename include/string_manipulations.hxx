/** @file
 *
 *  @brief function for manipulating string
 * */

#pragma once

#include <system_error>
#if __has_include(<fmt/ranges.h>) && !defined(WBR_FMT_RANGES_INCLUDED)
    #include <fmt/ranges.h>
    #define WBR_FMT_RANGES_INCLUDED
#endif

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstring>
#include <functional>
#include <optional>
#include <span>
#include <sstream>
#include <string>
#include <vector>
#include <version>

#include "concepts.hxx"

/** @brief all string manipulation functions are located in wbr::str namespace
 */
namespace wbr::str {

/** @brief helper predicate to check if a string is empty
 *
 * allow shorted code:
 * @code
 *     std::find_if(vector.begin(), vector.end(), empty);
 *     @endcode
 *
 * instead of
 * @code
 *     std::find_if(vector.begin(), vector.end(), [](std::string_view s){return s.empty();});
 *     @endcode
 */
[[nodiscard]] inline bool empty (std::string_view str) noexcept {
    return str.empty( );
}

/** @brief helper predicate to check if string is non-empty
 *
 * Convenience function for checking if a string has content.
 * @code
 *     std::vector<std::string_view> words = {"hello", "", "world"};
 *     auto count = std::count_if(words.begin(), words.end(), wbr::str::nonempty);
 *     // count == 2
 * @endcode
 */
[[nodiscard]] inline bool nonempty (std::string_view str) noexcept {
    return !empty(str);
}

/** @brief Function object wrapper for negated empty check
 *
 * @code
 *     std::vector<std::string_view> tokens = {"hello", "", "world", ""};
 *     std::erase_if(tokens, wbr::str::empty);
 *     // tokens: {"hello", "world"}
 * @endcode
 */
inline auto not_emtpy = std::not_fn(empty);

/** @brief Check if character is a whitespace character
 *
 * Safe wrapper around std::isspace that handles unsigned char conversion.
 * @code
 *     bool result = wbr::str::isspace(' ');   // true
 *     bool result2 = wbr::str::isspace('a');  // false
 * @endcode
 */
[[nodiscard]] inline bool isspace (char chr) noexcept {
    return std::isspace(static_cast<unsigned char>(chr)) != 0;
}

/** @brief Function object wrapper for negated isspace check
 *
 * @code
 *     std::string_view str = "  hello  ";
 *     auto first_non_space = std::find_if(str.begin(), str.end(), wbr::str::is_notspace);
 *     // *first_non_space == 'h'
 * @endcode
 */
inline auto is_notspace = std::not_fn(isspace);

/** @brief Check if character is a blank character (space or tab)
 *
 * Safe wrapper around std::isblank that handles unsigned char conversion.
 * @code
 *     bool result = wbr::str::isblank(' ');   // true
 *     bool result2 = wbr::str::isblank('\t');  // true
 *     bool result3 = wbr::str::isblank('\n');  // false
 * @endcode
 */
[[nodiscard]] inline bool isblank (char chr) noexcept {
    return std::isblank(static_cast<unsigned char>(chr)) != 0;
}

/** @brief Function object wrapper for negated isblank check */
inline auto is_notblank = std::not_fn(isblank);

/** @brief Check if character is a decimal digit (0-9)
 *
 * @code
 *     bool r1 = wbr::str::isdigit('5');  // true
 *     bool r2 = wbr::str::isdigit('a');  // false
 * @endcode
 */
[[nodiscard]] inline bool isdigit (char chr) noexcept {
    return std::isdigit(static_cast<unsigned char>(chr)) != 0;
}

inline auto it_notdigit = std::not_fn(isdigit);

/** @brief Check if character is a hexadecimal digit (0-9, a-f, A-F)
 *
 * @code
 *     bool r1 = wbr::str::isxdigit('F');  // true
 *     bool r2 = wbr::str::isxdigit('g');  // false
 * @endcode
 */
[[nodiscard]] inline bool isxdigit (char chr) noexcept {
    return std::isxdigit(static_cast<unsigned char>(chr)) != 0;
}

inline auto it_notxdigit = std::not_fn(isxdigit);

/** @brief Check if character is lowercase letter
 *
 * @code
 *     bool r1 = wbr::str::islower('a');  // true
 *     bool r2 = wbr::str::islower('A');  // false
 * @endcode
 */
[[nodiscard]] inline bool islower (char chr) noexcept {
    return std::islower(static_cast<unsigned char>(chr)) != 0;
}

inline auto is_notlower = std::not_fn(islower);

/** @brief Check if character is uppercase letter
 *
 * @code
 *     bool r1 = wbr::str::isupper('A');  // true
 *     bool r2 = wbr::str::isupper('a');  // false
 * @endcode
 */
[[nodiscard]] inline bool isupper (char chr) noexcept {
    return std::isupper(static_cast<unsigned char>(chr)) != 0;
}

inline auto is_notupper = std::not_fn(isupper);

/** @brief Check if character is a control character */
[[nodiscard]] inline bool iscntrl (char chr) noexcept {
    return std::iscntrl(static_cast<unsigned char>(chr)) != 0;
}

inline auto is_notcntrl = std::not_fn(iscntrl);

/** @brief Check if character is alphanumeric
 *
 * @code
 *     bool r1 = wbr::str::isalnum('a');  // true
 *     bool r2 = wbr::str::isalnum('5');  // true
 *     bool r3 = wbr::str::isalnum('_');  // false
 * @endcode
 */
[[nodiscard]] inline bool isalnum (char ch) noexcept {
    return std::isalnum(static_cast<unsigned char>(ch));
}

inline auto is_notalnum = std::not_fn(isalnum);

/** @brief Check if character is alphabetic
 *
 * @code
 *     bool r1 = wbr::str::isalpha('z');  // true
 *     bool r2 = wbr::str::isalpha('5');  // false
 * @endcode
 */
[[nodiscard]] inline bool isalpha (char ch) noexcept {
    return std::isalpha(static_cast<unsigned char>(ch));
}

inline auto is_notalpha = std::not_fn(isalpha);

[[nodiscard]] inline bool is_punct (char ch) noexcept {
    return std::ispunct(static_cast<unsigned char>(ch));
}

inline auto is_notpunct = std::not_fn(is_punct);

/** @brief Convert container/range to hexadecimal string representation
 *
 * @param arr Container with elements to convert
 * @param divider String to place between hex values
 * @return Hex string with 2-digit format per element
 *
 * @code
 *     std::vector<uint8_t> data = {0xDE, 0xAD, 0xBE, 0xEF};
 *     std::string hex = wbr::str::convertToHexString(data, " ");
 *     // hex == "de ad be ef"
 * @endcode
 */
template<typename T>
std::string convertToHexString (const T& arr, std::string_view divider = " ") {
    return fmt::format("{:02x}", fmt::join(arr.begin( ), arr.end( ), divider));
}

inline std::string convertToHexString (const char* arr, std::string_view divider = " ") {
    return convertToHexString(std::string_view {arr}, divider);
}

// template <typename T>
// inline std::string asHex (const std::span<T>  arr, std::string_view divider = " ")
//{
//     return asHex(std::string_view {arr.data(), a}, divider);
// }
/**
 * @brief remove  leading and trailing whitespaces from string
 *
 * @param[in] str input string
 * @param[in] symbols List of symbols that should be considered a whitespace. By default, ' ' and tabulation are used.
 * @return return a new string that begins with first non-white space character of original string and end at last non-white space character of original string
 *
 * @code
 *     std::string_view result = wbr::str::trim("  hello world  ");
 *     // result == "hello world"
 *
 *     std::string_view result2 = wbr::str::trim("___test___", "_");
 *     // result2 == "test"
 * @endcode
 */
inline std::string_view trim (std::string_view str, std::string_view symbols = " \t") {
    if ( str.empty( ) )
        return { };

    if ( std::string_view::size_type pos = str.find_first_not_of(symbols); pos != std::string_view::npos )
        str.remove_prefix(pos);
    else
        return { };

    if ( std::string_view::size_type pos = str.find_last_not_of(symbols); pos != std::string_view::npos )
        str.remove_suffix(str.size( ) - pos - 1);
    return str;
}

/** @brief Trim string using custom predicate function
 *
 * @param str Input string to trim
 * @param is_space_predicate Predicate that returns true for characters to remove
 * @return Trimmed string view
 *
 * @code
 *     auto is_punct = [](char c) { return std::ispunct(c); };
 *     std::string_view result = wbr::str::trim("...hello...", is_punct);
 *     // result == "hello"
 * @endcode
 */
template<std::invocable<char> P>
inline std::string_view trim (std::string_view str, P is_space_predicate) {
    if ( str.empty( ) )
        return { };

    auto pos_b = std::find_if_not(str.cbegin( ), str.cend( ), is_space_predicate);
    if ( pos_b == str.cend( ) )
        return { };
    auto pos_e = std::find_if_not(str.crbegin( ), str.crend( ), is_space_predicate);
    return std::string_view {pos_b, pos_e.base( )};
}

/**
 * @brief Split string into two parts at a given position
 *
 * Helper function to split a single string into 2 parts. Both splat parts will be trimmed.
 * If all symbols before pos are whitespaces -- function will fail
 * if pos is greater than input string length -- the first part will be equal to trimmed input string, and the second part will be empty
 *
 * Examples
 *    splitAtPos("Hello World!", 5) --> "Hello" "World!"
 *
 * @param[in] str input string. should not be empty
 * @param[in] pos position at which string should be split
 * @return if fail to split - return std::nullopt, otherwise return a pair of strings
 * @see trimWhitespace
 * @see splitAtFirst
 * @see splitAtLast
 */
inline std::optional<std::pair<std::string_view, std::string_view>> splitAtPos (std::string_view str, std::string_view::size_type pos) {
    if ( pos == std::string_view::npos )
        return std::nullopt;

    std::string_view key = {str.data( ), pos};
    std::string_view val = {str.data( ) + pos + 1, str.size( ) - pos - 1};
    key                  = trim(key);
    val                  = trim(val);

    if ( key.empty( ) )
        return std::nullopt;

    return std::pair {key, val};
}

/** @brief Convert string to lowercase (in-place)
 *
 * @param s String to convert (modified in place)
 * @return Reference to modified string
 *
 * @code
 *     std::string text = "Hello World";
 *     wbr::str::strlower(text);
 *     // text == "hello world"
 * @endcode
 */
template<StringType S>
S& strlower (S& s) {
    std::ranges::transform(s, s.begin( ), tolower);
    return s;
}

/** @brief Convert string to lowercase (returns new string)
 *
 * @param s Input string (not modified)
 * @return New lowercase string
 *
 * @code
 *     std::string result = wbr::str::strlower<std::string>("Hello World");
 *     // result == "hello world"
 * @endcode
 */
template<StringType S, StringType O = std::string>
O strlower (const S& s) {
    O out;
    std::ranges::transform(s, std::back_inserter(out), tolower);
    return out;
}

/** @brief Convert string to uppercase (in-place)
 *
 * @param s String to convert (modified in place)
 * @return Reference to modified string
 *
 * @code
 *     std::string text = "Hello World";
 *     wbr::str::strupper(text);
 *     // text == "HELLO WORLD"
 * @endcode
 */
template<StringType S>
S& strupper (S& s) {
    std::ranges::transform(s, s.begin( ), toupper);
    return s;
}

/** @brief Convert string to uppercase (returns new string)
 *
 * @param s Input string (not modified)
 * @return New uppercase string
 *
 * @code
 *     std::string result = wbr::str::strupper<std::string>("Hello World");
 *     // result == "HELLO WORLD"
 * @endcode
 */
template<StringType S, StringType O = std::string>
O strupper (const S& s) {
    O out;
    std::ranges::transform(s, std::back_inserter(out), toupper);
    return out;
}

using TokenAction = std::function<void(std::string_view)>;
using TokenFilter = std::function<bool(std::string_view)>;

/** @brief Tokenizes a string and applies an action to each token.

 * This function splits the input string using specified delimiters and applies a given callable to each resulting token.
 *
 * @param sv The string to be tokenized.
 * @param action A callable that takes a std::string_view parameter. It will be invoked for each token found in the input string.
 * @param delimiters The set of characters used to split the input string into tokens. Defaults to " " (space).
 *
 * Example:
 * @code
 *     auto print_token = [](std::string_view token) { std::cout << token << '\n'; };
 *     tokenize_callback("Hello world", print_token, " ");
 * @endcode
 */
inline void tokenize_callback (const std::string_view sv, const TokenAction& action, std::string_view delimiters = " ") {
    if ( sv.empty( ) )
        return;
    std::string_view::size_type pos_token_start = 0;
    std::string_view::size_type pos_token_end;
    do {
        pos_token_end = sv.find_first_of(delimiters, pos_token_start);
        action(sv.substr(pos_token_start, pos_token_end - pos_token_start));
        pos_token_start = pos_token_end + 1;
    } while ( pos_token_end != std::string_view::npos );
}

/** @brief Tokenizes a string view into substrings based on specified delimiters and a filtering predicate.
 *
 * This function splits the input string view `sv` into tokens using the characters in `delimiters`
 * as separators. Each token is then filtered using the provided `filter` predicate, and only those
 * tokens for which the filter returns true are included in the resulting vector of string views.
 *
 * @param sv The string view to be tokenized.
 * @param filter A predicate function that takes a string view and returns a boolean value. Only tokens
 *               for which this function returns true will be included in the result.
 * @param delimiters A string view containing characters used as delimiters for splitting `sv`.
 *                   Defaults to " " (a single space character).
 *
 * @return A vector of string views representing the filtered tokens extracted from `sv`. */
inline std::vector<std::string_view> tokenize_filtered (const std::string_view sv, TokenFilter filter, std::string_view delimiters = " ") {
    std::vector<std::string_view> tokens;
    tokenize_callback(sv, [&tokens, filter] (std::string_view token) {
        if ( filter(token) )
            tokens.emplace_back(token);
    }, delimiters);
    return tokens;
}

/**
 * @brief Tokenizes a string view using specified delimiters and applies a filter and an action to each token.
 *
 * This function allows for concise code by applying a filter and an action to each token generated from the input string.
 *
 * @param sv The string view to be tokenized.
 * @param filter A predicate that takes a string view and returns a boolean, indicating whether the token should be processed further.
 * @param action A function that takes a string view and performs some operation on it.
 * @param delimiters A string view containing the characters used as delimiters for tokenizing the input string. Defaults to " " (space).
 */
inline void tokenize (const std::string_view sv, TokenFilter filter, TokenAction action, std::string_view delimiters = " ") {
    tokenize_callback(sv, [&action, filter] (std::string_view token) {
        if ( filter(token) )
            action(token);
    }, delimiters);
}

/**
 * @brief Tokenizes a string based on specified delimiters.
 *
 * Splits the input string into tokens using the provided delimiters and returns them as a vector of string views.
 * The default delimiter is space character ' ', but this can be customized by specifying a different set of characters.
 *
 * @param str Input string to tokenize.
 * @param delimiters Characters used to separate tokens (default is space).
 * @return Vector of string views representing the tokens extracted from the input string.*/
inline std::vector<std::string_view> tokenize (const std::string_view str, std::string_view delimiters = " ") {
    if ( str.empty( ) )
        return { };

    std::vector<std::string_view> vsv;

    tokenize_callback(str, [&vsv] (std::string_view token) { vsv.emplace_back(token); }, delimiters);
    return vsv;
}

/** @brief Tokenizes a C-style string into a vector of string views based on specified delimiters.
 *
 * This function provides a convenient way to split a null-terminated character string
 * using the given delimiters. If the input string is nullptr, it returns an empty vector.
 * Otherwise, it converts the C-string to a std::string_view and delegates the tokenization
 * process to another overloaded version of this function that handles std::string_view inputs.
 *
 * @param str The null-terminated character string to be tokenized. If nullptr is provided,
 *            an empty vector will be returned.
 * @param delimiters The delimiters used to split the input string. Defaults to a single space
 *                   if not specified.
 * @return A std::vector<std::string_view> containing the tokens resulting from splitting
 *         the input string by the provided delimiters.
 */
inline std::vector<std::string_view> tokenize (const char* str, std::string_view delimiters = " ") {
    if ( str == nullptr )
        return { };

    return tokenize(std::string_view {str, strlen(str)}, delimiters);
}

/**
 * @brief Tokenizes a null input string using the specified delimiter.
 *
 * This function is designed to handle a specific case where the input string is nullptr. It returns an empty vector of string views, effectively providing a no-op for tokenization purposes when given
 * a null input.
 *
 * @param[in] ptr A pointer set to nullptr.
 * @param[in] delm The delimiter used for tokenizing (default is " "). This parameter is unused in this implementation as the function only handles nullptr inputs.
 *
 * @return An empty vector of string views, signifying that no tokens are generated from a null input.*/
inline std::vector<std::string_view> tokenize ([[maybe_unused]] std::nullptr_t ptr, [[maybe_unused]] std::string_view delm = " ") {
    return { };
}

template<typename T>
using TokenModifier = std::function<T(std::string_view)>;

/**
 * @brief Tokenizes a string and applies a modification function to each token.
 *
 * This function takes a string view, splits it into tokens based on the given delimiters,
 * and applies a modification function to each token. The modified tokens are then returned as a vector of strings.
 *
 * @param sv The input string to be tokenized.
 * @param modifier A function that modifies each token.
 * @param delimiters The characters used to split the input string into tokens (default is space).
 * @return A vector containing the modified tokens.
 */
template<typename T>
std::vector<T> tokenize_modify (const std::string_view sv, TokenModifier<T> modifier, std::string_view delimiters = " ") {
    std::vector<T> out;
    tokenize_callback(sv, [&out, modifier] (std::string_view token) { out.emplace_back(modifier(token)); }, delimiters);
    return out;
}

/** @brief Splits a string into tokens based on specified delimiters and modifies each token.

 * The function splits the input string `sv` using the provided `delimiters`.
 * Each token is filtered by `filter` and modified by `modifier`. The resulting modified tokens are returned as a vector of strings.
 *
 * @param sv Input string to be tokenized.
 * @param filter Function object that filters out tokens based on custom criteria.
 * @param modifier Function object that modifies each token.
 * @param delimiters Delimiters used for splitting the input string. Default is " " (space).
 * @return Vector of modified tokens.
 *
 * @par Example: Convert first letter of each word to uppercase
 * @code
 * auto result = wbr::str::tokenize_modify(
 *     "hello world from cpp",
 *     [](std::string_view) { return true; },  // keep all tokens
 *     [](std::string_view token) -> std::string {
 *         std::string s(token);
 *         if (!s.empty())
 *             s[0] = std::toupper(s[0]);
 *         return s;
 *     }
 * );
 * // result: {"Hello", "World", "From", "Cpp"}
 * @endcode
 *
 * @par Example: Filter and uppercase non-empty tokens
 * @code
 * auto result = wbr::str::tokenize_modify(
 *     "hello  world  ",
 *     [](std::string_view token) { return !token.empty(); },
 *     [](std::string_view token) { return wbr::str::strupper(std::string(token)); }
 * );
 * // result: {"HELLO", "WORLD"}
 * @endcode
 */
template<typename T>
std::vector<T> tokenize_modify (const std::string_view sv, TokenFilter filter, TokenModifier<T> modifier, std::string_view delimiters = " ") {
    std::vector<T> out;
    tokenize(sv, filter, [&out, modifier] (std::string_view token) { out.emplace_back(modifier(token)); }, delimiters);
    return out;
}

/** @brief Concept for types that can be checked for emptiness */
template<typename S>
concept CanBeEmpty = requires(S s) { wbr::str::empty(s); };

/** @brief Remove empty elements from container
 *
 * @param vec Container to filter
 * @return Container with empty elements removed
 *
 * @code
 *     std::vector<std::string> v = {"hello", "", "world", ""};
 *     auto result = wbr::str::removeEmptyTokens(v);
 *     // result: {"hello", "world"}
 * @endcode
 */
template<template<class> class V, class S>
    requires ContainerLike<V, S> && CanBeEmpty<S>
V<S> removeEmptyTokens (V<S> vec) {
    std::erase_if(vec, wbr::str::empty);
    return vec;
}

/** @brief Remove empty strings from vector
 *
 * @code
 *     std::vector<std::string_view> tokens = {"a", "", "b", "", "c"};
 *     auto result = wbr::str::removeEmptyTokens(tokens);
 *     // result: {"a", "b", "c"}
 * @endcode
 */
template<CanBeEmpty S>
std::vector<S> removeEmptyTokens (std::vector<S> tokens) {
    std::erase_if(tokens, empty);
    return tokens;
}

/** @brief Tokenize string after trimming whitespace
 *
 * First trims the input string, then tokenizes it. This removes leading/trailing
 * whitespace and ensures no empty tokens at the boundaries.
 *
 * @param str Input string
 * @param delimiters Delimiter characters
 * @return Vector of trimmed tokens
 *
 * @code
 *     auto tokens = wbr::str::tokenizeSimplified("  hello  world  ");
 *     // tokens: {"hello", "", "world"}
 * @endcode
 */
template<StringType String>
std::vector<std::string_view> tokenizeSimplified (const String& str, std::string_view delimiters = " ") {
    return tokenize(trim(str, delimiters), delimiters);
}

/** @brief Tokenize C-string after trimming */
inline std::vector<std::string_view> tokenizeSimplified (const char* str, std::string_view delimiters = " ") {
    if ( str == nullptr )
        return { };

    return tokenizeSimplified(std::string_view {str, strlen(str)}, delimiters);
}

/** @brief Tokenize nullptr (returns empty vector) */
inline std::vector<std::string_view> tokenizeSimplified (std::nullptr_t, std::string_view = " ") {
    return { };
}

/**
 *  @brief Parse string, with respect to quotes and escape symbols
 *
 *  This function parses input string input, splitting it to list of tokens with next rules:
 *  @li tokens are divided by any of @c dividerChars symbols (by default - space character):
 *  @code
 *   "this is simple text example", dividerChars = " "   ->   {"this", "is", "simple", "text", "example"}
 *   "this:is:simple:csv:example",  dividerChars = ":"   ->   {"this", "is", "simple", "csv",  "example"}
 *   @endcode
 *  @li all symbols in-between two quote symbols @c ' or two double quote symbols @c \" are parsed as single token. Quotes are not part of token and removed:
 *  @code
 *   "The song "Hotel California" is famous one"   ->   {"The", "song", "Hotel California", "is", "famous", "one"}
 *   "Another famous song is 'Jingle Bells'"       ->   {"Another", "famous", "song", "is", "Jingle Bells"}
 *   "And yet another song "What I've Done"        ->   {"And", "yet", "another", "song", "What I've Done"}
 *  @endcode
 *  @li divider symbols and quote symbols can be escaped by  @c escapeChar character (default is '\'), allowing them to be present in token as regular symbol:
 *  @code
 *    "Let\'s finish this task"                           ->   {"Let's", "finish", "this", "task"}
 *    "\"What\ I\'ve\ Done\" is a song of Linkin\ Park"   ->   {"\"What I've Done\"", "is", "a", "song", "of", "Linkin Park"}
 *  @endcode
 *
 *  Check unittests TokenizeEscapedTest in string_manipulations.unittest.cpp for more usage examples
 *
 *  @param[in] input input string for parsing.
 *  @param[in] dividerChars String with dividers that will be used to split tokens apart. Any character in this string is treated as a divider. Default is space character.
 *  @param[in] escapeChar Character that will be used for escaping divider and quote symbols (also can escape itself to allow escape character to be present in token). Default value is @c.
 *  @return vector of strings in case of successful parse.
 *
 *  @see TokenizeEscapedTest
 *  @see tokenizeEscapedInput
 */
inline std::vector<std::string> tokenizeEscaped (std::string_view input, std::string_view dividerChars = " ", char escapeChar = '\\') {
    auto is_quote   = [] (char c) { return c == '\'' || c == '"'; };
    auto is_divider = [dividerChars] (char c) { return dividerChars.find(c) != std::string_view::npos; };
    auto is_escape  = [escapeChar] (char c) { return c == escapeChar; };

    if ( is_divider(escapeChar) )
        return { };

    enum class E_ParserState {
        regular,
        regularEscape,
        quote,
        quoteEscape,
    };
    E_ParserState            curState {E_ParserState::regular};
    char                     quoteChar {'\0'};
    std::vector<std::string> tokensVec;

    auto startNewToken = [&tokensVec] ( ) { tokensVec.emplace_back( ); };
    auto appendChar    = [&tokensVec] (char c) { tokensVec.back( ).append(1, c); };

    startNewToken( );
    for ( const char curChar: input ) {
        switch ( curState ) {
            case E_ParserState::regular:
                if ( is_divider(curChar) ) {
                    startNewToken( );
                } else if ( is_escape(curChar) ) {
                    curState = E_ParserState::regularEscape;
                } else if ( is_quote(curChar) ) {
                    curState  = E_ParserState::quote;
                    quoteChar = curChar;
                } else
                    appendChar(curChar);
                break;

            case E_ParserState::quote:
                if ( is_escape(curChar) ) {
                    curState = E_ParserState::quoteEscape;
                } else if ( curChar == quoteChar ) {
                    curState = E_ParserState::regular;
                } else
                    appendChar(curChar);
                break;

            case E_ParserState::regularEscape:
                if ( is_divider(curChar) || is_escape(curChar) || is_quote(curChar) ) {
                    appendChar(curChar);
                    curState = E_ParserState::regular;
                    break;
                }
                return { };

            case E_ParserState::quoteEscape:
                if ( is_divider(curChar) || is_escape(curChar) || is_quote(curChar) ) {
                    appendChar(curChar);
                    curState = E_ParserState::quote;
                    break;
                }
                return { };
        }
    }

    if ( curState != E_ParserState::regular )
        return { };

    return tokensVec;
}

/** @brief Split string at first occurrence of delimiter
 *
 * @param str Input string
 * @param delimiter Delimiter characters (any character in the string is treated as delimiter)
 * @return Optional pair of trimmed string views (before and after delimiter), or nullopt if delimiter not found or first part is empty
 *
 * @code
 *     auto result = wbr::str::splitAtFirst("key = value", "=");
 *     // result.has_value() == true
 *     // result->first == "key"
 *     // result->second == "value"
 *
 *     auto result2 = wbr::str::splitAtFirst("path/to/file.txt", "/");
 *     // result2->first == "path"
 *     // result2->second == "to/file.txt"
 * @endcode
 */
inline std::optional<std::pair<std::string_view, std::string_view>> splitAtFirst (std::string_view str, std::string_view delimiter = " ") {
    return splitAtPos(str, str.find_first_of(delimiter));
}

/** @brief Split C-string at first occurrence of delimiter */
inline std::optional<std::pair<std::string_view, std::string_view>> splitAtFirst (const char* str, std::string_view delimiter = " ") {
    if ( str == nullptr )
        return std::nullopt;

    return splitAtFirst(std::string_view {str, strlen(str)}, delimiter);
}

/** @brief Split string at last occurrence of delimiter
 *
 * @param str Input string
 * @param delimiter Delimiter characters
 * @return Optional pair of trimmed string views (before and after last delimiter), or nullopt if not found
 *
 * @code
 *     auto result = wbr::str::splitAtLast("path/to/file.txt", "/");
 *     // result->first == "path/to"
 *     // result->second == "file.txt"
 *
 *     auto result2 = wbr::str::splitAtLast("one.two.three", ".");
 *     // result2->first == "one.two"
 *     // result2->second == "three"
 * @endcode
 */
inline std::optional<std::pair<std::string_view, std::string_view>> splitAtLast (std::string_view str, std::string_view delimiter = " ") {
    return splitAtPos(str, str.find_last_of(delimiter));
}

/** @brief Split string at last occurrence of single character delimiter */
inline std::optional<std::pair<std::string_view, std::string_view>> splitAtLast (std::string_view str, char delimiter = ' ') {
    return splitAtPos(str, str.find_last_of(delimiter));
}

/** @brief Split C-string at last occurrence of delimiter */
inline std::optional<std::pair<std::string_view, std::string_view>> splitAtLast (const char* str, std::string_view delimiter = " ") {
    if ( str == nullptr )
        return std::nullopt;

    return splitAtLast(std::string_view {str, strlen(str)}, delimiter);
}

/** @brief Join strings from iterator range
 *
 * @param b Begin iterator
 * @param e End iterator
 * @param delimiter Delimiter string to place between elements
 * @return Joined string
 *
 * @code
 *     std::vector<std::string> words = {"one", "two", "three"};
 *     std::string result = wbr::str::join(words.begin(), words.end(), "-");
 *     // result == "one-two-three"
 * @endcode
 */
template<InputStrIt InputIt>
std::string join (InputIt b, InputIt e, std::string_view delimiter = ", ") {
    std::string out;
    if ( b == e )
        return out;

    out.append(*b);
    for ( auto it = b + 1; it != e; ++it )
        out.append(delimiter).append(*it);

    return out;
}

/** @brief Join strings from vector
 *
 * @param v Vector of strings
 * @param delimiter Delimiter string to place between elements
 * @return Joined string
 *
 * @code
 *     std::vector<std::string_view> parts = {"hello", "world", "!"};
 *     std::string result = wbr::str::join(parts, " ");
 *     // result == "hello world !"
 * @endcode
 */
template<StringType String>
std::string join (const std::vector<String>& v, std::string_view delimiter = ", ") {
    return join(v.cbegin( ), v.cend( ), delimiter);
}

/** @brief Join two strings
 *
 * @code
 *     std::string result = wbr::str::join("first", "second", " :: ");
 *     // result == "first :: second"
 * @endcode
 */
template<StringType String>
std::string join (const String a, const String& b, std::string_view delimiter = ", ") {
    return join(std::vector<String> {a, b}, delimiter);
}

/** @brief Join pair of strings
 *
 * @code
 *     auto p = std::make_pair("key", "value");
 *     std::string result = wbr::str::join(p, " = ");
 *     // result == "key = value"
 * @endcode
 */
template<StringType String>
std::string join (const std::pair<String, String>& p, std::string_view delimiter = ", ") {
    return join(p.first, p.second, delimiter);
}

/** @brief Join strings from vector, skipping empty strings
 *
 * @param vec Vector of strings
 * @param delimiter Delimiter string to place between elements
 * @return Joined string with empty elements removed
 *
 * @code
 *     std::vector<std::string> parts = {"hello", "", "world", ""};
 *     std::string result = wbr::str::joinSkipEmpty(parts, " ");
 *     // result == "hello world"
 * @endcode
 */
template<StringType String>
std::string joinSkipEmpty (std::vector<String> vec, std::string_view delimiter = ", ") {
    std::erase_if(vec, empty);

    return join(vec, delimiter);
}

/** @brief Replace all occurrences of substring in string (in-place)
 *
 * @param inout String to modify
 * @param what Substring to find
 * @param with Replacement string
 * @return Number of replacements made
 *
 * @code
 *     std::string text = "hello world, hello universe";
 *     size_t count = wbr::str::replaceAll(text, "hello", "hi");
 *     // text == "hi world, hi universe"
 *     // count == 2
 * @endcode
 */
// taken cpp reference (https://en.cppreference.com/w/cpp/string/basic_string/replace)
template<class STR, typename SV>
std::size_t replaceAll (STR& inout, SV what, SV with) {
    if ( what.empty( ) )
        return 0;
    if ( what == with )
        return 0;
    std::size_t count {0};
    auto        pos = inout.find(what);
    while ( pos != std::string::npos ) {
        count++;
        inout.replace(pos, what.length( ), with);
        pos += with.length( );
        pos = inout.find(what, pos);
    }
    return count;
}

/** @brief Remove all occurrences of substring from string (in-place)
 *
 * @param inout String to modify
 * @param what Substring to remove
 * @return Number of removals made
 *
 * @code
 *     std::string text = "hel-lo wor-ld";
 *     wbr::str::removeAll(text, "-");
 *     // text == "hello world"
 * @endcode
 */
template<class STR, class SV>
std::size_t removeAll (STR& inout, SV what) {
    return replaceAll(inout, what, STR { });
}

/** @brief Replace characters that match predicate
 *
 * @param str Input string
 * @param checkfunction Predicate returning true for characters to replace
 * @param replace_char Replacement character
 * @return New string with characters replaced
 *
 * @code
 *     auto is_vowel = [](char c) { return std::string_view("aeiou").find(c) != std::string_view::npos; };
 *     std::string result = wbr::str::replaceCharacters("hello world", is_vowel, '*');
 *     // result == "h*ll* w*rld"
 * @endcode
 */
inline std::string replaceCharacters (std::string_view str, const std::function<bool(char)>& checkfunction, char replace_char) {
    std::string out;

    std::ranges::transform(str, std::back_inserter(out), [&checkfunction, replace_char] (char c) { return checkfunction(c) ? replace_char : c; });

    return out;
}

/** @brief Replace characters from charset with replacement character
 *
 * @param str Input string
 * @param charset Set of characters to replace
 * @param replace_char Replacement character
 * @return New string with characters replaced
 *
 * @code
 *     std::string result = wbr::str::replaceCharacters("hello-world_test", "-_", ' ');
 *     // result == "hello world test"
 * @endcode
 */
[[nodiscard]] inline std::string replaceCharacters (std::string_view str, std::string_view charset, char replace_char) {
    return replaceCharacters(str, [charset] (char c) { return charset.find(c) != std::string_view::npos; }, replace_char);
}

/** @brief Check if string starts with prefix
 *
 * @code
 *     bool result = wbr::str::starts_with("hello world", "hello");  // true
 *     bool result2 = wbr::str::starts_with("test", "exam");  // false
 * @endcode
 */
constexpr bool starts_with (std::string_view str, std::string_view prefix) {
    return str.starts_with(prefix);
}

/** @brief Check if string ends with suffix
 *
 * @code
 *     bool result = wbr::str::ends_with("test.txt", ".txt");  // true
 *     bool result2 = wbr::str::ends_with("file.cpp", ".h");  // false
 * @endcode
 */
constexpr bool ends_with (std::string_view str, std::string_view suffix) {
    return str.ends_with(suffix);
}

/** @brief Remove N characters from beginning of string (in-place)
 *
 * @code
 *     std::string text = "hello";
 *     wbr::str::remove_prefix(text, 2);
 *     // text == "llo"
 * @endcode
 */
template<StringType S>
S& remove_prefix (S& str, size_t n) {
    str.erase(0, n);
    return str;
}

/** @brief Remove N characters from end of string (in-place)
 *
 * @code
 *     std::string text = "hello";
 *     wbr::str::remove_suffix(text, 2);
 *     // text == "hel"
 * @endcode
 */
template<StringType S>
S& remove_suffix (S& str, size_t n) {
    str.erase(str.length( ) - n, n);
    return str;
}

/** @brief Remove prefix string if present (in-place)
 *
 * @code
 *     std::string text = "prefix_text";
 *     wbr::str::remove_prefix(text, "prefix_");
 *     // text == "text"
 * @endcode
 */
template<StringType S>
S& remove_prefix (S& s, std::string_view prefix) {
    if ( wbr::str::starts_with(s, prefix) )
        remove_prefix(s, prefix.length( ));
    return s;
}

/** @brief Remove suffix string if present (in-place)
 *
 * @code
 *     std::string text = "file.txt";
 *     wbr::str::remove_suffix(text, ".txt");
 *     // text == "file"
 * @endcode
 */
template<StringType S>
S& remove_suffix (S& s, std::string_view suffix) {
    if ( wbr::str::ends_with(s, suffix) )
        remove_suffix(s, suffix.length( ));
    return s;
}

/** @brief Remove N characters from beginning of string_view
 *
 * @code
 *     std::string_view sv = wbr::str::remove_prefix("hello", 2);
 *     // sv == "llo"
 * @endcode
 */
constexpr std::string_view remove_prefix (std::string_view sv, size_t n) {
    sv.remove_prefix(n);
    return sv;
}

/** @brief Remove prefix from string_view if present
 *
 * @code
 *     std::string_view sv = wbr::str::remove_prefix("http://example.com", "http://");
 *     // sv == "example.com"
 * @endcode
 */
constexpr std::string_view remove_prefix (std::string_view sv, std::string_view prefix) {
    if ( wbr::str::starts_with(sv, prefix) )
        sv.remove_prefix(prefix.length( ));
    return sv;
}

/** @brief Remove N characters from end of string_view */
constexpr std::string_view remove_suffix (std::string_view sv, size_t n) {
    sv.remove_suffix(n);
    return sv;
}

/** @brief Remove suffix from string_view if present
 *
 * @code
 *     std::string_view sv = wbr::str::remove_suffix("filename.txt", ".txt");
 *     // sv == "filename"
 * @endcode
 */
constexpr std::string_view remove_suffix (std::string_view sv, std::string_view suffix) {
    if ( wbr::str::ends_with(sv, suffix) )
        sv.remove_suffix(suffix.length( ));
    return sv;
}

/** @brief Case-insensitive string comparison
 *
 * @param a First string
 * @param b Second string
 * @return True if strings are equal ignoring case
 *
 * @code
 *     bool result = wbr::str::iequals("Hello", "HELLO");  // true
 *     bool result2 = wbr::str::iequals("test", "TEST");  // true
 * @endcode
 */
inline bool iequals (std::string_view a, std::string_view b) {
    return std::ranges::equal(a, b, [] (char aChar, char bChar) { return std::tolower(aChar) == std::tolower(bChar); });
}

/** @brief Match mode for string to number conversion
 *
 * - full: Entire string must be a valid number
 * - partial: Number is extracted from beginning, rest is ignored
 */
enum class num_match_t {
    full,
    partial,
};

/** @brief Convert num_match_t enum to string */
[[nodiscard]] constexpr const char* to_string (num_match_t e, const char* defValue = nullptr) noexcept {
    switch ( e ) {
        using enum num_match_t;
        case full:    return "full";
        case partial: return "partial";
    }
    return defValue;
}

/** @brief Convert string to number with specified base
 *
 * @tparam I Integer type to convert to
 * @tparam match Matching mode (full or partial)
 * @param sv String to convert
 * @param base Number base (2-36)
 * @return Converted number, or 0 on error
 *
 * @code
 *     int result = wbr::str::num<int>("123", 10);  // 123
 *     int hex = wbr::str::num<int>("FF", 16);  // 255
 *     int partial = wbr::str::num<int, wbr::str::num_match_t::partial>("123abc", 10);  // 123
 *     int full = wbr::str::num<int, wbr::str::num_match_t::full>("123abc", 10);  // 0 (error)
 * @endcode
 */
template<std::integral I = int, num_match_t match = num_match_t::partial>
I num (std::string_view sv, int base) {
    I ret {0};
    auto [ptr, err] = std::from_chars(sv.data( ), sv.data( ) + sv.length( ), ret, base);
    if ( err != std::errc { } )
        return 0;
    if constexpr ( match == num_match_t::full ) {
        if ( ptr != sv.data( ) + sv.length( ) )
            return 0;
    }
    return ret;
}

template<std::integral I = int, num_match_t match = num_match_t::partial>
I num (std::string_view sv, int base, std::errc& ec) {
    I ret {0};
    auto [ptr, err] = std::from_chars(sv.data( ), sv.data( ) + sv.length( ), ret, base);
    if constexpr ( match == num_match_t::full ) {
        if ( ptr != sv.data( ) + sv.length( ) ) {
            ec = std::errc::argument_out_of_domain;
            return 0;
        }
    }
    ec = err;
    return ret;
}

#if defined(__cpp_lib_to_chars)
    #define WBR_STR_NUM_DOUBLE_HAS_ERROR_CHECK 1
template<typename I>
concept arithmetic = std::is_integral_v<I> || std::is_floating_point_v<I>;

template<arithmetic I = int, num_match_t match = num_match_t::partial>
I num (std::string_view sv) {
    I ret {0};
    auto [ptr, err] = std::from_chars(sv.data( ), sv.data( ) + sv.length( ), ret);
    if ( err != std::errc { } )
        return 0;
    if constexpr ( match == num_match_t::full ) {
        if ( ptr != sv.data( ) + sv.length( ) )
            return 0;
    }
    return ret;
}

template<arithmetic I = int, num_match_t match = num_match_t::partial>
I num (std::string_view sv, std::errc& ec) {
    I ret {0};
    auto [ptr, err] = std::from_chars(sv.data( ), sv.data( ) + sv.length( ), ret);
    if constexpr ( match == num_match_t::full ) {
        if ( ptr != sv.data( ) + sv.length( ) ) {
            ec = std::errc::argument_out_of_domain;
            return 0;
        }
    }
    ec = err;
    return ret;
}
#else
    #define WBR_STR_NUM_DOUBLE_HAS_ERROR_CHECK 0

template<std::integral I = int, num_match_t match = num_match_t::partial>
I num (std::string_view sv) {
    return num<I, match>(sv, 10);
}

template<std::integral I = int, num_match_t match = num_match_t::partial>
I num (std::string_view sv, std::errc& ec) {
    return num<I, match>(sv, 10, ec);
}

    #include <sstream>

template<std::floating_point I>
I num (std::string_view sv) {
    std::stringstream str;
    str << sv;
    I ret;
    str >> ret;
    return ret;
}

template<std::floating_point I>
I num (std::string_view sv, std::errc& ec) {
    std::stringstream str;
    str << sv;
    I ret;
    str >> ret;

    ec = str ? std::errc { } : std::errc::invalid_argument;

    return ret;
}
#endif

/** @brief Quote symbol types for string quoting operations */
enum QuoteSymbol : char { Single = '\'', Double = '"' };

/** @brief Check if character is a quote character (single or double)
 *
 * @code
 *     bool r1 = wbr::str::isQuote('"');   // true
 *     bool r2 = wbr::str::isQuote('\'');  // true
 *     bool r3 = wbr::str::isQuote('a');   // false
 * @endcode
 */
inline bool isQuote (char c) noexcept {
    return c == QuoteSymbol::Single || c == QuoteSymbol::Double;
}

/**
 * @brief put string in quotes, escaping quote and escape symbols
 * @param inStr input string
 * @param quote quote symbol to use
 * @param escape escape symbol for escaping
 * @return string in quotes with all quote and escape symbols escaped
 *
 * @code
 *     std::string result = wbr::str::quoteString("hello world");
 *     // result == "\"hello world\""
 *
 *     std::string result2 = wbr::str::quoteString("it's fine", wbr::str::QuoteSymbol::Single);
 *     // result2 == "'it\\'s fine'"
 * @endcode
 */
inline std::string quoteString (std::string_view inStr, QuoteSymbol quote = QuoteSymbol::Double, char escape = '\\') {
    std::string outStr;
    outStr.append(1, quote);
    for ( char c: inStr ) {
        if ( c == quote || c == escape )
            outStr.append(1, escape);
        outStr.append(1, c);
    }
    outStr.append(1, quote);

    return outStr;
}

/**
 * @brief escape all 'special' symbols
 *
 * Check if string has any 'special' symbol (quote, dividers, escape) and escape them with @c escape symbol
 * @param[in] inStr input string
 * @param[in] dividers set of symbols that can be used as divider
 * @param[in] escape symbol, used for escaping
 * @return escaped string
 *
 * @code
 *     std::string result = wbr::str::escapeString("hello world");
 *     // result == "hello\\ world"
 *
 *     std::string result2 = wbr::str::escapeString("a,b,c", ",");
 *     // result2 == "a\\,b\\,c"
 * @endcode
 */
inline std::string escapeString (std::string_view inStr, std::string_view dividers = " ", char escape = '\\') {
    std::string outStr;
    for ( char c: inStr ) {
        if ( isQuote(c) || c == escape || std::ranges::find(dividers, c) != dividers.cend( ) )
            outStr.append(1, escape);
        outStr.append(1, c);
    }

    return outStr;
}

/** @brief Quote string only if it contains special characters
 *
 * @param inStr Input string
 * @param dividers Characters that trigger quoting
 * @param quote Quote symbol to use if needed
 * @param escape Escape character
 * @return Quoted string if needed, otherwise original string
 *
 * @code
 *     std::string r1 = wbr::str::quoteStringOnlyIfNeed("hello");
 *     // r1 == "hello"
 *
 *     std::string r2 = wbr::str::quoteStringOnlyIfNeed("hello world");
 *     // r2 == "\"hello world\""
 * @endcode
 */
inline std::string quoteStringOnlyIfNeed (std::string_view inStr, std::string_view dividers = " ", QuoteSymbol quote = QuoteSymbol::Double, char escape = '\\') {
    constexpr std::array<QuoteSymbol, 2> quoteSet {QuoteSymbol::Single, QuoteSymbol::Double};

    std::string outStr;
    const bool  req = std::ranges::find_first_of(inStr, dividers) != inStr.end( ) || std::ranges::find_first_of(inStr, quoteSet) != inStr.end( );
    if ( req )
        outStr.append(1, quote);
    for ( char c: inStr ) {
        if ( c == quote || c == escape )
            outStr.append(1, escape);
        outStr.append(1, c);
    }
    if ( req )
        outStr.append(1, quote);
    return outStr;
}
}  // namespace wbr::str
