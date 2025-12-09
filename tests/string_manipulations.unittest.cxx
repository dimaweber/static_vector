/** @file
 * @brief unittests for string manipulation functions from string_manupulations.h file
 *
 */
#include "string_manipulations.hxx"

#include <fmt/ostream.h>
#include <gtest/gtest.h>

#include <random>

#include "static_string.hxx"

class TrimTest : public testing::TestWithParam<std::tuple<std::string, std::string, std::string>> { };

TEST_P (TrimTest, TrimmingStrings) {
    const auto& [in, delimiters, expected] = GetParam( );
    std::string_view res                   = wbr::str::trim(in, delimiters);
    EXPECT_EQ(res, expected);
}

TEST_P (TrimTest, TrimmingStringsPredicate) {
    const auto& [in, delimiters, expected] = GetParam( );
    auto is_space_pred                     = [delimiters] (char c) { return delimiters.find(c) != std::string::npos; };

    std::string_view res = wbr::str::trim(in, is_space_pred);
    EXPECT_EQ(res, expected);
}

std::vector<std::tuple<std::string, std::string, std::string>> trimInput = {
    {"  abc ",          " ",  "abc"    },
    {" \tabc\t ",       " ",  "\tabc\t"},
    {"  a bc ",         " ",  "a bc"   },
    {"::abc::",         " ",  "::abc::"},
    {" :: :a:b:c: :: ", ": ", "a:b:c"  },
    {"",                " ",  ""       },
    {"  ",              " ",  ""       },
    {":: ::",           ": ", ""       },
    {"abc  ",           " ",  "abc"    },
    {"  abc",           " ",  "abc"    },
};

INSTANTIATE_TEST_SUITE_P(StringManipulations, TrimTest, testing::ValuesIn(trimInput));

struct TokenizeTestRecord {
    std::string                   in;
    std::string                   delimiter;
    std::vector<std::string_view> expected;
    std::vector<std::string_view> expected_simplified;
};

std::ostream& operator<< (std::ostream& stream, const TokenizeTestRecord& rec) {
    stream << rec.in;
    return stream;
}

class TokenizeTest : public testing::TestWithParam<TokenizeTestRecord> { };

TEST_F (TokenizeTest, TokenizeFromNullptr) {
    std::vector<std::string_view> out = wbr::str::tokenize(nullptr, ";");
    EXPECT_TRUE(out.empty( ));
}

TEST_P (TokenizeTest, TokenizeFromString) {
    const auto& [line, delim, expected, expected_simp] = GetParam( );
    std::vector<std::string_view> out                  = wbr::str::tokenize(line, delim);
    EXPECT_EQ(out.size( ), expected.size( ));
    EXPECT_TRUE(std::equal(out.begin( ), out.end( ), expected.begin( )));
}

TEST_P (TokenizeTest, TokenizeFromCString) {
    const auto& [line, delim, expected, expected_simp] = GetParam( );
    std::vector<std::string_view> out                  = wbr::str::tokenize(line.c_str( ), delim);
    EXPECT_EQ(out.size( ), expected.size( ));
    EXPECT_TRUE(std::equal(out.begin( ), out.end( ), expected.begin( )));
}

TEST_P (TokenizeTest, TokenizeSimplifiedFromString) {
    const auto& [line, delim, expected, expected_simp] = GetParam( );
    std::vector<std::string_view> out                  = wbr::str::tokenizeSimplified(line.c_str( ), delim);
    EXPECT_EQ(out.size( ), expected_simp.size( ));
    EXPECT_TRUE(std::equal(out.begin( ), out.end( ), expected_simp.begin( )));
}

TEST_P (TokenizeTest, TokenizeEscapedMatchTokenize) {
    const auto& [line, delim, expected, expected_simp] = GetParam( );
    std::vector<std::string_view> out_t                = wbr::str::tokenize(line, delim);
    std::vector<std::string>      out_te               = wbr::str::tokenizeEscaped(line, delim);
    EXPECT_TRUE(std::ranges::equal(out_t, out_te));
}

using namespace std::literals;
const std::vector<TokenizeTestRecord> linesInput = {
    {"  "s,                                    " "s,   {""s, ""s, ""s},                                                   { }                                                },
    {".one=1;two=2;three=3;four=4;five=5."s,   " "s,   {".one=1;two=2;three=3;four=4;five=5."},                           {".one=1;two=2;three=3;four=4;five=5."}            },
    {".one=1;two=2 three=3;four=4 five=5."s,   " "s,   {".one=1;two=2", "three=3;four=4", "five=5."},                     {".one=1;two=2", "three=3;four=4", "five=5."}      },
    {".one=1;two=2 three=3;four=4 five=5."s,   " "s,   {".one=1;two=2", "three=3;four=4", "five=5."},                     {".one=1;two=2", "three=3;four=4", "five=5."}      },
    {".one=1;two=2 three=3;;four=4 five=5."s,  ";"s,   {".one=1", "two=2 three=3", "", "four=4 five=5."},                 {".one=1", "two=2 three=3", "", "four=4 five=5."}  },
    {".one=1;two=2 three=3;four=4 five=5."s,   "; "s,  {".one=1", "two=2", "three=3", "four=4", "five=5."},               {".one=1", "two=2", "three=3", "four=4", "five=5."}},
    {" .one=1;two=2 three=3;four=4 five=5. "s, ".; "s, {"", "", "one=1", "two=2", "three=3", "four=4", "five=5", "", ""}, {"one=1", "two=2", "three=3", "four=4", "five=5"}  },
    {" one=1;two=2 three=3;four=4 five=5."s,   "; ."s, {"", "one=1", "two=2", "three=3", "four=4", "five=5", ""},         {"one=1", "two=2", "three=3", "four=4", "five=5"}  }
};

