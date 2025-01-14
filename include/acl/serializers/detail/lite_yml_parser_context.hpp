#pragma once

#include <acl/allocators/linear_arena_allocator.hpp>
#include <acl/dsl/lite_yml.hpp>
#include <acl/reflection/visitor.hpp>
#include <acl/utility/detail/concepts.hpp>
#include <string>
#include <type_traits>

namespace acl::detail
{
class parser_state;

struct in_context_base
{
  using pop_fn = void (*)(in_context_base*);

  std::reference_wrapper<parser_state> parser_state_;
  in_context_base*                     parent_  = nullptr;
  pop_fn                               pop_fn_  = nullptr;
  uint32_t                             xvalue_  = 0;
  bool                                 is_null_ = false;

  in_context_base(const in_context_base&)                    = default;
  in_context_base(in_context_base&&)                         = delete;
  auto operator=(const in_context_base&) -> in_context_base& = default;
  auto operator=(in_context_base&&) -> in_context_base&      = delete;

  in_context_base(parser_state& parser_state, in_context_base* parent) noexcept
      : parser_state_(parser_state), parent_(parent)
  {}

  virtual void set_key(std::string_view ikey)     = 0;
  virtual void ensure_container()                 = 0;
  virtual void end_array()                        = 0;
  virtual void end_object()                       = 0;
  virtual void prepare_push()                     = 0;
  virtual void push_value(std::string_view slice) = 0;
  virtual ~in_context_base() noexcept             = default;
};

constexpr uint32_t default_parser_buffer_size = 8096;
class parser_state final : public acl::yml::context
{
  acl::yml::lite_stream         stream_;
  acl::linear_arena_allocator<> allocator_;
  in_context_base*              context_ = nullptr;

public:
  auto operator=(const parser_state&) -> parser_state& = delete;
  auto operator=(parser_state&&) -> parser_state&      = delete;
  parser_state(const parser_state&)                    = delete;
  parser_state(parser_state&&)                         = delete;
  parser_state(std::string_view content) noexcept : stream_(content, this), allocator_(default_parser_buffer_size) {}
  ~parser_state() noexcept final
  {
    while (context_ != nullptr)
    {
      auto* parent = context_->parent_;
      if (context_->pop_fn_ != nullptr)
      {
        context_->pop_fn_(context_);
      }
      context_ = parent;
    }
  }

  template <typename C>
  void parse(C& handler)
  {
    context_ = &handler;
    handler.setup_proxy();
    stream_.parse();
  }

  template <typename Context, typename... Args>
  auto push(Args&&... args) -> Context*
  {
    void* cursor = allocator_.allocate(sizeof(Context), alignof(Context));
    // NOLINTNEXTLINE
    auto in_context_impl = std::construct_at(reinterpret_cast<Context*>(cursor), std::forward<Args>(args)...);
    in_context_impl->setup_proxy();
    context_ = in_context_impl;
    return in_context_impl;
  }

  template <typename Context>
  void pop(Context* ptr, in_context_base* parent)
  {
    std::destroy_at(ptr);
    allocator_.deallocate(ptr, sizeof(Context), alignof(Context));
    context_ = parent;
  }

  void begin_array() final
  {
    context_->ensure_container();
  }

  void end_array() final
  {
    context_->end_array();
  }

  void begin_object() final {}

  void end_object() final
  {
    context_->end_object();
  }

  void begin_new_array_item() final
  {
    context_->prepare_push();
  }

  void set_key(std::string_view ikey) final
  {
    context_->set_key(ikey);
  }

  void set_value(std::string_view slice) final
  {
    context_->push_value(slice);
  }
};

template <typename Class, typename Config>
class in_context_impl : public in_context_base
{
  using pop_fn         = std::function<void(void*)>;
  using class_type     = std::decay_t<Class>;
  using transform_type = transform_t<Config>;

public:
  in_context_impl(class_type& obj, parser_state& state, in_context_base* parent) noexcept
    requires(std::is_reference_v<Class>)
      : obj_(obj), in_context_base(state, parent)
  {}

  in_context_impl(parser_state& state, in_context_base* parent) noexcept
    requires(!std::is_reference_v<Class>)
      : in_context_base(state, parent)
  {}

  auto get() noexcept -> class_type&
  {
    return obj_;
  }

