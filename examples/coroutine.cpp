#include <fmt/base.h>

#include <concepts>
#include <coroutine>
#include <iostream>
#include <type_traits>

#include "co_generator.hxx"

// C++23 standard generator support
#if __has_include(<generator>)
  #include <generator>
#endif

#include "string_manipulations.hxx"

struct number_generator {
  struct promise_type {
    int currentValue = 0;

    auto get_return_object ( ) {
      return number_generator {std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    std::suspend_always yield_value (int value) {
      currentValue = value;
      return { };
    }

    std::suspend_never initial_suspend ( ) {
      return { };
    }

    std::suspend_always final_suspend ( ) noexcept {
      return { };
    }

    void return_void ( ) {
    }

    void unhandled_exception ( ) {
      std::terminate( );
    }
  };

  std::coroutine_handle<promise_type> coroutine;

  number_generator (std::coroutine_handle<promise_type> handle) : coroutine(handle) {
  }

  ~number_generator ( ) {
    if ( coroutine ) {
      coroutine.destroy( );
    }
  }

  bool next ( ) const {
    if ( coroutine.done( ) )
      return false;
    coroutine.resume( );
    return !coroutine.done( );
  }

  int value ( ) const {
    return coroutine.promise( ).currentValue;
  }

  struct iterator {
    std::coroutine_handle<promise_type> coroutine;

    int operator* ( ) const {
      return coroutine.promise( ).currentValue;
    }

    bool operator== (std::default_sentinel_t) const {
      return coroutine.done( );
    }

    void operator++ ( ) const {
      if ( !coroutine.done( ) )
        coroutine.resume( );
    }
  };

  iterator begin ( ) const {
    return {coroutine};
  }

  std::default_sentinel_t end ( ) const {
    return { };
  }
};

template<typename T>
number_generator generateNumbers (T start, T end) {
  for ( T i = start; i <= end; ++i ) {
    co_yield i;
  }
}

// Using the new generic generator template
template<typename T>
wbr::co::generator<T> generateNumbersGeneric (T start, T end) {
  for ( T i = start; i <= end; ++i ) {
    co_yield i;
  }
}

void coroutine_usage ( ) {
  fmt::print("=== Original number_generator ===\n");
  const auto gen = generateNumbers<int>(1, 5);
  for ( const std::integral auto i: gen )
    fmt::print("generated: {}\n", i);

  fmt::print("\n=== Generic generator<int> ===\n");
  const auto gen2 = generateNumbersGeneric<int>(10, 14);
  for ( const std::integral auto i: gen2 )
    fmt::print("generated: {}\n", i);
}

struct token_generator {
  struct promise_type {
    std::string currentValue;

