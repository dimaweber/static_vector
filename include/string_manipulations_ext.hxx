/** @file
 *
 *  @brief Extended string manipulation functions
 * */

#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "concepts.hxx"

/** @brief Extended string manipulation functions in wbr::str namespace
 */
namespace wbr::str {

// ============================================================================
// 1. String Padding Functions
// ============================================================================

/** @brief Pad string on the left to specified width
 *
 * @param str Input string
 * @param width Target width
 * @param fill Fill character (default: space)
 * @return Padded string
 *
 * @code
 *     std::string result = wbr::str::pad_left("hello", 10, '*');
 *     // result == "*****hello"
 * @endcode
 */
template<StringType S = std::string>
S pad_left (std::string_view str, size_t width, char fill = ' ') {
    if ( str.size( ) >= width )
        return S {str};

    S result;
    result.reserve(width);
    result.append(width - str.size( ), fill);
    result.append(str);
    return result;
}

/** @brief Pad string on the right to the specified width
 *
 * @param str Input string
 * @param width Target width
 * @param fill Fill character (default: space)
 * @return Padded string
 *
 * @code
 *     std::string result = wbr::str::pad_right("hello", 10, '-');
 *     // result == "hello-----"
 * @endcode
 */
inline std::string pad_right (std::string_view str, size_t width, char fill = ' ') {
    if ( str.size( ) >= width )
        return std::string {str};

    std::string result;
    result.reserve(width);
    result.append(str);
    result.append(width - str.size( ), fill);
    return result;
}

/** @brief Pad string on both sides to center it within the specified width
 *
 * @param str Input string
 * @param width Target width
 * @param fill Fill character (default: space)
 * @return Centered string
 *
 * @code
 *     std::string result = wbr::str::pad_center("hi", 10, ' ');
 *     // result == "    hi    "
 * @endcode
 */
inline std::string pad_center (std::string_view str, size_t width, char fill = ' ') {
    if ( str.size( ) >= width )
        return std::string {str};

    size_t      total_padding = width - str.size( );
    size_t      left_padding  = total_padding / 2;
    size_t      right_padding = total_padding - left_padding;
    std::string result;
    result.reserve(width);
    result.append(left_padding, fill);
    result.append(str);
    result.append(right_padding, fill);
    return result;
}

// ============================================================================
// 2. String Repeating
// ============================================================================

/** @brief Repeat string multiple times
 *
 * @param str String to repeat
 * @param count Number of repetitions
 * @return Repeated string
 *
 * @code
 *     std::string result = wbr::str::repeat("ab", 3);
 *     // result == "ababab"
 * @endcode
 */
inline std::string repeat (std::string_view str, size_t count) {
    if ( count == 0 || str.empty( ) )
        return { };

    std::string result;
    result.reserve(str.size( ) * count);
    for ( size_t i = 0; i < count; ++i )
        result.append(str);
    return result;
}

/** @brief Repeat character multiple times
 *
 * @param c Character to repeat
 * @param count Number of repetitions
 * @return String of repeated character
 *
 * @code
 *     std::string result = wbr::str::repeat('-', 5);
 *     // result == "-----"
 * @endcode
 */
inline std::string repeat (char c, size_t count) {
    return std::string(count, c);
}

// ============================================================================
// 3. Contains/Count Operations
// ============================================================================

/** @brief Check if string contains substring
 *
 * @param str String to search in
 * @param substr Substring to find
 * @return True if substring is found
 *
 * @code
 *     bool result = wbr::str::contains("hello world", "wor");
 *     // result == true
 * @endcode
 */
inline bool contains (std::string_view str, std::string_view substr) {
    return str.find(substr) != std::string_view::npos;
}

/** @brief Check if string contains any of the specified characters
 *
 * @param str String to search in
 * @param chars Characters to find
 * @return True if any character is found
 *
 * @code
 *     bool result = wbr::str::contains_any("hello", "xyz");
 *     // result == false
 *
 *     bool result2 = wbr::str::contains_any("hello", "aeiou");
 *     // result2 == true
 * @endcode
 */
inline bool contains_any (std::string_view str, std::string_view chars) {
    return str.find_first_of(chars) != std::string_view::npos;
}

/** @brief Check if string contains all of the specified characters
 *
 * @param str String to search in
 * @param chars Characters to find
 * @return True if all characters are found
 *
 * @code
 *     bool result = wbr::str::contains_all("hello world", "helo");
 *     // result == true
 *
 *     bool result2 = wbr::str::contains_all("hello", "xyz");
 *     // result2 == false
 * @endcode
 */
inline bool contains_all (std::string_view str, std::string_view chars) {
    for ( char c: chars ) {
        if ( str.find(c) == std::string_view::npos )
            return false;
    }
    return true;
}

/** @brief Count non-overlapping occurrences of substring
 *
 * @param str String to search in
 * @param substr Substring to count
 * @return Number of occurrences
 *
 * @code
 *     size_t count = wbr::str::count_occurrences("aaa bbb aaa", "aa");
 *     // count == 2 (non-overlapping)
 * @endcode
 */
inline size_t count_occurrences (std::string_view str, std::string_view substr) {
    if ( substr.empty( ) )
        return 0;

    size_t count = 0;
    size_t pos   = 0;
    while ( (pos = str.find(substr, pos)) != std::string_view::npos ) {
        ++count;
        pos += substr.size( );
    }
    return count;
}

/** @brief Count occurrences of character
 *
 * @param str String to search in
 * @param c Character to count
 * @return Number of occurrences
 *
 * @code
 *     size_t count = wbr::str::count_occurrences("hello world", 'l');
 *     // count == 3
 * @endcode
 */
inline size_t count_occurrences (std::string_view str, char c) {
    return std::count(str.begin( ), str.end( ), c);
}

// ============================================================================
// 4. String Truncation
// ============================================================================

/** @brief Truncate string to maximum length, adding suffix if truncated
 *
 * @param str Input string
 * @param max_length Maximum length (including suffix)
 * @param suffix Suffix to add when truncated (default: "...")
 * @return Truncated string
 *
 * @code
 *     std::string result = wbr::str::truncate_string("very long text here", 10);
 *     // result == "very lo..."
 * @endcode
 */
inline std::string truncate_string (std::string_view str, size_t max_length, std::string_view suffix = "...") {
    if ( str.size( ) <= max_length )
        return std::string {str};

    if ( max_length <= suffix.size( ) )
        return std::string {suffix.substr(0, max_length)};

    std::string result;
    result.reserve(max_length);
    result.append(str.substr(0, max_length - suffix.size( )));
    result.append(suffix);
    return result;
}

/** @brief Truncate string in the middle, keeping start and end
 *
 * @param str Input string
 * @param max_length Maximum length (including separator)
 * @param separator Separator for middle (default: "...")
 * @return Truncated string
 *
 * @code
 *     std::string result = wbr::str::truncate_string_middle("path/to/very/long/file.txt", 20, "...");
 *     // result == "path/to...file.txt"
 * @endcode
 */
inline std::string truncate_string_middle (std::string_view str, size_t max_length, std::string_view separator = "...") {
    if ( str.size( ) <= max_length )
        return std::string {str};

    if ( max_length <= separator.size( ) )
        return std::string {separator.substr(0, max_length)};

    size_t available = max_length - separator.size( );
    size_t start_len = (available + 1) / 2;
    size_t end_len   = available / 2;

    std::string result;
    result.reserve(max_length);
    result.append(str.substr(0, start_len));
    result.append(separator);
    result.append(str.substr(str.size( ) - end_len));
    return result;
}

// ============================================================================
// 5. Line Operations
// ============================================================================

/** @brief Split string into lines
 *
 * @param str Input string
 * @return Vector of lines
 *
 * @code
 *     auto result = wbr::str::lines("line1\nline2\nline3");
 *     // result == {"line1", "line2", "line3"}
 * @endcode
 */
inline std::vector<std::string_view> lines (std::string_view str) {
    std::vector<std::string_view> result;
    size_t                        start = 0;
    size_t                        pos;

    while ( (pos = str.find('\n', start)) != std::string_view::npos ) {
        auto line = str.substr(start, pos - start);
        // Remove \r if present (Windows line endings)
        if ( !line.empty( ) && line.back( ) == '\r' )
            line.remove_suffix(1);
        result.push_back(line);
        start = pos + 1;
    }

    // Add last line if not empty or if string doesn't end with newline
    if ( start < str.size( ) ) {
        auto line = str.substr(start);
        if ( !line.empty( ) && line.back( ) == '\r' )
            line.remove_suffix(1);
        result.push_back(line);
    }

    return result;
}

/** @brief Split string into lines and trim each line
 *
 * @param str Input string
 * @return Vector of trimmed lines
 *
 * @code
 *     auto result = wbr::str::lines_trimmed("  line1  \n  line2  ");
 *     // result == {"line1", "line2"}
 * @endcode
 */
inline std::vector<std::string_view> lines_trimmed(std::string_view str);

/** @brief Indent each line of string by specified number of spaces
 *
 * @param str Input string
 * @param spaces Number of spaces to indent
 * @return Indented string
 *
 * @code
 *     std::string result = wbr::str::indent("hello\nworld", 2);
 *     // result == "  hello\n  world"
 * @endcode
 */
inline std::string indent (std::string_view str, size_t spaces) {
    if ( spaces == 0 )
        return std::string {str};

    auto        line_vec = lines(str);
    std::string indent_str(spaces, ' ');
    std::string result;

    for ( size_t i = 0; i < line_vec.size( ); ++i ) {
        result.append(indent_str);
        result.append(line_vec[i]);
        if ( i < line_vec.size( ) - 1 )
            result.append("\n");
    }

    return result;
}

/** @brief Remove common leading whitespace from all lines
 *
 * @param str Input string
 * @return Dedented string
 *
 * @code
 *     std::string result = wbr::str::dedent("  hello\n  world");
 *     // result == "hello\nworld"
 * @endcode
 */
inline std::string dedent (std::string_view str) {
    auto line_vec = lines(str);
    if ( line_vec.empty( ) )
        return std::string {str};

    // Find minimum indentation
    size_t min_indent = std::string_view::npos;
    for ( const auto& line: line_vec ) {
        if ( line.empty( ) )
            continue;
        size_t indent = 0;
        while ( indent < line.size( ) && (line[indent] == ' ' || line[indent] == '\t') )
            ++indent;
        if ( indent < line.size( ) )  // Not all whitespace
            min_indent = std::min(min_indent, indent);
    }

    if ( min_indent == 0 || min_indent == std::string_view::npos )
        return std::string {str};

    // Remove common indentation
    std::string result;
    for ( size_t i = 0; i < line_vec.size( ); ++i ) {
        auto line = line_vec[i];
        if ( !line.empty( ) && line.size( ) >= min_indent )
            result.append(line.substr(min_indent));
        else
            result.append(line);
        if ( i < line_vec.size( ) - 1 )
            result.append("\n");
    }

    return result;
}

// ============================================================================
// 6. String Wrapping
// ============================================================================

/** @brief Wrap string to specified width
 *
 * @param str Input string
 * @param width Maximum line width
 * @return Vector of wrapped lines
 *
 * @code
 *     auto result = wbr::str::wrap("this is a long sentence", 10);
 *     // result == {"this is a", "long", "sentence"}
 * @endcode
 */
inline std::vector<std::string> wrap (std::string_view str, size_t width) {
    std::vector<std::string> result;
    if ( width == 0 )
        return result;

    size_t pos = 0;
    while ( pos < str.size( ) ) {
        // Skip leading whitespace
        while ( pos < str.size( ) && std::isspace(static_cast<unsigned char>(str[pos])) )
            ++pos;

        if ( pos >= str.size( ) )
            break;

        // Find end of current line
        size_t line_start = pos;
        size_t line_end   = pos;
        size_t last_space = std::string_view::npos;

        while ( line_end < str.size( ) && (line_end - line_start) < width ) {
            if ( std::isspace(static_cast<unsigned char>(str[line_end])) )
                last_space = line_end;
            ++line_end;
        }

        // If we reached end of string or found natural break
        if ( line_end >= str.size( ) ) {
            result.emplace_back(str.substr(line_start));
            break;
        }

        // Try to break at last space
        if ( last_space != std::string_view::npos && last_space > line_start ) {
            result.emplace_back(str.substr(line_start, last_space - line_start));
            pos = last_space + 1;
        } else {
            // Force break at width
            result.emplace_back(str.substr(line_start, width));
            pos = line_start + width;
        }
    }

    return result;
}

/** @brief Wrap string and join with separator
 *
 * @param str Input string
 * @param width Maximum line width
 * @param separator Line separator (default: newline)
 * @return Wrapped string
 *
 * @code
 *     std::string result = wbr::str::wrap_join("this is a long sentence", 10);
 *     // result == "this is a\nlong\nsentence"
 * @endcode
 */
inline std::string wrap_join (std::string_view str, size_t width, std::string_view separator = "\n") {
    auto        wrapped = wrap(str, width);
    std::string result;
    for ( size_t i = 0; i < wrapped.size( ); ++i ) {
        result.append(wrapped[i]);
        if ( i < wrapped.size( ) - 1 )
            result.append(separator);
    }
    return result;
}

// ============================================================================
// 7. Substring Extraction
// ============================================================================

/** @brief Extract substring between start and end markers
 *
 * @param str Input string
 * @param start Start marker
 * @param end End marker
 * @return Substring between markers, or empty if not found
 *
 * @code
 *     std::string_view result = wbr::str::substring_between("Hello [world]!", "[", "]");
 *     // result == "world"
 * @endcode
 */
inline std::string_view substring_between (std::string_view str, std::string_view start, std::string_view end) {
    size_t start_pos = str.find(start);
    if ( start_pos == std::string_view::npos )
        return { };

    start_pos += start.size( );
    size_t end_pos = str.find(end, start_pos);
    if ( end_pos == std::string_view::npos )
        return { };

    return str.substr(start_pos, end_pos - start_pos);
}

/** @brief Extract all substrings between start and end markers
 *
 * @param str Input string
 * @param start Start marker
 * @param end End marker
 * @return Vector of all substrings found between markers
 *
 * @code
 *     auto result = wbr::str::extract_all_between("a[1]b[2]c[3]", "[", "]");
 *     // result == {"1", "2", "3"}
 * @endcode
 */
inline std::vector<std::string_view> extract_all_between (std::string_view str, std::string_view start, std::string_view end) {
    std::vector<std::string_view> result;
    size_t                        pos = 0;

    while ( pos < str.size( ) ) {
        size_t start_pos = str.find(start, pos);
        if ( start_pos == std::string_view::npos )
            break;

        start_pos += start.size( );
        size_t end_pos = str.find(end, start_pos);
        if ( end_pos == std::string_view::npos )
            break;

        result.push_back(str.substr(start_pos, end_pos - start_pos));
        pos = end_pos + end.size( );
    }

    return result;
}

// ============================================================================
// 8. Case Conversion Variants
// ============================================================================

/** @brief Convert string to title case (capitalize first letter of each word)
 *
 * @param str Input string
 * @return Title case string
 *
 * @code
 *     std::string result = wbr::str::to_title_case("hello world");
 *     // result == "Hello World"
 * @endcode
 */
inline std::string to_title_case (std::string_view str) {
    std::string result;
    result.reserve(str.size( ));
    bool capitalize_next = true;

    for ( char c: str ) {
        if ( std::isspace(static_cast<unsigned char>(c)) ) {
            result.push_back(c);
            capitalize_next = true;
        } else if ( capitalize_next ) {
            result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
            capitalize_next = false;
        } else {
            result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
    }

    return result;
}

/** @brief Convert CamelCase or PascalCase to snake_case
 *
 * @param str Input string
 * @return snake_case string
 *
 * @code
 *     std::string result = wbr::str::to_snake_case("HelloWorld");
 *     // result == "hello_world"
 *
 *     std::string result2 = wbr::str::to_snake_case("someHTMLParser");
 *     // result2 == "some_html_parser"
 * @endcode
 */
inline std::string to_snake_case (std::string_view str) {
    std::string result;
    result.reserve(str.size( ) + str.size( ) / 2);  // Extra space for underscores

    for ( size_t i = 0; i < str.size( ); ++i ) {
        char c = str[i];

        if ( std::isupper(static_cast<unsigned char>(c)) ) {
            // Add underscore before uppercase letter if:
            // - Not at start
            // - Previous char is lowercase or digit
            // - Or next char is lowercase (for acronyms like "HTML")
            if ( i > 0 ) {
                char prev          = str[i - 1];
                bool prev_is_lower = std::islower(static_cast<unsigned char>(prev)) || std::isdigit(static_cast<unsigned char>(prev));
                bool next_is_lower = (i + 1 < str.size( )) && std::islower(static_cast<unsigned char>(str[i + 1]));

                if ( prev_is_lower || (next_is_lower && !std::islower(static_cast<unsigned char>(prev))) )
                    result.push_back('_');
            }
            result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        } else if ( c == ' ' || c == '-' ) {
            result.push_back('_');
        } else {
            result.push_back(c);
        }
    }

    return result;
}

/** @brief Convert snake_case or kebab-case to camelCase
 *
 * @param str Input string
 * @return camelCase string
 *
 * @code
 *     std::string result = wbr::str::to_camel_case("hello_world");
 *     // result == "helloWorld"
 *
 *     std::string result2 = wbr::str::to_camel_case("some-test-case");
 *     // result2 == "someTestCase"
 * @endcode
 */
inline std::string to_camel_case (std::string_view str) {
    std::string result;
    result.reserve(str.size( ));
    bool capitalize_next = false;

    for ( size_t i = 0; i < str.size( ); ++i ) {
        char c = str[i];

        if ( c == '_' || c == '-' || c == ' ' ) {
            capitalize_next = true;
        } else if ( capitalize_next ) {
            result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
            capitalize_next = false;
        } else {
            result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        }
    }

    return result;
}

/** @brief Convert string to kebab-case
 *
 * @param str Input string
 * @return kebab-case string
 *
 * @code
 *     std::string result = wbr::str::to_kebab_case("HelloWorld");
 *     // result == "hello-world"
 *
 *     std::string result2 = wbr::str::to_kebab_case("some_test_case");
 *     // result2 == "some-test-case"
 * @endcode
 */
inline std::string to_kebab_case (std::string_view str) {
    auto snake = to_snake_case(str);
    std::replace(snake.begin( ), snake.end( ), '_', '-');
    return snake;
}

// ============================================================================
// 9. Reverse Operations
// ============================================================================

/** @brief Reverse string
 *
 * @param str Input string
 * @return Reversed string
 *
 * @code
 *     std::string result = wbr::str::reverse("hello");
 *     // result == "olleh"
 * @endcode
 */
inline std::string reverse (std::string_view str) {
    return std::string {str.rbegin( ), str.rend( )};
}

/** @brief Reverse string in-place
 *
 * @param str String to reverse
 * @return Reference to modified string
 *
 * @code
 *     std::string text = "hello";
 *     wbr::str::reverse_inplace(text);
 *     // text == "olleh"
 * @endcode
 */
inline std::string& reverse_inplace (std::string& str) {
    std::reverse(str.begin( ), str.end( ));
    return str;
}

// ============================================================================
// 10. Common Prefix/Suffix Finding
// ============================================================================

/** @brief Find common prefix of multiple strings
 *
 * @param strings Span of strings to compare
 * @return Common prefix
 *
 * @code
 *     std::vector<std::string_view> v = {"prefix_a", "prefix_b", "prefix_c"};
 *     std::string_view result = wbr::str::common_prefix(v);
 *     // result == "prefix_"
 * @endcode
 */
inline std::string_view common_prefix (std::span<const std::string_view> strings) {
    if ( strings.empty( ) )
        return { };
    if ( strings.size( ) == 1 )
        return strings[0];

    std::string_view first = strings[0];
    size_t           len   = 0;

    while ( len < first.size( ) ) {
        char c = first[len];
        for ( size_t i = 1; i < strings.size( ); ++i ) {
            if ( len >= strings[i].size( ) || strings[i][len] != c )
                return first.substr(0, len);
        }
        ++len;
    }

    return first.substr(0, len);
}

/** @brief Find common suffix of multiple strings
 *
 * @param strings Span of strings to compare
 * @return Common suffix
 *
 * @code
 *     std::vector<std::string_view> v = {"a_suffix", "b_suffix", "c_suffix"};
 *     std::string_view result = wbr::str::common_suffix(v);
 *     // result == "_suffix"
 * @endcode
 */
inline std::string_view common_suffix (std::span<const std::string_view> strings) {
    if ( strings.empty( ) )
        return { };
    if ( strings.size( ) == 1 )
        return strings[0];

    std::string_view first = strings[0];
    size_t           len   = 0;

    while ( len < first.size( ) ) {
        char c = first[first.size( ) - 1 - len];
        for ( size_t i = 1; i < strings.size( ); ++i ) {
            const auto& str = strings[i];
            if ( len >= str.size( ) || str[str.size( ) - 1 - len] != c )
                return first.substr(first.size( ) - len);
        }
        ++len;
    }

    return first.substr(first.size( ) - len);
}

// ============================================================================
// 11. Whitespace Normalization
// ============================================================================

/** @brief Normalize whitespace (trim and collapse internal whitespace)
 *
 * @param str Input string
 * @return Normalized string
 *
 * @code
 *     std::string result = wbr::str::normalize_whitespace("  hello   world  ");
 *     // result == "hello world"
 * @endcode
 */
inline std::string normalize_whitespace (std::string_view str) {
    std::string result;
    result.reserve(str.size( ));
    bool in_whitespace = true;

    for ( char c: str ) {
        if ( std::isspace(static_cast<unsigned char>(c)) ) {
            if ( !in_whitespace && !result.empty( ) ) {
                result.push_back(' ');
                in_whitespace = true;
            }
        } else {
            result.push_back(c);
            in_whitespace = false;
        }
    }

    // Remove trailing space if added
    if ( !result.empty( ) && result.back( ) == ' ' )
        result.pop_back( );

    return result;
}

/** @brief Collapse all whitespace characters to single replacement character
 *
 * @param str Input string
 * @param replacement Replacement character (default: space)
 * @return String with collapsed whitespace
 *
 * @code
 *     std::string result = wbr::str::collapse_whitespace("a\t\nb  c", ' ');
 *     // result == "a b c"
 * @endcode
 */
inline std::string collapse_whitespace (std::string_view str, char replacement = ' ') {
    std::string result;
    result.reserve(str.size( ));

    for ( char c: str ) {
        if ( std::isspace(static_cast<unsigned char>(c)) )
            result.push_back(replacement);
        else
            result.push_back(c);
    }

    return result;
}

// ============================================================================
// 12. String Comparison Variants
// ============================================================================

/** @brief Check if string starts with any of the given prefixes
 *
 * @param str String to check
 * @param prefixes Possible prefixes
 * @return True if string starts with any prefix
 *
 * @code
 *     std::vector<std::string_view> prefixes = {"hi", "he", "ho"};
 *     bool result = wbr::str::starts_with_any("hello", prefixes);
 *     // result == true
 * @endcode
 */
inline bool starts_with_any (std::string_view str, std::span<const std::string_view> prefixes) {
    for ( const auto& prefix: prefixes ) {
        if ( str.starts_with(prefix) )
            return true;
    }
    return false;
}

/** @brief Check if string ends with any of the given suffixes
 *
 * @param str String to check
 * @param suffixes Possible suffixes
 * @return True if string ends with any suffix
 *
 * @code
 *     std::vector<std::string_view> suffixes = {".txt", ".md", ".cpp"};
 *     bool result = wbr::str::ends_with_any("file.txt", suffixes);
 *     // result == true
 * @endcode
 */
inline bool ends_with_any (std::string_view str, std::span<const std::string_view> suffixes) {
    for ( const auto& suffix: suffixes ) {
        if ( str.ends_with(suffix) )
            return true;
    }
    return false;
}

/** @brief Case-insensitive substring search
 *
 * @param haystack String to search in
 * @param needle Substring to find
 * @return True if substring is found (case-insensitive)
 *
 * @code
 *     bool result = wbr::str::icontains("Hello World", "WORLD");
 *     // result == true
 * @endcode
 */
inline bool icontains (std::string_view haystack, std::string_view needle) {
    if ( needle.empty( ) )
        return true;
    if ( haystack.size( ) < needle.size( ) )
        return false;

    for ( size_t i = 0; i <= haystack.size( ) - needle.size( ); ++i ) {
        bool match = true;
        for ( size_t j = 0; j < needle.size( ); ++j ) {
            if ( std::tolower(static_cast<unsigned char>(haystack[i + j])) != std::tolower(static_cast<unsigned char>(needle[j])) ) {
                match = false;
                break;
            }
        }
        if ( match )
            return true;
    }
    return false;
}

// ============================================================================
// 13. Character Set Operations
// ============================================================================

/** @brief Remove all occurrences of specified characters
 *
 * @param str Input string
 * @param chars_to_remove Characters to remove
 * @return String with characters removed
 *
 * @code
 *     std::string result = wbr::str::remove_chars("hello123world", "0123456789");
 *     // result == "helloworld"
 * @endcode
 */
inline std::string remove_chars (std::string_view str, std::string_view chars_to_remove) {
    std::string result;
    result.reserve(str.size( ));

    for ( char c: str ) {
        if ( chars_to_remove.find(c) == std::string_view::npos )
            result.push_back(c);
    }

    return result;
}

/** @brief Keep only specified characters
 *
 * @param str Input string
 * @param chars_to_keep Characters to keep
 * @return String with only specified characters
 *
 * @code
 *     std::string result = wbr::str::keep_only_chars("abc123def", "0123456789");
 *     // result == "123"
 * @endcode
 */
inline std::string keep_only_chars (std::string_view str, std::string_view chars_to_keep) {
    std::string result;
    result.reserve(str.size( ));

    for ( char c: str ) {
        if ( chars_to_keep.find(c) != std::string_view::npos )
            result.push_back(c);
    }

    return result;
}

/** @brief Remove characters matching predicate
 *
 * @param str Input string
 * @param predicate Predicate returning true for characters to remove
 * @return String with matching characters removed
 *
 * @code
 *     auto is_digit = [](char c) { return std::isdigit(c); };
 *     std::string result = wbr::str::remove_if("abc123def", is_digit);
 *     // result == "abcdef"
 * @endcode
 */
inline std::string remove_if (std::string_view str, std::function<bool(char)> predicate) {
    std::string result;
    result.reserve(str.size( ));

    for ( char c: str ) {
        if ( !predicate(c) )
            result.push_back(c);
    }

    return result;
}

// ============================================================================
// 14. Levenshtein Distance / Similarity
// ============================================================================

/** @brief Calculate Levenshtein distance between two strings
 *
 * Levenshtein distance is the minimum number of single-character edits
 * (insertions, deletions, or substitutions) required to change one string
 * into another.
 *
 * @param s1 First string
 * @param s2 Second string
 * @return Edit distance
 *
 * @code
 *     size_t dist = wbr::str::levenshtein_distance("kitten", "sitting");
 *     // dist == 3
 * @endcode
 */
inline size_t levenshtein_distance (std::string_view s1, std::string_view s2) {
    const size_t len1 = s1.size( );
    const size_t len2 = s2.size( );

    if ( len1 == 0 )
        return len2;
    if ( len2 == 0 )
        return len1;

    // Use single array and alternate between rows
    std::vector<size_t> prev_row(len2 + 1);
    std::vector<size_t> curr_row(len2 + 1);

    // Initialize first row
    for ( size_t j = 0; j <= len2; ++j )
        prev_row[j] = j;

    for ( size_t i = 1; i <= len1; ++i ) {
        curr_row[0] = i;

        for ( size_t j = 1; j <= len2; ++j ) {
            size_t cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;

            curr_row[j] = std::min({
                prev_row[j] + 1,        // deletion
                curr_row[j - 1] + 1,    // insertion
                prev_row[j - 1] + cost  // substitution
            });
        }

        std::swap(prev_row, curr_row);
    }

    return prev_row[len2];
}

/** @brief Calculate similarity between two strings (0.0 to 1.0)
 *
 * Similarity is calculated as: 1.0 - (distance / max_length)
 * where distance is the Levenshtein distance.
 *
 * @param s1 First string
 * @param s2 Second string
 * @return Similarity score (0.0 = completely different, 1.0 = identical)
 *
 * @code
 *     double sim = wbr::str::similarity("kitten", "sitting");
 *     // sim ≈ 0.571
 * @endcode
 */
inline double similarity (std::string_view s1, std::string_view s2) {
    if ( s1.empty( ) && s2.empty( ) )
        return 1.0;

    size_t max_len  = std::max(s1.size( ), s2.size( ));
    size_t distance = levenshtein_distance(s1, s2);

    return 1.0 - (static_cast<double>(distance) / static_cast<double>(max_len));
}

// Forward declaration for trim (used by lines_trimmed)
std::string_view trim(std::string_view str, std::string_view symbols);

// Implementation of lines_trimmed (needs trim declaration)
inline std::vector<std::string_view> lines_trimmed (std::string_view str) {
    auto result = lines(str);
    for ( auto& line: result )
        line = trim(line, " \t");
    return result;
}

}  // namespace wbr::str