  void set_key(std::string_view ikey) final
  {
    auto key = transform_type::transform(ikey);
    if constexpr (ExplicitlyReflected<class_type>)
    {
      if constexpr (StringMapValueType<class_type>)
      {
        read_string_map_value(key);
      }
      else
      {
        read_explicitly_reflected(key);
      }
    }
    else if constexpr (Aggregate<class_type>)
    {
      read_aggregate(key);
    }
    else if constexpr (VariantLike<class_type>)
    {
      read_variant(key);
    }
    else
    {
      throw visitor_error(visitor_error::type_is_not_an_object);
    }
  }

  void end_object() final
  {
    if (pop_fn_)
    {
      pop_fn_(this);
    }
    post_read(obj_);
  }

  void prepare_push() final
  {
    if constexpr (ContainerLike<class_type>)
    {
      push_container_item();
    }
    else if constexpr (TupleLike<class_type>)
    {
      read_tuple();
    }
  }

  void setup_proxy()
  {
    if constexpr (PointerLike<class_type>)
    {
      return read_pointer();
    }
    else if constexpr (OptionalLike<class_type>)
    {
      return read_optional();
    }
  }

  void push_value(std::string_view slice) final
  {
    if (slice == "null")
    {
      is_null_ = true;
      return;
    }

    if constexpr (Convertible<class_type>)
    {
      if constexpr (requires { typename Config::mutate_enums_type; } && std::is_enum_v<class_type>)
      {
        acl::convert<class_type>::from_string(obj_, transform_type::transform(slice));
      }
      else
      {
        acl::convert<class_type>::from_string(obj_, slice);
      }
    }
    else if constexpr (ConstructedFromStringView<class_type> || ContainerIsStringLike<class_type>)
    {
      obj_ = class_type(slice);
    }
    else if constexpr (BoolLike<class_type>)
    {
      return read_bool(slice);
    }
    else if constexpr (IntegerLike<class_type>)
    {
      return read_integer(slice);
    }
    else if constexpr (EnumLike<class_type>)
    {
      return read_enum(slice);
    }
    else if constexpr (FloatLike<class_type>)
    {
      return read_float(slice);
    }
    else if constexpr (MonostateLike<class_type>)
    {
    }
    else if constexpr (ContainerLike<class_type> || TupleLike<class_type>)
    {
      if (pop_fn_)
      {
        pop_fn_(this);
      }
    }
  }

  void read_bool(std::string_view slice)
  {
    using namespace std::string_view_literals;
    obj_ = slice == "true"sv || slice == "True"sv;
  }

  void error_check(std::from_chars_result result)
  {
    if (result.ec != std::errc())
    {
      throw std::runtime_error("Failed to parse value");
    }
  }

  void read_integer(std::string_view slice)
  {
    constexpr uint32_t base_10 = 10;
    constexpr uint32_t base_16 = 10;
    using namespace std::string_view_literals;
    if (slice.starts_with("0x"sv))
    {
      error_check(std::from_chars(slice.data(), slice.data() + slice.size(), obj_, base_16));
    }
    else
    {
      error_check(std::from_chars(slice.data(), slice.data() + slice.size(), obj_, base_10));
    }
  }

  void read_float(std::string_view slice)
  {
    using namespace std::string_view_literals;
    if (slice == ".nan"sv || slice == "nan"sv)
    {
      obj_ = std::numeric_limits<class_type>::quiet_NaN();
    }
    else if (slice == ".inf"sv || slice == "inf"sv)
    {
      obj_ = std::numeric_limits<class_type>::infinity();
    }
    else if (slice == "-.inf"sv || slice == "-inf"sv)
    {
      obj_ = -std::numeric_limits<class_type>::infinity();
    }
    else
    {
      error_check(std::from_chars(slice.data(), slice.data() + slice.size(), obj_));
    }
  }

  void read_enum(std::string_view slice)
  {
    constexpr uint32_t base_10 = 10;
    constexpr uint32_t base_16 = 10;

    std::underlying_type_t<class_type> value;
    if (slice.starts_with("0x"))
    {
      error_check(std::from_chars(slice.data(), slice.data() + slice.size(), value, base_16));
    }
    else
    {
      error_check(std::from_chars(slice.data(), slice.data() + slice.size(), value, base_10));
    }
    obj_ = static_cast<class_type>(value);
  }