    auto get_return_object ( ) {
      return token_generator {std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    std::suspend_always yield_value (std::string_view value) {
      currentValue = value;
      return { };
    }

    std::suspend_never initial_suspend ( ) {
      return { };
    }

    std::suspend_always final_suspend ( ) noexcept {
      return { };
    }

    void return_void ( ) {
    }

    void unhandled_exception ( ) {
      std::terminate( );
    }
  };

  std::coroutine_handle<promise_type> coroutine;

  token_generator (std::coroutine_handle<promise_type> handle) : coroutine(handle) {
  }

  ~token_generator ( ) {
    if ( coroutine ) {
      coroutine.destroy( );
    }
  }

  bool next ( ) const {
    if ( coroutine.done( ) )
      return false;
    coroutine.resume( );
    return !coroutine.done( );
  }

  std::string_view value ( ) const {
    return coroutine.promise( ).currentValue;
  }

  struct iterator {
    std::coroutine_handle<promise_type> coroutine;

    std::string_view operator* ( ) const {
      return coroutine.promise( ).currentValue;
    }

    bool operator== (std::default_sentinel_t) const {
      return coroutine.done( );
    }

    void operator++ ( ) const {
      if ( !coroutine.done( ) )
        coroutine.resume( );
    }
  };

  iterator begin ( ) const {
    return {coroutine};
  }

  std::default_sentinel_t end ( ) const {
    return { };
  }
};

token_generator tokenizeEscapedCoroutine (std::string_view input, std::string_view dividerChars = " ", char escapeChar = '\\') {
  auto is_quote   = [] (char c) { return c == '\'' || c == '"'; };
  auto is_divider = [dividerChars] (char c) { return dividerChars.find(c) != std::string_view::npos; };
  auto is_escape  = [escapeChar] (char c) { return c == escapeChar; };

  if ( is_divider(escapeChar) )
    co_return;

  enum class E_ParserState {
    regular,
    regularEscape,
    quote,
    quoteEscape,
  };
  E_ParserState curState {E_ParserState::regular};
  char          quoteChar {'\0'};
  std::string   currentToken;

  for ( const char curChar: input ) {
    switch ( curState ) {
      case E_ParserState::regular:
        if ( is_divider(curChar) ) {
          co_yield currentToken;
          currentToken.clear( );
        } else if ( is_escape(curChar) ) {
          curState = E_ParserState::regularEscape;
        } else if ( is_quote(curChar) ) {
          curState  = E_ParserState::quote;
          quoteChar = curChar;
        } else
          currentToken.append(1, curChar);
        break;

      case E_ParserState::quote:
        if ( is_escape(curChar) ) {
          curState = E_ParserState::quoteEscape;
        } else if ( curChar == quoteChar ) {
          curState = E_ParserState::regular;
        } else
          currentToken.append(1, curChar);
        break;

      case E_ParserState::regularEscape:
        if ( is_divider(curChar) || is_escape(curChar) || is_quote(curChar) ) {
          currentToken.append(1, curChar);
          curState = E_ParserState::regular;
          break;
        }
        co_return;

      case E_ParserState::quoteEscape:
        if ( is_divider(curChar) || is_escape(curChar) || is_quote(curChar) ) {
          currentToken.append(1, curChar);
          curState = E_ParserState::quote;
          break;
        }
        co_return;
    }
  }

  if ( curState != E_ParserState::regular )
    co_return;

  co_yield currentToken;
}

// Using the new generic generator template
// For strings: store std::string, accept std::string_view, return std::string_view

wbr::co::generator<std::string, std::string_view, std::string_view> tokenizeEscapedGeneric (std::string_view input, std::string_view dividerChars = " ", char escapeChar = '\\') {
  auto is_quote   = [] (char c) { return c == '\'' || c == '"'; };
  auto is_divider = [dividerChars] (char c) { return dividerChars.find(c) != std::string_view::npos; };
  auto is_escape  = [escapeChar] (char c) { return c == escapeChar; };

  if ( is_divider(escapeChar) )
    co_return;

  enum class E_ParserState {
    regular,
    regularEscape,
    quote,
    quoteEscape,
  };
  E_ParserState curState {E_ParserState::regular};
  char          quoteChar {'\0'};
  std::string   currentToken;

  for ( const char curChar: input ) {
    switch ( curState ) {
      case E_ParserState::regular:
        if ( is_divider(curChar) ) {
          co_yield currentToken;
          currentToken.clear( );
        } else if ( is_escape(curChar) ) {
          curState = E_ParserState::regularEscape;
        } else if ( is_quote(curChar) ) {
          curState  = E_ParserState::quote;
          quoteChar = curChar;
        } else
          currentToken.append(1, curChar);
        break;

      case E_ParserState::quote:
        if ( is_escape(curChar) ) {
          curState = E_ParserState::quoteEscape;
        } else if ( curChar == quoteChar ) {
          curState = E_ParserState::regular;
        } else
          currentToken.append(1, curChar);
        break;

      case E_ParserState::regularEscape:
        if ( is_divider(curChar) || is_escape(curChar) || is_quote(curChar) ) {
          currentToken.append(1, curChar);
          curState = E_ParserState::regular;
          break;
        }
        co_return;

      case E_ParserState::quoteEscape:
        if ( is_divider(curChar) || is_escape(curChar) || is_quote(curChar) ) {
          currentToken.append(1, curChar);
          curState = E_ParserState::quote;
          break;
        }
        co_return;
    }
  }

  if ( curState != E_ParserState::regular )
    co_return;

  co_yield currentToken;
}

// ============================================================================
// C++23 std::generator examples (if available)
// ============================================================================

#if defined(__cpp_lib_generator) && (__cpp_lib_generator >= 201907L)

// Simple number generator using std::generator
std::generator<int> generateNumbersStd (int start, int end) {
  for ( int i = start; i <= end; ++i ) {
    co_yield i;
  }
}

// String tokenizer using std::generator
// Note: std::generator<std::string_view> would be unsafe because views would dangle
// We must use std::generator<std::string> to own the data
std::generator<std::string> tokenizeEscapedStd (std::string_view input, std::string_view dividerChars = " ", char escapeChar = '\\') {
  auto is_quote   = [] (char c) { return c == '\'' || c == '"'; };
  auto is_divider = [dividerChars] (char c) { return dividerChars.find(c) != std::string_view::npos; };
  auto is_escape  = [escapeChar] (char c) { return c == escapeChar; };

  if ( is_divider(escapeChar) )
    co_return;

  enum class E_ParserState {
    regular,
    regularEscape,
    quote,
    quoteEscape,
  };
  E_ParserState curState {E_ParserState::regular};
  char          quoteChar {'\0'};
  std::string   currentToken;

  for ( const char curChar: input ) {
    switch ( curState ) {
      case E_ParserState::regular:
        if ( is_divider(curChar) ) {
          co_yield currentToken;
          currentToken.clear( );
        } else if ( is_escape(curChar) ) {
          curState = E_ParserState::regularEscape;
        } else if ( is_quote(curChar) ) {
          curState  = E_ParserState::quote;
          quoteChar = curChar;
        } else
          currentToken.append(1, curChar);
        break;

      case E_ParserState::quote:
        if ( is_escape(curChar) ) {
          curState = E_ParserState::quoteEscape;
        } else if ( curChar == quoteChar ) {
          curState = E_ParserState::regular;
        } else
          currentToken.append(1, curChar);
        break;

      case E_ParserState::regularEscape:
        if ( is_divider(curChar) || is_escape(curChar) || is_quote(curChar) ) {
          currentToken.append(1, curChar);
          curState = E_ParserState::regular;
          break;
        }
        co_return;

      case E_ParserState::quoteEscape:
        if ( is_divider(curChar) || is_escape(curChar) || is_quote(curChar) ) {
          currentToken.append(1, curChar);
          curState = E_ParserState::quote;
          break;
        }
        co_return;
    }
  }

  if ( curState != E_ParserState::regular )
    co_return;

  co_yield currentToken;
}

#endif  // HAS_STD_GENERATOR

void tokenizer_coroutine_usage ( ) {
  fmt::print("\n=== Original token_generator ===\n");
  const auto gen = tokenizeEscapedCoroutine("The song \"Hotel California\" is famous one");
  for ( const wbr::StringViewLike auto token: gen )
    fmt::print("token: '{}'\n", token);

  fmt::print("\n=== Generic generator<std::string, std::string_view, std::string_view> ===\n");
  const auto gen2 = tokenizeEscapedGeneric("Another example: 'Hello world' with escapes");
  for ( const wbr::StringViewLike auto token: gen2 )
    fmt::print("token: '{}'\n", token);

#if defined(__cpp_lib_generator) && (__cpp_lib_generator >= 201907L)
  fmt::print("\n=== C++23 std::generator<int> ===\n");
  for ( int i: generateNumbersStd(100, 103) ) {
    fmt::print("std::generator: {}\n", i);
  }

  fmt::print("\n=== C++23 std::generator<std::string> tokenizer ===\n");
  for ( const std::string& token: tokenizeEscapedStd("C++23 'standard library' rocks!") ) {
    fmt::print("token: '{}'\n", token);
  }
#else
  fmt::print("\n[std::generator not available - requires C++23 with library support]\n");
#endif
}
