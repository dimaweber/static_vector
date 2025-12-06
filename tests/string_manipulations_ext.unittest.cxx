/** @file
 *
 *  @brief Unit tests for extended string manipulation functions
 * */

#include "string_manipulations_ext.hxx"
#include "string_manipulations.hxx"  // For trim function

#include <gtest/gtest.h>

using namespace wbr::str;

// ============================================================================
// 1. String Padding Tests
// ============================================================================

TEST(StringPadding, PadLeft) {
    EXPECT_EQ(pad_left("hello", 10, '*'), "*****hello");
    EXPECT_EQ(pad_left("hello", 5, '*'), "hello");
    EXPECT_EQ(pad_left("hello", 3, '*'), "hello");
    EXPECT_EQ(pad_left("", 5, '-'), "-----");
    EXPECT_EQ(pad_left("test", 8, ' '), "    test");
}

TEST(StringPadding, PadRight) {
    EXPECT_EQ(pad_right("hello", 10, '-'), "hello-----");
    EXPECT_EQ(pad_right("hello", 5, '-'), "hello");
    EXPECT_EQ(pad_right("hello", 3, '-'), "hello");
    EXPECT_EQ(pad_right("", 5, '*'), "*****");
    EXPECT_EQ(pad_right("test", 8, ' '), "test    ");
}

TEST(StringPadding, PadCenter) {
    EXPECT_EQ(pad_center("hi", 10, ' '), "    hi    ");
    EXPECT_EQ(pad_center("test", 10, '*'), "***test***");
    EXPECT_EQ(pad_center("odd", 11, '-'), "----odd----");
    EXPECT_EQ(pad_center("hello", 5, ' '), "hello");
    EXPECT_EQ(pad_center("hello", 3, ' '), "hello");
    EXPECT_EQ(pad_center("", 5, 'x'), "xxxxx");
}

// ============================================================================
// 2. String Repeating Tests
// ============================================================================

TEST(StringRepeat, RepeatString) {
    EXPECT_EQ(repeat("ab", 3), "ababab");
    EXPECT_EQ(repeat("x", 5), "xxxxx");
    EXPECT_EQ(repeat("hello", 2), "hellohello");
    EXPECT_EQ(repeat("test", 0), "");
    EXPECT_EQ(repeat("", 5), "");
    EXPECT_EQ(repeat("abc", 1), "abc");
}

TEST(StringRepeat, RepeatChar) {
    EXPECT_EQ(repeat('-', 5), "-----");
    EXPECT_EQ(repeat('*', 3), "***");
    EXPECT_EQ(repeat('a', 0), "");
    EXPECT_EQ(repeat('x', 1), "x");
    EXPECT_EQ(repeat(' ', 10), "          ");
}

// ============================================================================
// 3. Contains/Count Tests
// ============================================================================

TEST(StringContains, Contains) {
    EXPECT_TRUE(contains("hello world", "wor"));
    EXPECT_TRUE(contains("hello world", "hello"));
    EXPECT_TRUE(contains("hello world", "world"));
    EXPECT_FALSE(contains("hello world", "xyz"));
    EXPECT_TRUE(contains("test", "test"));
    EXPECT_TRUE(contains("test", ""));
    EXPECT_FALSE(contains("", "test"));
}

TEST(StringContains, ContainsAny) {
    EXPECT_TRUE(contains_any("hello", "aeiou"));
    EXPECT_FALSE(contains_any("hello", "xyz"));
    EXPECT_TRUE(contains_any("test", "t"));
    EXPECT_FALSE(contains_any("", "abc"));
    EXPECT_FALSE(contains_any("abc", ""));
    EXPECT_TRUE(contains_any("hello world", " "));
}

TEST(StringContains, ContainsAll) {
    EXPECT_TRUE(contains_all("hello world", "helo"));
    EXPECT_FALSE(contains_all("hello", "xyz"));
    EXPECT_TRUE(contains_all("abcdef", "ace"));
    EXPECT_FALSE(contains_all("abc", "abcd"));
    EXPECT_TRUE(contains_all("test", ""));
    EXPECT_TRUE(contains_all("", ""));
}

