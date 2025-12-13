#pragma once

namespace wbr {
/**
 @brief Strategy used for bounds checking in static_vector and static_vector_adapter.

 This enumeration defines different strategies for handling out-of-bounds access:
 - NoCheck: No checks are performed, leading to undefined behavior on bound violations.
 - Assert: Bounds are checked using assertions (only effective in debug mode).
 - Exception: Throws exceptions when bounds are violated.
 - LimitToBound: Silently limits positions to valid range without throwing exceptions.
*/
enum class BoundCheckStrategy {
    NoCheck,       ///< No checks, leading to undefined behavior on bound violations
    UB = NoCheck,  ///< Alias for NoCheck, indicating unsafe (undefined behavior) mode
    Assert,        ///< Checks bounds using assertions (debug mode only)
    Exception,     ///< Throws exceptions when bounds are violated
    LimitToBound,  ///< Silently limits positions to valid range without throwing exceptions
};

constexpr const char* to_string (BoundCheckStrategy e, const char* defValue = nullptr) noexcept {
    switch ( e ) {
        using enum BoundCheckStrategy;
        case NoCheck:      return "no check (undefined behavior)";
        case Assert:       return "assert";
        case Exception:    return "exception";
        case LimitToBound: return "bound-limit";
    }
    return defValue;
}
}  // namespace wbr
