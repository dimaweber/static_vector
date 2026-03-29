#pragma once
#include <fmt/chrono.h>

#include <string>

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
