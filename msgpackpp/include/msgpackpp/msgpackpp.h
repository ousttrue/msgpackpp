#pragma once
#include <algorithm>
#include <assert.h>
#include <cstdint>
#include <functional>
#include <iosfwd>
#include <iterator>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)
#pragma warning(disable : 4018)

#if _HAS_CXX17
#include <string_view>
#else
namespace std {
class string_view {
  const char *m_p;
  size_t m_size;

public:
  string_view() : m_p(0), m_size(0) {}

  string_view(const char *p, size_t size) : m_p(p), m_size(size) {}

  const char *begin() const { return m_p; }
  size_t size() const { return m_size; }
  const char *end() const { return m_p + m_size; }

  bool operator==(const string &rhs) const {
    return equal(begin(), end(), rhs.begin(), rhs.end());
  }

  bool operator==(const string_view &rhs) const {
    return equal(begin(), end(), rhs.begin(), rhs.end());
  }

  string str() const { return string(begin(), end()); }

  operator string() const { return str(); }
};

inline ostream &operator<<(ostream &os, const string_view &view) {
  os << string(view.begin(), view.begin() + view.size());
  return os;
}

inline bool operator==(const string &lhs, const string_view &rhs) {
  return equal(lhs.begin(), lhs.end(), begin(rhs), end(rhs));
}
} // namespace std
#endif

namespace msgpackpp {

struct pack_error : public std::runtime_error {
  pack_error(const char *msg) : runtime_error(msg) {}
};

struct underflow_pack_error : public pack_error {
  underflow_pack_error() : pack_error("underflow") {}
};

struct overflow_pack_error : public pack_error {
  overflow_pack_error() : pack_error("overflow") {}
};

struct parse_error : public std::runtime_error {
  parse_error(const char *msg) : runtime_error(msg) {}
};

struct empty_parse_error : public parse_error {
  empty_parse_error() : parse_error("empty") {}
};

struct lack_parse_error : public parse_error {
  lack_parse_error() : parse_error("lack") {}
};

struct invalid_parse_error : public parse_error {
  invalid_parse_error() : parse_error("invalid") {}
};

struct not_implemented_parse_error : public parse_error {
  not_implemented_parse_error() : parse_error("not implemented") {}
};

struct type_parse_error : public parse_error {
  type_parse_error() : parse_error("type") {}
};

enum pack_type : std::uint8_t {
#pragma region POSITIVE_FIXNUM 0x00 - 0x7F
  POSITIVE_FIXNUM = 0x00,
  POSITIVE_FIXNUM_0x01 = 0x01,
  POSITIVE_FIXNUM_0x02 = 0x02,
  POSITIVE_FIXNUM_0x03 = 0x03,
  POSITIVE_FIXNUM_0x04 = 0x04,
  POSITIVE_FIXNUM_0x05 = 0x05,
  POSITIVE_FIXNUM_0x06 = 0x06,
  POSITIVE_FIXNUM_0x07 = 0x07,
  POSITIVE_FIXNUM_0x08 = 0x08,
  POSITIVE_FIXNUM_0x09 = 0x09,
  POSITIVE_FIXNUM_0x0A = 0x0A,
  POSITIVE_FIXNUM_0x0B = 0x0B,
  POSITIVE_FIXNUM_0x0C = 0x0C,
  POSITIVE_FIXNUM_0x0D = 0x0D,
  POSITIVE_FIXNUM_0x0E = 0x0E,
  POSITIVE_FIXNUM_0x0F = 0x0F,

  POSITIVE_FIXNUM_0x10 = 0x10,
  POSITIVE_FIXNUM_0x11 = 0x11,
  POSITIVE_FIXNUM_0x12 = 0x12,
  POSITIVE_FIXNUM_0x13 = 0x13,
  POSITIVE_FIXNUM_0x14 = 0x14,
  POSITIVE_FIXNUM_0x15 = 0x15,
  POSITIVE_FIXNUM_0x16 = 0x16,
  POSITIVE_FIXNUM_0x17 = 0x17,
  POSITIVE_FIXNUM_0x18 = 0x18,
  POSITIVE_FIXNUM_0x19 = 0x19,
  POSITIVE_FIXNUM_0x1A = 0x1A,
  POSITIVE_FIXNUM_0x1B = 0x1B,
  POSITIVE_FIXNUM_0x1C = 0x1C,
  POSITIVE_FIXNUM_0x1D = 0x1D,
  POSITIVE_FIXNUM_0x1E = 0x1E,
  POSITIVE_FIXNUM_0x1F = 0x1F,

  POSITIVE_FIXNUM_0x20 = 0x20,
  POSITIVE_FIXNUM_0x21 = 0x21,
  POSITIVE_FIXNUM_0x22 = 0x22,
  POSITIVE_FIXNUM_0x23 = 0x23,
  POSITIVE_FIXNUM_0x24 = 0x24,
  POSITIVE_FIXNUM_0x25 = 0x25,
  POSITIVE_FIXNUM_0x26 = 0x26,
  POSITIVE_FIXNUM_0x27 = 0x27,
  POSITIVE_FIXNUM_0x28 = 0x28,
  POSITIVE_FIXNUM_0x29 = 0x29,
  POSITIVE_FIXNUM_0x2A = 0x2A,
  POSITIVE_FIXNUM_0x2B = 0x2B,
  POSITIVE_FIXNUM_0x2C = 0x2C,
  POSITIVE_FIXNUM_0x2D = 0x2D,
  POSITIVE_FIXNUM_0x2E = 0x2E,
  POSITIVE_FIXNUM_0x2F = 0x2F,

  POSITIVE_FIXNUM_0x30 = 0x30,
  POSITIVE_FIXNUM_0x31 = 0x31,
  POSITIVE_FIXNUM_0x32 = 0x32,
  POSITIVE_FIXNUM_0x33 = 0x33,
  POSITIVE_FIXNUM_0x34 = 0x34,
  POSITIVE_FIXNUM_0x35 = 0x35,
  POSITIVE_FIXNUM_0x36 = 0x36,
  POSITIVE_FIXNUM_0x37 = 0x37,
  POSITIVE_FIXNUM_0x38 = 0x38,
  POSITIVE_FIXNUM_0x39 = 0x39,
  POSITIVE_FIXNUM_0x3A = 0x3A,
  POSITIVE_FIXNUM_0x3B = 0x3B,
  POSITIVE_FIXNUM_0x3C = 0x3C,
  POSITIVE_FIXNUM_0x3D = 0x3D,
  POSITIVE_FIXNUM_0x3E = 0x3E,
  POSITIVE_FIXNUM_0x3F = 0x3F,

  POSITIVE_FIXNUM_0x40 = 0x40,
  POSITIVE_FIXNUM_0x41 = 0x41,
  POSITIVE_FIXNUM_0x42 = 0x42,
  POSITIVE_FIXNUM_0x43 = 0x43,
  POSITIVE_FIXNUM_0x44 = 0x44,
  POSITIVE_FIXNUM_0x45 = 0x45,
  POSITIVE_FIXNUM_0x46 = 0x46,
  POSITIVE_FIXNUM_0x47 = 0x47,
  POSITIVE_FIXNUM_0x48 = 0x48,
  POSITIVE_FIXNUM_0x49 = 0x49,
  POSITIVE_FIXNUM_0x4A = 0x4A,
  POSITIVE_FIXNUM_0x4B = 0x4B,
  POSITIVE_FIXNUM_0x4C = 0x4C,
  POSITIVE_FIXNUM_0x4D = 0x4D,
  POSITIVE_FIXNUM_0x4E = 0x4E,
  POSITIVE_FIXNUM_0x4F = 0x4F,

  POSITIVE_FIXNUM_0x50 = 0x50,
  POSITIVE_FIXNUM_0x51 = 0x51,
  POSITIVE_FIXNUM_0x52 = 0x52,
  POSITIVE_FIXNUM_0x53 = 0x53,
  POSITIVE_FIXNUM_0x54 = 0x54,
  POSITIVE_FIXNUM_0x55 = 0x55,
  POSITIVE_FIXNUM_0x56 = 0x56,
  POSITIVE_FIXNUM_0x57 = 0x57,
  POSITIVE_FIXNUM_0x58 = 0x58,
  POSITIVE_FIXNUM_0x59 = 0x59,
  POSITIVE_FIXNUM_0x5A = 0x5A,
  POSITIVE_FIXNUM_0x5B = 0x5B,
  POSITIVE_FIXNUM_0x5C = 0x5C,
  POSITIVE_FIXNUM_0x5D = 0x5D,
  POSITIVE_FIXNUM_0x5E = 0x5E,
  POSITIVE_FIXNUM_0x5F = 0x5F,

  POSITIVE_FIXNUM_0x60 = 0x60,
  POSITIVE_FIXNUM_0x61 = 0x61,
  POSITIVE_FIXNUM_0x62 = 0x62,
  POSITIVE_FIXNUM_0x63 = 0x63,
  POSITIVE_FIXNUM_0x64 = 0x64,
  POSITIVE_FIXNUM_0x65 = 0x65,
  POSITIVE_FIXNUM_0x66 = 0x66,
  POSITIVE_FIXNUM_0x67 = 0x67,
  POSITIVE_FIXNUM_0x68 = 0x68,
  POSITIVE_FIXNUM_0x69 = 0x69,
  POSITIVE_FIXNUM_0x6A = 0x6A,
  POSITIVE_FIXNUM_0x6B = 0x6B,
  POSITIVE_FIXNUM_0x6C = 0x6C,
  POSITIVE_FIXNUM_0x6D = 0x6D,
  POSITIVE_FIXNUM_0x6E = 0x6E,
  POSITIVE_FIXNUM_0x6F = 0x6F,

  POSITIVE_FIXNUM_0x70 = 0x70,
  POSITIVE_FIXNUM_0x71 = 0x71,
  POSITIVE_FIXNUM_0x72 = 0x72,
  POSITIVE_FIXNUM_0x73 = 0x73,
  POSITIVE_FIXNUM_0x74 = 0x74,
  POSITIVE_FIXNUM_0x75 = 0x75,
  POSITIVE_FIXNUM_0x76 = 0x76,
  POSITIVE_FIXNUM_0x77 = 0x77,
  POSITIVE_FIXNUM_0x78 = 0x78,
  POSITIVE_FIXNUM_0x79 = 0x79,
  POSITIVE_FIXNUM_0x7A = 0x7A,
  POSITIVE_FIXNUM_0x7B = 0x7B,
  POSITIVE_FIXNUM_0x7C = 0x7C,
  POSITIVE_FIXNUM_0x7D = 0x7D,
  POSITIVE_FIXNUM_0x7E = 0x7E,
  POSITIVE_FIXNUM_0x7F = 0x7F,
#pragma endregion

#pragma region FIX_MAP 0x80 - 0x8F
  FIX_MAP = 0x80,
  FIX_MAP_0x1 = 0x81,
  FIX_MAP_0x2 = 0x82,
  FIX_MAP_0x3 = 0x83,
  FIX_MAP_0x4 = 0x84,
  FIX_MAP_0x5 = 0x85,
  FIX_MAP_0x6 = 0x86,
  FIX_MAP_0x7 = 0x87,
  FIX_MAP_0x8 = 0x88,
  FIX_MAP_0x9 = 0x89,
  FIX_MAP_0xA = 0x8A,
  FIX_MAP_0xB = 0x8B,
  FIX_MAP_0xC = 0x8C,
  FIX_MAP_0xD = 0x8D,
  FIX_MAP_0xE = 0x8E,
  FIX_MAP_0xF = 0x8F,
#pragma endregion

#pragma region FIX_ARRAY 0x90 - 0x9F
  FIX_ARRAY = 0x90,
  FIX_ARRAY_0x1 = 0x91,
  FIX_ARRAY_0x2 = 0x92,
  FIX_ARRAY_0x3 = 0x93,
  FIX_ARRAY_0x4 = 0x94,
  FIX_ARRAY_0x5 = 0x95,
  FIX_ARRAY_0x6 = 0x96,
  FIX_ARRAY_0x7 = 0x97,
  FIX_ARRAY_0x8 = 0x98,
  FIX_ARRAY_0x9 = 0x99,
  FIX_ARRAY_0xA = 0x9A,
  FIX_ARRAY_0xB = 0x9B,
  FIX_ARRAY_0xC = 0x9C,
  FIX_ARRAY_0xD = 0x9D,
  FIX_ARRAY_0xE = 0x9E,
  FIX_ARRAY_0xF = 0x9F,
#pragma endregion

#pragma region FIX_STR 0xA0 - 0xBF
  FIX_STR = 0xA0,
  FIX_STR_0x01 = 0xA1,
  FIX_STR_0x02 = 0xA2,
  FIX_STR_0x03 = 0xA3,
  FIX_STR_0x04 = 0xA4,
  FIX_STR_0x05 = 0xA5,
  FIX_STR_0x06 = 0xA6,
  FIX_STR_0x07 = 0xA7,
  FIX_STR_0x08 = 0xA8,
  FIX_STR_0x09 = 0xA9,
  FIX_STR_0x0A = 0xAA,
  FIX_STR_0x0B = 0xAB,
  FIX_STR_0x0C = 0xAC,
  FIX_STR_0x0D = 0xAD,
  FIX_STR_0x0E = 0xAE,
  FIX_STR_0x0F = 0xAF,
  FIX_STR_0x10 = 0xB0,
  FIX_STR_0x11 = 0xB1,
  FIX_STR_0x12 = 0xB2,
  FIX_STR_0x13 = 0xB3,
  FIX_STR_0x14 = 0xB4,
  FIX_STR_0x15 = 0xB5,
  FIX_STR_0x16 = 0xB6,
  FIX_STR_0x17 = 0xB7,
  FIX_STR_0x18 = 0xB8,
  FIX_STR_0x19 = 0xB9,
  FIX_STR_0x1A = 0xBA,
  FIX_STR_0x1B = 0xBB,
  FIX_STR_0x1C = 0xBC,
  FIX_STR_0x1D = 0xBD,
  FIX_STR_0x1E = 0xBE,
  FIX_STR_0x1F = 0xBF,
#pragma endregion

