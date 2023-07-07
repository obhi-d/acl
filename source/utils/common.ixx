
export module acl:detail;

import <algorithm>;
import <array>;
import <atomic>;
import <cassert>;
import <chrono>;
import <compare>;
import <cstdint>;
import <cstring>;
import <iostream>;
import <limits>;
import <new>;
import <random>;
import <sstream>;
import <string>;
import <type_traits>;
import <typeinfo>;
import <variant>;
import <vector>;

#include <acl/utility/type_traits.hpp>

#if defined(_MSC_VER)
#include <intrin.h>
#define ACL_EXPORT              __declspec(dllexport)
#define ACL_IMPORT              __declspec(dllimport)
#define ACL_EMPTY_BASES         __declspec(empty_bases)
#define ACL_POPCOUNT(v)         __popcnt(v)
#define ACL_PREFETCH_ONETIME(x) _mm_prefetch((const char*)(x), _MM_HINT_T0)
#define ACL_LIKELY(x)           (x)
#define ACL_UNLIKELY(x)         (x)
#else
#define ACL_EXPORT __attribute__((visibility("default")))
#define ACL_IMPORT __attribute__((visibility("default")))
#define ACL_EMPTY_BASES
#define ACL_POPCOUNT(v)         __builtin_popcount(v)
#define ACL_PREFETCH_ONETIME(x) __builtin_prefetch((x))
#define ACL_LIKELY(x)           __builtin_expect((x), 1)
#define ACL_UNLIKELY(x)         __builtin_expect((x), 0)
#endif

#ifdef ACL_DLL_IMPL
#ifdef ACL_EXPORT_SYMBOLS
#define ACL_API ACL_EXPORT
#else
#define ACL_API ACL_IMPORT
#endif
#else
#define ACL_API
#endif

#if _DEBUG
#define ACL_VALIDITY_CHECKS
#endif

#ifndef ACL_PRINT_DEBUG
#define ACL_PRINT_DEBUG(info) acl::detail::print_debug_info(info)
#endif

#define ACL_EXTERN extern "C"

namespace acl
{
constexpr std::uint32_t safety_offset = alignof(void*);

using uhandle = std::uint32_t;
using ihandle = std::uint32_t;

namespace detail
{

template <typename size_type>
constexpr size_type     k_null_sz  = std::numeric_limits<size_type>::max();
constexpr std::uint32_t k_null_0   = 0;
constexpr std::uint32_t k_null_32  = std::numeric_limits<std::uint32_t>::max();
constexpr std::int32_t  k_null_i32 = std::numeric_limits<std::int32_t>::min();
constexpr std::uint64_t k_null_64  = std::numeric_limits<std::uint64_t>::max();
constexpr uhandle       k_null_uh  = std::numeric_limits<uhandle>::max();

template <class T>
struct optional_ref
{
  using type               = std::remove_reference_t<T>;
  constexpr optional_ref() = default;
  constexpr optional_ref(type& iv) noexcept : value(&iv) {}

  inline constexpr operator bool() const noexcept
  {
    return value != nullptr;
  }

  inline constexpr type& operator*() const noexcept
  {
    ACL_ASSERT(value);
    return *value;
  }

  inline constexpr type* operator->() const noexcept
  {
    return value;
  }

  inline constexpr explicit operator type*() const noexcept
  {
    return value;
  }

  inline constexpr explicit operator type&() const noexcept
  {
    ACL_ASSERT(value);
    return *value;
  }

  inline constexpr bool has_value() const noexcept
  {
    return value != nullptr;
  }

  inline constexpr void reset() const noexcept
  {
    value = nullptr;
  }

  inline constexpr type* release() const noexcept
  {
    auto r = value;
    value  = nullptr;
    return r;
  }

  inline constexpr auto operator<=>(const optional_ref& other) const noexcept = default;

  type* value = nullptr;
};

template <auto nullv>
struct optional_val
{
  using vtype              = std::decay_t<decltype(nullv)>;
  constexpr optional_val() = default;
  constexpr optional_val(vtype iv) noexcept : value(iv) {}

  inline constexpr operator bool() const noexcept
  {
    return value != nullv;
  }

  inline constexpr vtype operator*() const noexcept
  {
    return value;
  }

  inline constexpr explicit operator vtype() const noexcept
  {
    return value;
  }

  inline constexpr bool has_value() const noexcept
  {
    return value != nullv;
  }

  inline constexpr void reset() const noexcept
  {
    value = nullv;
  }

  inline constexpr vtype release() const noexcept
  {
    auto r = value;
    value  = nullv;
    return r;
  }

  inline constexpr auto operator<=>(const optional_val& other) const noexcept = default;

  vtype value = nullv;
};

template <typename var>
struct variant_result
{
  variant_result() = default;
  template <typename... Args>
  variant_result(Args&&... args) : res(std::forward<Args>(args)...)
  {}

  var const& operator*() const
  {
    return res;
  }

  var& operator*()
  {
    return res;
  }

  inline explicit operator bool() const
  {
    return res.index() != 0;
  }

  var res;
};

} // namespace detail

} // namespace acl