TEST(StringCount, CountOccurrencesSubstring) {
    EXPECT_EQ(count_occurrences("aaa bbb aaa", "aa"), 2u);  // non-overlapping
    EXPECT_EQ(count_occurrences("hello hello hello", "hello"), 3u);
    EXPECT_EQ(count_occurrences("abcabc", "abc"), 2u);
    EXPECT_EQ(count_occurrences("test", "xyz"), 0u);
    EXPECT_EQ(count_occurrences("", "test"), 0u);
    EXPECT_EQ(count_occurrences("test", ""), 0u);
    EXPECT_EQ(count_occurrences("aaaa", "aa"), 2u);  // non-overlapping
}

TEST(StringCount, CountOccurrencesChar) {
    EXPECT_EQ(count_occurrences("hello world", 'l'), 3u);
    EXPECT_EQ(count_occurrences("test", 't'), 2u);
    EXPECT_EQ(count_occurrences("abc", 'x'), 0u);
    EXPECT_EQ(count_occurrences("", 'a'), 0u);
    EXPECT_EQ(count_occurrences("aaaaa", 'a'), 5u);
}

// ============================================================================
// 4. String Truncation Tests
// ============================================================================

TEST(StringTruncate, Truncate) {
    EXPECT_EQ(truncate_string("very long text here", 10), std::string("very lo..."));
    EXPECT_EQ(truncate_string("short", 10), std::string("short"));
    EXPECT_EQ(truncate_string("exactly ten", 11), std::string("exactly ten"));
    EXPECT_EQ(truncate_string("test", 7, "..."), std::string("test"));
    EXPECT_EQ(truncate_string("long text", 6, ".."), std::string("long.."));
    EXPECT_EQ(truncate_string("hello world", 8, "..."), std::string("hello..."));
    EXPECT_EQ(truncate_string("test", 3, "..."), std::string("..."));
    EXPECT_EQ(truncate_string("test", 2, "..."), std::string(".."));
}

TEST(StringTruncate, TruncateMiddle) {
    EXPECT_EQ(truncate_string_middle("path/to/very/long/file.txt", 20, "..."), "path/to/v...file.txt");
    EXPECT_EQ(truncate_string_middle("short", 10), "short");
    EXPECT_EQ(truncate_string_middle("abcdefghij", 10), "abcdefghij");
    EXPECT_EQ(truncate_string_middle("very_long_filename.txt", 15, ".."), "very_lo..me.txt");
    EXPECT_EQ(truncate_string_middle("test", 3, "..."), "...");
}

// ============================================================================
// 5. Line Operations Tests
// ============================================================================

TEST(LineOperations, Lines) {
    auto result = lines("line1\nline2\nline3");
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], "line1");
    EXPECT_EQ(result[1], "line2");
    EXPECT_EQ(result[2], "line3");
}

TEST(LineOperations, LinesWindowsLineEndings) {
    auto result = lines("line1\r\nline2\r\nline3");
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], "line1");
    EXPECT_EQ(result[1], "line2");
    EXPECT_EQ(result[2], "line3");
}

TEST(LineOperations, LinesEmpty) {
    auto result = lines("");
    EXPECT_TRUE(result.empty());
}

TEST(LineOperations, LinesSingleLine) {
    auto result = lines("single line");
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "single line");
}

TEST(LineOperations, LinesTrailingNewline) {
    auto result = lines("line1\nline2\n");
    ASSERT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], "line1");
    EXPECT_EQ(result[1], "line2");
}

TEST(LineOperations, LinesTrimmed) {
    auto result = lines_trimmed("  line1  \n  line2  \n  line3  ");
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], "line1");
    EXPECT_EQ(result[1], "line2");
    EXPECT_EQ(result[2], "line3");
}

TEST(LineOperations, Indent) {
    EXPECT_EQ(indent("hello\nworld", 2), "  hello\n  world");
    EXPECT_EQ(indent("test", 4), "    test");
    EXPECT_EQ(indent("a\nb\nc", 1), " a\n b\n c");
    EXPECT_EQ(indent("hello\nworld", 0), "hello\nworld");
}

TEST(LineOperations, Dedent) {
    EXPECT_EQ(dedent("  hello\n  world"), "hello\nworld");
    EXPECT_EQ(dedent("    a\n    b\n    c"), "a\nb\nc");
    EXPECT_EQ(dedent("  first\n    second"), "first\n  second");
    EXPECT_EQ(dedent("no indent"), "no indent");
    EXPECT_EQ(dedent(""), "");
}

