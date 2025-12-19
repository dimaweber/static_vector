#pragma once

#ifdef _MSC_VER
  // Define ssize_t for MSVC if not already defined
  #ifndef ssize_t
using ssize_t = intptr_t;
  #endif
#else
  // Use standard definition for non-MSVC compilers
  #include <sys/types.h>
#endif

#if !defined(__cpp_lib_to_underlying)
namespace std {
template<typename Enum>
  requires std::is_enum_v<Enum>
constexpr typename std::underlying_type<Enum>::type to_underlying (Enum e) noexcept {
  return static_cast<typename std::underlying_type<Enum>::type>(e);
}
}  // namespace std
#endif