  NIL = 0xC0,
  NEVER_USED = 0xC1,
  False = 0xC2, // avoid match windows False
  True = 0xC3,  // avoid match windows True

  BIN8 = 0xC4,
  BIN16 = 0xC5,
  BIN32 = 0xC6,

  EXT8 = 0xC7,
  EXT16 = 0xC8,
  EXT32 = 0xC9,

  FLOAT = 0xCA,
  DOUBLE = 0xCB,
  UINT8 = 0xCC,
  UINT16 = 0xCD,
  UINT32 = 0xCE,
  UINT64 = 0xCF,
  INT8 = 0xD0,
  INT16 = 0xD1,
  INT32 = 0xD2,
  INT64 = 0xD3,

  FIX_EXT_1 = 0xD4,
  FIX_EXT_2 = 0xD5,
  FIX_EXT_4 = 0xD6,
  FIX_EXT_8 = 0xD7,
  FIX_EXT_16 = 0xD8,

  STR8 = 0xD9,
  STR16 = 0xDA,
  STR32 = 0xDB,

  ARRAY16 = 0xDC,
  ARRAY32 = 0xDD,
  MAP16 = 0xDE,
  MAP32 = 0xDF,

#pragma region NEGATIVE_FIXNUM 0xE0 - 0xFF
  NEGATIVE_FIXNUM = 0xE0,      // 1110 0000 = -32
  NEGATIVE_FIXNUM_0x1F = 0xE1, // -31
  NEGATIVE_FIXNUM_0x1E = 0xE2,
  NEGATIVE_FIXNUM_0x1D = 0xE3,
  NEGATIVE_FIXNUM_0x1C = 0xE4,
  NEGATIVE_FIXNUM_0x1B = 0xE5,
  NEGATIVE_FIXNUM_0x1A = 0xE6,
  NEGATIVE_FIXNUM_0x19 = 0xE7,
  NEGATIVE_FIXNUM_0x18 = 0xE8,
  NEGATIVE_FIXNUM_0x17 = 0xE9,
  NEGATIVE_FIXNUM_0x16 = 0xEA,
  NEGATIVE_FIXNUM_0x15 = 0xEB,
  NEGATIVE_FIXNUM_0x14 = 0xEC,
  NEGATIVE_FIXNUM_0x13 = 0xED,
  NEGATIVE_FIXNUM_0x12 = 0xEE,
  NEGATIVE_FIXNUM_0x11 = 0xEF,
  NEGATIVE_FIXNUM_0x10 = 0xF0,
  NEGATIVE_FIXNUM_0x0F = 0xF1,
  NEGATIVE_FIXNUM_0x0E = 0xF2,
  NEGATIVE_FIXNUM_0x0D = 0xF3,
  NEGATIVE_FIXNUM_0x0C = 0xF4,
  NEGATIVE_FIXNUM_0x0B = 0xF5,
  NEGATIVE_FIXNUM_0x0A = 0xF6,
  NEGATIVE_FIXNUM_0x09 = 0xF7,
  NEGATIVE_FIXNUM_0x08 = 0xF8,
  NEGATIVE_FIXNUM_0x07 = 0xF9,
  NEGATIVE_FIXNUM_0x06 = 0xFA,
  NEGATIVE_FIXNUM_0x05 = 0xFB,
  NEGATIVE_FIXNUM_0x04 = 0xFC,
  NEGATIVE_FIXNUM_0x03 = 0xFD,
  NEGATIVE_FIXNUM_0x02 = 0xFE,
  NEGATIVE_FIXNUM_0x01 = 0xFF, // -1
#pragma endregion
};

enum class parse_status {
  ok,
  empty,
  lack,
  invalid,
};

template <typename T> struct parse_result {
  parse_status status;
  T value;
  operator T() const { return value; }
  bool is_ok() const { return status == parse_status::ok; }
  template <typename R> parse_result<R> cast() const {
    return {status, static_cast<R>(value)};
  }
};
template <typename T> parse_result<T> OK(const T &value) {
  return {parse_status::ok, value};
}

inline parse_result<uint32_t> no_size_parse_error(const uint8_t *, int) {
  return {parse_status::invalid};
}

template <uint32_t n>
inline parse_result<uint32_t> return_n(const uint8_t *, int) {
  return OK(n);
}

template <typename T>
parse_result<T> body_number(const uint8_t *m_p, int m_size) {
  switch (sizeof(T)) {
  case 1:
    if (m_size < 1 + sizeof(T)) {
      return {parse_status::lack};
    }
    return OK(static_cast<T>(m_p[1]));
  case 2:
    if (m_size < 1 + sizeof(T)) {
      return {parse_status::lack};
    }
    return OK(static_cast<T>((m_p[1] << 8) | m_p[2]));
  case 4: {
    if (m_size < 1 + sizeof(T)) {
      return {parse_status::lack};
    }
    std::uint8_t buf[] = {m_p[4], m_p[3], m_p[2], m_p[1]};
    return OK(*reinterpret_cast<T *>(buf));
  }
  case 8: {
    if (m_size < 1 + sizeof(T)) {
      return {parse_status::lack};
    }
    std::uint8_t buf[] = {m_p[8], m_p[7], m_p[6], m_p[5],
                          m_p[4], m_p[3], m_p[2], m_p[1]};
    return OK(*reinterpret_cast<T *>(buf));
  }
  }

  return {parse_status::invalid};
}

struct body_index_and_size {
  int index;

  parse_result<uint32_t> (*size)(const uint8_t *p, int size);

