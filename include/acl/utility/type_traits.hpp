#pragma once

#include <acl/utility/type_name.hpp>
#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>

namespace acl
{

template <typename... Option>
struct options : public Option...
{};

template <>
struct options<>
{};

template <typename T>
struct default_options
{};

template <typename Ty = std::void_t<>>
struct allocator_traits
{
  using is_always_equal                        = std::false_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_swap            = std::true_type;
};

constexpr uint32_t default_pool_size = 4096;
/**
 *--------------- Basic Options ----------------
 */
namespace opt
{

/**
 * @brief Option to provide member pointer
 * @tparam M
 */
template <auto M>
struct member;

/**
 * @brief Specialization that provided class_type
 * @tparam T class_type for the member
 * @tparam M member_type for the member
 * @tparam MPtr Pointer to member
 */
template <typename T, typename M, M T::* MPtr>
struct member<MPtr>
{
  using class_type  = T;
  using member_type = M;
  using self_index  = opt::member<MPtr>;

  static auto get(class_type& to) noexcept -> member_type&
  {
    return to.*MPtr;
  }

  static auto get(class_type const& to) noexcept -> member_type const&
  {
    return to.*MPtr;
  }
};

/**
 * @brief Option to control underlying pool size
 */
template <uint32_t PoolSize = default_pool_size>
struct pool_size
{
  static constexpr uint32_t pool_size_v = PoolSize;
};

/**
 * @brief Option to control the pool size of index maps used by container
 */
template <uint32_t PoolSize = default_pool_size>
struct index_pool_size
{
  static constexpr uint32_t index_pool_size_v = PoolSize;
};

/**
 * @brief Self index pool size controls the pool size for back references
 */
template <uint32_t PoolSize = default_pool_size>
struct self_index_pool_size
{
  static constexpr uint32_t self_index_pool_size_v = PoolSize;
};

/**
 * @brief Key index pool size controls the pool size for key indexes in tables
 */
template <uint32_t PoolSize = default_pool_size>
struct keys_index_pool_size
{
  static constexpr uint32_t keys_index_pool_size_v = PoolSize;
};

/**
 * @brief Null value, only applicable to constexpr types
 */
template <auto NullValue>
struct null_value
{
  static constexpr auto null_v = NullValue;
};

/**
 * @brief Indexes will have this as their size type
 */
template <typename T = uint32_t>
struct basic_size_type
{
  using size_type = T;
};

/**
 * @brief This option is used to indicated the projected member of a class to be used to store its own index in a sparse
 * or packed container. This will allow the container to do its book keeping for item deletion and addition.
 */
template <auto M>
using self_index_member = member<M>;

template <typename T = void>
struct basic_link_type
{
  using link_type = T;
};

struct assume_pod
{
  static constexpr bool assume_pod_v = true;
};

struct no_fill
{
  static constexpr bool no_fill_v = true;
};

struct trivially_destroyed_on_move
{
  static constexpr bool trivially_destroyed_on_move_v = true;
};

struct use_sparse
{
  static constexpr bool use_sparse_v = true;
};

struct use_sparse_index
{
  static constexpr bool use_sparse_index_v = true;
};

struct self_use_sparse_index
{
  static constexpr bool self_use_sparse_index_v = true;
};

struct keys_use_sparse_index
{
  static constexpr bool keys_use_sparse_index_v = true;
};

struct zero_out_memory
{
  static constexpr bool zero_out_memory_v = true;
};

struct disable_pool_tracking
{
  static constexpr bool disable_pool_tracking_v = true;
};

struct use_direct_mapping
{
  static constexpr bool use_direct_mapping_v = true;
};

// custom vector
template <typename T>
struct custom_vector
{
  using custom_vector_t = T;
};

} // namespace opt

/**
 *------------------------------------------------
 * @brief Class type to string_view name of the class
 */
template <typename T>
constexpr auto type_name() -> std::string_view
{
  return detail::type_name<std::remove_cv_t<std::remove_reference_t<T>>>();
}

template <typename T>
constexpr auto type_hash() -> std::uint32_t
{
  return detail::type_hash<std::remove_cv_t<std::remove_reference_t<T>>>();
}

struct nocheck : std::false_type
{};

template <typename T>
struct function_traits;

template <typename R, typename... Args>
struct function_traits<R (*)(Args...)>
{
  static constexpr std::size_t arity = sizeof...(Args);
  using return_type                  = R;
  using args                         = std::tuple<Args...>;
  template <std::size_t Index>
  using arg_type                           = typename std::tuple_element_t<Index, std::tuple<Args...>>;
  constexpr static bool is_free_function   = true;
  constexpr static bool is_member_function = false;
  constexpr static bool is_const_function  = false;
  constexpr static bool is_functor         = false;
};

template <typename R, typename C, typename... Args>
struct function_traits<R (C::*)(Args...)>
{
  static constexpr std::size_t arity = sizeof...(Args);
  using class_type                   = C;
  using return_type                  = R;
  using args                         = std::tuple<Args...>;
  template <std::size_t Index>
  using arg_type                           = typename std::tuple_element_t<Index, std::tuple<Args...>>;
  constexpr static bool is_free_function   = false;
  constexpr static bool is_member_function = true;
  constexpr static bool is_const_function  = false;
  constexpr static bool is_functor         = false;
};

template <typename R, typename C, typename... Args>
struct function_traits<R (C::*)(Args...) const>
{
  static constexpr std::size_t arity = sizeof...(Args);
  using return_type                  = R;
  using args                         = std::tuple<Args...>;
  template <std::size_t Index>
  using arg_type                           = typename std::tuple_element_t<Index, std::tuple<Args...>>;
  constexpr static bool is_free_function   = false;
  constexpr static bool is_member_function = true;
  constexpr static bool is_const_function  = true;
  constexpr static bool is_functor         = false;
};

template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())>
{};

