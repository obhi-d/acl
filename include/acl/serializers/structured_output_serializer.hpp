
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
concept OutputSerializer = requires(V v) {
  // begin array
  v.begin_array();

  // end array
  v.end_array();

  // begin object
  v.begin_object();

  // end array
  v.end_object();

  // key
  v.key(std::string_view());

  // value
  v.as_string(std::string_view());

  v.as_uint64(uint64_t());

  v.as_int64(int64_t());

  v.as_double(double());

  v.as_bool(bool());

  v.as_null();

  // begin of next key
  v.next_map_entry();
  v.next_array_entry();
};

// Given an input serializer, load
// a bound class
template <OutputSerializer Serializer, typename Config = acl::config<>>
class strucutred_output_serializer
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
  bool        first_      = true;

public:
  auto operator=(const strucutred_output_serializer&) -> strucutred_output_serializer&     = default;
  auto operator=(strucutred_output_serializer&&) noexcept -> strucutred_output_serializer& = default;
  strucutred_output_serializer(strucutred_output_serializer const&)                        = default;
  strucutred_output_serializer(strucutred_output_serializer&& i_other) noexcept : serializer_(i_other.serializer_) {}
  strucutred_output_serializer(Serializer& ser) : serializer_(ser) {}
  ~strucutred_output_serializer() noexcept
  {
    if (serializer_)
    {
      switch (type_)
      {
      case type::object:
        serializer_->end_object();
        break;
      case type::array:
        serializer_->end_array();
        break;
      case type::field:
        break;
      }
    }
  }

  strucutred_output_serializer(acl::detail::field_visitor_tag /*unused*/, strucutred_output_serializer& ser,
                               std::string_view key)
      : serializer_{ser.serializer_}, type_{type::field}
  {
    if (ser.first_)
    {
      ser.first_ = false;
    }
    else
    {
      serializer_->next_map_entry();
    }

    if (serializer_)
    {
      serializer_->key(key);
    }
  }

  strucutred_output_serializer(acl::detail::object_visitor_tag /*unused*/, strucutred_output_serializer& ser)
      : serializer_{ser.serializer_}, type_{type::object}
  {
    if (serializer_)
    {
      serializer_->begin_object();
    }
  }

  strucutred_output_serializer(acl::detail::array_visitor_tag /*unused*/, strucutred_output_serializer& ser)
      : serializer_{ser.serializer_}, type_{type::array}
  {
    if (serializer_)
    {
      serializer_->begin_array();
    }
  }

  template <typename Class>
  auto can_visit(Class const& obj) -> continue_token
  {
    return true;
  }

  template <acl::detail::OutputSerializableClass<Serializer> T>
  void visit(T& obj)
  {
    (*serializer_) >> obj;
  }

  template <typename Class>
  void for_each_field(Class const& obj, auto&& fn)
  {
    bool first = true;
    for (auto const& [key, value] : obj)
    {
      if (!first)
      {
        get().next_map_entry();
      }
      first = false;
      get().key(acl::transform<std::decay_t<decltype(key)>>::to_string(key));
      fn(value, *this);
    }
  }

  template <acl::detail::ComplexMapLike Class>
  void for_each_entry(Class const& obj, auto&& fn)
  {

    using type = std::decay_t<Class>;
    bool first = true;
    for (auto const& [key, value] : obj)
    {
      if (!first)
      {
        get().next_array_entry();
      }
      first = false;
      fn(key, value, *this);
    }
  }

  template <acl::detail::ArrayLike Class>
  void for_each_entry(Class const& obj, auto&& fn)
  {

    bool first = true;
    for (auto const& value : obj)
    {
      if (!first)
      {
        get().next_array_entry();
      }
      first = false;
      fn(value, *this);
    }
  }

  void visit(std::string_view str)
  {
    get().as_string(str);
  }

  template <acl::detail::BoolLike Class>
  auto visit(Class& obj)
  {
    get().as_bool(obj);
  }

  template <acl::detail::IntegerLike Class>
  auto visit(Class& obj) -> bool
  {
    if constexpr (std::is_unsigned_v<Class>)
    {
      get().as_uint64(obj);
    }
    else
    {
      get().as_int64(obj);
    }
  }

  template <acl::detail::FloatLike Class>
  auto visit(Class& obj) -> bool
  {
    get().as_double(obj);
  }

  void set_null()
  {
    get().as_null();
  }

  void set_not_null() {}

private:
  auto get() -> Serializer&
  {
    assert(serializer_ != nullptr);
    return *serializer_;
  }
};

} // namespace acl
