
#pragma once
#include <acl/serializers/detail/lite_yml_writer_context.hpp>
#include <string>

namespace acl::yml
{

template <typename Class, typename Config = acl::config<>>
auto to_string(Class const& obj) -> std::string
{
  auto state      = acl::detail::writer_state();
  auto serializer = output_serializer<acl::detail::writer_state, Config>(state);
  serializer << obj;
  return state.get();
}

} // namespace acl::yml
