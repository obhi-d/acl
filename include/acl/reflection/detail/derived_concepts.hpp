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
concept ExplicitlyReflected = (acl::detail::tuple_size<Class>) > 0;

template <typename Class>
concept ByteStreamable =
 std::is_standard_layout_v<Class> && std::is_trivial_v<Class> && std::has_unique_object_representations_v<Class>;

template <typename Class, typename Serializer>
concept ByteStreambleClass = ByteStreamable<Class> && !ExplicitlyReflected<Class> &&
                             !OutputSerializableClass<Class, Serializer> && !InputSerializableClass<Class, Serializer>;

template <typename Class, typename Serializer>
concept LinearArrayLike = requires(Class c) {
  typename Class::value_type;
  c.data();
  { c.size() } -> std::convertible_to<std::size_t>;
} && ByteStreambleClass<typename Class::value_type, Serializer>;

} // namespace acl::detail
