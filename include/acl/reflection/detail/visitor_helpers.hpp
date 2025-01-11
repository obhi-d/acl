
#pragma once
#include <acl/reflection/detail/aggregate.hpp>
#include <acl/reflection/detail/base_concepts.hpp>
#include <acl/reflection/detail/container_utils.hpp>
#include <acl/reflection/detail/map_value_type.hpp>
#include <acl/reflection/visitor.hpp>
#include <acl/utility/transforms.hpp>
#include <concepts>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace acl::detail
{

template <typename Class, typename Visitor, typename Decl, std::size_t I>
void process_field(Class& obj, Visitor& visitor, Decl const& decl, std::integral_constant<std::size_t, I> /*unused*/)
{
  using visitor_type   = std::decay_t<Visitor>;
  using serializer_tag = typename visitor_type::serializer_tag;
  using value_t        = typename Decl::MemTy;

  if (!visitor.begin_field(decl.key(), I != 0))
  {
    return;
  }

  if constexpr (std::same_as<serializer_tag, reader_tag>)
  {
    value_t load;
    if (visit(load, visitor))
    {
      decl.value(obj, std::move(load));
    }
  }
  else if constexpr (std::same_as<serializer_tag, writer_tag>)
  {
    visit(decl.value(obj), visitor);
  }

  visitor.end_field();
}

template <typename Class, typename Visitor>
void process_field(Class& obj, Visitor& visitor, std::string_view field, bool first)
{
  using visitor_type   = std::decay_t<Visitor>;
  using serializer_tag = typename visitor_type::serializer_tag;

  if (!visitor.begin_field(field, first))
  {
    return;
  }

  visit(obj, visitor);
  visitor.end_field();
}

template <typename Class, typename Visitor>
void visit_explicitly_reflected(Class& obj, Visitor& visitor)
{
  if (!visitor.begin_object(obj))
  {
    return;
  }

  for_each_field(
   [&]<typename Decl, std::size_t I>(Class& obj, Decl const& decl, std::integral_constant<std::size_t, I> field_idx)
   {
     process_field(obj, visitor, decl, field_idx);
   },
   obj);

  visitor.end_object(obj);
}

template <typename Class, typename Visitor>
void visit_transformable(Class& obj, Visitor& visitor)
{
  using visitor_type   = std::decay_t<Visitor>;
  using serializer_tag = typename visitor_type::serializer_tag;

  if constexpr (std::same_as<serializer_tag, reader_tag>)
  {
    visitor.read_string(
     [&](std::string_view str) -> void
     {
       acl::transform<std::decay_t<Class>>::from_string(obj, str);
     });
  }
  else if constexpr (std::same_as<serializer_tag, writer_tag>)
  {
    visitor.write_string(acl::transform<std::decay_t<Class>>::to_string(obj));
  }
}

template <typename Class, typename Visitor>
void visit_serializable(Class& obj, Visitor& visitor)
{
  visitor.serailize(obj);
}

template <typename Class, typename Visitor>
void visit_tuple(Class& obj, Visitor& visitor)
{
  if (!visitor.begin_array(obj))
  {
    return;
  }

  bool first      = false;
  auto visit_item = [&](auto&& arg) -> void
  {
    if (!visitor.next_array_entry(std::exchange(first, false)))
    {
      return;
    }

    visit(std::forward<decltype(arg)>(arg), visitor);
  };

  std::apply(
   [&]<typename... Args>(Args&&... args)
   {
     (visit_item(std::forward<Args>(args)), ...);
   },
   obj);

  visitor.end_array(obj);
}

template <detail::StringMapLike Class, typename Visitor>
  requires(std::same_as<typename Visitor::serializer_tag, reader_tag>)
void visit_container(Class& obj, Visitor& visitor)
{
  using key_type    = std::decay_t<typename Class::key_type>;
  using mapped_type = std::decay_t<typename Class::mapped_type>;

  if (!visitor.begin_object(obj))
  {
    return;
  }

  bool first = true;

  visitor.for_each_map_entry(
   [&](std::string_view key)
   {
     if (!visitor.begin_field(key, std::exchange(first, false)))
     {
       return;
     }

     mapped_type stream_val;

     visit(stream_val, visitor);
     detail::emplace(obj, key, std::move(stream_val));

     visitor.end_field();
   });

  visitor.end_object(obj);
}

template <detail::StringMapLike Class, typename Visitor>
  requires(std::same_as<typename Visitor::serializer_tag, writer_tag>)
void visit_container(Class const& obj, Visitor& visitor)
{
  using key_type    = std::decay_t<typename Class::key_type>;
  using mapped_type = std::decay_t<typename Class::mapped_type>;

  if (!visitor.begin_object(obj))
  {
    return;
  }

  bool first = true;

  for (auto const& [key, value] : obj)
  {
    process_field(obj, visitor, key, std::exchange(first, false));
  }

  visitor.end_object(obj);
}

template <detail::ComplexMapLike Class, typename Visitor>
  requires(std::same_as<typename Visitor::serializer_tag, reader_tag>)
void visit_container(Class& obj, Visitor& visitor)
{
  using key_type     = std::decay_t<typename Class::key_type>;
  using mapped_type  = std::decay_t<typename Class::mapped_type>;
  using options_type = std::conditional_t<HasOptions<Visitor>, typename Visitor::options_t, options<>>;

  if (!visitor.begin_array(obj))
  {
    return;
  }

  visitor.for_each_array_entry(
   [&]()
   {
     key_type                                                    key;
     mapped_type                                                 value;
     detail::map_value_type<key_type, mapped_type, options_type> stream_val{&key, &value};

     visit(stream_val, visitor);

     detail::emplace(obj, std::move(key), std::move(value));
   });

  visitor.end_array(obj);
}

template <detail::ComplexMapLike Class, typename Visitor>
  requires(std::same_as<typename Visitor::serializer_tag, writer_tag>)
void visit_container(Class const& obj, Visitor& visitor)
{
  using key_type     = std::decay_t<typename Class::key_type>;
  using mapped_type  = std::decay_t<typename Class::mapped_type>;
  using options_type = std::conditional_t<HasOptions<Visitor>, typename Visitor::options_t, options<>>;

  if (!visitor.begin_array(obj))
  {
    return;
  }

  for (auto const& [key, value] : obj)
  {
    detail::map_value_type<key_type const, mapped_type const, options_type> stream_val{&key, &value};

    visit(stream_val, visitor);
  }

  visitor.end_array(obj);
}

template <detail::ArrayLike Class, typename Visitor>
  requires(std::same_as<typename Visitor::serializer_tag, reader_tag>)
void visit_container(Class& obj, Visitor& visitor)
{
  using value_type = std::decay_t<typename Class::value_type>;

  if (!visitor.begin_array(obj))
  {
    return;
  }

  visitor.for_each_array_entry(
   [&]() -> continue_token
   {
     value_type stream_val;

     visit(stream_val, visitor);

     detail::emplace(obj, std::move(stream_val));
   });

  visitor.end_array(obj);
}

template <typename Class, typename Visitor>
  requires(std::same_as<typename Visitor::serializer_tag, reader_tag>)
void visit_variant(Class& obj, Visitor& visitor)
{
  using type = std::decay_t<Class>;
  if (!visitor.begin_object(obj))
  {
    return;
  }

  visitor.for_each_map_entry(
   [&](std::string_view key)
   {
     if (!visitor.begin_field(key, true))
     {
       return;
     }

     auto index = index_transform<type>::to_index(key);

     auto emplace_item = [&]<std::size_t I>(std::integral_constant<std::size_t, I>)
     {
       std::variant_alternative_t<I, Class> value;
       visit(value, visitor);
       obj = std::move(value);
     };

     [&]<std::size_t... I>(std::index_sequence<I...>)
     {
       (((I == index ? (emplace_item(std::integral_constant<std::size_t, I>()), true) : false) || ...), false);
     }(std::make_index_sequence<std::variant_size_v<type>>{});

     visitor.end_field();
   });

  visitor.end_field();

  visitor.end_object(obj);
}

template <typename Class, typename Visitor>
  requires(std::same_as<typename Visitor::serializer_tag, writer_tag>)
void visit_variant(Class& obj, Visitor& visitor)
{
  using type = std::decay_t<Class>;
  if (!visitor.begin_object(obj))
  {
    return;
  }

  auto index = obj.index();
  auto key   = index_transform<type>::from_index(index);

  visitor.begin_field(key, true);
  std::visit(
   [&](auto const& value)
   {
     visit(value, visitor);
   },
   obj);
  visitor.end_field();
  visitor.end_object(obj);
}

template <typename Class, typename Visitor>
void visit_value(Class& obj, Visitor& visitor)
{
  visitor.value(obj);
}

template <typename Class, typename Visitor>
void visit_enum(Class& obj, Visitor& visitor)
{
  visitor.value(static_cast<std::underlying_type_t<std::decay_t<Class>>>(obj));
}

template <typename Class, typename Visitor>
  requires(std::same_as<typename Visitor::serializer_tag, reader_tag>)
void visit_pointer(Class& obj, Visitor& visitor)
{
  if (visitor.is_null())
  {
    obj = nullptr;
    return;
  }

  if (!obj)
  {
    using class_type  = std::decay_t<Class>;
    using pvalue_type = detail::pointer_class_type<Class>;

    if constexpr (is_specialization_of<std::shared_ptr, class_type>::value)
    {
      obj = std::make_shared<pvalue_type>();
    }
    else
    {
      obj = std::make_unique<std::decay_t<Class>>();
    }
  }

  visit(*obj, visitor);
}

template <typename Class, typename Visitor>
  requires(std::same_as<typename Visitor::serializer_tag, writer_tag>)
void visit_pointer(Class& obj, Visitor& visitor)
{
  if (!obj)
  {
    visitor.null();
    return;
  }

  visit(*obj, visitor);
}

template <typename Class, typename Visitor>
  requires(std::same_as<typename Visitor::serializer_tag, reader_tag>)
void visit_optional(Class& obj, Visitor& visitor)
{
  if (visitor.is_null())
  {
    obj = nullptr;
    return;
  }

  if (!obj)
  {
    obj.emplace();
  }

  visit(*obj, visitor);
}

template <typename Class, typename Visitor>
  requires(std::same_as<typename Visitor::serializer_tag, writer_tag>)
void visit_optional(Class& obj, Visitor& visitor)
{
  if (!obj)
  {
    visitor.null();
    return;
  }

  visit(*obj, visitor);
}

template <typename Class, typename Visitor>
void visit_monostate(Class& obj, Visitor& visitor)
{
  if constexpr (std::is_same_v<typename Visitor::serializer_tag, writer_tag>)
  {
    visitor.null();
  }
}
template <typename Class, typename Visitor>
void visit_aggregate(Class& obj, Visitor& visitor)
{
  if (!visitor.begin_object(obj))
  {
    return;
  }

  constexpr auto field_names = get_field_names<Class>();
  constexpr auto refs        = get_field_refs(obj);

  [&]<std::size_t... I>(std::index_sequence<I...>)
  {
    ((process_field(std::get<I>(refs), visitor, std::get<I>(field_names), I == 0)), ...);
  }(std::make_index_sequence<std::tuple_size_v<decltype(field_names)>>());

  visitor.end_object(obj);
}
} // namespace acl::detail