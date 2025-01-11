#include <acl/reflection/bind.hpp>
#include <type_traits>

namespace acl::detail
{

template <typename K, typename V, typename Opt>
struct map_value_type
{
  using key_type   = std::remove_cvref_t<K>;
  using value_type = std::remove_cvref_t<V>;

  using map_key_field_name   = key_field_name_t<Opt>;
  using map_value_field_name = value_field_name_t<Opt>;

  K* key_   = nullptr;
  V* value_ = nullptr;

  map_value_type() noexcept = default;
  map_value_type(K* k, V* v) noexcept : key_(k), value_(v) {}
  ~map_value_type() noexcept = default;

  auto key() const noexcept -> key_type const&
  {
    return *key_;
  }

  auto value() const noexcept -> value_type const&
  {
    return *value_;
  }

  auto set_key(key_type k) noexcept -> void
  {
    *key_ = std::move(k);
  }

  auto set_value(value_type v) noexcept -> void
  {
    *value_ = std::move(v);
  }

  map_value_type(map_value_type const&) noexcept                    = default;
  map_value_type(map_value_type&&) noexcept                         = default;
  auto operator=(map_value_type const&) noexcept -> map_value_type& = default;
  auto operator=(map_value_type&&) noexcept -> map_value_type&      = default;

  static auto constexpr reflect() noexcept
  {
    return acl::bind(acl::bind<map_key_field_name::value, &map_value_type::key, &map_value_type::set_key>(),
                     acl::bind<map_value_field_name::value, &map_value_type::value, &map_value_type::set_value>());
  }
};

} // namespace acl::detail