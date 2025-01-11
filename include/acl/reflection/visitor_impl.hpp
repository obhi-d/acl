
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
    return detail::visit_explicitly_reflected(obj, visitor);
  }
  else if constexpr ((std::same_as<serializer_tag, reader_tag> &&
                      detail::InputSerializableClass<type, serializer_type>) ||
                     (std::same_as<serializer_tag, writer_tag> &&
                      detail::OutputSerializableClass<type, serializer_type>))
  {
    return detail::visit_serializable(obj, visitor);
  }
  else if constexpr (detail::Transformable<type>)
  {
    return detail::visit_transformable(obj, visitor);
  }
  else if constexpr (detail::TupleLike<type>)
  {
    return detail::visit_tuple(obj, visitor);
  }
  else if constexpr (detail::ContainerLike<type>)
  {
    return detail::visit_container(obj, visitor);
  }
  else if constexpr (detail::VariantLike<type>)
  {
    return detail::visit_variant(obj, visitor);
  }
  else if constexpr (detail::BoolLike<type> || detail::IntegerLike<type> || detail::FloatLike<type>)
  {
    return detail::visit_value(obj, visitor);
  }
  else if constexpr (detail::EnumLike<type>)
  {
    return detail::visit_enum(obj, visitor);
  }
  else if constexpr (detail::PointerLike<type>)
  {
    return detail::visit_pointer(obj, visitor);
  }
  else if constexpr (detail::OptionalLike<type>)
  {
    return detail::visit_optional(obj, visitor);
  }
  else if constexpr (detail::MonostateLike<type>)
  {
    return detail::visit_monostate(obj, visitor);
  }
  else if constexpr (detail::Aggregate<type>)
  {
    return detail::visit_aggregate(obj, visitor);
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