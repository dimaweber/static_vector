/** @file
 *
 *  @brief function for manipulating string
 * */

#pragma once

#include <system_error>
#if __has_include(<fmt/ranges.h>)
    #include <fmt/ranges.h>
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

/** @brief all string manipulation string are located in SkString namespace
 *
 *  @todo rename it to @c skl::str
 */
namespace wbr::str {
template<class S>
concept StringType = requires(S s) {
                         s.find_first_of(' ', 0);
                         s.size( );
                         s.data( );
                         s.empty( );
                     };
template<typename InputIt>
concept InputStrIt = requires(InputIt i) {
                         i++;
                         { *i } -> std::convertible_to<std::string_view>;
                     };

/** @brief helper predicate to check if string is empty
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
inline bool empty (std::string_view str) {
    return str.empty( );
}

inline bool isspace (char chr) {
    return std::isspace(static_cast<unsigned char>(chr)) != 0;
}

inline bool isdigit (char chr) {
    return std::isdigit(static_cast<unsigned char>(chr)) != 0;
}

inline bool isxdigit (char chr) {
    return std::isxdigit(static_cast<unsigned char>(chr)) != 0;
}

inline bool islower (char chr) {
    return std::islower(static_cast<unsigned char>(chr)) != 0;
}

inline bool isupper (char chr) {
    return std::isupper(static_cast<unsigned char>(chr)) != 0;
}

inline bool iscntrl (char chr) {
    return std::iscntrl(static_cast<unsigned char>(chr)) != 0;
}

inline bool isalnum (char ch) {
    return std::isalnum(static_cast<unsigned char>(ch));
}

inline bool isalpha (char ch) {
    return std::isalpha(static_cast<unsigned char>(ch));
}

template<typename T>
std::string asHex (const T& arr, std::string_view divider = " ") {
    return fmt::format("{:02x}", fmt::join(arr.begin( ), arr.end( ), divider));
}

inline std::string asHex (const char* arr, std::string_view divider = " ") {
    return asHex(std::string_view {arr}, divider);
}

// template <typename T>
// inline std::string asHex (const std::span<T>  arr, std::string_view divider = " ")
//{
//     return asHex(std::string_view {arr.data(), a}, divider);
// }
/**
 * @brief remove  leading and trailing whitespaces from string
 *
 * @param[in] input string
 * @param[in] symbols list of symbols that should be considered a whitespace. By default ' ' and tabulation are used.
 * @return return new string, that begins with first non-white space character of original string and end at last non-white space character of original string
 */
inline std::string_view trimWhitespaces (std::string_view str, std::string_view symbols = " \t") {
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

template<std::invocable<char> P>
inline std::string_view trimWhitespaces (std::string_view str, P is_space_predicate) {
    if ( str.empty( ) )
        return { };

    auto pos_b = std::find_if_not(str.cbegin( ), str.cend( ), is_space_predicate);
    if ( pos_b == str.cend( ) )
        return { };
    auto pos_e = std::find_if_not(str.crbegin( ), str.crend( ), is_space_predicate);
    return std::string_view {pos_b, pos_e.base( )};
}

/**
 * @brief split string into two parts at given position
 *
 * Helper function to split single string into 2 parts. Both splat parts will be trimmed.
 * If all symbols before pos are whitespaces -- function will fail
 * if pos is greater then input string length -- first part will be equal to trimmed input string and second part will be empty
 *
 * Examples
 *    splitAtPos("Hello World!", 5) --> "Hello" "World!"
 *
 * @param[in] str input string. should not be empty
 * @param[in] pos position at which string should be splitted
 * @return if fail to split - return std::nullopt, otherwise return pair of strings
 * @see trimWhitespace
 * @see splitAtFirst
 * @see splitAtLast
 */
inline std::optional<std::pair<std::string_view, std::string_view>> splitAtPos (std::string_view str, typename std::string_view::size_type pos) {
    if ( pos == std::string_view::npos )
        return std::nullopt;

    std::string_view key = {str.data( ), pos};
    std::string_view val = {str.data( ) + pos + 1, str.size( ) - pos - 1};
    key                  = trimWhitespaces(key);
    val                  = trimWhitespaces(val);

    if ( key.empty( ) )
        return std::nullopt;

    return std::pair {key, val};
}

template<StringType S>
S& strlower (S& s) {
    std::ranges::transform(s, s.begin( ), tolower);
    return s;
}

template<StringType S>
S& strupper (S& s) {
    std::ranges::transform(s, s.begin( ), toupper);
    return s;
}

template<StringType S, StringType O = std::string>
O strupper (const S& s) {
    O out;
    std::ranges::transform(s, std::back_inserter(out), toupper);
    return out;
}

inline std::vector<std::string_view> tokenize (const std::string_view str, std::string_view delimiters = " ") {
    if ( str.empty( ) )
        return { };

    std::vector<std::string_view> vsv;
    std::string_view::size_type   pos_token_start = 0;
    std::string_view::size_type   pos_token_end;

    do {
        pos_token_end = str.find_first_of(delimiters, pos_token_start);
        vsv.emplace_back(str.data( ) + pos_token_start, (pos_token_end == std::string_view::npos) ? str.size( ) - pos_token_start : pos_token_end - pos_token_start);
        pos_token_start = pos_token_end + 1;
    } while ( pos_token_end != std::string_view::npos );

    return vsv;
}

inline std::vector<std::string_view> tokenize (const char* str, std::string_view delimiters = " ") {
    if ( str == nullptr )
        return { };

    return tokenize(std::string_view {str, strlen(str)}, delimiters);
}

inline std::vector<std::string_view> tokenize (std::nullptr_t, std::string_view = " ") {
    return { };
}

template<typename S>
concept CanBeEmpty = requires(S s) { wbr::str::empty(s); };

//
// template<template<class> class V, class S>
// concept HasBeginEnd = requires(V<S> v) {
//    std::begin(v);
//    std::end(v);
//};
//
// template<template<class> class V, class S>
//    requires HasBeginEnd<V, S> && CanBeEmpty<S>
// V<S> removeEmptyTokens (V<S> vec)
//{
//    std::erase_if(vec, SkString::empty);
//    return vec;
//}

template<CanBeEmpty S>
std::vector<S> removeEmptyTokens (std::vector<S> tokens) {
    std::erase_if(tokens, empty);
    return tokens;
}

template<StringType String>
std::vector<std::string_view> tokenizeSimplified (const String& str, std::string_view delimiters = " ") {
    return tokenize(trimWhitespaces(str, delimiters), delimiters);
}

inline std::vector<std::string_view> tokenizeSimplified (const char* str, std::string_view delimiters = " ") {
    if ( str == nullptr )
        return { };

    return tokenizeSimplified(std::string_view {str, strlen(str)}, delimiters);
}

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
 *  @param[in] input input string for parsing
 *  @param[in] dividerChars string with dividers that will be used to split tokens apart. Any character in this string is treated as a divider. Default is space character.
 *  @param[in] escapeChar character that will be used for escaping divider and quote symbols (also can escape itself to allow escape character to be present in token). Default value is @c \\
 *  @return vector of strings in case of successful parse
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
    char                     quoteChar;
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

inline std::optional<std::pair<std::string_view, std::string_view>> splitAtFirst (std::string_view str, std::string_view delimiter = " ") {
    return splitAtPos(str, str.find_first_of(delimiter));
}

inline std::optional<std::pair<std::string_view, std::string_view>> splitAtFirst (const char* str, std::string_view delimiter = " ") {
    if ( str == nullptr )
        return std::nullopt;

    return splitAtFirst(std::string_view {str, strlen(str)}, delimiter);
}

inline std::optional<std::pair<std::string_view, std::string_view>> splitAtLast (std::string_view str, std::string_view delimiter = " ") {
    return splitAtPos(str, str.find_last_of(delimiter));
}

inline std::optional<std::pair<std::string_view, std::string_view>> splitAtLast (std::string_view str, char delimiter = ' ') {
    return splitAtPos(str, str.find_last_of(delimiter));
}

inline std::optional<std::pair<std::string_view, std::string_view>> splitAtLast (const char* str, std::string_view delimiter = " ") {
    if ( str == nullptr )
        return std::nullopt;

    return splitAtLast(std::string_view {str, strlen(str)}, delimiter);
}

template<InputStrIt InputIt>
std::string join (InputIt b, InputIt e, std::string_view delimiter = ", ") {
    std::string out;
    if ( b == e )
        return out;

    out.append(*b);
    for ( auto it = b + 1; it != e; it++ )
        out.append(delimiter).append(*it);

    return out;
}

template<StringType String>
std::string join (const std::vector<String>& v, std::string_view delimiter = ", ") {
    return join(v.cbegin( ), v.cend( ), delimiter);
}

template<StringType String>
std::string join (const String a, const String& b, std::string_view delimiter = ", ") {
    return join(std::vector<String> {a, b}, delimiter);
}

template<StringType String>
std::string join (const std::pair<String, String>& p, std::string_view delimiter = ", ") {
    return join(p.first, p.second, delimiter);
}

template<StringType String>
std::string joinSkipEmpty (std::vector<String> vec, std::string_view delimiter = ", ") {
    std::erase_if(vec, empty);

    return join(vec, delimiter);
}

// taken cpp reference (https://en.cppreference.com/w/cpp/string/basic_string/replace)
template<class STR>
std::size_t replaceAll (STR& inout, std::string_view what, std::string_view with) {
    if ( what.empty( ) )
        return 0;
    if ( what == with )
        return 0;
    std::size_t            count {0};
    std::string::size_type pos = inout.find(what);
    while ( pos != std::string::npos ) {
        count++;
        inout.replace(pos, what.length( ), with);
        pos += with.length( );
        pos = inout.find(what, pos);
    }
    return count;
}

template<class STR>
std::size_t removeAll (STR& inout, std::string_view what) {
    return replaceAll(inout, what, "");
}

inline std::string replaceCharacters (std::string_view str, const std::function<bool(char)>& checkfunction, char replace_char) {
    std::string out;

    std::ranges::transform(str, std::back_inserter(out), [&checkfunction, replace_char] (char c) { return checkfunction(c) ? replace_char : c; });

    return out;
}

[[nodiscard]] inline std::string replaceCharacters (std::string_view str, std::string_view charset, char replace_char) {
    return replaceCharacters(str, [charset] (char c) { return charset.find(c) != std::string_view::npos; }, replace_char);
}

constexpr bool starts_with (std::string_view str, std::string_view prefix) {
    return str.starts_with(prefix);
}

constexpr bool ends_with (std::string_view str, std::string_view suffix) {
    return str.ends_with(suffix);
}

template<StringType S>
S& remove_prefix (S& str, size_t n) {
    str.erase(0, n);
    return str;
}

template<StringType S>
S& remove_suffix (S& str, size_t n) {
    str.erase(str.length( ) - n, n);
    return str;
}

template<StringType S>
S& remove_prefix (S& s, std::string_view prefix) {
    if ( wbr::str::starts_with(s, prefix) )
        remove_prefix(s, prefix.length( ));
    return s;
}

template<StringType S>
S& remove_suffix (S& s, std::string_view suffix) {
    if ( wbr::str::ends_with(s, suffix) )
        remove_suffix(s, suffix.length( ));
    return s;
}

constexpr std::string_view remove_prefix (std::string_view sv, size_t n) {
    sv.remove_prefix(n);
    return sv;
}

constexpr std::string_view remove_prefix (std::string_view sv, std::string_view prefix) {
    if ( wbr::str::starts_with(sv, prefix) )
        sv.remove_prefix(prefix.length( ));
    return sv;
}

constexpr std::string_view remove_suffix (std::string_view sv, size_t n) {
    sv.remove_suffix(n);
    return sv;
}

constexpr std::string_view remove_suffix (std::string_view sv, std::string_view suffix) {
    if ( wbr::str::ends_with(sv, suffix) )
        sv.remove_suffix(suffix.length( ));
    return sv;
}

inline bool iequals (std::string_view a, std::string_view b) {
    return std::ranges::equal(a, b, [] (char aChar, char bChar) { return std::tolower(aChar) == std::tolower(bChar); });
}

enum class num_match_t {
    full,
    partial,
};

[[nodiscard]] constexpr const char* to_string (num_match_t e, const char* defValue = nullptr) noexcept {
    switch ( e ) {
        using enum num_match_t;
        case full:    return "full";
        case partial: return "partial";
    }
    return defValue;
}

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

template<arithmetic I = int, num_match_t match = num_match_t::full>
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
    #define WBR_STR_NUM_DOUBLE_HAS_ERROR_CHECK 1

template<std::integral I = int>
I num (std::string_view sv) {
    I ret {0};
    if ( auto [ptr, err] = std::from_chars(sv.data( ), sv.data( ) + sv.length( ), ret); err != std::errc { } )
        return 0;
    return ret;
}

template<std::integral I = int>
I num (std::string_view sv, std::errc& ec) {
    I ret {0};
    auto [ptr, err] = std::from_chars(sv.data( ), sv.data( ) + sv.length( ), ret);
    ec              = err;
    return ret;
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

enum QuoteSymbol : char { Single = '\'', Double = '"' };

inline bool isQuote (char c) noexcept {
    return c == QuoteSymbol::Single || c == QuoteSymbol::Double;
}

/**
 * @brief put string in quotes, escaping quote and escape symbols
 * @param inStr input string
 * @param quote quote symbol to use
 * @param escape escape symbol for escaping
 * @return string in quotes with all quote and escape symbols escaped
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

inline std::string quoteStringOnlyIfNeed (std::string_view inStr, std::string_view dividers = " ", QuoteSymbol quote = QuoteSymbol::Double, char escape = '\\') {
    const std::array<QuoteSymbol, 2> quoteSet {QuoteSymbol::Single, QuoteSymbol::Double};

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