// ============================================================================
// 6. String Wrapping Tests
// ============================================================================

TEST(StringWrap, Wrap) {
    auto result = wrap("this is a long sentence", 10);
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], "this is a");
    EXPECT_EQ(result[1], "long");
    EXPECT_EQ(result[2], "sentence");
}

TEST(StringWrap, WrapShortString) {
    auto result = wrap("short", 10);
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "short");
}

TEST(StringWrap, WrapNoSpaces) {
    auto result = wrap("verylongword", 5);
    ASSERT_GE(result.size(), 2u);
    EXPECT_EQ(result[0], "veryl");
}

TEST(StringWrap, WrapEmpty) {
    auto result = wrap("", 10);
    EXPECT_TRUE(result.empty());
}

TEST(StringWrap, WrapJoin) {
    EXPECT_EQ(wrap_join("this is a long sentence", 10), "this is a\nlong\nsentence");
    EXPECT_EQ(wrap_join("short", 10), "short");
    EXPECT_EQ(wrap_join("a b c", 5, " | "), "a b c");
}

// ============================================================================
// 7. Substring Extraction Tests
// ============================================================================

TEST(SubstringExtraction, SubstringBetween) {
    EXPECT_EQ(substring_between("Hello [world]!", "[", "]"), "world");
    EXPECT_EQ(substring_between("(test)", "(", ")"), "test");
    EXPECT_EQ(substring_between("no markers here", "[", "]"), "");
    EXPECT_EQ(substring_between("[only start", "[", "]"), "");
    EXPECT_EQ(substring_between("only end]", "[", "]"), "");
    EXPECT_EQ(substring_between("<<nested>>", "<<", ">>"), "nested");
}

TEST(SubstringExtraction, ExtractAllBetween) {
    auto result = extract_all_between("a[1]b[2]c[3]", "[", "]");
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], "1");
    EXPECT_EQ(result[1], "2");
    EXPECT_EQ(result[2], "3");
}

TEST(SubstringExtraction, ExtractAllBetweenNone) {
    auto result = extract_all_between("no markers", "[", "]");
    EXPECT_TRUE(result.empty());
}

TEST(SubstringExtraction, ExtractAllBetweenNested) {
    auto result = extract_all_between("(a)(b)(c)", "(", ")");
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], "a");
    EXPECT_EQ(result[1], "b");
    EXPECT_EQ(result[2], "c");
}

// ============================================================================
// 8. Case Conversion Tests
// ============================================================================

TEST(CaseConversion, ToTitleCase) {
    EXPECT_EQ(to_title_case("hello world"), "Hello World");
    EXPECT_EQ(to_title_case("HELLO WORLD"), "Hello World");
    EXPECT_EQ(to_title_case("hello"), "Hello");
    EXPECT_EQ(to_title_case(""), "");
    EXPECT_EQ(to_title_case("a b c"), "A B C");
    EXPECT_EQ(to_title_case("test  multiple  spaces"), "Test  Multiple  Spaces");
}

TEST(CaseConversion, ToSnakeCase) {
    EXPECT_EQ(to_snake_case("HelloWorld"), "hello_world");
    EXPECT_EQ(to_snake_case("someHTMLParser"), "some_html_parser");
    EXPECT_EQ(to_snake_case("simpleTest"), "simple_test");
    EXPECT_EQ(to_snake_case("XMLHttpRequest"), "xml_http_request");
    EXPECT_EQ(to_snake_case("already_snake"), "already_snake");
    EXPECT_EQ(to_snake_case("kebab-case"), "kebab_case");
    EXPECT_EQ(to_snake_case("ALLCAPS"), "allcaps");
}

TEST(CaseConversion, ToCamelCase) {
    EXPECT_EQ(to_camel_case("hello_world"), "helloWorld");
    EXPECT_EQ(to_camel_case("some-test-case"), "someTestCase");
    EXPECT_EQ(to_camel_case("simple_test"), "simpleTest");
    EXPECT_EQ(to_camel_case("already_camel"), "alreadyCamel");
    EXPECT_EQ(to_camel_case("HELLO_WORLD"), "helloWorld");
    EXPECT_EQ(to_camel_case("test"), "test");
}

