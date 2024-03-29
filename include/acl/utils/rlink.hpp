#pragma once

#include "type_traits.hpp"
#include <acl/utils/utils.hpp>
#include <compare>
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace acl
{

/** @brief Revisable links */
template <typename Ty, typename SizeType = std::uint32_t, std::uint32_t N = 8>
struct rlink
{

  using size_type                              = SizeType;
  static constexpr size_type null_v            = 0;
  static constexpr size_type num_revision_bits = N;
  static constexpr size_type num_usable_bits   = (sizeof(size_type) * 8 - N);
  static constexpr size_type mask_v            = std::numeric_limits<size_type>::max() >> N;
  static constexpr size_type rmask_v           = std::numeric_limits<size_type>::max() << num_usable_bits;
  static constexpr size_type version_inc_v     = 1 << num_usable_bits;

  constexpr rlink() noexcept               = default;
  constexpr rlink(const rlink& i) noexcept = default;
  constexpr explicit rlink(size_type i) noexcept : offset(i) {}
  constexpr explicit rlink(size_type i, size_type revision) noexcept : offset(revision << num_usable_bits | i) {}

  template <typename Uy>
  constexpr explicit rlink(const rlink<Uy, SizeType, N>& i) noexcept
    requires std::convertible_to<Uy*, Ty*> || std::derived_from<Ty, Uy> || std::is_void_v<Uy> || std::is_void_v<Ty>
      : offset(i.offset)
  {}

  constexpr rlink& operator=(const rlink& i) noexcept = default;

  template <typename Uy>
  constexpr rlink& operator=(const rlink<Uy, SizeType, N>& i) noexcept
    requires std::convertible_to<Uy*, Ty*> || std::derived_from<Ty, Uy> || std::is_void_v<Uy> || std::is_void_v<Ty>
  {
    offset = i.offset;
    return *this;
  }

  constexpr inline size_type as_index() const noexcept
  {
    return (offset & mask_v);
  }

  constexpr inline uint8_t revision() const noexcept
  {
    return offset >> num_usable_bits;
  }

  constexpr inline rlink revise() const noexcept
  {
    return rlink(offset + version_inc_v);
  }

  constexpr inline size_type value() const noexcept
  {
    return offset;
  }

  constexpr inline size_type unmasked() const noexcept
  {
    return offset & mask_v;
  }

  constexpr inline size_type get_mask() const noexcept
  {
    return (offset & (~mask_v));
  }

  constexpr inline size_type has_mask(size_type m) const noexcept
  {
    return (offset & m) != 0;
  }

  constexpr inline void mask(size_type m) noexcept
  {
    offset |= (m & ~mask_v);
  }

  constexpr inline void unmask() noexcept
  {
    offset &= mask_v;
  }

  constexpr inline explicit operator size_type() const noexcept
  {
    return offset;
  }

  constexpr inline explicit operator bool() const noexcept
  {
    return offset != null_v;
  }

  constexpr inline auto operator<=>(rlink const& second) const noexcept = default;

  size_type offset = null_v;
};

} // namespace acl