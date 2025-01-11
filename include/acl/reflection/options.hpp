#pragma once

#include <acl/reflection/detail/deduced_types.hpp>

namespace acl::opt
{
/**
 * @brief Option to change the `key` field name in a map-like object
 *
 * This template struct creates a type alias for a field name using a compile-time
 * string literal. It is used in reflection mechanisms to store and reference
 * field names at compile time.
 *
 * @tparam Name A string_literal representing the field name
 *
 * @note This struct is typically used in reflection and meta-programming contexts
 *       where field names need to be known at compile time.
 */
template <string_literal Name>
struct key_field_name
{
  using key_field_name_t = detail::field_name<Name>;
};

/**
 * @brief Option to change the `value` field name in a map-like object
 *
 * This template struct creates a type alias for a field name using a compile-time
 * string literal. It is used in reflection mechanisms to store and reference
 * field names at compile time.
 *
 * @tparam Name A string_literal representing the field name
 *
 * @note This struct is typically used in reflection and meta-programming contexts
 *       where field names need to be known at compile time.
 */
template <string_literal Name>
struct value_field_name
{
  using value_field_name_t = detail::field_name<Name>;
};

} // namespace acl::opt