TEST(CaseConversion, ToKebabCase) {
    EXPECT_EQ(to_kebab_case("HelloWorld"), "hello-world");
    EXPECT_EQ(to_kebab_case("some_test_case"), "some-test-case");
    EXPECT_EQ(to_kebab_case("simpleTest"), "simple-test");
    EXPECT_EQ(to_kebab_case("XMLParser"), "xml-parser");
    EXPECT_EQ(to_kebab_case("already-kebab"), "already-kebab");
}

// ============================================================================
// 9. Reverse Tests
// ============================================================================

TEST(StringReverse, Reverse) {
    EXPECT_EQ(reverse("hello"), "olleh");
    EXPECT_EQ(reverse("a"), "a");
    EXPECT_EQ(reverse(""), "");
    EXPECT_EQ(reverse("racecar"), "racecar");
    EXPECT_EQ(reverse("123"), "321");
}

TEST(StringReverse, ReverseInplace) {
    std::string s1 = "hello";
    reverse_inplace(s1);
    EXPECT_EQ(s1, "olleh");

    std::string s2 = "test";
    reverse_inplace(s2);
    EXPECT_EQ(s2, "tset");

    std::string s3 = "";
    reverse_inplace(s3);
    EXPECT_EQ(s3, "");
}

// ============================================================================
// 10. Common Prefix/Suffix Tests
// ============================================================================

TEST(CommonPrefixSuffix, CommonPrefix) {
    std::vector<std::string_view> v1 = {"prefix_a", "prefix_b", "prefix_c"};
    EXPECT_EQ(common_prefix(v1), "prefix_");

    std::vector<std::string_view> v2 = {"test", "temp", "terminal"};
    EXPECT_EQ(common_prefix(v2), "te");

    std::vector<std::string_view> v3 = {"abc", "xyz"};
    EXPECT_EQ(common_prefix(v3), "");

    std::vector<std::string_view> v4 = {"same", "same", "same"};
    EXPECT_EQ(common_prefix(v4), "same");

    std::vector<std::string_view> v5 = {"only_one"};
    EXPECT_EQ(common_prefix(v5), "only_one");

    std::vector<std::string_view> v6;
    EXPECT_EQ(common_prefix(v6), "");
}

TEST(CommonPrefixSuffix, CommonSuffix) {
    std::vector<std::string_view> v1 = {"a_suffix", "b_suffix", "c_suffix"};
    EXPECT_EQ(common_suffix(v1), "_suffix");

    std::vector<std::string_view> v2 = {"test.txt", "file.txt", "doc.txt"};
    EXPECT_EQ(common_suffix(v2), ".txt");

    std::vector<std::string_view> v3 = {"abc", "xyz"};
    EXPECT_EQ(common_suffix(v3), "");

    std::vector<std::string_view> v4 = {"same", "same", "same"};
    EXPECT_EQ(common_suffix(v4), "same");
}

// ============================================================================
// 11. Whitespace Normalization Tests
// ============================================================================

TEST(WhitespaceNormalization, NormalizeWhitespace) {
    EXPECT_EQ(normalize_whitespace("  hello   world  "), "hello world");
    EXPECT_EQ(normalize_whitespace("test"), "test");
    EXPECT_EQ(normalize_whitespace("  "), "");
    EXPECT_EQ(normalize_whitespace("a  b  c"), "a b c");
    EXPECT_EQ(normalize_whitespace("\t\nhello\t\nworld\t\n"), "hello world");
    EXPECT_EQ(normalize_whitespace(""), "");
}

TEST(WhitespaceNormalization, CollapseWhitespace) {
    EXPECT_EQ(collapse_whitespace("a\t\nb  c", ' '), "a  b  c");
    EXPECT_EQ(collapse_whitespace("hello\nworld", '_'), "hello_world");
    EXPECT_EQ(collapse_whitespace("test\r\n\tfile", ' '), "test   file");
    EXPECT_EQ(collapse_whitespace("no whitespace", ' '), "no whitespace");
}

// ============================================================================
// 12. String Comparison Variants Tests
// ============================================================================

TEST(StringComparisonVariants, StartsWithAny) {
    std::vector<std::string_view> prefixes = {"hi", "he", "ho"};
    EXPECT_TRUE(starts_with_any("hello", prefixes));
    EXPECT_FALSE(starts_with_any("world", prefixes));

    std::vector<std::string_view> empty_prefixes;
    EXPECT_FALSE(starts_with_any("test", empty_prefixes));

    std::vector<std::string_view> prefixes2 = {"test", "temp"};
    EXPECT_TRUE(starts_with_any("testing", prefixes2));
}