  static constexpr parse_result<body_index_and_size> from_type(pack_type t) {
    switch (t) {
#pragma region POSITIVE_FIXNUM 0x00 - 0x7F
    case POSITIVE_FIXNUM:
    case POSITIVE_FIXNUM_0x01:
    case POSITIVE_FIXNUM_0x02:
    case POSITIVE_FIXNUM_0x03:
    case POSITIVE_FIXNUM_0x04:
    case POSITIVE_FIXNUM_0x05:
    case POSITIVE_FIXNUM_0x06:
    case POSITIVE_FIXNUM_0x07:
    case POSITIVE_FIXNUM_0x08:
    case POSITIVE_FIXNUM_0x09:
    case POSITIVE_FIXNUM_0x0A:
    case POSITIVE_FIXNUM_0x0B:
    case POSITIVE_FIXNUM_0x0C:
    case POSITIVE_FIXNUM_0x0D:
    case POSITIVE_FIXNUM_0x0E:
    case POSITIVE_FIXNUM_0x0F:

    case POSITIVE_FIXNUM_0x10:
    case POSITIVE_FIXNUM_0x11:
    case POSITIVE_FIXNUM_0x12:
    case POSITIVE_FIXNUM_0x13:
    case POSITIVE_FIXNUM_0x14:
    case POSITIVE_FIXNUM_0x15:
    case POSITIVE_FIXNUM_0x16:
    case POSITIVE_FIXNUM_0x17:
    case POSITIVE_FIXNUM_0x18:
    case POSITIVE_FIXNUM_0x19:
    case POSITIVE_FIXNUM_0x1A:
    case POSITIVE_FIXNUM_0x1B:
    case POSITIVE_FIXNUM_0x1C:
    case POSITIVE_FIXNUM_0x1D:
    case POSITIVE_FIXNUM_0x1E:
    case POSITIVE_FIXNUM_0x1F:

    case POSITIVE_FIXNUM_0x20:
    case POSITIVE_FIXNUM_0x21:
    case POSITIVE_FIXNUM_0x22:
    case POSITIVE_FIXNUM_0x23:
    case POSITIVE_FIXNUM_0x24:
    case POSITIVE_FIXNUM_0x25:
    case POSITIVE_FIXNUM_0x26:
    case POSITIVE_FIXNUM_0x27:
    case POSITIVE_FIXNUM_0x28:
    case POSITIVE_FIXNUM_0x29:
    case POSITIVE_FIXNUM_0x2A:
    case POSITIVE_FIXNUM_0x2B:
    case POSITIVE_FIXNUM_0x2C:
    case POSITIVE_FIXNUM_0x2D:
    case POSITIVE_FIXNUM_0x2E:
    case POSITIVE_FIXNUM_0x2F:

    case POSITIVE_FIXNUM_0x30:
    case POSITIVE_FIXNUM_0x31:
    case POSITIVE_FIXNUM_0x32:
    case POSITIVE_FIXNUM_0x33:
    case POSITIVE_FIXNUM_0x34:
    case POSITIVE_FIXNUM_0x35:
    case POSITIVE_FIXNUM_0x36:
    case POSITIVE_FIXNUM_0x37:
    case POSITIVE_FIXNUM_0x38:
    case POSITIVE_FIXNUM_0x39:
    case POSITIVE_FIXNUM_0x3A:
    case POSITIVE_FIXNUM_0x3B:
    case POSITIVE_FIXNUM_0x3C:
    case POSITIVE_FIXNUM_0x3D:
    case POSITIVE_FIXNUM_0x3E:
    case POSITIVE_FIXNUM_0x3F:

    case POSITIVE_FIXNUM_0x40:
    case POSITIVE_FIXNUM_0x41:
    case POSITIVE_FIXNUM_0x42:
    case POSITIVE_FIXNUM_0x43:
    case POSITIVE_FIXNUM_0x44:
    case POSITIVE_FIXNUM_0x45:
    case POSITIVE_FIXNUM_0x46:
    case POSITIVE_FIXNUM_0x47:
    case POSITIVE_FIXNUM_0x48:
    case POSITIVE_FIXNUM_0x49:
    case POSITIVE_FIXNUM_0x4A:
    case POSITIVE_FIXNUM_0x4B:
    case POSITIVE_FIXNUM_0x4C:
    case POSITIVE_FIXNUM_0x4D:
    case POSITIVE_FIXNUM_0x4E:
    case POSITIVE_FIXNUM_0x4F:

    case POSITIVE_FIXNUM_0x50:
    case POSITIVE_FIXNUM_0x51:
    case POSITIVE_FIXNUM_0x52:
    case POSITIVE_FIXNUM_0x53:
    case POSITIVE_FIXNUM_0x54:
    case POSITIVE_FIXNUM_0x55:
    case POSITIVE_FIXNUM_0x56:
    case POSITIVE_FIXNUM_0x57:
    case POSITIVE_FIXNUM_0x58:
    case POSITIVE_FIXNUM_0x59:
    case POSITIVE_FIXNUM_0x5A:
    case POSITIVE_FIXNUM_0x5B:
    case POSITIVE_FIXNUM_0x5C:
    case POSITIVE_FIXNUM_0x5D:
    case POSITIVE_FIXNUM_0x5E:
    case POSITIVE_FIXNUM_0x5F:

    case POSITIVE_FIXNUM_0x60:
    case POSITIVE_FIXNUM_0x61:
    case POSITIVE_FIXNUM_0x62:
    case POSITIVE_FIXNUM_0x63:
    case POSITIVE_FIXNUM_0x64:
    case POSITIVE_FIXNUM_0x65:
    case POSITIVE_FIXNUM_0x66:
    case POSITIVE_FIXNUM_0x67:
    case POSITIVE_FIXNUM_0x68:
    case POSITIVE_FIXNUM_0x69:
    case POSITIVE_FIXNUM_0x6A:
    case POSITIVE_FIXNUM_0x6B:
    case POSITIVE_FIXNUM_0x6C:
    case POSITIVE_FIXNUM_0x6D:
    case POSITIVE_FIXNUM_0x6E:
    case POSITIVE_FIXNUM_0x6F:

    case POSITIVE_FIXNUM_0x70:
    case POSITIVE_FIXNUM_0x71:
    case POSITIVE_FIXNUM_0x72:
    case POSITIVE_FIXNUM_0x73:
    case POSITIVE_FIXNUM_0x74:
    case POSITIVE_FIXNUM_0x75:
    case POSITIVE_FIXNUM_0x76:
    case POSITIVE_FIXNUM_0x77:
    case POSITIVE_FIXNUM_0x78:
    case POSITIVE_FIXNUM_0x79:
    case POSITIVE_FIXNUM_0x7A:
    case POSITIVE_FIXNUM_0x7B:
    case POSITIVE_FIXNUM_0x7C:
    case POSITIVE_FIXNUM_0x7D:
    case POSITIVE_FIXNUM_0x7E:
    case POSITIVE_FIXNUM_0x7F:
      return OK(body_index_and_size{1, return_n<0>});
#pragma endregion

#pragma region FIX_MAP 0x80 - 0x8F
    case FIX_MAP:
    case FIX_MAP_0x1:
    case FIX_MAP_0x2:
    case FIX_MAP_0x3:
    case FIX_MAP_0x4:
    case FIX_MAP_0x5:
    case FIX_MAP_0x6:
    case FIX_MAP_0x7:
    case FIX_MAP_0x8:
    case FIX_MAP_0x9:
    case FIX_MAP_0xA:
    case FIX_MAP_0xB:
    case FIX_MAP_0xC:
    case FIX_MAP_0xD:
    case FIX_MAP_0xE:
    case FIX_MAP_0xF:
      return OK(body_index_and_size{1, no_size_parse_error});
#pragma endregion

#pragma region FIX_ARRAY 0x90 - 0x9F
    case FIX_ARRAY:
    case FIX_ARRAY_0x1:
    case FIX_ARRAY_0x2:
    case FIX_ARRAY_0x3:
    case FIX_ARRAY_0x4:
    case FIX_ARRAY_0x5:
    case FIX_ARRAY_0x6:
    case FIX_ARRAY_0x7:
    case FIX_ARRAY_0x8:
    case FIX_ARRAY_0x9:
    case FIX_ARRAY_0xA:
    case FIX_ARRAY_0xB:
    case FIX_ARRAY_0xC:
    case FIX_ARRAY_0xD:
    case FIX_ARRAY_0xE:
    case FIX_ARRAY_0xF:
      return OK(body_index_and_size{1, no_size_parse_error});
#pragma endregion

#pragma region FIX_STR 0xA0 - 0xBF
    case FIX_STR:
      return OK(body_index_and_size{1, return_n<0>});
    case FIX_STR_0x01:
      return OK(body_index_and_size{1, return_n<1>});
    case FIX_STR_0x02:
      return OK(body_index_and_size{1, return_n<2>});
    case FIX_STR_0x03:
      return OK(body_index_and_size{1, return_n<3>});
    case FIX_STR_0x04:
      return OK(body_index_and_size{1, return_n<4>});
    case FIX_STR_0x05:
      return OK(body_index_and_size{1, return_n<5>});
    case FIX_STR_0x06:
      return OK(body_index_and_size{1, return_n<6>});
    case FIX_STR_0x07:
      return OK(body_index_and_size{1, return_n<7>});
    case FIX_STR_0x08:
      return OK(body_index_and_size{1, return_n<8>});
    case FIX_STR_0x09:
      return OK(body_index_and_size{1, return_n<9>});
    case FIX_STR_0x0A:
      return OK(body_index_and_size{1, return_n<10>});
    case FIX_STR_0x0B:
      return OK(body_index_and_size{1, return_n<11>});
    case FIX_STR_0x0C:
      return OK(body_index_and_size{1, return_n<12>});
    case FIX_STR_0x0D:
      return OK(body_index_and_size{1, return_n<13>});
    case FIX_STR_0x0E:
      return OK(body_index_and_size{1, return_n<14>});
    case FIX_STR_0x0F:
      return OK(body_index_and_size{1, return_n<15>});
    case FIX_STR_0x10:
      return OK(body_index_and_size{1, return_n<16>});
    case FIX_STR_0x11:
      return OK(body_index_and_size{1, return_n<17>});
    case FIX_STR_0x12:
      return OK(body_index_and_size{1, return_n<18>});
    case FIX_STR_0x13:
      return OK(body_index_and_size{1, return_n<19>});
    case FIX_STR_0x14:
      return OK(body_index_and_size{1, return_n<20>});
    case FIX_STR_0x15:
      return OK(body_index_and_size{1, return_n<21>});
    case FIX_STR_0x16:
      return OK(body_index_and_size{1, return_n<22>});
    case FIX_STR_0x17:
      return OK(body_index_and_size{1, return_n<23>});
    case FIX_STR_0x18:
      return OK(body_index_and_size{1, return_n<24>});
    case FIX_STR_0x19:
      return OK(body_index_and_size{1, return_n<25>});
    case FIX_STR_0x1A:
      return OK(body_index_and_size{1, return_n<26>});
    case FIX_STR_0x1B:
      return OK(body_index_and_size{1, return_n<27>});
    case FIX_STR_0x1C:
      return OK(body_index_and_size{1, return_n<28>});
    case FIX_STR_0x1D:
      return OK(body_index_and_size{1, return_n<29>});
    case FIX_STR_0x1E:
      return OK(body_index_and_size{1, return_n<30>});
    case FIX_STR_0x1F:
      return OK(body_index_and_size{1, return_n<31>});
#pragma endregion

    case NIL:
      return OK(body_index_and_size{1, return_n<0>});
    case NEVER_USED:
      return {parse_status::invalid};
    case False:
      return OK(body_index_and_size{1, return_n<0>});
    case True:
      return OK(body_index_and_size{1, return_n<0>});

    case BIN8:
      return OK(body_index_and_size{
          1 + 1, [](auto p, int size) {
            return body_number<uint8_t>(p, size).cast<uint32_t>();
          }});
    case BIN16:
      return OK(body_index_and_size{
          1 + 2, [](auto p, int size) {
            return body_number<uint16_t>(p, size).cast<uint32_t>();
          }});
    case BIN32:
      return OK(body_index_and_size{1 + 4, [](auto p, int size) {
                                      return body_number<uint32_t>(p, size);
                                    }});

    case EXT8:
      return OK(body_index_and_size{
          1 + 1 + 1, [](auto p, int size) {
            return body_number<uint8_t>(p, size).cast<uint32_t>();
          }});
    case EXT16:
      return OK(body_index_and_size{
          1 + 2 + 1, [](auto p, int size) {
            return body_number<uint16_t>(p, size).cast<uint32_t>();
          }});
    case EXT32:
      return OK(body_index_and_size{1 + 4 + 1, [](auto p, int size) {
                                      return body_number<uint32_t>(p, size);
                                    }});

    case FLOAT:
      return OK(body_index_and_size{1, return_n<4>});
    case DOUBLE:
      return OK(body_index_and_size{1, return_n<8>});
    case UINT8:
      return OK(body_index_and_size{1, return_n<1>});
    case UINT16:
      return OK(body_index_and_size{1, return_n<2>});
    case UINT32:
      return OK(body_index_and_size{1, return_n<4>});
    case UINT64:
      return OK(body_index_and_size{1, return_n<8>});
    case INT8:
      return OK(body_index_and_size{1, return_n<1>});
    case INT16:
      return OK(body_index_and_size{1, return_n<2>});
    case INT32:
      return OK(body_index_and_size{1, return_n<4>});
    case INT64:
      return OK(body_index_and_size{1, return_n<8>});

    case FIX_EXT_1:
      return OK(body_index_and_size{1 + 1, return_n<1>});
    case FIX_EXT_2:
      return OK(body_index_and_size{1 + 1, return_n<2>});
    case FIX_EXT_4:
      return OK(body_index_and_size{1 + 1, return_n<4>});
    case FIX_EXT_8:
      return OK(body_index_and_size{1 + 1, return_n<8>});
    case FIX_EXT_16:
      return OK(body_index_and_size{1 + 1, return_n<16>});

    case STR8:
      return OK(body_index_and_size{
          1 + 1, [](auto p, int size) {
            return body_number<uint8_t>(p, size).cast<uint32_t>();
          }});
    case STR16:
      return OK(body_index_and_size{
          1 + 2, [](auto p, int size) {
            return body_number<uint16_t>(p, size).cast<uint32_t>();
          }});
    case STR32:
      return OK(body_index_and_size{1 + 4, [](auto p, int size) {
                                      return body_number<uint32_t>(p, size);
                                    }});

    case ARRAY16:
      return OK(body_index_and_size{1 + 2, no_size_parse_error});
    case ARRAY32:
      return OK(body_index_and_size{1 + 4, no_size_parse_error});
    case MAP16:
      return OK(body_index_and_size{1 + 2, no_size_parse_error});
    case MAP32:
      return OK(body_index_and_size{1 + 4, no_size_parse_error});

#pragma region NEGATIVE_FIXNUM 0xE0 - 0xFF
    case NEGATIVE_FIXNUM:
    case NEGATIVE_FIXNUM_0x1F:
    case NEGATIVE_FIXNUM_0x1E:
    case NEGATIVE_FIXNUM_0x1D:
    case NEGATIVE_FIXNUM_0x1C:
    case NEGATIVE_FIXNUM_0x1B:
    case NEGATIVE_FIXNUM_0x1A:
    case NEGATIVE_FIXNUM_0x19:
    case NEGATIVE_FIXNUM_0x18:
    case NEGATIVE_FIXNUM_0x17:
    case NEGATIVE_FIXNUM_0x16:
    case NEGATIVE_FIXNUM_0x15:
    case NEGATIVE_FIXNUM_0x14:
    case NEGATIVE_FIXNUM_0x13:
    case NEGATIVE_FIXNUM_0x12:
    case NEGATIVE_FIXNUM_0x11:
    case NEGATIVE_FIXNUM_0x10:
    case NEGATIVE_FIXNUM_0x0F:
    case NEGATIVE_FIXNUM_0x0E:
    case NEGATIVE_FIXNUM_0x0D:
    case NEGATIVE_FIXNUM_0x0C:
    case NEGATIVE_FIXNUM_0x0B:
    case NEGATIVE_FIXNUM_0x0A:
    case NEGATIVE_FIXNUM_0x09:
    case NEGATIVE_FIXNUM_0x08:
    case NEGATIVE_FIXNUM_0x07:
    case NEGATIVE_FIXNUM_0x06:
    case NEGATIVE_FIXNUM_0x05:
    case NEGATIVE_FIXNUM_0x04:
    case NEGATIVE_FIXNUM_0x03:
    case NEGATIVE_FIXNUM_0x02:
    case NEGATIVE_FIXNUM_0x01:
      return OK(body_index_and_size{1, 0});
#pragma endregion

    default:
      return {parse_status::invalid};
    }
  }
};

class packer {
  typedef std::vector<std::uint8_t> buffer;
  std::shared_ptr<buffer> m_buffer;

private:
  template <typename T> void push_number_reverse(T n) {
    auto size = sizeof(T);
    auto _p = reinterpret_cast<std::uint8_t *>(&n) + (size - 1);
    for (size_t i = 0; i < size; ++i, --_p) {
      m_buffer->push_back(*_p);
    }
  }

public:
  packer() : m_buffer(new buffer) {}

  packer(const std::shared_ptr<buffer> &buffer) : m_buffer(buffer) {}

  packer(const packer &) = delete;
  packer &operator=(const packer &) = delete;

  template <class Range> void push(const Range &r) {
    m_buffer->insert(m_buffer->end(), std::begin(r), std::end(r));
  }

