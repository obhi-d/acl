//
// Created by obhi on 9/18/20.
//
#pragma once

#include <acl/reflection/detail/container_utils.hpp>
#include <acl/reflection/detail/derived_concepts.hpp>
#include <acl/reflection/detail/visitor_helpers.hpp>
#include <acl/reflection/reflection.hpp>
#include <acl/reflection/type_name.hpp>
#include <acl/reflection/visitor.hpp>
#include <acl/serializers/byteswap.hpp>

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace acl
{

template <typename V>
concept BinaryInputStream = requires(V v, std::size_t N) {
  // function: Must have a read function to read bytes
  { v.read(std::declval<std::byte*>(), N) } -> std::same_as<void>;
};

/**
 * @brief Given an input serializer, load a bound class
 * @note The endian parameter should match between output and input
 */
template <BinaryInputStream Serializer, std::endian Endian = std::endian::little>
class binary_input_serializer
{
private:
  enum class type : uint8_t
  {
    object,
    array,
    field
  };

  Serializer* serializer_ = nullptr;
  uint32_t    object_id_  = 0;
  type        type_       = type::object;

  static constexpr bool has_fast_path = (Endian == std::endian::native);

public:
  using serializer_type = Serializer;
  using serializer_tag  = reader_tag;

  auto operator=(const binary_input_serializer&) -> binary_input_serializer&     = default;
  auto operator=(binary_input_serializer&&) noexcept -> binary_input_serializer& = default;
  binary_input_serializer(binary_input_serializer const&)                        = default;
  binary_input_serializer(binary_input_serializer&& i_other) noexcept            = default;
  binary_input_serializer(Serializer& ser) : serializer_{&ser} {}
  ~binary_input_serializer() noexcept = default;

  binary_input_serializer(acl::detail::field_visitor_tag /*unused*/, binary_input_serializer& ser, std::string_view key)
      : serializer_{ser.serializer_}, object_id_(ser.object_id_), type_{type::field}
  {
    // No-op
  }

  binary_input_serializer(acl::detail::object_visitor_tag /*unused*/, binary_input_serializer& ser)
      : serializer_{ser.serializer_}, object_id_(ser.object_id_), type_{type::object}
  {
    // No-op
  }

  binary_input_serializer(acl::detail::array_visitor_tag /*unused*/, binary_input_serializer& ser)
      : serializer_{ser.serializer_}, object_id_(ser.object_id_), type_{type::array}
  {
    // No-op
  }

  template <typename Class>
  auto can_visit(Class& obj) -> continue_token
  {
    using type = std::decay_t<Class>;
    if constexpr (acl::detail::ByteStreambleClass<Class, Serializer>)
    {
      return true;
    }
    if (type_ == type::object)
    {
      constexpr uint32_t match_id = type_name<type>().hash();
      return (read_id() == match_id);
    }
    return true;
  }

  void visit(auto&& fn)
  {
    fn(read_string());
  }

  template <acl::detail::InputSerializableClass<Serializer> T>
  void visit(T& obj)
  {
    (*serializer_) >> obj;
  }

  void for_each_field(auto&& fn)
  {
    uint32_t count = 0;
    visit(count);
    for (uint32_t i = 0; i < count; ++i)
    {
      fn(read_string(), *this);
    }
  }

  template <typename Class>
  void for_each_entry(Class& obj, auto&& fn)
  {
    uint32_t count = 0;
    visit(count);
    acl::detail::reserve(obj, count);

    using type = std::decay_t<Class>;
    if constexpr (acl::detail::LinearArrayLike<type, Serializer> && has_fast_path)
    {
      acl::detail::resize(obj, count);
      // NOLINTNEXTLINE
      get().read(reinterpret_cast<std::byte*>(obj.data()), sizeof(typename Class::value_type) * count);
      return;
    }
    else if constexpr (!acl::detail::ContainerCanAppendValue<Class>)
    {
      acl::detail::resize(obj, count);
    }

    for (uint32_t i = 0; i < count; ++i)
    {
      fn(*this);
    }
  }

  template <acl::detail::BoolLike Class>
  auto visit(Class& obj)
  {
    // NOLINTNEXTLINE
    get().read(reinterpret_cast<std::byte*>(&obj), sizeof(obj));
  }

  template <typename Class>
    requires(acl::detail::IntegerLike<Class> || acl::detail::FloatLike<Class>)
  auto visit(Class& obj) -> bool
  {
    if constexpr (has_fast_path)
    {
      // NOLINTNEXTLINE
      return get().read(reinterpret_cast<std::byte*>(&obj), sizeof(obj));
    }
    else
    {
      // NOLINTNEXTLINE
      get().read(reinterpret_cast<std::byte*>(&obj), sizeof(obj));
      obj = byteswap(obj);
    }
  }

  [[nodiscard]] auto is_null() const -> bool
  {
    constexpr uint8_t null_value = 0x6f;
    uint8_t           value      = 0;
    visit(value);
    return value == null_value;
  }

private:
  auto read_id() -> uint32_t
  {
    if (object_id_ == 0)
    {
      visit(object_id_);
    }
    return object_id_;
  }

  auto read_string() -> std::string
  {
    uint32_t count = 0;
    visit(count);
    std::string buffer;
    buffer.resize(count);
    // NOLINTNEXTLINE
    serializer_->read(reinterpret_cast<std::byte*>(buffer.data()), count);
    return buffer;
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

struct empty_input_streamer
{
  static void read(std::byte* data, size_t s) {}
};

} // namespace acl
