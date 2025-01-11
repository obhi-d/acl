#pragma once

#include <algorithm>
#include <charconv>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

namespace acl
{

template <typename T>
struct transform; // Specialization examples are given

// Variant type transform
template <typename T>
struct index_transform
{
  static auto to_index(std::string_view ref) -> std::size_t
  {
    uint32_t index = 0;
    std::from_chars(ref.data(), ref.data() + ref.size(), index);
    return index;
  }

  static auto from_index(std::size_t ref) -> std::string
  {
    return std::to_string(ref);
  }
};

template <>
struct transform<std::string>
{
  static auto to_string(std::string const& ref) -> std::string_view
  {
    return ref;
  }

  static auto to_string(std::string ref) -> std::string
  {
    return std::move(ref);
  }

  static auto from_string(std::string& ref, std::string_view v) -> void
  {
    ref = std::string(v);
  }

  static auto from_string(std::string& ref, std::string&& v) -> void
  {
    ref = std::move(v);
  }
};

template <>
struct transform<std::unique_ptr<char[]>>
{
  static auto to_string(std::unique_ptr<char[]> const& ref) -> std::string_view
  {
    return {ref.get()};
  }

  static auto from_string(std::unique_ptr<char[]>& ref, std::string_view v) -> void
  {
    ref = std::make_unique<char[]>(v.size());
    std::ranges::copy(v, ref.get());
  }
};

template <>
struct transform<std::string_view>
{
  static auto to_string(std::string_view const& ref) -> std::string_view
  {
    return ref;
  }

  static auto from_string(std::string_view& ref, std::string_view v) -> void
  {
    ref = v;
  }
};

} // namespace acl