INSTANTIATE_TEST_SUITE_P(StringManipulations, TokenizeTest, testing::ValuesIn(linesInput));

class JoinTest : public testing::TestWithParam<std::tuple<std::string, std::string, std::string, std::string>> { };

TEST_P (JoinTest, JoinNoSkip) {
    const auto& [line, delim, expected, expected_skip] = GetParam( );
    std::string out                                    = wbr::str::join(wbr::str::tokenizeSimplified(line, "; ."), delim);
    EXPECT_EQ(out, expected);
}

TEST_P (JoinTest, JoinSkip) {
    const auto& [line, delim, expected, expected_skip] = GetParam( );
    std::string out                                    = wbr::str::joinSkipEmpty(wbr::str::tokenizeSimplified(line, "; ."), delim);
    EXPECT_EQ(out, expected_skip);
}

using namespace std::literals;
std::vector<std::tuple<std::string, std::string, std::string, std::string>> joinLinesInput = {
    {" one=1;two=2 three=3;four=4 five=5."s,     ", ", "one=1, two=2, three=3, four=4, five=5",         "one=1, two=2, three=3, four=4, five=5"},
    {" one=1;;;two=2 three=3;;;four=4 five=5."s, ", ", "one=1, , , two=2, three=3, , , four=4, five=5", "one=1, two=2, three=3, four=4, five=5"},
    {"  "s,                                      ", ", "",                                              ""                                     }
};

INSTANTIATE_TEST_SUITE_P(StringManipulations, JoinTest, testing::ValuesIn(joinLinesInput));

class SplitTest : public testing::TestWithParam<std::tuple<std::string, std::pair<std::string, std::string>, std::pair<std::string, std::string>>> { };

TEST_P (SplitTest, SplitOnFirstLast) {
    const auto& [line, onfirst, onlast]        = GetParam( );
    const auto& [splitfirstkey, splitfirstval] = wbr::str::splitAtFirst(line, "=").value_or(std::pair {""s, ""s});
    const auto& [splitlastkey, splitlastval]   = wbr::str::splitAtLast(line, "=").value_or(std::pair {""s, ""s});
    EXPECT_EQ(onfirst.first, splitfirstkey);
    EXPECT_EQ(onfirst.second, splitfirstval);
    EXPECT_EQ(onlast.first, splitlastkey);
    EXPECT_EQ(onlast.second, splitlastval);
}

using namespace std::literals;
std::vector<std::tuple<std::string, std::pair<std::string, std::string>, std::pair<std::string, std::string>>> spliLinesInput = {
    {"param1=value1",             {"param1", "value1"},      {"param1", "value1"}      },
    {"param1=value1=2",           {"param1", "value1=2"},    {"param1=value1", "2"}    },
    {" \tparam1 =   value1 =  2", {"param1", "value1 =  2"}, {"param1 =   value1", "2"}},
    {"param1value1",              {"", ""},                  {"", ""}                  },
    {"param1=",                   {"param1", ""},            {"param1", ""}            },
    {"=value",                    {"", ""},                  {"", ""}                  },
};

INSTANTIATE_TEST_SUITE_P(StringManipulations, SplitTest, testing::ValuesIn(spliLinesInput));

struct ReplaceTestRec {
    std::string origString;
    std::string what;
    std::string with;
    std::string expect;
};

class ReplaceAllTest : public testing::TestWithParam<ReplaceAllTest> { };

class ReplaceCharactersTest : public testing::TestWithParam<std::tuple<std::string, std::string, char, std::string>> { };

TEST_P (ReplaceCharactersTest, ReplacingSet) {
    const auto& [in, charset, replace, expected] = GetParam( );
    std::string res                              = wbr::str::replaceCharacters(in, charset, replace);
    EXPECT_EQ(res, expected);
}

std::vector<std::tuple<std::string, std::string, char, std::string>> replSetInput = {
    {"  abc ",    " ",   '-', "--abc-" },
    {" \tabc\t ", " \t", '_', "__abc__"},
    {"a,bc",      ", +", '_', "a_bc"   },
    {"  abc::",   "  ",  '*', "**abc::"},
    {"  abc::",   ":",   ' ', "  abc  "},
};

INSTANTIATE_TEST_SUITE_P(StringManipulations, ReplaceCharactersTest, testing::ValuesIn(replSetInput));

class ReplaceFuncTest : public testing::TestWithParam<std::tuple<std::string, std::function<bool(char)>, char, std::string>> { };

TEST_P (ReplaceFuncTest, ReplacingFunc) {
    const auto& [in, func, replace, expected] = GetParam( );
    std::string res                           = wbr::str::replaceCharacters(in, func, replace);
    EXPECT_EQ(res, expected);
}

std::vector<std::tuple<std::string, std::function<bool(char)>, char, std::string>> replFuncInput = {
    {"  abc ",    [] (char c) { return c == ' '; },                         '-', "--abc-" },
    {" \tabc\t ", [] (char c) { return c == ' ' || c == '\t'; },            '_', "__abc__"},
    {"a,bc",      [] (char c) { return c == ',' || c == ' ' || c == '+'; }, '_', "a_bc"   },
    {"  abc::",   [] (char c) { return c == ' '; },                         '*', "**abc::"},
    {"  abc::",   [] (char c) { return c == ':'; },                         ' ', "  abc  "},
};

