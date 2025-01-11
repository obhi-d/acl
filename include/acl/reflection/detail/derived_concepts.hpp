#pragma once

#include <acl/reflection/reflect.hpp>

namespace acl::detail
{
// Concepts
// Decl
template <typename Class>
using bind_type = decltype(reflect<Class>());

// Utils
template <typename Class>
inline constexpr std::size_t tuple_size = tuple_size<bind_type<std::decay_t<Class>>>;

template <typename Class>
concept ExplicitlyReflected = (detail::tuple_size<Class>) > 0;

template <typename Class, typename Serializer>
concept LinearArrayLike =
 requires(Class c) {
   typename Class::value_type;
   c.data();
   { c.size() } -> std::convertible_to<std::size_t>;
 } && std::is_standard_layout_v<typename Class::value_type> &&
 std::is_trivially_copyable_v<typename Class::value_type> &&
 std::has_unique_object_representations_v<typename Class::value_type> &&
 !ExplicitlyReflected<typename Class::value_type> && !OutputSerializableClass<typename Class::value_type, Serializer> &&
 !InputSerializableClass<typename Class::value_type, Serializer>;

} // namespace acl::detail