  packer &pack_nil() {
    m_buffer->push_back(pack_type::NIL);
    return *this;
  }

  template <typename T> packer &pack_integer(T n) {
    if (n < 0) {
      if (n >= -32) {
        m_buffer->push_back(pack_type::NEGATIVE_FIXNUM |
                            static_cast<std::uint8_t>((n + 32)));
      } else if (n >= std::numeric_limits<std::int8_t>::min()) {
        m_buffer->push_back(pack_type::INT8);
        m_buffer->push_back(n);
      } else if (n >= std::numeric_limits<std::int16_t>::min()) {
        m_buffer->push_back(pack_type::INT16);
        // network byteorder
        push_number_reverse(static_cast<std::int16_t>(n));
      } else if (n >= std::numeric_limits<std::int32_t>::min()) {
        m_buffer->push_back(pack_type::INT32);
        // network byteorder
        push_number_reverse(static_cast<std::int32_t>(n));
      } else if (n >= std::numeric_limits<std::int64_t>::min()) {
        m_buffer->push_back(pack_type::INT64);
        // network byteorder
        push_number_reverse(static_cast<std::int64_t>(n));
      } else {
        throw underflow_pack_error();
      }
    } else {
      if (n <= 0x7F) {
        m_buffer->push_back(static_cast<std::uint8_t>(n));
      } else if (n <= std::numeric_limits<std::uint8_t>::max()) {
        m_buffer->push_back(pack_type::UINT8);
        m_buffer->push_back(static_cast<std::uint8_t>(n));
      } else if (n <= std::numeric_limits<std::uint16_t>::max()) {
        m_buffer->push_back(pack_type::UINT16);
        // network byteorder
        push_number_reverse(static_cast<std::uint16_t>(n));
      } else if (n <= std::numeric_limits<std::uint32_t>::max()) {
        m_buffer->push_back(pack_type::UINT32);
        // network byteorder
        push_number_reverse(static_cast<std::uint32_t>(n));
      } else if (n <= std::numeric_limits<std::uint64_t>::max()) {
        m_buffer->push_back(pack_type::UINT64);
        // network byteorder
        push_number_reverse(static_cast<std::uint64_t>(n));
      } else {
        throw overflow_pack_error();
      }
    }
    return *this;
  }

  packer &pack_float(float n) {
    m_buffer->push_back(pack_type::FLOAT);
    push_number_reverse(n);
    return *this;
  }

  packer &pack_double(double n) {
    m_buffer->push_back(pack_type::DOUBLE);
    push_number_reverse(n);
    return *this;
  }

  packer &pack_bool(bool isTrue) {
    if (isTrue) {
      m_buffer->push_back(pack_type::True);
    } else {
      m_buffer->push_back(pack_type::False);
    }
    return *this;
  }

  packer &pack_str(const char *str) { return pack_str(std::string(str)); }

  template <class Range> packer &pack_str(const Range &r) {
    auto size = static_cast<size_t>(std::distance(std::begin(r), std::end(r)));
    if (size < 32) {
      m_buffer->push_back(pack_type::FIX_STR | static_cast<std::uint8_t>(size));
      push(r);
    } else if (size <= std::numeric_limits<std::uint8_t>::max()) {
      m_buffer->push_back(pack_type::STR8);
      m_buffer->push_back(static_cast<std::uint8_t>(size));
      push(r);
    } else if (size <= std::numeric_limits<std::uint16_t>::max()) {
      m_buffer->push_back(pack_type::STR16);
      push_number_reverse(static_cast<std::uint16_t>(size));
      push(r);
    } else if (size <= std::numeric_limits<std::uint32_t>::max()) {
      m_buffer->push_back(pack_type::STR32);
      push_number_reverse(static_cast<std::uint32_t>(size));
      push(r);
    } else {
      throw overflow_pack_error();
    }
    return *this;
  }

  template <class Range> packer &pack_bin(const Range &r) {
    auto size = static_cast<size_t>(std::distance(std::begin(r), std::end(r)));
    if (size <= std::numeric_limits<std::uint8_t>::max()) {
      m_buffer->push_back(pack_type::BIN8);
      m_buffer->push_back(static_cast<std::uint8_t>(size));
      push(r);
    } else if (size <= std::numeric_limits<std::uint16_t>::max()) {
      m_buffer->push_back(pack_type::BIN16);
      push_number_reverse(static_cast<std::uint16_t>(size));
      push(r);
    } else if (size <= std::numeric_limits<std::uint32_t>::max()) {
      m_buffer->push_back(pack_type::BIN32);
      push_number_reverse(static_cast<std::uint32_t>(size));
      push(r);
    } else {
      throw overflow_pack_error();
    }
    return *this;
  }

  packer &pack_array(size_t n) {
    if (n <= 15) {
      m_buffer->push_back(pack_type::FIX_ARRAY | static_cast<std::uint8_t>(n));
    } else if (n <= std::numeric_limits<std::uint16_t>::max()) {
      m_buffer->push_back(pack_type::ARRAY16);
      push_number_reverse(static_cast<std::uint16_t>(n));
    } else if (n <= std::numeric_limits<std::uint32_t>::max()) {
      m_buffer->push_back(pack_type::ARRAY32);
      push_number_reverse(static_cast<std::uint32_t>(n));
    } else {
      throw overflow_pack_error();
    }
    return *this;
  }

  packer &pack_map(size_t n) {
    if (n <= 15) {
      m_buffer->push_back(pack_type::FIX_MAP | static_cast<std::uint8_t>(n));
    } else if (n <= std::numeric_limits<std::uint16_t>::max()) {
      m_buffer->push_back(pack_type::MAP16);
      push_number_reverse(static_cast<std::uint16_t>(n));
    } else if (n <= std::numeric_limits<std::uint32_t>::max()) {
      m_buffer->push_back(pack_type::MAP32);
      push_number_reverse(static_cast<std::uint32_t>(n));
    } else {
      throw overflow_pack_error();
    }
    return *this;
  }

  template <class Range> packer &pack_ext(char type, const Range &r) {
    auto size = static_cast<size_t>(std::distance(std::begin(r), std::end(r)));

    // FIXEXT
    switch (size) {
    case 1:
      m_buffer->push_back(pack_type::FIX_EXT_1);
      m_buffer->push_back(type);
      push(r);
      break;

    case 2:
      m_buffer->push_back(pack_type::FIX_EXT_2);
      m_buffer->push_back(type);
      push(r);
      break;

    case 4:
      m_buffer->push_back(pack_type::FIX_EXT_4);
      m_buffer->push_back(type);
      push(r);
      break;

    case 8:
      m_buffer->push_back(pack_type::FIX_EXT_8);
      m_buffer->push_back(type);
      push(r);
      break;

    case 16:
      m_buffer->push_back(pack_type::FIX_EXT_16);
      m_buffer->push_back(type);
      push(r);
      break;

    default:
      // EXT
      if (size <= std::numeric_limits<std::uint8_t>::max()) {
        m_buffer->push_back(pack_type::EXT8);
        m_buffer->push_back(static_cast<std::uint8_t>(size));
        m_buffer->push_back(type);
        push(r);
      } else if (size <= std::numeric_limits<std::uint16_t>::max()) {
        m_buffer->push_back(pack_type::EXT16);
        push_number_reverse(static_cast<std::uint16_t>(size));
        m_buffer->push_back(type);
        push(r);
      } else if (size <= std::numeric_limits<std::uint32_t>::max()) {
        m_buffer->push_back(pack_type::EXT32);
        push_number_reverse(static_cast<std::uint32_t>(size));
        m_buffer->push_back(type);
        push(r);
      } else {
        throw overflow_pack_error();
      }
    }

    return *this;
  }

  const buffer &get_payload() const { return *m_buffer; }
};

class parser {
  const std::uint8_t *m_p = nullptr;
  inline parse_result<pack_type> header() const {
    if (m_size < 1) {
      return {parse_status::empty};
    }
    return OK(static_cast<pack_type>(m_p[0]));
  }
  int m_size = -1;

public:
  parser() {}

  parser(const std::vector<std::uint8_t> &v)
      : m_p(v.data()), m_size(static_cast<int>(v.size())) {
    if (m_size < 0) {
      throw lack_parse_error();
    }
  }

  parser(const std::uint8_t *p, int size) : m_p(p), m_size(size) {
    if (m_size < 0) {
      throw lack_parse_error();
    }
  }

  const uint8_t *data() const { return m_p; }

  int consumed_size() const {
    auto n = next().value;
    return n.m_p - m_p;
  }

  std::vector<uint8_t> copy_bytes() const {
    auto p = data();
    return std::vector<uint8_t>(p, p + consumed_size());
  }

  std::string to_json() const {
    std::stringstream ss;
    to_json(ss);
    return ss.str();
  }

  void to_json(std::ostream &os) const {
    if (is_array()) {

      os << '[';

      auto item_count = count();
      auto type = header();
      auto offset = body_index_and_size::from_type(type).value.index;
      auto current = parser(m_p + offset, m_size - offset);
      for (uint8_t i = 0; i < item_count; ++i) {
        if (i > 0) {
          os << ',';
        }

        current.to_json(os);

        current = current.next();
      }

      os << ']';

    } else if (is_map()) {

      os << '{';

      auto item_count = count();
      auto type = header();
      auto offset = body_index_and_size::from_type(type).value.index;
      auto current = parser(m_p + offset, m_size - offset);
      for (uint8_t i = 0; i < item_count; ++i) {
        if (i > 0) {
          os << ',';
        }

        // key
        current.to_json(os);
        current = current.next();

        os << ':';

        // value
        current.to_json(os);
        current = current.next();
      }

      os << '}';

    } else {

      if (is_nil()) {

        os << "null";

      } else if (is_bool()) {
        if (get_bool()) {
          os << "true";
        } else {
          os << "false";
        }
      } else if (is_number()) {
        os << get_number<double>();
      } else if (is_string()) {
        os << '"' << get_string() << '"';
      } else if (is_binary()) {
        auto type = header();
        auto body = body_index_and_size::from_type(type).value;
        os << "[bin:" << body.size(m_p, m_size).value << "bytes]";
      } else {
        throw invalid_parse_error();
      }
    }
  }

#pragma region leaf
  parser get_bool(bool &value) const {
    auto type = header();
    if (type == pack_type::True)
      value = true;
    else if (type == pack_type::False)
      value = false;
    else
      throw type_parse_error();
    return advance(1);
  }

  bool get_bool() const {
    bool value;
    get_bool(value);
    return value;
  }

private:
  parser get_string(std::string_view &value, size_t offset, size_t size) const {
    auto head = m_p + offset;
    value = std::string_view((char *)head, size);
    return advance(offset + size);
  }

public:
  parser get_string(std::string_view &value) const {
    auto type = header();
    switch (type) {
    case pack_type::STR32:
      return get_string(value, 1 + 4, body_number<std::uint32_t>(m_p, m_size));
    case pack_type::STR16:
      return get_string(value, 1 + 2, body_number<std::uint16_t>(m_p, m_size));
    case pack_type::STR8:
      return get_string(value, 1 + 1, body_number<std::uint8_t>(m_p, m_size));
    case pack_type::FIX_STR:
      return get_string(value, 1, 0);
    case pack_type::FIX_STR_0x01:
      return get_string(value, 1, 1);
    case pack_type::FIX_STR_0x02:
      return get_string(value, 1, 2);
    case pack_type::FIX_STR_0x03:
      return get_string(value, 1, 3);
    case pack_type::FIX_STR_0x04:
      return get_string(value, 1, 4);
    case pack_type::FIX_STR_0x05:
      return get_string(value, 1, 5);
    case pack_type::FIX_STR_0x06:
      return get_string(value, 1, 6);
    case pack_type::FIX_STR_0x07:
      return get_string(value, 1, 7);
    case pack_type::FIX_STR_0x08:
      return get_string(value, 1, 8);
    case pack_type::FIX_STR_0x09:
      return get_string(value, 1, 9);
    case pack_type::FIX_STR_0x0A:
      return get_string(value, 1, 10);
    case pack_type::FIX_STR_0x0B:
      return get_string(value, 1, 11);
    case pack_type::FIX_STR_0x0C:
      return get_string(value, 1, 12);
    case pack_type::FIX_STR_0x0D:
      return get_string(value, 1, 13);
    case pack_type::FIX_STR_0x0E:
      return get_string(value, 1, 14);
    case pack_type::FIX_STR_0x0F:
      return get_string(value, 1, 15);
    case pack_type::FIX_STR_0x10:
      return get_string(value, 1, 16);
    case pack_type::FIX_STR_0x11:
      return get_string(value, 1, 17);
    case pack_type::FIX_STR_0x12:
      return get_string(value, 1, 18);
    case pack_type::FIX_STR_0x13:
      return get_string(value, 1, 19);
    case pack_type::FIX_STR_0x14:
      return get_string(value, 1, 20);
    case pack_type::FIX_STR_0x15:
      return get_string(value, 1, 21);
    case pack_type::FIX_STR_0x16:
      return get_string(value, 1, 22);
    case pack_type::FIX_STR_0x17:
      return get_string(value, 1, 23);
    case pack_type::FIX_STR_0x18:
      return get_string(value, 1, 24);
    case pack_type::FIX_STR_0x19:
      return get_string(value, 1, 25);
    case pack_type::FIX_STR_0x1A:
      return get_string(value, 1, 26);
    case pack_type::FIX_STR_0x1B:
      return get_string(value, 1, 27);
    case pack_type::FIX_STR_0x1C:
      return get_string(value, 1, 28);
    case pack_type::FIX_STR_0x1D:
      return get_string(value, 1, 29);
    case pack_type::FIX_STR_0x1E:
      return get_string(value, 1, 30);
    case pack_type::FIX_STR_0x1F:
      return get_string(value, 1, 31);
    }

    throw type_parse_error();
  }