INSTANTIATE_TEST_SUITE_P(StringManipulations, ReplaceFuncTest, testing::ValuesIn(replFuncInput));

struct NumConversionRecord {
    std::string_view in;
    uint             _uint;
    int              _int;
    double           _double;
    uint             _uint_0x;
    int              _int_0x;
    bool             _uint_ok;
    bool             _int_ok;
    bool             _double_ok;
    bool             _uint_0x_ok;
    bool             _int_0x_ok;
};

std::ostream& operator<< (std::ostream& stream, const NumConversionRecord& rec) {
    stream << rec.in;
    return stream;
}

class NumConversionTest : public testing::TestWithParam<NumConversionRecord> { };

TEST_P (NumConversionTest, ConversionUint) {
    const NumConversionRecord& rec = GetParam( );
    auto                       ret = wbr::str::num<uint>(rec.in);
    EXPECT_EQ(ret, rec._uint);

    std::errc ec;
    ret = wbr::str::num<uint>(rec.in, ec);
    EXPECT_EQ(ec == std::errc { }, rec._uint_ok);
}

TEST_P (NumConversionTest, ConversionInt) {
    const NumConversionRecord& rec = GetParam( );
    auto                       ret = wbr::str::num<int>(rec.in);
    EXPECT_EQ(ret, rec._int);

    std::errc ec;
    ret = wbr::str::num<int>(rec.in, ec);
    EXPECT_EQ(ec == std::errc { }, rec._int_ok);
}

TEST_P (NumConversionTest, ConversionDouble) {
    const NumConversionRecord& rec = GetParam( );
    auto                       ret = wbr::str::num<double>(rec.in);
    EXPECT_EQ(ret, rec._double);

#if WBR_STR_NUM_DOUBLE_HAS_ERROR_CHECK
    std::errc ec;
    ret = wbr::str::num<double>(rec.in, ec);
    EXPECT_EQ(ec == std::errc { }, rec._double_ok);
#endif
}

TEST_P (NumConversionTest, ConversionUint0x) {
    const NumConversionRecord& rec = GetParam( );
    uint                       ret = wbr::str::num<uint>(rec.in, 16);
    EXPECT_EQ(ret, rec._uint_0x);
}

TEST_P (NumConversionTest, ConversionInt0x) {
    const NumConversionRecord& rec = GetParam( );
    int                        ret = wbr::str::num<int>(rec.in, 16);
    EXPECT_EQ(ret, rec._int_0x);
}

const std::vector<NumConversionRecord> numConversionInput = {
    {.in = "0", ._uint = 0, ._int = 0, ._double = 0, ._uint_0x = 0, ._int_0x = 0, ._uint_ok = true, ._int_ok = true, ._double_ok = true, ._uint_0x_ok = true, ._int_0x_ok = true},
    {"1",       1,          1,         1,            1,             1,            true,             true,            true,               true,                true              },
    {"112.34",  112,        112,       112.34,       0x112,         0x112,        true,             true,            true,               true,                true              },
    {"-1",      0,          -1,        -1,           0x0,           -0x1,         false,            true,            true,               false,               true              },
    {"-112.09", 0,          -112,      -112.09,      0x0,           -0x112,       false,            true,            true,               false,               true              },
    {"beef",    0,          0,         0,            0xbeef,        0xbeef,       false,            false,           false,              true,                true              },
    {"-beef",   0,          0,         0,            0x0,           -0xbeef,      false,            false,           false,              false,               true              },
};

INSTANTIATE_TEST_SUITE_P(StringManipulations, NumConversionTest, testing::ValuesIn(numConversionInput));

struct CaseConversionRecord {
    std::string in;
    std::string l;
    std::string u;
};

std::ostream& operator<< (std::ostream& stream, const CaseConversionRecord& rec) {
    stream << rec.in;
    return stream;
}

class CaseConversionTest : public testing::TestWithParam<CaseConversionRecord> { };

TEST_P (CaseConversionTest, ToLower) {
    auto [in, l, u] = GetParam( );
    EXPECT_EQ(wbr::str::strlower(in), l);
}

TEST_P (CaseConversionTest, ToUpper) {
    auto [in, l, u] = GetParam( );
    EXPECT_EQ(wbr::str::strupper(in), u);
}

const std::vector<CaseConversionRecord> caseConversionInput = {
    {"Lorem Ipsum Dolor Sit Amet", "lorem ipsum dolor sit amet", "LOREM IPSUM DOLOR SIT AMET"},
    {"lorem ipsum dolor sit amet", "lorem ipsum dolor sit amet", "LOREM IPSUM DOLOR SIT AMET"},
    {"lOREM iPSUM dOLOR sIT aMET", "lorem ipsum dolor sit amet", "LOREM IPSUM DOLOR SIT AMET"},
};

INSTANTIATE_TEST_SUITE_P(StringManipulations, CaseConversionTest, testing::ValuesIn(caseConversionInput));

struct TokenizeEscapedRecord {
    std::string              input;
    std::vector<std::string> expected;
    std::string              divider {" "};
    char                     escape {'\\'};
};

std::ostream& operator<< (std::ostream& stream, const TokenizeEscapedRecord& test) {
    stream << fmt::format("{} (d:'{}' e:'{}')", test.input, test.divider, test.escape);
    return stream;
}

