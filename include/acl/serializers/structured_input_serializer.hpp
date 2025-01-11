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

#include <cassert>

namespace acl
{

template <typename V>
concept InputSerializer = requires(V v) {
  // function: Must return object_type
  { v.is_object() } -> ::std::same_as<bool>;

  // function: Must return object_type
  { v.is_array() } -> ::std::same_as<bool>;

  // function: Must return object_type
  { v.is_null() } -> ::std::same_as<bool>;

  // size
  { v.size() } -> ::std::convertible_to<std::size_t>;

  // function for_each: Should accept a lambda that accepts a key and value_type
  // This function should consume a field that is a map of key, values
  {
    v.for_each(
     [](::std::string_view key, V) -> bool
     {
       return false;
     })
  } -> std::same_as<void>;

  // function for_each: Should accept a lambda that accepts a value_type
  // This function should consume a field that is an array of values
  {
    v.for_each(
     [](V) -> bool
     {
       return false;
     })
  } -> std::same_as<void>;

  // function object: Must return object_type
  { v.at(::std::string_view()) } -> acl::detail::OptionalValueLike;

  // function object: Must return object_type given an array index
  { v.at(uint32_t(0)) } -> acl::detail::OptionalValueLike;

  // Must convert value_type to double
  { v.as_double() } -> acl::detail::OptionalValueLike;

  // Must convert value_type to float
  { v.as_uint64() } -> acl::detail::OptionalValueLike;

  // Must convert value_type to float
  { v.as_int64() } -> acl::detail::OptionalValueLike;

  // Must convert value_type to float
  { v.as_bool() } -> acl::detail::OptionalValueLike;

  // Must convert value_type to float
  { v.as_string() } -> acl::detail::OptionalValueLike;
};

// Given an input serializer, load
// a bound class
template <InputSerializer Serializer, typename Config = acl::config<>>
class structured_input_serializer
{

private:
  Serializer* serializer_ = nullptr;

public:
  auto operator=(const structured_input_serializer&) -> structured_input_serializer& = default;
  auto operator=(structured_input_serializer&&) -> structured_input_serializer&      = default;
  structured_input_serializer(structured_input_serializer const&) noexcept           = default;
  structured_input_serializer(structured_input_serializer&& i_other) noexcept : serializer_(i_other.serializer_) {}
  structured_input_serializer(Serializer& ser) noexcept : serializer_(&ser) {}
  ~structured_input_serializer() noexcept = default;

  structured_input_serializer(acl::detail::field_visitor_tag /*unused*/, structured_input_serializer& ser,
                              std::string_view key)

  {
    auto ref = ser.get().at(key);
    if (ref)
    {
      serializer_ = &*ref;
    }
  }

  structured_input_serializer(acl::detail::object_visitor_tag /*unused*/, structured_input_serializer& ser)
      : serializer_{ser.serializer_}
  {
    // No-op
    if (serializer_ != nullptr)
    {
      if (!serializer_->is_object())
      {
        serializer_ = nullptr;
      }
    }
  }

  structured_input_serializer(acl::detail::array_visitor_tag /*unused*/, structured_input_serializer& ser)
      : serializer_{ser.serializer_}
  {
    // No-op
    if (serializer_ != nullptr)
    {
      if (!serializer_->is_array())
      {
        serializer_ = nullptr;
      }
    }
  }

  template <typename Class>
  auto can_visit(Class& obj) -> continue_token
  {
    return serializer_ != nullptr;
  }

  void visit(auto&& fn)
  {
    fn(get().as_string());
  }

  template <acl::detail::InputSerializableClass<Serializer> T>
  void visit(T& obj)
  {
    (*serializer_) >> obj;
  }

  void for_each_field(auto&& fn)
  {
    get().for_each(
     [&](std::string_view key, Serializer& value)
     {
       fn(key, value, *this);
     });
  }

  template <typename Class>
  void for_each_entry(Class& obj, auto&& fn)
  {
    get().for_each(
     [&](Serializer& value)
     {
       fn(value, *this);
     });
  }

  template <acl::detail::BoolLike Class>
  auto visit(Class& obj)
  {
    obj = get().as_bool().value_or(false);
  }

  template <acl::detail::IntegerLike Class>
  auto visit(Class& obj) -> bool
  {
    if constexpr (std::is_unsigned_v<Class>)
    {
      obj = static_cast<std::decay_t<Class>>(get().as_uint64().value_or(0));
    }
    else
    {
      obj = static_cast<std::decay_t<Class>>(get().as_int64().value_or(0));
    }
  }

  template <acl::detail::FloatLike Class>
  auto visit(Class& obj) -> bool
  {
    obj = static_cast<std::decay_t<Class>>(get().as_double().value_or(0.0));
  }

  [[nodiscard]] auto is_null() const -> bool
  {
    return serializer_ == nullptr || get().is_null();
  }

private:
  auto get() -> Serializer&
  {
    assert(serializer_ != nullptr);
    return *serializer_;
  }
};

} // namespace acl