  std::string_view get_string() const {
    std::string_view value;
    get_string(value);
    return value;
  }

  parser get_binary_view(std::string_view &value) const {
    auto type = header();
    switch (type) {
    case pack_type::BIN32: {
      auto begin = m_p + 1 + 4;
      auto n = body_number<std::uint32_t>(m_p, m_size);
      value = std::string_view((char *)begin, n);
      return advance(1 + 4 + n);
    }

    case pack_type::BIN16: {
      auto begin = m_p + 1 + 2;
      auto n = body_number<std::uint16_t>(m_p, m_size);
      value = std::string_view((char *)begin, n);
      return advance(1 + 2 + n);
    }
    case pack_type::BIN8: {
      auto begin = m_p + 1 + 1;
      auto n = body_number<std::uint8_t>(m_p, m_size);
      value = std::string_view((char *)begin, n);
      return advance(1 + 1 + n);
    }

    case pack_type::EXT32: {
      auto begin = m_p + 2 + 4;
      auto n = body_number<std::uint32_t>(m_p, m_size);
      value = std::string_view((char *)begin, n);
      return advance(2 + 4 + n);
    }

    case pack_type::EXT16: {
      auto begin = m_p + 2 + 2;
      auto n = body_number<std::uint16_t>(m_p, m_size);
      value = std::string_view((char *)begin, n);
      return advance(2 + 2 + n);
    }
    case pack_type::EXT8: {
      auto begin = m_p + 2 + 1;
      auto n = body_number<std::uint8_t>(m_p, m_size);
      value = std::string_view((char *)begin, n);
      return advance(2 + 1 + n);
    }

    case pack_type::FIX_EXT_1: {
      auto begin = m_p + 1 + 1;
      auto n = 1;
      value = std::string_view((char *)begin, n);
      return advance(1 + 1 + n);
    }

    case pack_type::FIX_EXT_2: {
      auto begin = m_p + 1 + 1;
      auto n = 2;
      value = std::string_view((char *)begin, n);
      return advance(1 + 1 + n);
    }

    case pack_type::FIX_EXT_4: {
      auto begin = m_p + 1 + 1;
      auto n = 4;
      value = std::string_view((char *)begin, n);
      return advance(1 + 1 + n);
    }

    case pack_type::FIX_EXT_8: {
      auto begin = m_p + 1 + 1;
      auto n = 8;
      value = std::string_view((char *)begin, n);
      return advance(1 + 1 + n);
    }

    case pack_type::FIX_EXT_16: {
      auto begin = m_p + 1 + 1;
      auto n = 16;
      value = std::string_view((char *)begin, n);
      return advance(1 + 1 + n);
    }
    }

    throw type_parse_error();
  }

  std::string_view get_binary_view() const {
    std::string_view bytes;
    get_binary_view(bytes);
    return bytes;
  }

  parser get_binary(std::vector<std::uint8_t> &value) const {
    std::string_view view;
    auto parser = get_binary_view(view);
    value = std::vector<std::uint8_t>(view.begin(), view.end());
    return parser;
  }

  std::vector<std::uint8_t> get_binary() const {
    auto view = get_binary_view();
    std::vector<std::uint8_t> bytes(view.begin(), view.end());
    return bytes;
  }

  std::tuple<char, std::string_view> get_ext() const {
    std::string_view bytes;
    char type;
    switch (header()) {
    case FIX_EXT_1:
    case FIX_EXT_2:
    case FIX_EXT_4:
    case FIX_EXT_8:
    case FIX_EXT_16:
      type = m_p[1];
      break;
    case EXT8:
      type = m_p[1 + 1];
      break;
    case EXT16:
      type = m_p[1 + 2];
      break;
    case EXT32:
      type = m_p[1 + 4];
      break;

    default:
      throw type_parse_error();
    }

    get_binary_view(bytes);
    return std::make_tuple(type, bytes);
  }

  parser advance(size_t n) const {
    return parser(m_p + n, static_cast<int>(m_size - n));
  }

  template <typename T> parser get_number(T &value) const {
    auto type = header();
    if (type <= 0x7f) {
      // small int(0 - 127)
      value = type;
      return advance(1);
    }

    switch (type) {
    case pack_type::UINT8:
      value = m_p[1];
      return advance(1 + 1);
    case pack_type::UINT16:
      value = body_number<std::uint16_t>(m_p, m_size);
      return advance(1 + 2);
    case pack_type::UINT32:
      value = body_number<std::uint32_t>(m_p, m_size);
      return advance(1 + 4);
    case pack_type::UINT64:
      value = body_number<std::uint64_t>(m_p, m_size);
      return advance(1 + 8);
    case pack_type::INT8:
      value = m_p[1];
      return advance(1 + 1);
    case pack_type::INT16:
      value = body_number<std::int16_t>(m_p, m_size);
      return advance(1 + 2);
    case pack_type::INT32:
      value = body_number<std::int32_t>(m_p, m_size);
      return advance(1 + 4);
    case pack_type::INT64:
      value = body_number<std::int64_t>(m_p, m_size);
      return advance(1 + 8);
    case pack_type::FLOAT:
      value = body_number<float>(m_p, m_size);
      return advance(1 + 4);
    case pack_type::DOUBLE:
      value = body_number<double>(m_p, m_size);
      return advance(1 + 8);
    case pack_type::NEGATIVE_FIXNUM:
      value = -32;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x1F:
      value = -31;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x1E:
      value = -30;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x1D:
      value = -29;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x1C:
      value = -28;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x1B:
      value = -27;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x1A:
      value = -26;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x19:
      value = -25;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x18:
      value = -24;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x17:
      value = -23;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x16:
      value = -22;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x15:
      value = -21;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x14:
      value = -20;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x13:
      value = -19;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x12:
      value = -18;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x11:
      value = -17;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x10:
      value = -16;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x0F:
      value = -15;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x0E:
      value = -14;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x0D:
      value = -13;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x0C:
      value = -12;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x0B:
      value = -11;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x0A:
      value = -10;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x09:
      value = -9;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x08:
      value = -8;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x07:
      value = -7;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x06:
      value = -6;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x05:
      value = -5;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x04:
      value = -4;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x03:
      value = -3;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x02:
      value = -2;
      return advance(1);
    case pack_type::NEGATIVE_FIXNUM_0x01:
      value = -1;
      return advance(1);
    }

    throw type_parse_error();
  }

  template <typename T> T get_number() const {
    T value;
    get_number(value);
    return value;
  }

  bool is_nil() const {
    auto type = header();
    return type == pack_type::NIL;
  }

  bool is_bool() const {
    auto type = header();
    return type == pack_type::True || type == pack_type::False;
  }

