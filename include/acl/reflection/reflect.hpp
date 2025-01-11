
#pragma once

#include <acl/reflection/bind.hpp>
#include <acl/reflection/detail/base_concepts.hpp>

namespace acl
{

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