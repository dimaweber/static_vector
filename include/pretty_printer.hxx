#pragma once
#include <fmt/chrono.h>

#include <string>

#include "concepts.hxx"
#include "static_string.hxx"

namespace wbr {
inline std::string pretty_printer (const std::chrono::duration<double> d) {
  auto conv = []<typename A, typename B>(const auto& elaps) {
    const auto a = std::chrono::duration_cast<A>(elaps);
    const auto b = std::chrono::duration_cast<B>(elaps - a);
    return fmt::format("{0:%Q}.{1:%Q}{0:%q}", a, b);
  };

  using namespace std::chrono;
  using namespace std::chrono_literals;
  const auto elaps = d;
  if ( elaps > 1h ) {
    const auto a = duration_cast<hours>(elaps);
    const auto b = duration_cast<minutes>(elaps - a);
    const auto c = duration_cast<seconds>(elaps - a - b);
    return fmt::format("{:%Q}:{:%Q}:{:%Q}", a, b, c);
  }
  if ( elaps > 2min ) {
    const auto a = duration_cast<minutes>(elaps);
    const auto b = duration_cast<seconds>(elaps - a);
    return fmt::format("{:%Q}:{:%Q}", a, b);
  }
  if ( elaps > 2s )
    return conv.template operator( )<seconds, milliseconds>(elaps);

  if ( elaps > 2ms )
    return conv.template operator( )<milliseconds, microseconds>(elaps);

  if ( elaps > 2us )
    return conv.template operator( )<microseconds, nanoseconds>(elaps);

  return fmt::format("{:%Q%q}", elaps);
}
}  // namespace wbr

FMT_BEGIN_NAMESPACE

template<typename E>
concept FormattableViaToString = wbr::ConvertibleToString<E> && std::is_enum_v<E>;

template<FormattableViaToString E>
struct formatter<E> : formatter<std::string_view> {
  template<typename FormatContext>
  auto format (const E e, FormatContext& ctx) const {
    return formatter<std::string_view>::format(to_string<std::string_view>(e), ctx);
  }
};

FMT_END_NAMESPACE