  bool is_number() const {
    auto type = header();
    switch (type) {
#pragma region POSITIVE_FIXNUM 0x00 - 0x7F
    case POSITIVE_FIXNUM:
    case POSITIVE_FIXNUM_0x01:
    case POSITIVE_FIXNUM_0x02:
    case POSITIVE_FIXNUM_0x03:
    case POSITIVE_FIXNUM_0x04:
    case POSITIVE_FIXNUM_0x05:
    case POSITIVE_FIXNUM_0x06:
    case POSITIVE_FIXNUM_0x07:
    case POSITIVE_FIXNUM_0x08:
    case POSITIVE_FIXNUM_0x09:
    case POSITIVE_FIXNUM_0x0A:
    case POSITIVE_FIXNUM_0x0B:
    case POSITIVE_FIXNUM_0x0C:
    case POSITIVE_FIXNUM_0x0D:
    case POSITIVE_FIXNUM_0x0E:
    case POSITIVE_FIXNUM_0x0F:

    case POSITIVE_FIXNUM_0x10:
    case POSITIVE_FIXNUM_0x11:
    case POSITIVE_FIXNUM_0x12:
    case POSITIVE_FIXNUM_0x13:
    case POSITIVE_FIXNUM_0x14:
    case POSITIVE_FIXNUM_0x15:
    case POSITIVE_FIXNUM_0x16:
    case POSITIVE_FIXNUM_0x17:
    case POSITIVE_FIXNUM_0x18:
    case POSITIVE_FIXNUM_0x19:
    case POSITIVE_FIXNUM_0x1A:
    case POSITIVE_FIXNUM_0x1B:
    case POSITIVE_FIXNUM_0x1C:
    case POSITIVE_FIXNUM_0x1D:
    case POSITIVE_FIXNUM_0x1E:
    case POSITIVE_FIXNUM_0x1F:

    case POSITIVE_FIXNUM_0x20:
    case POSITIVE_FIXNUM_0x21:
    case POSITIVE_FIXNUM_0x22:
    case POSITIVE_FIXNUM_0x23:
    case POSITIVE_FIXNUM_0x24:
    case POSITIVE_FIXNUM_0x25:
    case POSITIVE_FIXNUM_0x26:
    case POSITIVE_FIXNUM_0x27:
    case POSITIVE_FIXNUM_0x28:
    case POSITIVE_FIXNUM_0x29:
    case POSITIVE_FIXNUM_0x2A:
    case POSITIVE_FIXNUM_0x2B:
    case POSITIVE_FIXNUM_0x2C:
    case POSITIVE_FIXNUM_0x2D:
    case POSITIVE_FIXNUM_0x2E:
    case POSITIVE_FIXNUM_0x2F:

    case POSITIVE_FIXNUM_0x30:
    case POSITIVE_FIXNUM_0x31:
    case POSITIVE_FIXNUM_0x32:
    case POSITIVE_FIXNUM_0x33:
    case POSITIVE_FIXNUM_0x34:
    case POSITIVE_FIXNUM_0x35:
    case POSITIVE_FIXNUM_0x36:
    case POSITIVE_FIXNUM_0x37:
    case POSITIVE_FIXNUM_0x38:
    case POSITIVE_FIXNUM_0x39:
    case POSITIVE_FIXNUM_0x3A:
    case POSITIVE_FIXNUM_0x3B:
    case POSITIVE_FIXNUM_0x3C:
    case POSITIVE_FIXNUM_0x3D:
    case POSITIVE_FIXNUM_0x3E:
    case POSITIVE_FIXNUM_0x3F:

    case POSITIVE_FIXNUM_0x40:
    case POSITIVE_FIXNUM_0x41:
    case POSITIVE_FIXNUM_0x42:
    case POSITIVE_FIXNUM_0x43:
    case POSITIVE_FIXNUM_0x44:
    case POSITIVE_FIXNUM_0x45:
    case POSITIVE_FIXNUM_0x46:
    case POSITIVE_FIXNUM_0x47:
    case POSITIVE_FIXNUM_0x48:
    case POSITIVE_FIXNUM_0x49:
    case POSITIVE_FIXNUM_0x4A:
    case POSITIVE_FIXNUM_0x4B:
    case POSITIVE_FIXNUM_0x4C:
    case POSITIVE_FIXNUM_0x4D:
    case POSITIVE_FIXNUM_0x4E:
    case POSITIVE_FIXNUM_0x4F:

    case POSITIVE_FIXNUM_0x50:
    case POSITIVE_FIXNUM_0x51:
    case POSITIVE_FIXNUM_0x52:
    case POSITIVE_FIXNUM_0x53:
    case POSITIVE_FIXNUM_0x54:
    case POSITIVE_FIXNUM_0x55:
    case POSITIVE_FIXNUM_0x56:
    case POSITIVE_FIXNUM_0x57:
    case POSITIVE_FIXNUM_0x58:
    case POSITIVE_FIXNUM_0x59:
    case POSITIVE_FIXNUM_0x5A:
    case POSITIVE_FIXNUM_0x5B:
    case POSITIVE_FIXNUM_0x5C:
    case POSITIVE_FIXNUM_0x5D:
    case POSITIVE_FIXNUM_0x5E:
    case POSITIVE_FIXNUM_0x5F:

    case POSITIVE_FIXNUM_0x60:
    case POSITIVE_FIXNUM_0x61:
    case POSITIVE_FIXNUM_0x62:
    case POSITIVE_FIXNUM_0x63:
    case POSITIVE_FIXNUM_0x64:
    case POSITIVE_FIXNUM_0x65:
    case POSITIVE_FIXNUM_0x66:
    case POSITIVE_FIXNUM_0x67:
    case POSITIVE_FIXNUM_0x68:
    case POSITIVE_FIXNUM_0x69:
    case POSITIVE_FIXNUM_0x6A:
    case POSITIVE_FIXNUM_0x6B:
    case POSITIVE_FIXNUM_0x6C:
    case POSITIVE_FIXNUM_0x6D:
    case POSITIVE_FIXNUM_0x6E:
    case POSITIVE_FIXNUM_0x6F:

    case POSITIVE_FIXNUM_0x70:
    case POSITIVE_FIXNUM_0x71:
    case POSITIVE_FIXNUM_0x72:
    case POSITIVE_FIXNUM_0x73:
    case POSITIVE_FIXNUM_0x74:
    case POSITIVE_FIXNUM_0x75:
    case POSITIVE_FIXNUM_0x76:
    case POSITIVE_FIXNUM_0x77:
    case POSITIVE_FIXNUM_0x78:
    case POSITIVE_FIXNUM_0x79:
    case POSITIVE_FIXNUM_0x7A:
    case POSITIVE_FIXNUM_0x7B:
    case POSITIVE_FIXNUM_0x7C:
    case POSITIVE_FIXNUM_0x7D:
    case POSITIVE_FIXNUM_0x7E:
    case POSITIVE_FIXNUM_0x7F:
#pragma endregion

    case FLOAT:
    case DOUBLE:
    case UINT8:
    case UINT16:
    case UINT32:
    case UINT64:
    case INT8:
    case INT16:
    case INT32:
    case INT64:

#pragma region NEGATIVE_FIXNUM 0xE0 - 0xFF
    case NEGATIVE_FIXNUM:
    case NEGATIVE_FIXNUM_0x1F:
    case NEGATIVE_FIXNUM_0x1E:
    case NEGATIVE_FIXNUM_0x1D:
    case NEGATIVE_FIXNUM_0x1C:
    case NEGATIVE_FIXNUM_0x1B:
    case NEGATIVE_FIXNUM_0x1A:
    case NEGATIVE_FIXNUM_0x19:
    case NEGATIVE_FIXNUM_0x18:
    case NEGATIVE_FIXNUM_0x17:
    case NEGATIVE_FIXNUM_0x16:
    case NEGATIVE_FIXNUM_0x15:
    case NEGATIVE_FIXNUM_0x14:
    case NEGATIVE_FIXNUM_0x13:
    case NEGATIVE_FIXNUM_0x12:
    case NEGATIVE_FIXNUM_0x11:
    case NEGATIVE_FIXNUM_0x10:
    case NEGATIVE_FIXNUM_0x0F:
    case NEGATIVE_FIXNUM_0x0E:
    case NEGATIVE_FIXNUM_0x0D:
    case NEGATIVE_FIXNUM_0x0C:
    case NEGATIVE_FIXNUM_0x0B:
    case NEGATIVE_FIXNUM_0x0A:
    case NEGATIVE_FIXNUM_0x09:
    case NEGATIVE_FIXNUM_0x08:
    case NEGATIVE_FIXNUM_0x07:
    case NEGATIVE_FIXNUM_0x06:
    case NEGATIVE_FIXNUM_0x05:
    case NEGATIVE_FIXNUM_0x04:
    case NEGATIVE_FIXNUM_0x03:
    case NEGATIVE_FIXNUM_0x02:
    case NEGATIVE_FIXNUM_0x01:
#pragma endregion
      return true;
    }

    return false;
  }

  bool is_binary() const {
    auto type = header();
    switch (type) {
    case pack_type::BIN8:
    case pack_type::BIN16:
    case pack_type::BIN32:
      return true;
    }

    return false;
  }

  bool is_string() const {
    auto type = header();
    switch (type) {
    case pack_type::STR32:
      return true;
    case pack_type::STR16:
      return true;
    case pack_type::STR8:
      return true;
    case pack_type::FIX_STR:
      return true;
    case pack_type::FIX_STR_0x01:
      return true;
    case pack_type::FIX_STR_0x02:
      return true;
    case pack_type::FIX_STR_0x03:
      return true;
    case pack_type::FIX_STR_0x04:
      return true;
    case pack_type::FIX_STR_0x05:
      return true;
    case pack_type::FIX_STR_0x06:
      return true;
    case pack_type::FIX_STR_0x07:
      return true;
    case pack_type::FIX_STR_0x08:
      return true;
    case pack_type::FIX_STR_0x09:
      return true;
    case pack_type::FIX_STR_0x0A:
      return true;
    case pack_type::FIX_STR_0x0B:
      return true;
    case pack_type::FIX_STR_0x0C:
      return true;
    case pack_type::FIX_STR_0x0D:
      return true;
    case pack_type::FIX_STR_0x0E:
      return true;
    case pack_type::FIX_STR_0x0F:
      return true;
    case pack_type::FIX_STR_0x10:
      return true;
    case pack_type::FIX_STR_0x11:
      return true;
    case pack_type::FIX_STR_0x12:
      return true;
    case pack_type::FIX_STR_0x13:
      return true;
    case pack_type::FIX_STR_0x14:
      return true;
    case pack_type::FIX_STR_0x15:
      return true;
    case pack_type::FIX_STR_0x16:
      return true;
    case pack_type::FIX_STR_0x17:
      return true;
    case pack_type::FIX_STR_0x18:
      return true;
    case pack_type::FIX_STR_0x19:
      return true;
    case pack_type::FIX_STR_0x1A:
      return true;
    case pack_type::FIX_STR_0x1B:
      return true;
    case pack_type::FIX_STR_0x1C:
      return true;
    case pack_type::FIX_STR_0x1D:
      return true;
    case pack_type::FIX_STR_0x1E:
      return true;
    case pack_type::FIX_STR_0x1F:
      return true;
    }

    return false;
  }

#pragma endregion

#pragma region array or map
  bool is_array() const {
    auto type = header();
    switch (type) {
    case pack_type::FIX_ARRAY:
    case pack_type::FIX_ARRAY_0x1:
    case pack_type::FIX_ARRAY_0x2:
    case pack_type::FIX_ARRAY_0x3:
    case pack_type::FIX_ARRAY_0x4:
    case pack_type::FIX_ARRAY_0x5:
    case pack_type::FIX_ARRAY_0x6:
    case pack_type::FIX_ARRAY_0x7:
    case pack_type::FIX_ARRAY_0x8:
    case pack_type::FIX_ARRAY_0x9:
    case pack_type::FIX_ARRAY_0xA:
    case pack_type::FIX_ARRAY_0xB:
    case pack_type::FIX_ARRAY_0xC:
    case pack_type::FIX_ARRAY_0xD:
    case pack_type::FIX_ARRAY_0xE:
    case pack_type::FIX_ARRAY_0xF:
    case pack_type::ARRAY16:
    case pack_type::ARRAY32:
      return true;
    }

    return false;
  }

  bool is_map() const {
    auto type = header();
    switch (type) {
    case pack_type::FIX_MAP:
    case pack_type::FIX_MAP_0x1:
    case pack_type::FIX_MAP_0x2:
    case pack_type::FIX_MAP_0x3:
    case pack_type::FIX_MAP_0x4:
    case pack_type::FIX_MAP_0x5:
    case pack_type::FIX_MAP_0x6:
    case pack_type::FIX_MAP_0x7:
    case pack_type::FIX_MAP_0x8:
    case pack_type::FIX_MAP_0x9:
    case pack_type::FIX_MAP_0xA:
    case pack_type::FIX_MAP_0xB:
    case pack_type::FIX_MAP_0xC:
    case pack_type::FIX_MAP_0xD:
    case pack_type::FIX_MAP_0xE:
    case pack_type::FIX_MAP_0xF:
    case pack_type::MAP16:
    case pack_type::MAP32:
      return true;
    }

    return false;
  }

  parse_result<uint32_t> count() const {
    auto _header = header();
    if (!_header.is_ok()) {
      return {_header.status};
    }
    auto type = _header;
    switch (type) {
    case pack_type::FIX_ARRAY:
      return OK(static_cast<uint32_t>(0));
    case pack_type::FIX_ARRAY_0x1:
      return OK(static_cast<uint32_t>(1));
    case pack_type::FIX_ARRAY_0x2:
      return OK(static_cast<uint32_t>(2));
    case pack_type::FIX_ARRAY_0x3:
      return OK(static_cast<uint32_t>(3));
    case pack_type::FIX_ARRAY_0x4:
      return OK(static_cast<uint32_t>(4));
    case pack_type::FIX_ARRAY_0x5:
      return OK(static_cast<uint32_t>(5));
    case pack_type::FIX_ARRAY_0x6:
      return OK(static_cast<uint32_t>(6));
    case pack_type::FIX_ARRAY_0x7:
      return OK(static_cast<uint32_t>(7));
    case pack_type::FIX_ARRAY_0x8:
      return OK(static_cast<uint32_t>(8));
    case pack_type::FIX_ARRAY_0x9:
      return OK(static_cast<uint32_t>(9));
    case pack_type::FIX_ARRAY_0xA:
      return OK(static_cast<uint32_t>(10));
    case pack_type::FIX_ARRAY_0xB:
      return OK(static_cast<uint32_t>(11));
    case pack_type::FIX_ARRAY_0xC:
      return OK(static_cast<uint32_t>(12));
    case pack_type::FIX_ARRAY_0xD:
      return OK(static_cast<uint32_t>(13));
    case pack_type::FIX_ARRAY_0xE:
      return OK(static_cast<uint32_t>(14));
    case pack_type::FIX_ARRAY_0xF:
      return OK(static_cast<uint32_t>(15));
    case pack_type::ARRAY16:
      return body_number<std::uint16_t>(m_p, m_size).cast<uint32_t>();
    case pack_type::ARRAY32:
      return body_number<std::uint32_t>(m_p, m_size);
    case pack_type::FIX_MAP:
      return OK(static_cast<uint32_t>(0));
    case pack_type::FIX_MAP_0x1:
      return OK(static_cast<uint32_t>(1));
    case pack_type::FIX_MAP_0x2:
      return OK(static_cast<uint32_t>(2));
    case pack_type::FIX_MAP_0x3:
      return OK(static_cast<uint32_t>(3));
    case pack_type::FIX_MAP_0x4:
      return OK(static_cast<uint32_t>(4));
    case pack_type::FIX_MAP_0x5:
      return OK(static_cast<uint32_t>(5));
    case pack_type::FIX_MAP_0x6:
      return OK(static_cast<uint32_t>(6));
    case pack_type::FIX_MAP_0x7:
      return OK(static_cast<uint32_t>(7));
    case pack_type::FIX_MAP_0x8:
      return OK(static_cast<uint32_t>(8));
    case pack_type::FIX_MAP_0x9:
      return OK(static_cast<uint32_t>(9));
    case pack_type::FIX_MAP_0xA:
      return OK(static_cast<uint32_t>(10));
    case pack_type::FIX_MAP_0xB:
      return OK(static_cast<uint32_t>(11));
    case pack_type::FIX_MAP_0xC:
      return OK(static_cast<uint32_t>(12));
    case pack_type::FIX_MAP_0xD:
      return OK(static_cast<uint32_t>(13));
    case pack_type::FIX_MAP_0xE:
      return OK(static_cast<uint32_t>(14));
    case pack_type::FIX_MAP_0xF:
      return OK(static_cast<uint32_t>(15));
    case pack_type::MAP16:
      return body_number<std::uint16_t>(m_p, m_size).cast<uint32_t>();
    case pack_type::MAP32:
      return body_number<std::uint32_t>(m_p, m_size);
    }

    return {parse_status::invalid};
  }