// Trait to extract class_type and function_type from a member function pointer
template <auto>
struct member_function
{
  constexpr static bool is_member_function_traits = false;
};

template <typename C, typename Ret, typename... Args, Ret (C::*M)(Args...)>
struct member_function<M>
{
  using class_type         = C;
  using function_type      = Ret (C::*)(Args...);
  using free_function_type = Ret (*)(Args...);
  using return_type        = Ret;
  using args               = std::tuple<Args...>;
  template <std::size_t Index>
  using arg_type = typename std::tuple_element_t<Index, std::tuple<Args...>>;

  static auto invoke(C& instance, Args&&... args)
  {
    return std::invoke(M, instance, std::forward<Args>(args)...);
  }

  constexpr static bool is_member_function_traits = true;
};

template <typename C, typename Ret, typename... Args, Ret (C::*M)(Args...) const>
struct member_function<M>
{
  using class_type         = const C;
  using function_type      = Ret (C::*)(Args...) const;
  using free_function_type = Ret (*)(Args...);
  using return_type        = Ret;
  using args               = std::tuple<Args...>;
  template <std::size_t Index>
  using arg_type = typename std::tuple_element_t<Index, std::tuple<Args...>>;

  static auto invoke(C const& instance, Args&&... args)
  {
    return std::invoke(M, instance, std::forward<Args>(args)...);
  }

  constexpr static bool is_member_function_traits = true;
};

