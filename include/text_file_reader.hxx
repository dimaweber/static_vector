#pragma once

#include <filesystem>
#include <fstream>

#include "platform.hxx"
#include "string_manipulations.hxx"

namespace wbr {
//@TODO: at the moment only '#' denotes comment, need add an ability to customize it

/**
 * @enum TextLineReadOpt
 * @brief Options for controlling text line reading behavior.
 *
 * These flags can be combined using bitwise OR to control how lines are read and processed
 * from text files.
 */
enum TextLineReadOpt {
  None           = 0,    ///< No special processing
  SkipEmpty      = 0x1,  ///< Skip empty lines
  SkipComment    = 0x2,  ///< Skip lines starting with '#' (after trimming whitespace)
  TrimWhitespace = 0x4   ///< Trim leading and trailing whitespace from lines
};

/**
 * @brief Bitwise OR operator for combining TextLineReadOpt flags.
 */
constexpr TextLineReadOpt operator| (TextLineReadOpt a, TextLineReadOpt b) {
  return static_cast<TextLineReadOpt>(std::to_underlying(a) | std::to_underlying(b));
}

/**
 * @struct text_line_t
 * @brief A string type that can be extracted from an input stream with specific line reading options.
 *
 * @tparam OPT Combination of TextLineReadOpt flags controlling how lines are processed
 *
 * This type extends std::string and provides custom stream extraction behavior based on
 * the template options. When extracted from a stream using operator>>, it will:
 * - Skip empty lines if SkipEmpty is set
 * - Skip comment lines (starting with '#') if SkipComment is set
 * - Trim whitespace if TrimWhitespace is set
 */
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
      if ( wbr::str::starts_with(wbr::str::trim(line, &wbr::str::isspace), "#") )
        continue;
    }
    break;
  }
  if ( OPT & TrimWhitespace )
    line.assign(wbr::str::trim(line, &wbr::str::isspace));
  return stream;
}

/**
 * @class text_file_reader_t
 * @brief A convenient RAII wrapper for reading text files line-by-line with configurable filtering.
 *
 * @tparam OPT Line reading options (default: SkipComment | SkipEmpty | TrimWhitespace)
 *
 * This class provides a range-based interface for reading text files, automatically handling
 * common text processing tasks like skipping comments, empty lines, and trimming whitespace.
 * It implements the input_range concept, allowing it to be used with range-based for loops
 * and standard algorithms.
 *
 * @par Features
 * - Automatic file opening and closing (RAII)
 * - Configurable line processing through template options
 * - Range-based interface compatible with C++20 ranges
 * - Support for skipping empty lines and comment lines (starting with '#')
 * - Automatic whitespace trimming
 *
 * @par Example Usage
 * @code
 * // Read configuration file, skipping comments and empty lines
 * text_file_reader_t reader("config.txt");
 * for (const auto& line : reader) {
 *     std::cout << line << '\n';
 * }
 *
 * // Read file without any filtering
 * text_file_reader_t<TextLineReadOpt::None> raw_reader("data.txt");
 * for (const auto& line : raw_reader) {
 *     process(line);
 * }
 *
 * // Custom options: skip only empty lines
 * text_file_reader_t<TextLineReadOpt::SkipEmpty> reader2("input.txt");
 * @endcode
 *
 * @par Thread Safety
 * Not thread-safe. Each instance should be accessed from a single thread.
 *
 * @par Performance Notes
 * - Lines are read lazily on demand (not all at once)
 * - Memory efficient for large files
 * - Suitable for processing files that don't fit in memory
 *
 * @note The class is move-only (not copyable) to maintain unique ownership of the file stream.
 * @note Comments are identified by lines starting with '#' after trimming whitespace.
 * @note The file is opened in text mode using std::ifstream defaults.
 */
template<TextLineReadOpt OPT = TextLineReadOpt::SkipComment | TextLineReadOpt::SkipEmpty | TextLineReadOpt::TrimWhitespace>
class text_file_reader_t {
  const std::filesystem::path filepath;  ///< Path to the file being read
  std::ifstream               stream;    ///< Input file stream

public:
  /**
   * @brief Constructs a text file reader and opens the specified file.
   * @param filepath Path to the text file to read
   */
  text_file_reader_t (std::filesystem::path filepath) : filepath {filepath}, stream {filepath} {
  }

  text_file_reader_t( )                             = default;
  ~text_file_reader_t( )                            = default;
  text_file_reader_t(text_file_reader_t&&) noexcept = default;
  text_file_reader_t(const text_file_reader_t&)     = delete;

  text_file_reader_t& operator= (text_file_reader_t&&) noexcept = default;
  text_file_reader_t& operator= (const text_file_reader_t&)     = delete;

  /**
   * @brief Checks if the file stream is open.
   * @return true if the file is successfully opened, false otherwise
   */
  [[nodiscard]] bool is_open ( ) const noexcept(noexcept(std::ifstream { }.is_open( ))) {
    return stream.is_open( );
  }

  /**
   * @brief Returns an iterator to the beginning of the file.
   * @return Input iterator positioned at the first line (after filtering)
   *
   * The iterator reads and processes lines according to the template options.
   * Lines are read lazily as the iterator is advanced.
   */
  [[nodiscard]] auto begin ( ) {
    return std::istream_iterator<text_line_t<OPT>> {stream};
  }

  /**
   * @brief Returns a sentinel iterator representing the end of the file.
   * @return End-of-stream iterator
   */
  [[nodiscard]] auto end ( ) {
    return std::istream_iterator<text_line_t<OPT>> { };
  }

  /**
   * @brief Filter predicate: checks if a line is empty.
   * @param s String to check
   * @return true if the string is empty
   */
  static bool empty_line_filter (const wbr::str::StringType auto& s) {
    return wbr::str::empty(s);
  }

  /**
   * @brief Filter predicate: checks if a line is a comment (starts with '#').
   * @param s String to check
   * @return true if the string starts with '#'
   */
  static bool comment_line_filter (const wbr::str::StringType auto& s) {
    return wbr::str::starts_with(s, "#");
  }

  /**
   * @brief Filter predicate: checks if a line is not empty.
   * @param s String to check
   * @return true if the string is not empty
   */
  static bool not_empty_line_filter (const wbr::str::StringType auto& s) {
    return !empty_line_filter(s);
  }

  /**
   * @brief Filter predicate: checks if a line is not a comment.
   * @param s String to check
   * @return true if the string does not start with '#'
   */
  static bool not_comment_line_filter (const wbr::str::StringType auto& s) {
    return !comment_line_filter(s);
  }

  /**
   * @brief Transform function: trims whitespace from a string.
   * @param s String to trim
   * @return New string with leading and trailing whitespace removed
   */
  static wbr::str::StringType auto trim_transform (const wbr::str::StringType auto& s) {
    return decltype(s) {wbr::str::trim(s, &wbr::str::isspace)};
  }
};

static_assert(std::ranges::range<text_file_reader_t<TextLineReadOpt::None>>);
static_assert(std::ranges::input_range<text_file_reader_t<TextLineReadOpt::None>>);
}  // namespace wbr
