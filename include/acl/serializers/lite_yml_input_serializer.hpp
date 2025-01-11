
#pragma once
#include <acl/allocators/linear_arena_allocator.hpp>
#include <acl/dsl/lite_yml.hpp>
#include <acl/serializers/detail/lite_yml_parser_context.hpp>
#include <acl/utility/type_traits.hpp>

namespace acl::yml
{

template <typename Class, typename Config = acl::config<>>
void from_string(Class& obj, std::string_view data)
{
  auto state    = acl::detail::parser_state(data);
  auto icontext = acl::detail::icontext<Class&, Config>(obj, state, nullptr);
  state.parse(icontext);
}

} // namespace acl::yml
