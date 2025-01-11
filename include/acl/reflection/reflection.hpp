
#pragma once

#include <acl/reflection/bind.hpp>
#include <acl/reflection/detail/base_concepts.hpp>

namespace acl
{

/**
 * @file reflection.hpp
 * @brief Provides compile-time reflection utilities for user-defined types.
 *
 * This header defines various concepts, utility types, and functions intended to enable
 * reflection of struct members, free functions, and class accessors. It is designed to
 * serve as a customization point for types, allowing them to be:
 *   - Bound for static introspection via @c bind(...)
 *   - Serialized, deserialized, or otherwise processed based on compile-time type properties
 */

/**
 * @brief Reflects on a class type to obtain its metadata.
 *
 * This function template provides reflection capabilities for any class type.
 * If the class has a static reflect() member function, it calls that function.
 * Otherwise, it returns an empty tuple.
 *
 * @tparam Class The class type to reflect on. Defaults to void.
 *
 * @return If Class has a static reflect() member function, returns its result.
 *         Otherwise, returns an empty std::tuple.
 *
 * @note This function is marked noexcept and will be evaluated at compile-time
 *       through if constexpr.
 */
template <typename Class = void>
auto reflect() noexcept -> decltype(auto)
{
  if constexpr (requires { Class::reflect(); })
  {
    return Class::reflect();
  }
  return std::tuple<>();
}

} // namespace acl