  parser operator[](int index) const {
    auto type = header();
    auto offset = body_index_and_size::from_type(type).value.index;
    auto current = parser(m_p + offset, m_size - offset);
    for (int i = 0; i < index; ++i) {
      current = current.next();
    }
    return current;
  }

  // string key accessor for map
  parser operator[](const std::string &key) const {
    auto type = header();
    auto offset = body_index_and_size::from_type(type).value.index;
    auto current = parser(m_p + offset, m_size - offset);
    auto item_count = count();
    for (uint8_t i = 0; i < item_count; ++i) {
      // key
      if (current.is_string()) {
        if (current.get_string() == key) {
          return current.next();
        }
      }

      current = current.next();
      current = current.next();
    }

    throw std::runtime_error("key not found");
  }

  parse_result<parser> next() const {
    if (m_size < 1) {
      return {parse_status::empty};
    }
    auto type = static_cast<pack_type>(m_p[0]);
    auto _body = body_index_and_size::from_type(type);
    if (!_body.is_ok()) {
      return {parse_status::invalid};
    }
    auto body = _body.value;

    if (is_array()) {
      auto offset = body.index;
      auto current = parser(m_p + offset, m_size - offset);
      auto item_count = count();
      if (!item_count.is_ok()) {
        return {item_count.status};
      }
      for (uint8_t i = 0; i < item_count; ++i) {
        auto _current = current.next();
        if (!_current.is_ok()) {
          return {_current.status};
        }
        current = _current.value;
      }
      return OK(current);
    } else if (is_map()) {
      auto offset = body.index;
      auto current = parser(m_p + offset, m_size - offset);
      auto item_count = count();
      if (!item_count.is_ok()) {
        return {item_count.status};
      }
      for (uint8_t i = 0; i < item_count; ++i) {
        // k
        auto _key = current.next();
        if (!_key.is_ok()) {
          return {_key.status};
        }
        current = _key.value;
        // v
        auto _value = current.next();
        if (!_value.is_ok()) {
          return {_value.status};
        }
        current = _value.value;
      }
      return OK(current);
    } else {
      auto size = body.size(m_p, m_size);
      if (!size.is_ok()) {
        return {parse_status::invalid};
      }
      auto offset = body.index + size;
      auto current = parser(m_p + offset, m_size - offset);
      return OK(current);
    }
  }
#pragma endregion
};

#pragma region tuple helper
template <typename T, typename... TS>
std::tuple<T, TS...> cons(const T &car, const std::tuple<TS...> &cdr) {
  return std::tuple_cat(std::make_tuple(car), cdr);
}

//
template <typename T, typename Seq> struct tuple_cdr_impl;

template <typename T, std::size_t I0, std::size_t... I>
struct tuple_cdr_impl<T, std::index_sequence<I0, I...>> {
  using type = std::tuple<typename std::tuple_element<I, T>::type...>;
};

template <typename T>
struct tuple_cdr
    : tuple_cdr_impl<T, std::make_index_sequence<std::tuple_size<T>::value>> {};

template <typename T, std::size_t I0, std::size_t... I>
typename tuple_cdr<typename std::remove_reference<T>::type>::type
cdr_impl(T &&t, std::index_sequence<I0, I...>) {
  return std::make_tuple(std::get<I>(t)...);
}

template <typename T>
typename tuple_cdr<typename std::remove_reference<T>::type>::type cdr(T &&t) {
  return cdr_impl(
      std::forward<T>(t),
      std::make_index_sequence<
          std::tuple_size<typename std::remove_reference<T>::type>::value>{});
}
#pragma endregion

#pragma region serializer
template <typename T> inline packer &operator<<(packer &p, const T &t) {
  serialize(p, t);
  return p;
}

struct nil_t {};
constexpr nil_t nil = nil_t{};
inline void serialize(packer &p, const nil_t) { p.pack_nil(); }
inline void serialize(packer &p, const char *t) { p.pack_str(t); }
inline void serialize(packer &p, bool t) { p.pack_bool(t); }

inline void serialize(packer &p, signed char t) { p.pack_integer(t); }
inline void serialize(packer &p, signed short t) { p.pack_integer(t); }
inline void serialize(packer &p, signed int t) { p.pack_integer(t); }
inline void serialize(packer &p, signed long long t) { p.pack_integer(t); }
inline void serialize(packer &p, unsigned char t) { p.pack_integer(t); }
inline void serialize(packer &p, unsigned short t) { p.pack_integer(t); }
inline void serialize(packer &p, unsigned int t) { p.pack_integer(t); }
inline void serialize(packer &p, unsigned long long t) { p.pack_integer(t); }

inline void serialize(packer &p, float t) { p.pack_float(t); }
inline void serialize(packer &p, double t) { p.pack_double(t); }

#pragma region serialize tuple
inline void _serialize(const std::tuple<> &t, packer &p) {}

template <typename T>
inline void _serialize(const std::tuple<T> &t, packer &p) {
  serialize(p, std::get<0>(t));
}

template <typename T, typename... TS>
inline void _serialize(const std::tuple<T, TS...> &t, packer &p) {
  serialize(p, std::get<0>(t));
  _serialize(cdr(t), p);
}

template <typename... TS>
inline void serialize(packer &p, const std::tuple<TS...> &t) {
  auto size =
      std::tuple_size<typename std::remove_reference<decltype(t)>::type>::value;
  p.pack_array(size);
  _serialize(t, p);
}
#pragma endregion
#pragma endregion

#pragma region deserializer
template <typename T> inline parser operator>>(const parser &u, T &t) {
  return deserialize(u, t);
}

inline parser deserialize(const parser &u, bool &value) {
  return u.get_bool(value);
}

inline parser deserialize(const parser &u, signed char &value) {
  return u.get_number(value);
}
inline parser deserialize(const parser &u, signed short &value) {
  return u.get_number(value);
}
inline parser deserialize(const parser &u, signed int &value) {
  return u.get_number(value);
}
inline parser deserialize(const parser &u, signed long long &value) {
  return u.get_number(value);
}
inline parser deserialize(const parser &u, unsigned char &value) {
  return u.get_number(value);
}
inline parser deserialize(const parser &u, unsigned short &value) {
  return u.get_number(value);
}
inline parser deserialize(const parser &u, unsigned int &value) {
  return u.get_number(value);
}
inline parser deserialize(const parser &u, unsigned long long &value) {
  return u.get_number(value);
}
inline parser deserialize(const parser &u, float &value) {
  return u.get_number(value);
}
inline parser deserialize(const parser &u, double &value) {
  return u.get_number(value);
}

inline parser deserialize(const parser &u, parser &x) {
  x = u[0];
  return u[0].next();
}

#pragma region deserialize tuple
inline parser _deserialize(std::tuple<> &value, const parser &u) { return u; }

template <typename T>
inline parser _deserialize(std::tuple<T> &value, const parser &u) {
  // unpack
  T t;
  auto uu = deserialize(u, t);

  value = std::make_tuple(t);

  return uu;
}

template <typename T, typename... TS>
inline parser _deserialize(std::tuple<T, TS...> &value, const parser &u) {
  // unpack
  T t;
  auto uu = deserialize(u, t);

  decltype(cdr(value)) remain;
  auto uuu = _deserialize(remain, uu);

  value = std::tuple_cat(std::make_tuple(t), remain);

  return uuu;
}

template <typename... TS>
inline parser deserialize(const parser &u, std::tuple<TS...> &value) {
  assert(u.is_array());
  auto count = u.count();
  if (count !=
      std::tuple_size<
          typename std::remove_reference<decltype(value)>::type>::value) {
    std::stringstream ss;
    ss << "invalid arguments count: " << count << ", but expected: "
       << std::tuple_size<
              typename std::remove_reference<decltype(value)>::type>::value;
    throw std::runtime_error(ss.str());
  }
  return _deserialize(value, u[0]);
}
#pragma endregion
#pragma endregion

#pragma region Range serialization
template <typename Range> inline void serialize(packer &p, const Range &value) {
  auto b = std::begin(value);
  auto e = std::end(value);
  p.pack_array(std::distance(b, e));
  for (auto it = b; it != e; ++it) {
    p << *it;
  }
}
template <typename Range>
inline parser deserialize(const parser &u, Range &value) {
  auto count = u.count();
  value.resize(count);
  auto uu = u[0];
  for (auto &v : value)
  // for (int i = 0; i < count; ++i)
  {
    uu >> v;
    uu = uu.next();
  }
  return uu;
}

template <>
inline void
serialize<std::vector<std::uint8_t>>(packer &p,
                                     const std::vector<std::uint8_t> &t) {
  p.pack_bin(t);
}
template <>
inline parser
deserialize<std::vector<std::uint8_t>>(const parser &u,
                                       std::vector<std::uint8_t> &value) {
  return u.get_binary(value);
}

template <>
inline parser deserialize<std::string>(const parser &u, std::string &value) {
  std::string_view view;
  auto uu = u.get_string(view);
  value.assign(view.begin(), view.end());
  return uu;
}

template <>
inline parser deserialize<std::string_view>(const parser &u,
                                            std::string_view &value) {
  auto uu = u.get_string(value);
  return uu;
}

template <typename T> inline T deserialize(const std::vector<uint8_t> &b) {
  T value;
  parser(b) >> value;
  return value;
}

template <>
inline void serialize<std::string>(packer &p, const std::string &t) {
  p.pack_str(t);
}

#pragma region

#pragma region stream out
// json like
inline std::ostream &operator<<(std::ostream &os, const parser &p) {
  p.to_json(os);
  return os;
}
#pragma endregion

#pragma region procedure call
using bytes = std::vector<std::uint8_t>;
using procedurecall = std::function<bytes(const parser &)>;

#pragma region void
template <typename F, typename C, typename... AS, std::size_t... IS>
procedurecall _make_procedurecall(const F &f, void (C::*)(AS...) const,
                                  std::index_sequence<IS...>) {
  return [f](const parser &parser) -> bytes {
    // unpack args
    std::tuple<AS...> args;
    parser >> args;

    // call
    f(std::move(std::get<IS>(args))...);

    // pack result
    msgpackpp::packer packer;
    packer.pack_nil();
    return packer.get_payload();
  };
}

template <typename F, typename C, typename... AS>
procedurecall _make_procedurecall(F f, void (C::*)(AS...) const) {
  return _make_procedurecall(f, &decltype(f)::operator(),
                             std::index_sequence_for<AS...>{});
}

template <typename... AS, std::size_t... IS>
procedurecall _make_procedurecall(void (*f)(AS...),
                                  std::index_sequence<IS...>) {
  return [f](const parser &parser) -> bytes {
    // unpack args
    std::tuple<AS...> args;
    parser >> args;

    // call
    f(std::move(std::get<IS>(args))...);

    // pack result
    msgpackpp::packer packer;
    packer.pack_nil();
    return packer.get_payload();
  };
}

#pragma endregion

#pragma region result
template <typename F, typename R, typename C, typename... AS, std::size_t... IS>
procedurecall _make_procedurecall(const F &f, R (C::*)(AS...) const,
                                  std::index_sequence<IS...>) {
  return [f](const parser &parser) -> bytes {
    // unpack args
    std::tuple<AS...> args;
    parser >> args;

    // call
    auto r = f(std::move(std::get<IS>(args))...);

    // pack result
    msgpackpp::packer packer;
    packer << r;
    return packer.get_payload();
  };
}

template <typename F, typename R, typename C, typename... AS>
procedurecall _make_procedurecall(F f, R (C::*)(AS...) const) {
  return _make_procedurecall(f, &decltype(f)::operator(),
                             std::index_sequence_for<AS...>{});
}

template <typename R, typename... AS, std::size_t... IS>
procedurecall _make_procedurecall(R (*f)(AS...), std::index_sequence<IS...>) {
  return [f](const parser &parser) -> bytes {
    // unpack args
    std::tuple<AS...> args;
    parser >> args;

    // call
    auto r = f(std::move(std::get<IS>(args))...);

    // pack result
    msgpackpp::packer packer;
    packer << r;
    return packer.get_payload();
  };
}
#pragma endregion

template <typename F> procedurecall make_procedurecall(F f) {
  return _make_procedurecall(f, &decltype(f)::operator());
}

template <typename R, typename... AS>
procedurecall make_procedurecall(R (*f)(AS...)) {
  return _make_procedurecall(f, std::index_sequence_for<AS...>{});
}

template <typename C, typename R, typename... AS>
procedurecall make_methodcall(C *c, R (C::*f)(AS...)) {
  return make_procedurecall([c, f](AS... args) { (*c.*f)(args...); });
}

#pragma region call
template <typename F, typename R, typename C, typename... AS>
decltype(auto) _procedure_call(F f, R (C::*)(AS...) const, AS... args) {
  auto proc = make_procedurecall(f);
  packer packer;
  packer << std::make_tuple(args...);
  auto result = proc(packer.get_payload());

  R value;
  parser(result) >> value;
  return value;
}

template <typename F, typename... AS>
decltype(auto) procedure_call(F f, AS... args) {
  return _procedure_call(f, &decltype(f)::operator(), args...);
}
#pragma endregion

#pragma endregion
} // namespace msgpackpp