TEST(StringComparisonVariants, EndsWithAny) {
    std::vector<std::string_view> suffixes = {".txt", ".md", ".cpp"};
    EXPECT_TRUE(ends_with_any("file.txt", suffixes));
    EXPECT_TRUE(ends_with_any("readme.md", suffixes));
    EXPECT_FALSE(ends_with_any("file.exe", suffixes));

    std::vector<std::string_view> empty_suffixes;
    EXPECT_FALSE(ends_with_any("test", empty_suffixes));
}

TEST(StringComparisonVariants, IContains) {
    EXPECT_TRUE(icontains("Hello World", "WORLD"));
    EXPECT_TRUE(icontains("Hello World", "hello"));
    EXPECT_TRUE(icontains("TEST", "test"));
    EXPECT_FALSE(icontains("Hello", "xyz"));
    EXPECT_TRUE(icontains("anything", ""));
    EXPECT_FALSE(icontains("", "something"));
    EXPECT_TRUE(icontains("CamelCase", "camel"));
}

// ============================================================================
// 13. Character Set Operations Tests
// ============================================================================

TEST(CharacterSetOperations, RemoveChars) {
    EXPECT_EQ(remove_chars("hello123world", "0123456789"), "helloworld");
    EXPECT_EQ(remove_chars("test", "xyz"), "test");
    EXPECT_EQ(remove_chars("abc123", "abc"), "123");
    EXPECT_EQ(remove_chars("", "abc"), "");
    EXPECT_EQ(remove_chars("test", ""), "test");
    EXPECT_EQ(remove_chars("a-b-c", "-"), "abc");
}

TEST(CharacterSetOperations, KeepOnlyChars) {
    EXPECT_EQ(keep_only_chars("abc123def", "0123456789"), "123");
    EXPECT_EQ(keep_only_chars("test123", "0123456789"), "123");
    EXPECT_EQ(keep_only_chars("no digits here", "0123456789"), "");
    EXPECT_EQ(keep_only_chars("", "abc"), "");
    EXPECT_EQ(keep_only_chars("test", ""), "");
    EXPECT_EQ(keep_only_chars("abc123", "abc123"), "abc123");
}

TEST(CharacterSetOperations, RemoveIf) {
    auto is_digit = [](char c) { return std::isdigit(static_cast<unsigned char>(c)) != 0; };
    EXPECT_EQ(remove_if("abc123def", is_digit), "abcdef");
    EXPECT_EQ(remove_if("no digits", is_digit), "no digits");
    EXPECT_EQ(remove_if("123", is_digit), "");

    auto is_space = [](char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; };
    EXPECT_EQ(remove_if("hello world", is_space), "helloworld");
}

// ============================================================================
// 14. Levenshtein Distance / Similarity Tests
// ============================================================================

TEST(LevenshteinDistance, BasicCases) {
    EXPECT_EQ(levenshtein_distance("kitten", "sitting"), 3u);
    EXPECT_EQ(levenshtein_distance("saturday", "sunday"), 3u);
    EXPECT_EQ(levenshtein_distance("", ""), 0u);
    EXPECT_EQ(levenshtein_distance("abc", ""), 3u);
    EXPECT_EQ(levenshtein_distance("", "abc"), 3u);
    EXPECT_EQ(levenshtein_distance("same", "same"), 0u);
}

TEST(LevenshteinDistance, SingleCharChanges) {
    EXPECT_EQ(levenshtein_distance("cat", "bat"), 1u);  // substitution
    EXPECT_EQ(levenshtein_distance("cat", "at"), 1u);   // deletion
    EXPECT_EQ(levenshtein_distance("cat", "cart"), 1u); // insertion
}

TEST(Similarity, BasicCases) {
    EXPECT_DOUBLE_EQ(similarity("same", "same"), 1.0);
    EXPECT_GT(similarity("kitten", "sitting"), 0.5);
    EXPECT_LT(similarity("kitten", "sitting"), 0.6);
    EXPECT_DOUBLE_EQ(similarity("", ""), 1.0);
    EXPECT_DOUBLE_EQ(similarity("abc", "xyz"), 0.0);
}

