
#pragma once

#include <acl/reflection/detail/aggregate.hpp>
#include <acl/reflection/detail/base_concepts.hpp>
#include <acl/reflection/detail/container_utils.hpp>
#include <acl/reflection/detail/map_value_type.hpp>
#include <acl/reflection/visitor.hpp>
#include <acl/utility/config.hpp>
#include <acl/utility/transforms.hpp>
#include <concepts>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

namespace acl::detail
{
template <typename Visitor>
constexpr bool is_reader = std::same_as<typename Visitor::serializer_tag, reader_tag>;
template <typename Visitor>
constexpr bool is_writer = std::same_as<typename Visitor::serializer_tag, writer_tag>;

struct field_visitor_tag
{};
struct object_visitor_tag
{};
struct array_visitor_tag
{};

template <typename Class, typename Visitor, typename Decl>
void process_field(Class& obj, Visitor& visitor, Decl const& decl)
{
  using value_t = typename Decl::MemTy;

  Visitor field_visitor{field_visitor_tag{}, visitor, decl.key()};

  if (!field_visitor.can_visit(obj))
  {
    return;
  }

  if constexpr (is_reader<Visitor>)
  {
    value_t load;
    visit(load, field_visitor);
    decl.value(obj, std::move(load));
  }
  else if constexpr (is_writer<Visitor>)
  {
    visit(decl.value(obj), field_visitor);
  }
}

template <typename Class, typename Visitor>
void visit_explicitly_reflected(Class& obj, Visitor& visitor)
{

  Visitor object_visitor{object_visitor_tag{}, visitor};

  if (!object_visitor.can_visit(obj))
  {
    return;
  }

  for_each_field(
   [&]<typename Decl, std::size_t I>(Class& obj, Decl const& decl, std::integral_constant<std::size_t, I> /*unused*/)
   {
     process_field(obj, object_visitor, decl);
   },
   obj);
}

template <typename Class, typename Visitor>
void visit_transformable(Class& obj, Visitor& visitor)
{
  if constexpr (is_reader<Visitor>)
  {

    visitor.visit(
     [&](std::string_view str) -> void
     {
       acl::transform<std::decay_t<Class>>::from_string(obj, str);
     });
  }
  else if constexpr (is_writer<Visitor>)
  {
    visitor.visit(acl::transform<std::decay_t<Class>>::to_string(obj));
  }
}

template <typename Class, typename Visitor>
void visit_serializable(Class& obj, Visitor& visitor)
{
  visitor.visit(obj);
}

template <typename Class, typename Visitor>
void visit_tuple(Class& obj, Visitor& visitor)
{
  Visitor array_visitor{array_visitor_tag{}, visitor};
  if (!array_visitor.can_visit(obj))
  {
    return;
  }

  std::apply(
   [&]<typename... Args>(Args&&... arg)
   {
     (visit(std::forward<decltype(arg)>(arg), array_visitor), ...);
   },
   obj);
}

template <acl::detail::StringMapLike Class, typename Visitor>
  requires(is_reader<Visitor>)
void visit_container(Class& obj, Visitor& visitor)
{
  using key_type    = std::decay_t<typename Class::key_type>;
  using mapped_type = std::decay_t<typename Class::mapped_type>;

  Visitor object_visitor{object_visitor_tag{}, visitor};

  if (!object_visitor.can_visit(obj))
  {
    return;
  }

  object_visitor.for_each_field(obj,
                                [&](std::string_view skey, auto& field_visitor)
                                {
                                  mapped_type stream_val;
                                  key_type    key;

                                  visit(stream_val, field_visitor);
                                  acl::transform<mapped_type>::from_string(stream_val, skey);
                                  acl::detail::emplace(obj, std::move(key), std::move(stream_val));
                                });
}

template <acl::detail::StringMapLike Class, typename Visitor>
  requires(is_writer<Visitor>)
void visit_container(Class const& obj, Visitor& visitor)
{
  using key_type    = std::decay_t<typename Class::key_type>;
  using mapped_type = std::decay_t<typename Class::mapped_type>;

  Visitor object_visitor{object_visitor_tag{}, visitor};

  if (!object_visitor.can_visit(obj))
  {
    return;
  }

  object_visitor.for_each_field(obj,
                                [&](mapped_type const& entry, auto& field_visitor)
                                {
                                  visit(entry, field_visitor);
                                });
}

template <acl::detail::ComplexMapLike Class, typename Visitor>
  requires(is_reader<Visitor>)
void visit_container(Class& obj, Visitor& visitor)
{
  using key_type     = std::decay_t<typename Class::key_type>;
  using mapped_type  = std::decay_t<typename Class::mapped_type>;
  using options_type = std::conditional_t<HasConfig<Visitor>, typename Visitor::config_type, config<>>;

  Visitor array_visitor{array_visitor_tag{}, visitor};

  if (!array_visitor.can_visit(obj))
  {
    return;
  }

  array_visitor.for_each_entry(obj,
                               [&](auto& field_visitor)
                               {
                                 key_type                                                         key;
                                 mapped_type                                                      value;
                                 acl::detail::map_value_type<key_type, mapped_type, options_type> stream_val{&key,
                                                                                                             &value};

                                 visit(stream_val, field_visitor);

                                 acl::detail::emplace(obj, std::move(key), std::move(value));
                               });

  visitor.end_array(obj);
}

template <acl::detail::ComplexMapLike Class, typename Visitor>
  requires(is_writer<Visitor>)
void visit_container(Class const& obj, Visitor& visitor)
{
  using key_type     = std::decay_t<typename Class::key_type>;
  using mapped_type  = std::decay_t<typename Class::mapped_type>;
  using options_type = std::conditional_t<HasConfig<Visitor>, typename Visitor::config_type, config<>>;

  Visitor array_visitor{array_visitor_tag{}, visitor};

  if (!array_visitor.can_visit(obj))
  {
    return;
  }

  array_visitor.for_each_entry(
   obj,
   [&](key_type const& key, mapped_type const& value, auto& field_visitor)
   {
     acl::detail::map_value_type<key_type const, mapped_type const, options_type> stream_val{&key, &value};

     visit(stream_val, field_visitor);
   });
}

template <acl::detail::ArrayLike Class, typename Visitor>
  requires(is_reader<Visitor>)
void visit_container(Class& obj, Visitor& visitor)
{
  using value_type = std::decay_t<typename Class::value_type>;

  Visitor array_visitor{array_visitor_tag{}, visitor};

  if (!array_visitor.can_visit(obj))
  {
    return;
  }

  array_visitor.for_each_entry(obj,
                               [&](auto& field_visitor)
                               {
                                 value_type stream_val;

                                 visit(stream_val, field_visitor);

                                 acl::detail::emplace(obj, std::move(stream_val));
                               });
}
template <acl::detail::ArrayLike Class, typename Visitor>
  requires(is_writer<Visitor>)
void visit_container(Class& obj, Visitor& visitor)
{
  using value_type = std::decay_t<typename Class::value_type>;

  Visitor array_visitor{array_visitor_tag{}, visitor};

  if (!array_visitor.can_visit(obj))
  {
    return;
  }

  array_visitor.for_each_entry(obj,
                               [&](value_type const& stream_val, auto& field_visitor)
                               {
                                 visit(stream_val, field_visitor);

                                 acl::detail::emplace(obj, std::move(stream_val));
                               });
}

template <typename Class, typename Visitor>
  requires(is_reader<Visitor>)
void visit_variant(Class& obj, Visitor& visitor)
{
  using type = std::decay_t<Class>;
  Visitor object_visitor{object_visitor_tag{}, visitor};

  if (!object_visitor.can_visit(obj))
  {
    return;
  }

  Visitor field_visitor{field_visitor_tag{}, object_visitor};

  auto key = field_visitor.key();

  auto index = index_transform<type>::to_index(key);

  auto emplace_item = [&]<std::size_t I>(std::integral_constant<std::size_t, I>)
  {
    std::variant_alternative_t<I, Class> value;
    visit(value, field_visitor);
    obj = std::move(value);
  };

  [&]<std::size_t... I>(std::index_sequence<I...>)
  {
    (((I == index ? (emplace_item(std::integral_constant<std::size_t, I>()), true) : false) || ...), false);
  }(std::make_index_sequence<std::variant_size_v<type>>{});
}

template <typename Class, typename Visitor>
  requires(is_writer<Visitor>)
void visit_variant(Class& obj, Visitor& visitor)
{
  using type = std::decay_t<Class>;
  Visitor object_visitor{object_visitor_tag{}, visitor};

  if (!object_visitor.can_visit(obj))
  {
    return;
  }

  auto index = obj.index();
  auto key   = index_transform<type>::from_index(index);

  Visitor field_visitor{field_visitor_tag{}, object_visitor, key};

  std::visit(
   [&](auto const& value)
   {
     visit(value, field_visitor);
   },
   obj);
}

template <typename Class, typename Visitor>
void visit_value(Class& obj, Visitor& visitor)
{
  visitor.visit(obj);
}

template <typename Class, typename Visitor>
void visit_enum(Class& obj, Visitor& visitor)
{
  visitor.visit(static_cast<std::underlying_type_t<std::decay_t<Class>>>(obj));
}

template <typename Class, typename Visitor>
  requires(is_reader<Visitor>)
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
    using pvalue_type = acl::detail::pointer_class_type<Class>;

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
  requires(is_writer<Visitor>)
void visit_pointer(Class& obj, Visitor& visitor)
{
  if (!obj)
  {
    visitor.set_null();
    return;
  }

  visitor.set_not_null();
  visit(*obj, visitor);
}

template <typename Class, typename Visitor>
  requires(is_reader<Visitor>)
void visit_optional(Class& obj, Visitor& visitor)
{
  if (visitor.is_null())
  {
    obj = nullptr;
    return;
  }

  visitor.set_not_null();

  if (!obj)
  {
    obj.emplace();
  }

  visit(*obj, visitor);
}

template <typename Class, typename Visitor>
  requires(is_writer<Visitor>)
void visit_optional(Class& obj, Visitor& visitor)
{
  if (!obj)
  {
    visitor.set_null();
    return;
  }

  visitor.set_not_null();

  visit(*obj, visitor);
}

template <typename Class, typename Visitor>
void visit_monostate(Class& obj, Visitor& visitor)
{
  if constexpr (is_writer<Visitor>)
  {
  }
}

template <typename Class, typename Visitor>
void visit_aggregate(Class& obj, Visitor& visitor)
{
  Visitor object_visitor{object_visitor_tag{}, visitor};

  if (!object_visitor.can_visit(obj))
  {
    return;
  }

  constexpr auto field_names = get_field_names<Class>();
  constexpr auto refs        = get_field_refs(obj);

  [&]<std::size_t... I>(std::index_sequence<I...>)
  {
    ((process_field(obj, std::get<I>(refs), object_visitor, std::get<I>(field_names))), ...);
  }(std::make_index_sequence<std::tuple_size_v<decltype(field_names)>>());
}
} // namespace acl::detail