  void read_pointer()
  {
    using class_type  = std::decay_t<class_type>;
    using pvalue_type = pointer_class_type<class_type>;
    if (!obj_)
    {
      if constexpr (std::same_as<class_type, std::shared_ptr<pvalue_type>>)
      {
        obj_ = std::make_shared<pvalue_type>();
      }
      else
      {
        obj_ = class_type(new pointer_class_type<class_type>());
      }
    }

    auto mapping =
     parser_state_.get().template push<in_context_impl<pvalue_type&, Config>>(*obj_, parser_state_.get(), this);
    mapping->pop_fn_ = [](in_context_base* mapping)
    {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      auto* object = reinterpret_cast<in_context_impl<pvalue_type&, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);

      bool nullify = object->is_null_;
      parent->parser_state_.get().pop(object, parent);

      if (nullify)
      {
        parent->obj_ = nullptr;
      }

      if (parent->pop_fn_)
      {
        parent->pop_fn_(parent);
      }
    };
  }

  void read_optional()
  {
    using pvalue_type = typename class_type::value_type;
    if (!obj_)
    {
      obj_.emplace();
    }

    auto mapping =
     parser_state_.get().template push<in_context_impl<pvalue_type&, Config>>(*obj_, parser_state_.get(), this);
    mapping->pop_fn_ = [](in_context_base* mapping)
    {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      auto* object = reinterpret_cast<in_context_impl<pvalue_type&, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);

      bool nullify = object->is_null_;
      parent->parser_state_.get().pop(object, parent);

      if (nullify)
      {
        parent->obj_.reset();
      }

      if (parent->pop_fn_)
      {
        parent->pop_fn_(parent);
      }
    };
  }

  void read_tuple()
  {
    read_tuple_value<std::tuple_size_v<class_type> - 1>(xvalue_++);
  }

  void read_variant(std::string_view key)
  {
    using namespace std::string_view_literals;
    if (key == "type"sv)
    {
      read_variant_type();
    }
    else if (key == "value"sv)
    {
      read_variant_value<std::variant_size_v<class_type> - 1>(xvalue_);
    }
  }

  void read_variant_type()
  {
    auto mapping =
     parser_state_.get().template push<in_context_impl<std::string_view, Config>>(parser_state_.get(), this);
    mapping->pop_fn_ = [](in_context_base* mapping)
    {
      auto object              = static_cast<in_context_impl<std::string_view, Config>*>(mapping);
      object->parent_->xvalue_ = acl::index_transform<class_type>::to_index(object->get());
      object->parent_->parser_state_.get().pop(object, object->parent_);
    };
  }

  template <std::size_t const I>
  void read_variant_value(uint32_t i) noexcept
  {
    if (I == i)
    {
      using type = std::variant_alternative_t<I, class_type>;

      auto mapping     = parser_state_.get().template push<in_context_impl<type, Config>>(parser_state_.get(), this);
      mapping->pop_fn_ = [](in_context_base* mapping)
      {
        auto* object = static_cast<in_context_impl<type, Config>*>(mapping);
        auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
        parent->get().template emplace<type>(std::move(object->get()));
        parent->parser_state_.get().pop(object, parent);
      };
    }
    else if constexpr (I > 0)
    {
      read_variant_value<I - 1>(i);
    }
    else
    {
      return;
    }
  }

  template <std::size_t const I>
  void read_tuple_value(uint32_t i) noexcept
  {
    if (I == i)
    {
      using type = std::tuple_element_t<I, class_type>;

      auto mapping     = parser_state_.get().template push<in_context_impl<type, Config>>(parser_state_.get(), this);
      mapping->pop_fn_ = [](in_context_base* mapping)
      {
        auto* object               = static_cast<in_context_impl<type, Config>*>(mapping);
        auto  parent               = static_cast<in_context_impl<Class, Config>*>(object->parent_);
        std::get<I>(parent->get()) = std::move(object->get());
        parent->parser_state_.get().pop(object, parent);
      };
    }
    else if constexpr (I > 0)
    {
      read_tuple_value<I - 1>(i);
    }
    else
    {
      return;
    }
  }

  void push_container_item()
    requires(!ContainerHasEmplaceBack<class_type> && ContainerHasArrayValueAssignable<class_type>)
  {
    auto mapping = parser_state_.get().template push<in_context_impl<array_value_type<class_type>, Config>>(
     parser_state_.get(), this);
    mapping->pop_fn_ = [](in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<array_value_type<class_type>, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
      if (parent->xvalue_ < parent->get().size())
      {
        parent->get()[parent->xvalue_++] = std::move(object->get());
      }
      parent->parser_state_.get().pop(object, parent);
    };
  }

  void push_container_item()
    requires(ContainerHasEmplaceBack<class_type>)
  {
    auto mapping = parser_state_.get().template push<in_context_impl<typename class_type::value_type, Config>>(
     parser_state_.get(), this);
    mapping->pop_fn_ = [](in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<typename class_type::value_type, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
      parent->get().emplace_back(std::move(object->get()));
      parent->parser_state_.get().pop(object, parent);
    };
  }

  void push_container_item()
    requires(ContainerHasEmplace<class_type> && !MapLike<class_type>)
  {
    using value_t    = typename class_type::value_type;
    auto mapping     = parser_state_.get().template push<in_context_impl<value_t, Config>>(parser_state_.get(), this);
    mapping->pop_fn_ = [](in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<value_t, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
      parent->get().emplace(std::move(object->get()));
      parent->parser_state_.get().pop(object, parent);
    };
  }

  void push_container_item()
    requires(MapLike<class_type>)
  {
    using value_t    = std::pair<typename class_type::key_type, typename class_type::mapped_type>;
    auto mapping     = parser_state_.get().template push<in_context_impl<value_t, Config>>(parser_state_.get(), this);
    mapping->pop_fn_ = [](in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<value_t, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
      auto& pair   = object->get();
      parent->get().emplace(std::move(pair.first), std::move(pair.second));
      parent->parser_state_.get().pop(object, parent);
    };
  }

  void read_string_map_value(std::string_view key)
  {
    obj_.key_     = key;
    using value_t = typename class_type::value_type;
    auto mapping =
     parser_state_.get().template push<in_context_impl<value_t&, Config>>(obj_.value_, parser_state_.get(), this);
    mapping->pop_fn_ = [](in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<value_t&, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
      parent->parser_state_.get().pop(object, parent);
    };
  }

  void read_explicitly_reflected(std::string_view key)
  {
    for_each_field(
     [this, key]<typename Decl>(class_type& obj, Decl const& decl, auto) noexcept
     {
       if (decl.key() == key)
       {
         using value_t = typename Decl::MemTy;

         auto mapping = parser_state_.get().template push<in_context_impl<value_t, Config>>(parser_state_.get(), this);
         mapping->pop_fn_ = [](in_context_base* mapping)
         {
           auto object = static_cast<in_context_impl<value_t, Config>*>(mapping);
           auto parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
           Decl decl;
           decl.value(parent->get(), std::move(object->get()));
           parent->parser_state_.get().pop(object, parent);
         };
       }
     },
     obj_);
  }

  template <std::size_t I>
  static void pop_aggregate_field(in_context_base* mapping)
  {
    using type                      = field_type<I, class_type>;
    auto object                     = static_cast<in_context_impl<type, Config>*>(mapping);
    auto parent                     = static_cast<in_context_impl<class_type, Config>*>(object->parent_);
    get_field_ref<I>(parent->get()) = std::move(object->get());
    parent->parser_state_.get().pop(object, parent);
  };

  template <std::size_t I>
  void read_aggregate_field(std::string_view field_key, auto const& field_names)
  {
    if (std::get<I>(field_names) == field_key)
    {
      using type       = field_type<I, class_type>;
      auto mapping     = parser_state_.get().template push<in_context_impl<type, Config>>(parser_state_.get(), this);
      mapping->pop_fn_ = &pop_aggregate_field<I>;
    }
  }

  void read_aggregate(std::string_view field_key)
  {
    constexpr auto field_names = get_field_names<class_type>();

    [&]<std::size_t... I>(std::index_sequence<I...>, std::string_view key)
    {
      (read_aggregate_field<I>(key, field_names), ...);
    }(std::make_index_sequence<std::tuple_size_v<decltype(field_names)>>(), field_key);
  }

private:
  Class obj_;
};
} // namespace acl::detail