/// https://stackoverflow.com/questions/36925989/how-to-define-recursive-variadic-macros
#define MPPP_EXPAND(x) x
#define MPPP_CAT_I(x, y) x##y
#define MPPP_CAT(x, y) MPPP_CAT_I(x, y)

#pragma region PACK_MAP
#define MPPP_PACK_KV(v, k) << #k << (v).k
#define MPPP_PACK_KV_1(v, k0) MPPP_PACK_KV(v, k0)
#define MPPP_PACK_KV_2(v, k0, k1) MPPP_PACK_KV_1(v, k0) MPPP_PACK_KV(v, k1)
#define MPPP_PACK_KV_3(v, k0, k1, k2)                                          \
  MPPP_PACK_KV_2(v, k0, k1) MPPP_PACK_KV(v, k2)
#define MPPP_PACK_KV_4(v, k0, k1, k2, k3)                                      \
  MPPP_PACK_KV_3(v, k0, k1, k2) MPPP_PACK_KV(v, k3)
#define MPPP_PACK_KV_5(v, k0, k1, k2, k3, k4)                                  \
  MPPP_PACK_KV_4(v, k0, k1, k2, k3) MPPP_PACK_KV(v, k4)
#define MPPP_PACK_KV_N(n, v, ...)                                              \
  p.pack_map(n);                                                               \
  p MPPP_CAT(MPPP_PACK_KV_, n) MPPP_EXPAND((v, __VA_ARGS__))
#define MPPP_PACK_KV_TYPE(TYPE, n, ...)                                        \
  inline void serialize(packer &p, const TYPE &value) {                        \
    MPPP_PACK_KV_N MPPP_EXPAND((n, value, __VA_ARGS__));                       \
  }
#define MPPP_PACK_KV_SHARED(TYPE, n, ...)                                      \
  inline void serialize(packer &p, const std::shared_ptr<TYPE> &value) {       \
    MPPP_PACK_KV_N MPPP_EXPAND((n, *value, __VA_ARGS__));                      \
  }
#pragma endregion

#pragma region UNPACK_MAP
#define MPPP_UNPACK_KV(v, k) else if (key == #k) uu >> (v).k;
#define MPPP_UNPACK_KV_1(v, k0) MPPP_UNPACK_KV(v, k0)
#define MPPP_UNPACK_KV_2(v, k0, k1)                                            \
  MPPP_UNPACK_KV_1(v, k0) MPPP_UNPACK_KV(v, k1)
#define MPPP_UNPACK_KV_3(v, k0, k1, k2)                                        \
  MPPP_UNPACK_KV_2(v, k0, k1) MPPP_UNPACK_KV(v, k2)
#define MPPP_UNPACK_KV_4(v, k0, k1, k2, k3)                                    \
  MPPP_UNPACK_KV_3(v, k0, k1, k2) MPPP_UNPACK_KV(v, k3)
#define MPPP_UNPACK_KV_5(v, k0, k1, k2, k3, k4)                                \
  MPPP_UNPACK_KV_4(v, k0, k1, k2, k3) MPPP_UNPACK_KV(v, k4)
#define MPPP_UNPACK_KV_N(n, v, ...)                                            \
  MPPP_UNPACK_KV_BEGIN()                                                       \
  MPPP_CAT(MPPP_UNPACK_KV_, n)                                                 \
  MPPP_EXPAND((v, __VA_ARGS__)) MPPP_UNPACK_KV_END()
#define MPPP_UNPACK_KV_TYPE(TYPE, n, ...)                                      \
  inline parser deserialize(const parser &u, TYPE &value) {                    \
    MPPP_UNPACK_KV_N MPPP_EXPAND((n, value, __VA_ARGS__));                     \
  }
#define MPPP_UNPACK_KV_SHARED(TYPE, n, ...)                                    \
  inline parser deserialize(const parser &u, std::shared_ptr<TYPE> &value) {   \
    if (!value) {                                                              \
      value = std::make_shared<TYPE>();                                        \
    }                                                                          \
    MPPP_UNPACK_KV_N MPPP_EXPAND((n, *value, __VA_ARGS__));                    \
  }

#define MPPP_UNPACK_KV_BEGIN()                                                 \
  auto count = u.count();                                                      \
  auto uu = u[0];                                                              \
  for (uint8_t i = 0; i < count; ++i) {                                        \
    std::string key;                                                           \
    uu >> key;                                                                 \
    uu = uu.next();                                                            \
                                                                               \
    if (false) {                                                               \
    }

#define MPPP_UNPACK_KV_END()                                                   \
  else {                                                                       \
  }                                                                            \
  uu = uu.next();                                                              \
  }                                                                            \
  return uu;

#pragma endregion

#define MPPP_MAP_SERIALIZER(TYPE, n, ...)                                      \
  namespace msgpackpp {                                                        \
  MPPP_PACK_KV_TYPE MPPP_EXPAND((TYPE, n, __VA_ARGS__)) MPPP_UNPACK_KV_TYPE    \
      MPPP_EXPAND((TYPE, n, __VA_ARGS__))                                      \
  }
#define MPPP_MAP_SERIALIZER_SHAREDP(TYPE, n, ...)                              \
  namespace msgpackpp {                                                        \
  MPPP_PACK_KV_SHARED MPPP_EXPAND((TYPE, n,                                    \
                                   __VA_ARGS__)) MPPP_UNPACK_KV_SHARED         \
      MPPP_EXPAND((TYPE, n, __VA_ARGS__))                                      \
  }

#pragma region PACK_ARRAY
#define MPPP_PACK_ARRAY(v, k) << (v).k
#define MPPP_PACK_ARRAY_1(v, k0) MPPP_PACK_ARRAY(v, k0)
#define MPPP_PACK_ARRAY_2(v, k0, k1)                                           \
  MPPP_PACK_ARRAY_1(v, k0) MPPP_PACK_ARRAY(v, k1)
#define MPPP_PACK_ARRAY_3(v, k0, k1, k2)                                       \
  MPPP_PACK_ARRAY_2(v, k0, k1) MPPP_PACK_ARRAY(v, k2)
#define MPPP_PACK_ARRAY_4(v, k0, k1, k2, k3)                                   \
  MPPP_PACK_ARRAY_3(v, k0, k1, k2) MPPP_PACK_ARRAY(v, k3)
#define MPPP_PACK_ARRAY_5(v, k0, k1, k2, k3, k4)                               \
  MPPP_PACK_ARRAY_4(v, k0, k1, k2, k3) MPPP_PACK_ARRAY(v, k4)
#define MPPP_PACK_ARRAY_N(n, v, ...)                                           \
  p.pack_array(n);                                                             \
  p MPPP_CAT(MPPP_PACK_ARRAY_, n) MPPP_EXPAND((v, __VA_ARGS__))
#define MPPP_PACK_ARRAY_TYPE(TYPE, n, ...)                                     \
  inline void serialize(packer &p, const TYPE &value) {                        \
    MPPP_PACK_ARRAY_N MPPP_EXPAND((n, value, __VA_ARGS__));                    \
  }
#define MPPP_PACK_ARRAY_SHARED(TYPE, n, ...)                                   \
  inline void serialize(packer &p, const std::shared_ptr<TYPE> &value) {       \
    MPPP_PACK_ARRAY_N MPPP_EXPAND((n, *value, __VA_ARGS__));                   \
  }
#pragma endregion

#pragma region UNPACK_ARRAY
#define MPPP_UNPACK_ARRAY(v, k)                                                \
  uu >> (v).k;                                                                 \
  uu = uu.next();
#define MPPP_UNPACK_ARRAY_1(v, k0) MPPP_UNPACK_ARRAY(v, k0)
#define MPPP_UNPACK_ARRAY_2(v, k0, k1)                                         \
  MPPP_UNPACK_ARRAY_1(v, k0) MPPP_UNPACK_ARRAY(v, k1)
#define MPPP_UNPACK_ARRAY_3(v, k0, k1, k2)                                     \
  MPPP_UNPACK_ARRAY_2(v, k0, k1) MPPP_UNPACK_ARRAY(v, k2)
#define MPPP_UNPACK_ARRAY_4(v, k0, k1, k2, k3)                                 \
  MPPP_UNPACK_ARRAY_3(v, k0, k1, k2) MPPP_UNPACK_ARRAY(v, k3)
#define MPPP_UNPACK_ARRAY_5(v, k0, k1, k2, k3, k4)                             \
  MPPP_UNPACK_ARRAY_4(v, k0, k1, k2, k3) MPPP_UNPACK_ARRAY(v, k4)
#define MPPP_UNPACK_ARRAY_N(n, v, ...)                                         \
  auto uu = u[0];                                                              \
  MPPP_CAT(MPPP_UNPACK_ARRAY_, n) MPPP_EXPAND((v, __VA_ARGS__)) return uu;
#define MPPP_UNPACK_ARRAY_TYPE(TYPE, n, ...)                                   \
  inline parser deserialize(const parser &u, TYPE &value) {                    \
    MPPP_UNPACK_ARRAY_N MPPP_EXPAND((n, value, __VA_ARGS__));                  \
  }
#define MPPP_UNPACK_ARRAY_SHARED(TYPE, n, ...)                                 \
  inline parser deserialize(const parser &u, std::shared_ptr<TYPE> &value) {   \
    if (!value) {                                                              \
      value = std::make_shared<TYPE>();                                        \
    }                                                                          \
    MPPP_UNPACK_ARRAY_N MPPP_EXPAND((n, *value, __VA_ARGS__));                 \
  }

#pragma endregion

#define MPPP_ARRAY_SERIALIZER(TYPE, n, ...)                                    \
  namespace msgpackpp {                                                        \
  MPPP_PACK_ARRAY_TYPE MPPP_EXPAND((TYPE, n,                                   \
                                    __VA_ARGS__)) MPPP_UNPACK_ARRAY_TYPE       \
      MPPP_EXPAND((TYPE, n, __VA_ARGS__))                                      \
  }

// RPC request
namespace msgpackpp {
template <typename... AS>
std::vector<std::uint8_t> make_rpc_request(int id, const std::string &method,
                                           AS... args) {
  packer packer;
  packer.pack_array(4);
  packer << 0; // request type
  packer << id;
  packer << method;
  packer << std::make_tuple(args...);
  return packer.get_payload();
}

template <typename A>
std::vector<std::uint8_t> make_rpc_response(int id, const std::string &error,
                                            A result) {
  packer packer;
  packer.pack_array(4);
  packer << 1; // response type
  packer << id;
  packer << error;
  packer << result;
  return packer.get_payload();
}

inline std::vector<std::uint8_t>
make_rpc_response_packed(int id, const std::string &error,
                         const bytes &packed) {
  packer packer;
  packer.pack_array(4);
  packer << 1; // response type
  packer << id;
  packer << error;
  packer.push(packed);
  return packer.get_payload();
}

template <typename... AS>
std::vector<std::uint8_t> make_rpc_notify(const std::string &method,
                                          AS... args) {
  packer packer;
  packer.pack_array(3);
  packer << 2; // notify type
  packer << method;
  packer << std::make_tuple(args...);
  return packer.get_payload();
}

inline std::vector<std::uint8_t>
make_rpc_notify_packed(const std::string &method, const bytes &packed) {
  packer packer;
  packer.pack_array(3);
  packer << 2; // notify type
  packer << method;
  packer.push(packed);
  return packer.get_payload();
}

} // namespace msgpackpp

#pragma warning(pop)