TEST(Similarity, PartialMatches) {
    double sim1 = similarity("hello", "hallo");
    EXPECT_GT(sim1, 0.7);
    EXPECT_LT(sim1, 1.0);

    double sim2 = similarity("test", "best");
    EXPECT_GT(sim2, 0.7);
}

// ============================================================================
// Edge Cases and Integration Tests
// ============================================================================

TEST(EdgeCases, EmptyStrings) {
    EXPECT_EQ(pad_left("", 5, '*'), "*****");
    EXPECT_EQ(repeat("", 5), "");
    EXPECT_FALSE(contains("", "test"));
    EXPECT_EQ(truncate_string("", 10), "");
    EXPECT_TRUE(lines("").empty());
    EXPECT_EQ(reverse(""), "");
    EXPECT_EQ(normalize_whitespace(""), "");
}

TEST(EdgeCases, SingleCharacter) {
    EXPECT_EQ(pad_center("x", 5, '-'), "--x--");
    EXPECT_EQ(repeat("a", 3), "aaa");
    EXPECT_TRUE(contains("x", "x"));
    EXPECT_EQ(reverse("a"), "a");
}

TEST(Integration, CombinedOperations) {
    // Pad, then truncate
    std::string padded = pad_right("test", 20, ' ');
    std::string truncated = truncate_string(padded, 10);
    EXPECT_EQ(truncated.size(), 10u);

    // Normalize then split into lines
    // normalize_whitespace converts ALL whitespace (including \n) to single spaces
    std::string normalized = normalize_whitespace("  line1  \n  line2  ");
    auto line_list = lines(normalized);
    EXPECT_EQ(line_list.size(), 1u);  // Single line because \n was converted to space
    EXPECT_EQ(normalized, "line1 line2");

    // Case conversion chain
    std::string snake = to_snake_case("HelloWorld");
    std::string camel = to_camel_case(snake);
    EXPECT_EQ(camel, "helloWorld");
}

TEST(Integration, RealWorldUseCases) {
    // File path truncation
    std::string long_path = "/very/long/path/to/some/file/deep/in/filesystem/document.txt";
    std::string truncated_path = truncate_string_middle(long_path, 40, "...");
    EXPECT_LE(truncated_path.size(), 40u);
    std::vector<std::string_view> suffixes = {".txt"};
    EXPECT_TRUE(ends_with_any(truncated_path, suffixes));

    // Text cleaning
    std::string messy_text = "  hello   world  \n  extra   spaces  ";
    auto lines_vec = lines_trimmed(messy_text);
    std::string cleaned;
    for (const auto& line : lines_vec) {
        if (!cleaned.empty()) cleaned += "\n";
        cleaned += normalize_whitespace(line);
    }
    EXPECT_EQ(cleaned, "hello world\nextra spaces");

    // Extract tags from markup
    std::string markup = "This is <b>bold</b> and <i>italic</i> text";
    auto tags = extract_all_between(markup, "<", ">");
    ASSERT_EQ(tags.size(), 4u);
    EXPECT_EQ(tags[0], "b");
    EXPECT_EQ(tags[1], "/b");
}

TEST(Performance, LargeStrings) {
    // Test with reasonably large strings
    std::string large_str(10000, 'a');

    // Padding
    auto padded = pad_right(large_str, 11000, ' ');
    EXPECT_EQ(padded.size(), 11000u);

    // Repeat
    auto repeated = repeat("abc", 1000);
    EXPECT_EQ(repeated.size(), 3000u);

    // Contains
    EXPECT_TRUE(contains(large_str, "aaa"));

    // Count
    EXPECT_EQ(count_occurrences(large_str, 'a'), 10000u);
}

TEST(Unicode, BasicUTF8Support) {
    // Basic UTF-8 handling (treating as byte sequences)
    std::string utf8_str = "Héllo Wörld";

    // These should work with UTF-8 as byte sequences
    EXPECT_TRUE(contains(utf8_str, "llo"));
    EXPECT_TRUE(contains(utf8_str, "Wör"));

    // Reverse will reverse bytes (may break UTF-8 sequences)
    auto reversed = reverse(utf8_str);
    EXPECT_EQ(reversed.size(), utf8_str.size());
}