namespace detail
{

template <typename F, typename... Args>
concept Callable = requires(F f, Args&&... args) {
  { f(std::forward<Args>(args)...) };
};

// Concept to check if a type is a free function pointer
template <typename F, typename... Args>
concept Function = std::is_function_v<std::remove_pointer_t<F>>;

template <typename Traits, typename U>
concept HasNullValue = requires(U t) {
  { Traits::null_v } -> std::convertible_to<U>;
  { Traits::null_v == t } -> std::same_as<bool>;
};

template <typename Traits, typename U>
concept HasNullMethod = requires(U v) {
  { Traits::is_null(v) } noexcept -> std::same_as<bool>;
};

template <typename Traits, typename U>
concept HasNullConstruct = requires(U v) {
  Traits::null_construct(v);
  Traits::null_reset(v);
};

template <typename Traits>
concept HasIndexPoolSize = requires {
  { Traits::index_pool_size_v } -> std::convertible_to<uint32_t>;
};

template <typename Traits>
concept HasPoolSize = requires {
  { Traits::pool_size_v } -> std::convertible_to<uint32_t>;
};

template <typename Traits>
concept HasSelfIndexPoolSize = requires {
  { Traits::self_index_pool_size_v } -> std::convertible_to<uint32_t>;
};

template <typename Traits>
concept HasKeysIndexPoolSize = requires {
  { Traits::keys_index_pool_size_v } -> std::convertible_to<uint32_t>;
};

template <typename Traits>
concept HasSelfIndexValue = requires { typename Traits::self_index; };

template <typename Traits>
concept HasDirectMapping = Traits::use_direct_mapping_v;
;

template <typename Traits>
concept HasSizeType = requires { typename Traits::size_type; };

template <typename Traits>
concept HasTrivialAttrib = Traits::assume_pod_v;

template <typename Traits>
concept HasNoFillAttrib = Traits::no_fill_v;

template <typename Traits>
concept HasTriviallyDestroyedOnMoveAttrib = Traits::trivially_destroyed_on_move_v;

template <typename Traits>
concept HasUseSparseAttrib = Traits::use_sparse_v;

template <typename Traits>
concept HasUseSparseIndexAttrib = Traits::use_sparse_index_v;

template <typename Traits>
concept HasSelfUseSparseIndexAttrib = Traits::self_use_sparse_index_v;

template <typename Traits>
concept HasKeysUseSparseIndexAttrib = Traits::keys_use_sparse_index_v;

template <typename Traits>
concept HasZeroMemoryAttrib = Traits::zero_out_memory_v;

template <typename Traits>
concept HasDisablePoolTrackingAttrib = Traits::disble_pool_tracking_v;

template <typename Traits>
concept HasLinkType = requires { typename Traits::link_type; };

template <typename V, typename R>
concept OptionalValueLike = requires(V v) {
  { *v } -> std::convertible_to<R>;
  { (bool)v } -> std::convertible_to<bool>;
};
template <typename T>
concept HasAllocatorAttribs = requires { typename T::allocator_t; };

template <typename S, typename T1, typename... Args>
struct choose_size_ty
{
  using type =
   std::conditional_t<HasSizeType<T1>, typename choose_size_ty<S, T1>::type, typename choose_size_ty<Args...>::type>;
};

template <typename S, typename T1>
struct choose_size_ty<S, T1>
{
  using type = S;
};

template <typename S, HasSizeType T1>
struct choose_size_ty<S, T1>
{
  using type = typename T1::size_type;
};

template <typename S, typename... Args>
using choose_size_t = typename choose_size_ty<S, Args...>::type;

template <typename UnderlyingAllocatorTag>
struct is_static
{
  constexpr static bool value = false;
};

template <typename UnderlyingAllocatorTag>
constexpr static bool is_static_v = is_static<UnderlyingAllocatorTag>::value;

template <typename T, bool>
struct link_value;

template <typename T>
struct link_value<T, true>
{
  using type = typename T::link_type;
};

template <typename T>
struct link_value<T, false>
{
  using type = void;
};

template <typename T>
using link_value_t = typename link_value<T, HasLinkType<T>>::type;

template <typename UaT, class = std::void_t<>>
struct tag
{
  using type = void;
};
template <typename UaT>
struct tag<UaT, std::void_t<typename UaT::tag>>
{
  using type = typename UaT::tag;
};

template <typename UaT>
using tag_t = typename tag<UaT>::type;

template <typename UaT, class = std::void_t<>>
struct size_type
{
  using type = std::size_t;
};
template <typename UaT>
struct size_type<UaT, std::void_t<typename UaT::size_type>>
{
  using type = typename UaT::size_type;
};

template <typename T>
struct pool_size
{
  static constexpr uint32_t value = default_pool_size;
};

template <HasPoolSize T>
struct pool_size<T>
{
  static constexpr uint32_t value = T::pool_size_v;
};

template <typename T>
constexpr uint32_t pool_size_v = detail::pool_size<T>::value;

} // namespace detail

} // namespace acl
