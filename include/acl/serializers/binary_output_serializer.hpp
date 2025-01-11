
#pragma once

#include "acl/utility/transforms.hpp"
#include <acl/reflection/detail/container_utils.hpp>
#include <acl/reflection/detail/derived_concepts.hpp>
#include <acl/reflection/detail/visitor_helpers.hpp>
#include <acl/reflection/reflection.hpp>
#include <acl/utility/type_traits.hpp>
#include <cassert>
#include <limits>
#include <string_view>

namespace acl
{
template <typename V>
concept BinaryOutputStream = requires(V v, std::size_t N) {
  // function: Must have a write function to write bytes
  v.write(std::declval<std::byte const*>(), N);
};

template <BinaryOutputStream Serializer, std::endian Endian = std::endian::little>
class binary_output_serializer
{
private:
  enum class type : uint8_t
  {
    object,
    array,
    field
  };

  Serializer* serializer_ = nullptr;
  type        type_       = type::object;

  static constexpr bool has_fast_path = (Endian == std::endian::native);

public:
  using serializer_type = Serializer;
  using serializer_tag  = writer_tag;

  auto operator=(const binary_output_serializer&) -> binary_output_serializer&     = default;
  auto operator=(binary_output_serializer&&) noexcept -> binary_output_serializer& = default;
  binary_output_serializer(binary_output_serializer const&)                        = default;
  binary_output_serializer(binary_output_serializer&& i_other) noexcept : serializer_(i_other.serializer_) {}
  binary_output_serializer(Serializer& ser) : serializer_(&ser) {}
  ~binary_output_serializer() noexcept = default;

  binary_output_serializer(acl::detail::field_visitor_tag /*unused*/, binary_output_serializer& ser,
                           std::string_view key)
      : serializer_{ser.serializer_}, type_{type::field}
  {
    // No-op
  }

  binary_output_serializer(acl::detail::object_visitor_tag /*unused*/, binary_output_serializer& ser)
      : serializer_{ser.serializer_}, type_{type::object}
  {
    // No-op
  }

  binary_output_serializer(acl::detail::array_visitor_tag /*unused*/, binary_output_serializer& ser)
      : serializer_{ser.serializer_}, type_{type::array}
  {
    // No-op
  }

  template <typename Class>
  auto can_visit(Class const& obj) -> continue_token
  {
    using type = std::decay_t<Class>;
    if constexpr (acl::detail::ByteStreambleClass<Class, Serializer>)
    {
      return true;
    }
    if (type_ == type::object)
    {
      write_id(type_name<type>().hash());
    }
    return true;
  }

  template <acl::detail::OutputSerializableClass<Serializer> T>
  void visit(T& obj)
  {
    (*serializer_) << obj;
  }

  template <typename Class>
  void for_each_field(Class const& obj, auto&& fn)
  {
    auto count = static_cast<uint32_t>(obj.size());
    visit(count);
    for (auto const& [key, value] : obj)
    {
      visit(acl::transform<std::decay_t<decltype(key)>>::to_string(key));
      fn(value, *this);
    }
  }

  template <acl::detail::ComplexMapLike Class>
  void for_each_entry(Class const& obj, auto&& fn)
  {
    auto count = static_cast<uint32_t>(obj.size());
    visit(count);

    using type = std::decay_t<Class>;

    for (auto const& [key, value] : obj)
    {
      fn(key, value, *this);
    }
  }

  template <acl::detail::ArrayLike Class>
  void for_each_entry(Class const& obj, auto&& fn)
  {
    auto count = static_cast<uint32_t>(obj.size());
    visit(count);

    using type = std::decay_t<Class>;
    if constexpr (acl::detail::LinearArrayLike<type, Serializer> && has_fast_path)
    {
      // NOLINTNEXTLINE
      get().write(reinterpret_cast<std::byte const*>(obj.data()), sizeof(typename Class::value_type) * count);
      return;
    }

    for (auto const& value : obj)
    {
      fn(value, *this);
    }
  }

  void visit(std::string_view str)
  {
    auto count = static_cast<uint32_t>(str.length());
    visit(count);
    // NOLINTNEXTLINE
    get().write(reinterpret_cast<std::byte const*>(str.data()), str.length());
  }

  template <acl::detail::BoolLike Class>
  auto visit(Class& obj)
  {
    // NOLINTNEXTLINE
    get().write(reinterpret_cast<std::byte*>(&obj), sizeof(obj));
  }

  template <typename Class>
    requires(acl::detail::IntegerLike<Class> || acl::detail::FloatLike<Class>)
  auto visit(Class& obj) -> bool
  {
    if constexpr (has_fast_path)
    {
      // NOLINTNEXTLINE
      return get().write(reinterpret_cast<std::byte*>(&obj), sizeof(obj));
    }
    else
    {
      // NOLINTNEXTLINE
      get().write(reinterpret_cast<std::byte*>(&obj), sizeof(obj));
      obj = byteswap(obj);
    }
  }

  void set_null()
  {
    constexpr uint8_t null_value = 0x6f;
    visit(null_value);
  }

  void set_not_null()
  {
    constexpr uint8_t not_null_value = 0x11;
    visit(not_null_value);
  }

private:
  auto write_id(uint32_t id)
  {
    visit(id);
  }

  auto get() -> auto&
  {
    assert(serializer_ != nullptr);
    return *serializer_;
  }

  auto get() const -> auto const&
  {
    assert(serializer_ != nullptr);
    return *serializer_;
  }
};

struct empty_output_streamer
{
  void write(std::byte* data, size_t s) {}
};

} // namespace acl
