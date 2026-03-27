//***************************************************************
// File: MacAddress.h
// Class/Module:MacAddress
// Purpose:
// Description:
//
// Modification history:
//           28/05/2000   [author]  Created
//
// Class Invariant:
// Copy Semantics:
// ***************************************************************
#pragma once

#include <fmt/format.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <string>

namespace wbr::net {
/**
 * @brief The Mac class represents Ethernet Mac Address
 *
 * This class might be used in low-level messaging and in casts from low-level data.
 * As such, it's memory footprint should be exactly 6 bytes. Because of this we can't add
 * cached string representation of it as member and have to call AsString to calculate
 * Mac representation all the time.
 */
class mac_address_t {
public:
  static constexpr size_t address_length = 6;

  constexpr mac_address_t( )                        = default;
  mac_address_t(const mac_address_t& m)             = default;
  mac_address_t& operator= (const mac_address_t& m) = default;
  mac_address_t(mac_address_t&&)                    = default;
  mac_address_t& operator= (mac_address_t&&)        = default;

  /** Create mac from 6-bytes array. */
  explicit mac_address_t (const uint8_t* srcP) : mac_address_t( ) {
    if ( srcP )
      std::copy_n(srcP, address_length, mac);
  }

  /** Create mac address from direct values. Most usefull in form
   *  @code{.cpp}
   *  const Mac sikluDeviceMac = {0x00, 0x2a, 0xa4, 0x12, 0xf5, 0x14};
   *  @endcode
   */
  constexpr mac_address_t (uint8_t v1, uint8_t v2, uint8_t v3, uint8_t v4, uint8_t v5, uint8_t v6) {
    mac[0] = v1;
    mac[1] = v2;
    mac[2] = v3;
    mac[3] = v4;
    mac[4] = v5;
    mac[5] = v6;
  }

  /** Compare operations. */
  /**@{*/
  /**  Equal compare.
   *   @return true if bytes in both addresses are equal. false otherwise*/
  bool operator== (const mac_address_t& m) const {
    return std::ranges::equal(mac, m.mac);
  }

  /** three-way compare.
   *  @return true if the first differing byte in object is less than the corresponding byte in m. false otherwise */
  int operator<=> (const mac_address_t& m) const {
    return ::memcmp(mac, m.mac, address_length);
  }

  /**@}*/

  /** set all mac address bytes to zero */
  void clear ( ) {
    std::ranges::fill(mac, 0);
  }

  /** copy mac address bytes into 6-bytes array and return number of copies bytes
      if dstP is nullptr - just report required destination array length to fit all the bytes (6)
  */
  size_t copy (uint8_t* dstP) const {
    if ( dstP )
      std::ranges::copy(mac, dstP);
    return address_length;
  }

  /** returns string representation of Mac address in "xx:xx:xx:xx:xx:xx" format.
      @param delimiter specify which character will be used as delimiter between octets. if '\0' specified - no delimiter will be used.
                       You can specify any character, but it is not recommended to use any other than ':' or '-'.
      @return string representation of mac address. hex digits are lower case.
  */
  std::string str (char delimiter = ':') const {
    std::string result;
    result.reserve(address_length * 3 - 1);  // 2 hex digits + delimiter for each byte except the last one

    for ( size_t i = 0; i < address_length; ++i ) {
      if ( i > 0 && delimiter != '\0' ) {
        result.push_back(delimiter);
      }
      result += fmt::format("{:02x}", mac[i]);
    }

    return result;
  }

  /** change last byte of mac address, incrementing it by given value. If offs is negative, last byte will be decremented.
   *  Does not perform wrap check for now.
   *  @todo add wrap check
   */
  void AddOffs(int8_t offs);

  /** return reference to byte. Direct access to memory. Use with caution!
   */
  uint8_t& byte (uint i) {
    return (i < mac_address_t::address_length) ? mac[i] : throw std::out_of_range("index out of range while accessing mac address bytes");
  }

  /** return value of byte.
   *  If index is out of range -- throws std::out_of_range exception.
   */
  uint8_t operator[] (uint i) const {
    return (i < address_length) ? mac[i] : throw std::out_of_range("index out of range while accessing mac address bytes");
  }

  /** consturct Mac address from string representation
   *  @param[in] str string in format "xx:xx:xx:xx:xx:xx" or "xx-xx-xx-xx-xx-xx". Will fail if string shorted or longer then this
   *                 format. Delimiters might be either ':' or '-'. Delimiter can be mixed. Any other symbols used as delimiter will
   *                 fail the function. Other symbols (in places marked with 'x' in format) should be hex digit upper or lower case. Any other
   *                 symbol in places marked with 'x' will lead to fail
   *  @return if conversion performed, result is valid value with parsed Mac address. If conversion failed - result is invalid
   */
  static std::optional<mac_address_t> from_string (std::string_view str) {
    auto IsDelim = [] (char symbol) -> bool {
      if ( symbol == ':' )
        return true;
      if ( symbol == '-' )
        return true;
      return false;
    };

    auto hex2int = [] (char ch) -> uint8_t {
      char ret = 0;
      if ( ch >= '0' && ch <= '9' )
        ret = ch - '0';
      else if ( ch >= 'A' && ch <= 'F' )
        ret = ch - 'A' + 10;
      else if ( ch >= 'a' && ch <= 'f' )
        ret = ch - 'a' + 10;
      return static_cast<uint8_t>(ret);
    };

    if ( str.empty( ) )
      return std::nullopt;

    constexpr const char* const expectedFormat = "xx:xx:xx:xx:xx:xx";
    const size_t                expectedLength = strlen(expectedFormat);

    if ( str.length( ) != expectedLength )
      return std::nullopt;

    char macStr[18] {0};
    strncpy(macStr, str.data( ), sizeof(macStr) - 1);

    int           chCount = 0;
    mac_address_t m;
    for ( size_t idx = 0; idx < expectedLength; idx++ ) {
      switch ( expectedFormat[idx] ) {
        case ':':
          if ( !IsDelim(macStr[idx]) )
            return std::nullopt;
          macStr[idx] = '\0';
          chCount     = 0;
          break;
        case 'x':
          if ( !isxdigit(macStr[idx]) )
            return std::nullopt;
          if ( ++chCount == 2 )
            m.mac[idx / 3] = static_cast<uint8_t>(hex2int(macStr[idx - 1]) << 4 | hex2int(macStr[idx]));
          break;
        default: break;
      }
    }

    return m;
  }

  [[nodiscard]] constexpr bool is_broadcast ( ) const noexcept {
    return std::ranges::all_of(mac, [] (uint8_t b) { return b == 0xff; });
  }

  [[nodiscard]] constexpr bool is_multicast ( ) const noexcept {
    return mac[0] & 0x01;
  }

  [[nodiscard]] constexpr bool is_unicast ( ) const noexcept {
    return !is_broadcast( ) && !is_multicast( );
  }

  [[nodiscard]] constexpr bool empty ( ) const noexcept {
    constexpr mac_address_t ZERO;
    return std::ranges::equal(mac, ZERO.mac);
  }

private:
  uint8_t mac[address_length] = {0};
} __attribute__((packed));
}  // namespace wbr::net

FMT_BEGIN_NAMESPACE

template<>
struct formatter<wbr::net::mac_address_t> : formatter<std::string> {
  template<typename FormatterContext>
  void format (const wbr::net::mac_address_t& mac, FormatterContext& ctx) const {
    return formatter<std::string>::format(mac.str( ), ctx);
  }
};

FMT_END_NAMESPACE
