#pragma once
#include <concepts>
#include <coroutine>
#include <exception>

namespace wbr::co {
// Generic generator template
// T: Storage type (what we store in currentValue)
// YieldT: What type we accept in co_yield
// ReturnT: What type we return from value() and operator*
template<typename T, typename YieldT = T, typename ReturnT = T>
  requires std::constructible_from<T, YieldT> && std::convertible_to<T, ReturnT>
struct generator {
  struct promise_type {
    T currentValue { };

    auto get_return_object ( ) {
      return generator {std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    std::suspend_always yield_value (YieldT value) {
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

  generator (std::coroutine_handle<promise_type> handle) : coroutine(handle) {
  }

  ~generator ( ) {
    if ( coroutine ) {
      coroutine.destroy( );
    }
  }

  // Delete copy, allow move
  generator(const generator&)             = delete;
  generator& operator= (const generator&) = delete;

  generator (generator&& other) noexcept : coroutine(other.coroutine) {
    other.coroutine = nullptr;
  }

  generator& operator= (generator&& other) noexcept {
    if ( this != &other ) {
      if ( coroutine ) {
        coroutine.destroy( );
      }
      coroutine       = other.coroutine;
      other.coroutine = nullptr;
    }
    return *this;
  }

  bool next ( ) const {
    if ( coroutine.done( ) )
      return false;
    coroutine.resume( );
    return !coroutine.done( );
  }

  ReturnT value ( ) const {
    return coroutine.promise( ).currentValue;
  }

  struct iterator {
    std::coroutine_handle<promise_type> coroutine;

    ReturnT operator* ( ) const {
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
}  // namespace wbr::co
