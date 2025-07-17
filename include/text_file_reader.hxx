#pragma once

#include <filesystem>
#include <fstream>

#include "string_manipulations.hxx"

namespace wbr {
//@TODO: at the moment only '#' denotes comment, need add an ability to customize it

enum TextLineReadOpt { None, SkipEmpty = 0x1, SkipComment = 0x2, TrimWhitespace = 0x4 };

constexpr TextLineReadOpt operator| (TextLineReadOpt a, TextLineReadOpt b) {
    return static_cast<TextLineReadOpt>(std::to_underlying(a) | std::to_underlying(b));
}

template<TextLineReadOpt OPT>
struct text_line_t : public std::string { };

template<TextLineReadOpt OPT>
std::istream& operator>> (std::istream& stream, text_line_t<OPT>& line) {
    while ( stream ) {
        std::getline(stream, line);
        if ( OPT & SkipEmpty ) {
            if ( line.empty( ) )
                continue;
        }
        if ( OPT & SkipComment ) {
            if ( wbr::str::starts_with(wbr::str::trimWhitespaces(line, &wbr::str::isspace), "#") )
                continue;
        }
        break;
    }
    if ( OPT & TrimWhitespace )
        line.assign(wbr::str::trimWhitespaces(line, &wbr::str::isspace));
    return stream;
}

template<TextLineReadOpt OPT = TextLineReadOpt::SkipComment | TextLineReadOpt::SkipEmpty | TextLineReadOpt::TrimWhitespace>
class text_file_reader_t {
    const std::filesystem::path filepath;
    std::ifstream               stream;

public:
    text_file_reader_t (std::filesystem::path filepath) : filepath {filepath}, stream {filepath} {
    }

    text_file_reader_t( )                             = default;
    ~text_file_reader_t( )                            = default;
    text_file_reader_t(text_file_reader_t&&) noexcept = default;
    text_file_reader_t(const text_file_reader_t&)     = delete;

    text_file_reader_t& operator= (text_file_reader_t&&) noexcept = default;
    text_file_reader_t& operator= (const text_file_reader_t&)     = delete;

    [[nodiscard]] bool is_open ( ) const noexcept(noexcept(std::ifstream { }.is_open( ))) {
        return stream.is_open( );
    }

    [[nodiscard]] auto begin ( ) {
        return std::istream_iterator<text_line_t<OPT>> {stream};
    }

    [[nodiscard]] auto end ( ) {
        return std::istream_iterator<text_line_t<OPT>> { };
    }

    static bool empty_line_filter (const wbr::str::StringType auto& s) {
        return wbr::str::empty(s);
    }

    static bool comment_line_filter (const wbr::str::StringType auto& s) {
        return wbr::str::starts_with(s, "#");
    }

    static bool not_empty_line_filter (const wbr::str::StringType auto& s) {
        return !empty_line_filter(s);
    }

    static bool not_comment_line_filter (const wbr::str::StringType auto& s) {
        return !comment(s);
    }

    static wbr::str::StringType auto trim_transform (const wbr::str::StringType auto& s) {
        return decltype(s) {wbr::str::trimWhitespaces(s, &wbr::str::isspace)};
    }
};

static_assert(std::ranges::range<text_file_reader_t<TextLineReadOpt::None>>);
static_assert(std::ranges::input_range<text_file_reader_t<TextLineReadOpt::None>>);
}  // namespace wbr
