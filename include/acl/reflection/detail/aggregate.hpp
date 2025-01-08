#include <acl/reflection/detail/aggregate_lookup.hpp>
#include <cstdint>
#include <source_location>

namespace acl::detail
{

template <auto A>
[[nodiscard]] constexpr auto function_name() noexcept -> std::string_view
{
  return std::source_location::current().function_name();
}

template <typename T>
struct ref
{
  T& member_;
};
template <class T>
ref(T&) -> ref<T>;

void aggregate_tuple()
{
  // std::cout << get_field<MyStruct, 0>();
  constexpr auto refs = for_each<MyStruct>(
   [](auto&&... args)
   {
     return std::make_tuple(ref(static_cast<decltype(args)&&>(args))...);
   });
}

} // namespace acl::detail