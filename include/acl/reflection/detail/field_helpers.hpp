#pragma once

#include <acl/reflection/detail/aggregate.hpp>
#include <acl/reflection/detail/concepts.hpp>
#include <acl/reflection/detail/deduced_types.hpp>

#include <source_location>

namespace acl::detail
{

/**
 * @brief This function iterates over members registered by bind
 * @param fn A lambda accepting the instance of the object, a type info and the member index. The type info has a value
 * method to access the internal member given the instance of the object
 */
template <ExplicitlyReflected Class, typename Fn>
void for_each_field(Fn fn, Class& obj) noexcept
{
  using ClassType = std::remove_const_t<Class>;
  static_assert(tuple_size<ClassType> > 0, "Invalid tuple size");
  return [&]<std::size_t... I>(std::index_sequence<I...>, auto tup)
  {
    (fn(obj, std::get<I>(tup), std::integral_constant<std::size_t, I>()), ...);
  }(std::make_index_sequence<tuple_size<ClassType>>(), reflect<ClassType>());
}

/**
 * @brief This function iterates over members registered by bind without the class object specified
 * @param fn A lambda accepting a type info and the member index. The type info has a value method to access the
 * internal member given the instance of the object
 */
template <ExplicitlyReflected Class, typename Fn>
void for_each_field(Fn fn) noexcept
{
  using ClassType = std::remove_const_t<Class>;
  static_assert(tuple_size<ClassType> > 0, "Invalid tuple size");
  return [&]<std::size_t... I>(std::index_sequence<I...>, auto tup)
  {
    (fn(std::get<I>(tup), std::integral_constant<std::size_t, I>()), ...);
  }(std::make_index_sequence<tuple_size<ClassType>>(), reflect<ClassType>());
}

template <ExplicitlyReflected Class, std::size_t I>
constexpr auto field_at() noexcept
{
  using ClassType = std::remove_const_t<Class>;
  static_assert(tuple_size<ClassType> > 0, "Invalid tuple size");
  return std::get<I>(reflect<ClassType>());
}

template <ExplicitlyReflected ClassType>
constexpr auto field_size() noexcept -> uint32_t
{
  return tuple_size<ClassType>;
}

template <typename T>
struct field_ref
{
  // NOLINTNEXTLINE
  T& member_;
};

template <class T>
field_ref(T&) -> field_ref<T>;

template <auto A>
[[nodiscard]] constexpr auto function_name() noexcept -> std::string_view
{
  return std::source_location::current().function_name();
}

template <auto M>
constexpr auto deduce_field_name() -> std::string_view
{
  auto name = std::string_view{std::source_location::current().function_name()};
  auto pos  = name.find_last_of('&');
  pos       = name.find("::", pos);
  if (pos != std::string_view::npos)
  {
    pos += 2;
    auto next = name.find_first_of(";]>", pos);
    return name.substr(pos, next - pos);
  }
  assert(false && "Failed to deduce field name");
  return {};
}

template <Aggregate T>
constexpr auto get_field_names() noexcept -> decltype(auto)
{
  return aggregate_lookup<T>(
   [](auto&&... args) constexpr
   {
     return std::make_tuple(deduce_field_name<field_ref{args}>()...);
   });
}

} // namespace acl::detail