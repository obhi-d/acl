
#pragma once
#include "acl/reflection/detail/base_concepts.hpp"
#include <acl/reflection/detail/visitor_helpers.hpp>
#include <acl/reflection/visitor.hpp>

namespace acl
{

/** @see acl::visit */
template <typename Class, typename Visitor>
void visit(Class& obj, Visitor& visitor)
{
  using type            = std::decay_t<Class>;
  using visitor_type    = std::decay_t<Visitor>;
  using serializer_tag  = typename visitor_type::serializer_tag;
  using serializer_type = typename visitor_type::serializer_type;

  if constexpr (acl::detail::ExplicitlyReflected<type>)
  {
    return acl::detail::visit_explicitly_reflected(obj, visitor);
  }
  else if constexpr ((std::same_as<serializer_tag, reader_tag> &&
                      acl::detail::InputSerializableClass<type, serializer_type>) ||
                     (std::same_as<serializer_tag, writer_tag> &&
                      acl::detail::OutputSerializableClass<type, serializer_type>))
  {
    return acl::detail::visit_serializable(obj, visitor);
  }
  else if constexpr (acl::detail::Transformable<type>)
  {
    return acl::detail::visit_transformable(obj, visitor);
  }
  else if constexpr (acl::detail::TupleLike<type>)
  {
    return acl::detail::visit_tuple(obj, visitor);
  }
  else if constexpr (acl::detail::ContainerLike<type>)
  {
    return acl::detail::visit_container(obj, visitor);
  }
  else if constexpr (acl::detail::VariantLike<type>)
  {
    return acl::detail::visit_variant(obj, visitor);
  }
  else if constexpr (acl::detail::BoolLike<type> || acl::detail::IntegerLike<type> || acl::detail::FloatLike<type>)
  {
    return acl::detail::visit_value(obj, visitor);
  }
  else if constexpr (acl::detail::EnumLike<type>)
  {
    return acl::detail::visit_enum(obj, visitor);
  }
  else if constexpr (acl::detail::PointerLike<type>)
  {
    return acl::detail::visit_pointer(obj, visitor);
  }
  else if constexpr (acl::detail::OptionalLike<type>)
  {
    return acl::detail::visit_optional(obj, visitor);
  }
  else if constexpr (acl::detail::MonostateLike<type>)
  {
    return acl::detail::visit_monostate(obj, visitor);
  }
  else if constexpr (acl::detail::Aggregate<type>)
  {
    return acl::detail::visit_aggregate(obj, visitor);
  }
  else
  {
    []<bool Flag = false>()
    {
      static_assert(Flag, "This type is not serializable");
    }();
  }
}

} // namespace acl