/**
 *  @brief data set to test tokenizeEscaped function
 *
 *  @li First element - input string
 *  @li Second element - expected output
 *  @li (optional) Third element - divider symbols (default is ' ')
 *  @li (optional) Fourth element - escape symbol (default is \\ )
 */
const std::vector<TokenizeEscapedRecord> tokenizeEscapedInput = {
    {R"(show eth)",                          {R"(show)", R"(eth)"},                                      " ",    '\\'},
    {R"(show\ eth eth1)",                    {R"(show eth)", R"(eth1)"},                                 " ",    '\\'},
    {R"(show\\ eth eth1)",                   {R"(show\)", R"(eth)", "eth1"},                             " ",    '\\'},
    {R"(show 'eth eth1' mode)",              {R"(show)", R"(eth eth1)", "mode"},                         " ",    '\\'},
    {R"(show 'et\'h eth1' mode)",            {R"(show)", R"(et'h eth1)", "mode"},                        " ",    '\\'},
    {R"(show 'eth "eth1"' mode)",            {R"(show)", R"(eth "eth1")", "mode"},                       " ",    '\\'},
    {R"(show "eth eth1" mode)",              {R"(show)", R"(eth eth1)", "mode"},                         " ",    '\\'},
    {R"(show "eth \"eth1\"" mode)",          {R"(show)", R"(eth "eth1")", "mode"},                       " ",    '\\'},
    {R"(show "eth 'eth1'" mode)",            {R"(show)", R"(eth 'eth1')", "mode"},                       " ",    '\\'},
    {R"(show""eth''eth1)",                   {"showetheth1"},                                            " ",    '\\'},
    {R"(lorem  ipsum  dolor  sit amet)",     {"lorem", "", "ipsum", "", "dolor", "", "sit", "amet"},     " ",    '\\'},
    {R"(lorem  \"ipsum\"  dolor  sit amet)", {"lorem", "", "\"ipsum\"", "", "dolor", "", "sit", "amet"}, " ",    '\\'},
    {R"(lorem  \'ipsum\'  dolor  sit amet)", {"lorem", "", "'ipsum'", "", "dolor", "", "sit", "amet"},   " ",    '\\'},
    {R"(lorem  \"ipsum\'  dolor  sit amet)", {"lorem", "", "\"ipsum'", "", "dolor", "", "sit", "amet"},  " ",    '\\'},
    {R"( lorem ipsum dolor sit amet)",       {"", "lorem", "ipsum", "dolor", "sit", "amet"},             " ",    '\\'},
    {R"( lorem ip\ 'sum' dolor sit amet)",   {"", "lorem", "ip sum", "dolor", "sit", "amet"},            " ",    '\\'},
    {R"(lorem ipsum dolor sit amet )",       {"lorem", "ipsum", "dolor", "sit", "amet", ""},             " ",    '\\'},
    {R"(lorem ipsum dolor sit 'amet ')",     {"lorem", "ipsum", "dolor", "sit", "amet "},                " ",    '\\'},
    {R"(lorem:ipsum:dolor:sit:amet)",        {"lorem", "ipsum", "dolor", "sit", "amet"},                 ":",    '\\'},
    {R"(lorem ipsum:dolor sit:amet)",        {"lorem ipsum", "dolor sit", "amet"},                       ":",    '\\'},
    {R"(:lorem:ipsum:dolor:sit:amet)",       {"", "lorem", "ipsum", "dolor", "sit", "amet"},             ":",    '\\'},
    {R"(\:lorem:ipsum:dolor:sit:amet)",      {":lorem", "ipsum", "dolor", "sit", "amet"},                ":",    '\\'},
    {R"(lorem# ipsum# dolor\ sit amet)",     {"lorem ipsum dolor\\", "sit", "amet"},                     " ",    '#' },
    {R"(lorem## ipsum## dolor\ sit amet)",   {"lorem#", "ipsum#", "dolor\\", "sit", "amet"},             " ",    '#' },
    {R"(lorem"ipsum dolor" sit amet)",       {"loremipsum dolor", "sit", "amet"},                        " ",    '\\'},
    {R"("lorem ipsum dolor"sit amet)",       {"lorem ipsum dolorsit", "amet"},                           " ",    '\\'},
    {R"(lorem ipsum dolor"sit amet")",       {"lorem", "ipsum", "dolorsit amet"},                        " ",    '\\'},
    {R"(lorem.ipsum,dolor;sit amet)",        {"lorem", "ipsum", "dolor", "sit", "amet"},                 " ;.,", '\\'},
    {R"(lorem.ipsum,dolor;sit amet)",        {"lorem", "ipsum", "dolor", "sit amet"},                    ";.,",  '\\'},
    {R"(lorem\.ipsum\,dolor\;sit amet)",     {"lorem.ipsum,dolor;sit amet"},                             ";.,",  '\\'},
    {R"(lorem "ipsum\' dolor\" sit" amet)",  {"lorem", R"(ipsum' dolor" sit)", "amet"},                  " ",    '\\'},
    {R"(lorem 'ipsum\' dolor\" sit' amet)",  {"lorem", R"(ipsum' dolor" sit)", "amet"},                  " ",    '\\'},
    {R"(lorem "ipsum\" dolor\' sit" amet)",  {"lorem", R"(ipsum" dolor' sit)", "amet"},                  " ",    '\\'},
    {R"(lorem 'ipsum\" dolor\' sit' amet)",  {"lorem", R"(ipsum" dolor' sit)", "amet"},                  " ",    '\\'},
    {R"(Let\'s call it \"Success\"\.)",      {"Let's call it \"Success\"."},                             ".",    '\\'},
    {"single",                               {"single"},                                                 " ",    '\\'},
    {"",                                     {""},                                                       " ",    '\\'},
    {"cornercase",                           {"cornercase"},                                             "",     '\\'},
    {R"(show "eth eth1)",                    { },                                                        " ",    '\\'},
    {R"(show \eth eth1)",                    { },                                                        " ",    '\\'},
    {R"(show "eth' eth1)",                   { },                                                        " ",    '\\'},
    {R"(lorem ipsum dolor sit amet\)",       { },                                                        " ",    '\\'},
    {R"(lorem#\ ipsum dolor sit amet)",      { },                                                        " ",    '#' },
    {R"(lorem ipsum dolor sit amet)",        { },                                                        " ",    ' ' },
    {R"(lorem ipsum dolor sit amet)",        { },                                                        " ,;",  ',' },
};

/**
 *  @class TokenizeEscapedTest
 */
class TokenizeEscapedTest : public testing::TestWithParam<TokenizeEscapedRecord> { };

TEST_P (TokenizeEscapedTest, Parse) {
    TokenizeEscapedRecord data = GetParam( );

    std::optional<std::vector<std::string>> tokensVec = wbr::str::tokenizeEscaped(data.input, data.divider, data.escape);

    EXPECT_EQ(tokensVec.value_or(std::vector<std::string> { }), data.expected);
}

INSTANTIATE_TEST_SUITE_P(StringManipulations, TokenizeEscapedTest, testing::ValuesIn(tokenizeEscapedInput));

/**
 * @brief Random-generated strings parser
 *
 * This tests generates vector of random strings, each string can have 1..12 'words'
 * Each word can have 0..12 symbols randomly selected from @c symbolSet
 * Divider randomly selected from @c dividersSet
 * Escape symbol randomly selected from @c escapeSet
 * Each 'word' can be:
 * @li left as is if no escaping required (mean - neither divider no escape no quote symbol present in it)
 * @li forcefully quoted by randomly selected quote (either @c \' or @c \" )
 * @li escaped all divider / escape / quote symbols
 */
class TokenizeEscapedPredefinedTest : public testing::Test {
    std::mt19937                               rng;
    static constexpr std::string_view          symbolSet {R"(qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM0123456789_:.;'\" %$)"};  ///< symbols used for constructing 'word'
    const std::array<char, 5>                  dividerSet {':', ';', ',', '.', '_'};                                                       ///< symbols, used for choosing 'divider'
    const std::array<char, 3>                  escapeSet {'\\', '%', '$'};                                                                 ///< symbols, used for choosing 'escape'
    static constexpr size_t                    wordMaxLen {10};
    static constexpr size_t                    minWordCount {1};
    static constexpr size_t                    maxWordCount {12};
    const std::array<wbr::str::QuoteSymbol, 2> quoteSet {wbr::str::QuoteSymbol::Single, wbr::str::QuoteSymbol::Double};

    ///{ @defgroup
    /*!
     * @brief get random element from  container
     * @tparam Vec container type (might be C-array, std::array, std::vector, std::string (_view) etc). Any container that has 2 functions -- std::size and operator[] defined will work
     * @param vec input container
     * @return copy of random element from container
     */
    template<class Vec>
    auto getRandomElement (const Vec& vec) -> decltype(*vec.begin( )) {
        std::uniform_int_distribution<size_t> idx(0, std::size(vec) - 1);
        return vec[idx(rng)];
    }

    /*
     * @brief get reference to random element from  container
     * @tparam Vec container type (might be C-array, std::array, std::vector, std::string (_view) etc). Any container that has 2 functions -- std::size and operator[] defined will work
     * @param vec input container
     * @return reference to random element from container
     */
    template<class Vec>
    auto getRandomElementRef (const Vec& vec) -> decltype(std::cref(*vec.begin( ))) {
        std::uniform_int_distribution<size_t> idx(0, std::size(vec) - 1);
        return vec[idx(rng)];
    }

    ///}

    std::string generateRandomString (size_t maxLen, size_t minLen = 0) {
        std::uniform_int_distribution<uint64_t> wordSizeRng(minLen, maxLen);
        std::string                             word;
        std::ranges::sample(symbolSet, std::back_inserter(word), wordSizeRng(rng), rng);
        return word;
    }

    std::string processWord (std::string_view word, std::string_view dividers, char escape) {
        std::uniform_int_distribution<uint8_t> r_style(0, 2);
        switch ( r_style(rng) ) {
            case 0: return wbr::str::quoteString(word, getRandomElement(quoteSet), escape);
            case 1: return quoteStringOnlyIfNeed(word, dividers, getRandomElement(quoteSet), escape);
            case 2: return wbr::str::escapeString(word, dividers, escape);
        }
        return std::string {word};
    }

    std::vector<TokenizeEscapedRecord> createDataSet (size_t n) {
        std::vector<TokenizeEscapedRecord>     vec;
        std::uniform_int_distribution<uint8_t> wordCount(minWordCount, maxWordCount);

        rng.seed(1);

        for ( size_t i = 0; i < n; i++ ) {
            TokenizeEscapedRecord& rec = vec.emplace_back( );
            rec.escape                 = getRandomElementRef(escapeSet);
            rec.divider.assign(1, getRandomElementRef(dividerSet));
            std::vector<std::string> escapedVec;
            std::generate_n(std::back_inserter(rec.expected), wordCount(rng), [this] ( ) { return generateRandomString(wordMaxLen); });
            std::ranges::transform(rec.expected, std::back_inserter(escapedVec), [this, &rec] (const std::string& s) { return processWord(s, rec.divider, rec.escape); });
            rec.input = wbr::str::join(escapedVec, rec.divider);
        }
        return vec;
    }

protected:
    const std::vector<TokenizeEscapedRecord> dataSet {createDataSet(1000)};
};

TEST_F (TokenizeEscapedPredefinedTest, Parse) {
    for ( const TokenizeEscapedRecord& data: dataSet ) {
        std::optional<std::vector<std::string>> tokensVec = wbr::str::tokenizeEscaped(data.input, data.divider, data.escape);
        //        fmt::print("{}\n", fmt::streamed(data));

        EXPECT_EQ(tokensVec.value_or(std::vector<std::string> { }), data.expected);
    }
}

TEST (AsHexTest, StringAsHex) {
    const std::string str {"HelloKitty"};
    EXPECT_EQ(wbr::str::convertToHexString(str), "48 65 6c 6c 6f 4b 69 74 74 79");
    EXPECT_EQ(wbr::str::convertToHexString(str, ""), "48656c6c6f4b69747479");
    EXPECT_EQ(wbr::str::convertToHexString(str, ":"), "48:65:6c:6c:6f:4b:69:74:74:79");
}

TEST (AsHexTest, SklStringAsHex) {
    const wbr::static_string<16> str {"HelloKitty"};
    EXPECT_EQ(wbr::str::convertToHexString(str), "48 65 6c 6c 6f 4b 69 74 74 79");
    EXPECT_EQ(wbr::str::convertToHexString(str, ""), "48656c6c6f4b69747479");
    EXPECT_EQ(wbr::str::convertToHexString(str, ":"), "48:65:6c:6c:6f:4b:69:74:74:79");
}

TEST (AsHexTest, StringViewAsHex) {
    const std::string_view sv {"HelloKitty"};
    EXPECT_EQ(wbr::str::convertToHexString(sv), "48 65 6c 6c 6f 4b 69 74 74 79");
    EXPECT_EQ(wbr::str::convertToHexString(sv, ""), "48656c6c6f4b69747479");
    EXPECT_EQ(wbr::str::convertToHexString(sv, ":"), "48:65:6c:6c:6f:4b:69:74:74:79");
}

TEST (AsHexTest, ArrayAsHex) {
    const std::array<uint8_t, 10> arr {'H', 'e', 'l', 'l', 'o', 'K', 'i', 't', 't', 'y'};
    EXPECT_EQ(wbr::str::convertToHexString(arr), "48 65 6c 6c 6f 4b 69 74 74 79");
    EXPECT_EQ(wbr::str::convertToHexString(arr, ""), "48656c6c6f4b69747479");
    EXPECT_EQ(wbr::str::convertToHexString(arr, ":"), "48:65:6c:6c:6f:4b:69:74:74:79");
}

TEST (AsHexTest, CArrayAsHex) {
    const char carr[] = "HelloKitty";
    EXPECT_EQ(wbr::str::convertToHexString(carr), "48 65 6c 6c 6f 4b 69 74 74 79");
    EXPECT_EQ(wbr::str::convertToHexString(carr, ""), "48656c6c6f4b69747479");
    EXPECT_EQ(wbr::str::convertToHexString(carr, ":"), "48:65:6c:6c:6f:4b:69:74:74:79");
}

TEST (AsHexTest, CStringAsHex) {
    const char* cstr = "HelloKitty";
    EXPECT_EQ(wbr::str::convertToHexString(cstr), "48 65 6c 6c 6f 4b 69 74 74 79");
    EXPECT_EQ(wbr::str::convertToHexString(cstr, ""), "48656c6c6f4b69747479");
    EXPECT_EQ(wbr::str::convertToHexString(cstr, ":"), "48:65:6c:6c:6f:4b:69:74:74:79");
}

TEST (AsHexTest, SpanAsHex) {
    const uint8_t id[] = {0xa1, 0xb2, 0xc3, 0xd4, 0xe5, 0xf6, 0x78, 0x90};
    EXPECT_EQ(wbr::str::convertToHexString(std::span {id, 8}), "a1 b2 c3 d4 e5 f6 78 90");
    EXPECT_EQ(wbr::str::convertToHexString(std::span {id, 8}, ""), "a1b2c3d4e5f67890");
    EXPECT_EQ(wbr::str::convertToHexString(std::span {id, 8}, ":"), "a1:b2:c3:d4:e5:f6:78:90");
}

// Tests for tokenize with action callback
TEST (TokenizeActionTest, CollectAllTokens) {
    std::vector<std::string> collected;
    wbr::str::tokenize_callback("one two three", [&collected] (std::string_view token) { collected.emplace_back(token); });
    EXPECT_EQ(collected.size( ), 3);
    EXPECT_EQ(collected[0], "one");
    EXPECT_EQ(collected[1], "two");
    EXPECT_EQ(collected[2], "three");
}

TEST (TokenizeActionTest, CountTokens) {
    size_t count = 0;
    wbr::str::tokenize_callback("one two three four", [&count] (std::string_view) { count++; });
    EXPECT_EQ(count, 4);
}

TEST (TokenizeActionTest, EmptyStringAction) {
    size_t count = 0;
    wbr::str::tokenize_callback("", [&count] (std::string_view) { count++; });
    EXPECT_EQ(count, 0);
}

TEST (TokenizeActionTest, CustomDelimiters) {
    std::vector<std::string> collected;
    wbr::str::tokenize_callback("one:two:three", [&collected] (std::string_view token) { collected.emplace_back(token); }, ":");
    EXPECT_EQ(collected.size( ), 3);
    EXPECT_EQ(collected[0], "one");
    EXPECT_EQ(collected[1], "two");
    EXPECT_EQ(collected[2], "three");
}

TEST (TokenizeActionTest, MultipleDelimiters) {
    std::vector<std::string> collected;
    wbr::str::tokenize_callback("one:two;three,four", [&collected] (std::string_view token) { collected.emplace_back(token); }, ":;,");
    EXPECT_EQ(collected.size( ), 4);
    EXPECT_EQ(collected[0], "one");
    EXPECT_EQ(collected[1], "two");
    EXPECT_EQ(collected[2], "three");
    EXPECT_EQ(collected[3], "four");
}

// Tests for tokenize with filter callback
TEST (TokenizeFilterTest, FilterNonEmpty) {
    const auto result = wbr::str::tokenize_filtered("one  two  three", wbr::str::nonempty);
    EXPECT_EQ(result.size( ), 3);
    EXPECT_EQ(result[0], "one");
    EXPECT_EQ(result[1], "two");
    EXPECT_EQ(result[2], "three");
}

TEST (TokenizeFilterTest, FilterByLength) {
    auto result = wbr::str::tokenize_filtered("a bb ccc dddd", (wbr::str::TokenFilter)[](std::string_view token) { return token.length( ) >= 2; });
    EXPECT_EQ(result.size( ), 3);
    EXPECT_EQ(result[0], "bb");
    EXPECT_EQ(result[1], "ccc");
    EXPECT_EQ(result[2], "dddd");
}

TEST (TokenizeFilterTest, FilterStartsWith) {
    auto result = wbr::str::tokenize_filtered("apple banana apricot cherry", [] (std::string_view token) { return token.starts_with('a'); });
    EXPECT_EQ(result.size( ), 2);
    EXPECT_EQ(result[0], "apple");
    EXPECT_EQ(result[1], "apricot");
}

TEST (TokenizeFilterTest, FilterAllOut) {
    auto result = wbr::str::tokenize_filtered("one two three", [] (std::string_view) { return false; });
    EXPECT_TRUE(result.empty( ));
}

TEST (TokenizeFilterTest, FilterNoneOut) {
    auto result = wbr::str::tokenize_filtered("one two three", [] (std::string_view) { return true; });
    EXPECT_EQ(result.size( ), 3);
    EXPECT_EQ(result[0], "one");
    EXPECT_EQ(result[1], "two");
    EXPECT_EQ(result[2], "three");
}

// Tests for tokenize with filter and action callbacks
TEST (TokenizeFilterActionTest, FilterAndCollect) {
    std::vector<std::string> collected;
    wbr::str::tokenize("one two three four", [] (std::string_view token) { return token.length( ) > 3; }, [&collected] (std::string_view token) { collected.emplace_back(token); });
    EXPECT_EQ(collected.size( ), 2);
    EXPECT_EQ(collected[0], "three");
    EXPECT_EQ(collected[1], "four");
}

TEST (TokenizeFilterActionTest, FilterAndCount) {
    size_t count = 0;
    wbr::str::tokenize("one two three four five", [] (std::string_view token) { return !token.empty( ); }, [&count] (std::string_view) { count++; });
    EXPECT_EQ(count, 5);
}

TEST (TokenizeFilterActionTest, FilterEmptyAndCount) {
    size_t count = 0;
    wbr::str::tokenize("one  two  three", [] (std::string_view token) { return !token.empty( ); }, [&count] (std::string_view) { count++; });
    EXPECT_EQ(count, 3);
}

TEST (TokenizeFilterActionTest, FilterAndTransform) {
    std::vector<std::string> uppercased;
    wbr::str::tokenize("hello world from cpp", [] (std::string_view) { return true; }, [&uppercased] (std::string_view token) { uppercased.push_back(wbr::str::strupper(std::string(token))); });
    EXPECT_EQ(uppercased.size( ), 4);
    EXPECT_EQ(uppercased[0], "HELLO");
    EXPECT_EQ(uppercased[1], "WORLD");
    EXPECT_EQ(uppercased[2], "FROM");
    EXPECT_EQ(uppercased[3], "CPP");
}

// Tests for tokenizeModify without filter
TEST (TokenizeModifyTest, UppercaseAll) {
    auto result = wbr::str::tokenize_modify<std::string>("hello world from cpp", [] (std::string_view token) -> std::string { return wbr::str::strupper(std::string(token)); });
    EXPECT_EQ(result.size( ), 4);
    EXPECT_EQ(result[0], "HELLO");
    EXPECT_EQ(result[1], "WORLD");
    EXPECT_EQ(result[2], "FROM");
    EXPECT_EQ(result[3], "CPP");
}

TEST (TokenizeModifyTest, LowercaseAll) {
    auto result = wbr::str::tokenize_modify<std::string>("HELLO WORLD FROM CPP", [] (std::string_view token) -> std::string { return wbr::str::strlower(std::string(token)); });
    EXPECT_EQ(result.size( ), 4);
    EXPECT_EQ(result[0], "hello");
    EXPECT_EQ(result[1], "world");
    EXPECT_EQ(result[2], "from");
    EXPECT_EQ(result[3], "cpp");
}

TEST (TokenizeModifyTest, CapitalizeFirstLetter) {
    auto result = wbr::str::tokenize_modify<std::string>("hello world from cpp", [] (std::string_view token) -> std::string {
        std::string s(token);
        if ( !s.empty( ) )
            s[0] = std::toupper(s[0]);
        return s;
    });
    EXPECT_EQ(result.size( ), 4);
    EXPECT_EQ(result[0], "Hello");
    EXPECT_EQ(result[1], "World");
    EXPECT_EQ(result[2], "From");
    EXPECT_EQ(result[3], "Cpp");
}

TEST (TokenizeModifyTest, AddPrefix) {
    auto result = wbr::str::tokenize_modify<std::string>("one two three", [] (std::string_view token) -> std::string { return std::string("prefix_") + std::string(token); });
    EXPECT_EQ(result.size( ), 3);
    EXPECT_EQ(result[0], "prefix_one");
    EXPECT_EQ(result[1], "prefix_two");
    EXPECT_EQ(result[2], "prefix_three");
}

TEST (TokenizeModifyTest, ConvertToLength) {
    auto result = wbr::str::tokenize_modify<size_t>("one two three", [] (std::string_view token) -> size_t { return token.length( ); });
    EXPECT_EQ(result.size( ), 3);
    EXPECT_EQ(result[0], 3);
    EXPECT_EQ(result[1], 3);
    EXPECT_EQ(result[2], 5);
}

TEST (TokenizeModifyTest, EmptyString) {
    auto result = wbr::str::tokenize_modify<std::string>("", [] (std::string_view token) -> std::string { return wbr::str::strupper(std::string(token)); });
    EXPECT_TRUE(result.empty( ));
}

TEST (TokenizeModifyTest, CustomDelimiters) {
    auto result = wbr::str::tokenize_modify<std::string>("one:two:three", [] (std::string_view token) -> std::string { return wbr::str::strupper(std::string(token)); }, ":");
    EXPECT_EQ(result.size( ), 3);
    EXPECT_EQ(result[0], "ONE");
    EXPECT_EQ(result[1], "TWO");
    EXPECT_EQ(result[2], "THREE");
}

// Tests for tokenizeModify with filter
TEST (TokenizeModifyFilterTest, FilterAndUppercase) {
    auto result = wbr::str::tokenize_modify<std::string>(
        "hello  world  ", [] (std::string_view token) { return !token.empty( ); }, [] (std::string_view token) -> std::string { return wbr::str::strupper(std::string(token)); });
    EXPECT_EQ(result.size( ), 2);
    EXPECT_EQ(result[0], "HELLO");
    EXPECT_EQ(result[1], "WORLD");
}

TEST (TokenizeModifyFilterTest, FilterByLengthAndCapitalize) {
    auto result = wbr::str::tokenize_modify<std::string>("a bb ccc dddd eeeee", [] (std::string_view token) { return token.length( ) >= 3; }, [] (std::string_view token) -> std::string {
        std::string s(token);
        if ( !s.empty( ) )
            s[0] = std::toupper(s[0]);
        return s;
    });
    EXPECT_EQ(result.size( ), 3);
    EXPECT_EQ(result[0], "Ccc");
    EXPECT_EQ(result[1], "Dddd");
    EXPECT_EQ(result[2], "Eeeee");
}

TEST (TokenizeModifyFilterTest, FilterStartsWithAndLength) {
    auto result = wbr::str::tokenize_modify<size_t>(
        "apple banana apricot cherry avocado", [] (std::string_view token) { return token.starts_with('a'); }, [] (std::string_view token) -> size_t { return token.length( ); });
    EXPECT_EQ(result.size( ), 3);
    EXPECT_EQ(result[0], 5);  // apple
    EXPECT_EQ(result[1], 7);  // apricot
    EXPECT_EQ(result[2], 7);  // avocado
}

TEST (TokenizeModifyFilterTest, FilterAllOut) {
    auto result = wbr::str::tokenize_modify<std::string>("one two three", [] (std::string_view) { return false; }, [] (std::string_view token) -> std::string { return std::string(token); });
    EXPECT_TRUE(result.empty( ));
}

TEST (TokenizeModifyFilterTest, FilterNoneOutAndReverse) {
    auto result = wbr::str::tokenize_modify<std::string>("one two three", [] (std::string_view) { return true; }, [] (std::string_view token) -> std::string {
        std::string s(token);
        std::ranges::reverse(s);
        return s;
    });
    EXPECT_EQ(result.size( ), 3);
    EXPECT_EQ(result[0], "eno");
    EXPECT_EQ(result[1], "owt");
    EXPECT_EQ(result[2], "eerht");
}

TEST (TokenizeModifyFilterTest, ComplexExample) {
    // Capitalize first letter of non-empty words longer than 2 characters
    auto result =
        wbr::str::tokenize_modify<std::string>("a is the cat on mat", [] (std::string_view token) { return !token.empty( ) && token.length( ) > 2; }, [] (std::string_view token) -> std::string {
        std::string s(token);
        if ( !s.empty( ) )
            s[0] = std::toupper(s[0]);
        return s;
    });
    EXPECT_EQ(result.size( ), 3);
    EXPECT_EQ(result[0], "The");
    EXPECT_EQ(result[1], "Cat");
    EXPECT_EQ(result[2], "Mat");
}
