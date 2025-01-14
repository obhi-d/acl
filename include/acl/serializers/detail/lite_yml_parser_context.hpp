#pragma once

#include <acl/allocators/linear_arena_allocator.hpp>
#include <acl/dsl/lite_yml.hpp>
#include <acl/reflection/visitor.hpp>
#include <acl/utility/detail/concepts.hpp>
#include <cstddef>
#include <string>
#include <type_traits>

namespace acl::detail
{
class parser_state;

struct in_context_base
{
  using pop_fn = void (*)(parser_state* parser, in_context_base*);

  in_context_base* parent_  = nullptr;
  pop_fn           pop_fn_  = nullptr;
  uint32_t         xvalue_  = 0;
  uint16_t         depth_   = 0;
  bool             is_null_ = false;

  in_context_base(const in_context_base&)                    = default;
  in_context_base(in_context_base&&)                         = delete;
  auto operator=(const in_context_base&) -> in_context_base& = default;
  auto operator=(in_context_base&&) -> in_context_base&      = delete;

  in_context_base(in_context_base* parent) noexcept : parent_(parent) {}

  virtual void set_key(parser_state* parser, std::string_view ikey) = 0;
  virtual void add_item(parser_state* parser)                       = 0;
  virtual void set_value(std::string_view slice)                    = 0;
  virtual ~in_context_base() noexcept                               = default;
};

constexpr uint32_t default_parser_buffer_size = 8096;
class parser_state final : public acl::yml::context
{
  acl::yml::lite_stream         stream_;
  acl::linear_arena_allocator<> allocator_;
  in_context_base*              context_ = nullptr;
  uint16_t                      depth_   = 0;

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
      pop_last();
    }
  }

  void pop_last()
  {
    if (context_ != nullptr)
    {
      auto* parent = context_->parent_;
      if (context_->pop_fn_ != nullptr)
      {
        context_->pop_fn_(this, context_);
      }
      context_ = parent;
    }
  }

  template <typename C>
  void parse(C& handler)
  {
    context_ = &handler;
    handler.setup_proxy(this);
    stream_.parse();
  }

  template <typename Context, typename... Args>
  auto push(Args&&... args) -> Context*
  {
    void* cursor = allocator_.allocate(sizeof(Context), alignof(Context));
    // NOLINTNEXTLINE
    auto in_context_impl = std::construct_at(reinterpret_cast<Context*>(cursor), std::forward<Args>(args)...);
    in_context_impl->setup_proxy(this);
    context_ = in_context_impl;
    return in_context_impl;
  }

  template <typename Context>
  void pop(Context* ptr, in_context_base* parent)
  {
    ptr->post_init_object();
    std::destroy_at(ptr);
    allocator_.deallocate(ptr, sizeof(Context), alignof(Context));
    context_ = parent;
  }

  void begin_array() final
  {
    context_->depth_ = depth_++;
  }

  void end_array() final
  {
    if (context_->depth_ == depth_)
    {
      pop_last();
    }
    depth_--;
  }

  void begin_object() final
  {
    depth_++;
  }

  void end_object() final
  {
    pop_last();
    depth_--;
  }

  void begin_new_array_item() final
  {
    if (context_->depth_ == depth_)
    {
      pop_last();
    }
    context_->add_item(this);
    context_->depth_ = depth_;
  }

  void set_key(std::string_view ikey) final
  {
    context_->set_key(this, ikey);
    context_->depth_ = depth_;
  }

  void set_value(std::string_view slice) final
  {
    context_->set_value(slice);
    if (context_->depth_ == depth_)
    {
      pop_last();
    }
  }
};

template <typename Class, typename Config>
class in_context_impl : public in_context_base
{
  using pop_fn         = std::function<void(void*)>;
  using class_type     = std::decay_t<Class>;
  using transform_type = transform_t<Config>;

public:
  in_context_impl(class_type& obj, in_context_base* parent) noexcept
    requires(std::is_reference_v<Class>)
      : obj_(obj), in_context_base(parent)
  {}

  in_context_impl(in_context_base* parent) noexcept
    requires(!std::is_reference_v<Class>)
      : in_context_base(parent)
  {}

  auto get() noexcept -> class_type&
  {
    return obj_;
  }

  void set_key(parser_state* parser, std::string_view ikey) final
  {
    auto key = transform_type::transform(ikey);
    if constexpr (ExplicitlyReflected<class_type>)
    {
      if constexpr (StringMapValueType<class_type>)
      {
        read_string_map_value(parser, key);
      }
      else
      {
        read_explicitly_reflected(parser, key);
      }
    }
    else if constexpr (Aggregate<class_type>)
    {
      read_aggregate(parser, key);
    }
    else if constexpr (VariantLike<class_type>)
    {
      read_variant(parser, key);
    }
    else
    {
      throw visitor_error(visitor_error::type_is_not_an_object);
    }
  }

  void post_init_object()
  {
    post_read(obj_);
  }

  void add_item(parser_state* parser) final
  {
    if constexpr (ContainerLike<class_type>)
    {
      push_container_item(parser);
    }
    else if constexpr (TupleLike<class_type>)
    {
      read_tuple(parser);
    }
    else
    {
      throw visitor_error(visitor_error::type_is_not_an_array);
    }
  }

  void setup_proxy(parser_state* parser)
  {
    if constexpr (PointerLike<class_type>)
    {
      return read_pointer(parser);
    }
    else if constexpr (OptionalLike<class_type>)
    {
      return read_optional(parser);
    }
  }

  void set_value(std::string_view slice) final
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

  void read_pointer(parser_state* parser)
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

    auto mapping     = parser->template push<in_context_impl<pvalue_type&, Config>>(*obj_, this);
    mapping->pop_fn_ = [](parser_state* parser, in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<pvalue_type&, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);

      bool nullify = object->is_null_;
      parser->pop(object, parent);

      if (nullify)
      {
        parent->obj_ = nullptr;
      }

      if (parent->pop_fn_)
      {
        parent->pop_fn_(parser, parent);
      }
    };
  }

  void read_optional(parser_state* parser)
  {
    using pvalue_type = typename class_type::value_type;
    if (!obj_)
    {
      obj_.emplace();
    }

    auto mapping     = parser->template push<in_context_impl<pvalue_type&, Config>>(*obj_, this);
    mapping->pop_fn_ = [](parser_state* parser, in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<pvalue_type&, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);

      bool nullify = object->is_null_;
      parser->pop(object, parent);

      if (nullify)
      {
        parent->obj_.reset();
      }

      if (parent->pop_fn_)
      {
        parent->pop_fn_(parser, parent);
      }
    };
  }

  void read_tuple(parser_state* parser)
  {
    read_tuple_value<std::tuple_size_v<class_type>>(parser, xvalue_++);
  }

  void read_variant(parser_state* parser, std::string_view key)
  {
    using namespace std::string_view_literals;
    if (key == "type"sv)
    {
      read_variant_type(parser);
    }
    else if (key == "value"sv)
    {
      read_variant_value<std::variant_size_v<class_type>>(parser, xvalue_);
    }
  }

  void read_variant_type(parser_state* parser)
  {
    auto mapping     = parser->template push<in_context_impl<std::string_view, Config>>(this);
    mapping->pop_fn_ = [](parser_state* parser, in_context_base* mapping)
    {
      auto object              = static_cast<in_context_impl<std::string_view, Config>*>(mapping);
      object->parent_->xvalue_ = acl::index_transform<class_type>::to_index(object->get());
      parser->pop(object, object->parent_);
    };
  }

  template <std::size_t const I>
  void read_variant_at(parser_state* parser)
  {
    using type = std::variant_alternative_t<I, class_type>;

    auto mapping     = parser->template push<in_context_impl<type, Config>>(this);
    mapping->pop_fn_ = [](parser_state* parser, in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<type, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
      parent->get().template emplace<type>(std::move(object->get()));
      parser->pop(object, parent);
    };
  }

  template <std::size_t const N>
  void read_variant_value(parser_state* parser, uint32_t i) noexcept
  {
    [&]<std::size_t... I>(std::index_sequence<I...>)
    {
      ((i == I ? read_variant_at<I>(parser) : void()), ...);
    }(std::make_index_sequence<N>());
  }

  template <std::size_t I>
  void read_tuple_element(parser_state* parser) noexcept
  {
    using type = std::tuple_element_t<I, class_type>;

    auto mapping     = parser->template push<in_context_impl<type, Config>>(this);
    mapping->pop_fn_ = [](parser_state* parser, in_context_base* mapping)
    {
      auto* object               = static_cast<in_context_impl<type, Config>*>(mapping);
      auto  parent               = static_cast<in_context_impl<Class, Config>*>(object->parent_);
      std::get<I>(parent->get()) = std::move(object->get());
      parser->pop(object, parent);
    };
  }

  template <std::size_t const N>
  void read_tuple_value(parser_state* parser, uint32_t i) noexcept
  {
    [&]<std::size_t... I>(std::index_sequence<I...>)
    {
      ((i == I ? read_tuple_element<I>(parser) : void()), ...);
    }(std::make_index_sequence<N>());
  }

  void push_container_item(parser_state* parser)
    requires(!ContainerHasEmplaceBack<class_type> && ContainerHasArrayValueAssignable<class_type>)
  {
    auto mapping     = parser->template push<in_context_impl<array_value_type<class_type>, Config>>(this);
    mapping->pop_fn_ = [](parser_state* parser, in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<array_value_type<class_type>, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
      if (parent->xvalue_ < parent->get().size())
      {
        parent->get()[parent->xvalue_++] = std::move(object->get());
      }
      parser->pop(object, parent);
    };
  }

  void push_container_item(parser_state* parser)
    requires(ContainerHasEmplaceBack<class_type>)
  {
    auto mapping     = parser->template push<in_context_impl<typename class_type::value_type, Config>>(this);
    mapping->pop_fn_ = [](parser_state* parser, in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<typename class_type::value_type, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
      parent->get().emplace_back(std::move(object->get()));
      parser->pop(object, parent);
    };
  }

  void push_container_item(parser_state* parser)
    requires(ContainerHasEmplace<class_type> && !MapLike<class_type>)
  {
    using value_t    = typename class_type::value_type;
    auto mapping     = parser->template push<in_context_impl<value_t, Config>>(this);
    mapping->pop_fn_ = [](parser_state* parser, in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<value_t, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
      parent->get().emplace(std::move(object->get()));
      parser->pop(object, parent);
    };
  }

  void push_container_item(parser_state* parser)
    requires(MapLike<class_type>)
  {
    using value_t    = std::pair<typename class_type::key_type, typename class_type::mapped_type>;
    auto mapping     = parser->template push<in_context_impl<value_t, Config>>(this);
    mapping->pop_fn_ = [](parser_state* parser, in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<value_t, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
      auto& pair   = object->get();
      parent->get().emplace(std::move(pair.first), std::move(pair.second));
      parser->pop(object, parent);
    };
  }

  void read_string_map_value(parser_state* parser, std::string_view key)
  {
    obj_.key_        = key;
    using value_t    = typename class_type::value_type;
    auto mapping     = parser->template push<in_context_impl<value_t&, Config>>(obj_.value_, this);
    mapping->pop_fn_ = [](parser_state* parser, in_context_base* mapping)
    {
      auto* object = static_cast<in_context_impl<value_t&, Config>*>(mapping);
      auto  parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
      parser->pop(object, parent);
    };
  }

  void read_explicitly_reflected(parser_state* parser, std::string_view key)
  {
    for_each_field(
     [&]<typename Decl>(class_type& obj, Decl const& decl, auto) noexcept
     {
       if (decl.key() == key)
       {
         using value_t = typename Decl::MemTy;

         auto mapping     = parser->template push<in_context_impl<value_t, Config>>(this);
         mapping->pop_fn_ = [](parser_state* parser, in_context_base* mapping)
         {
           auto object = static_cast<in_context_impl<value_t, Config>*>(mapping);
           auto parent = static_cast<in_context_impl<Class, Config>*>(object->parent_);
           Decl decl;
           decl.value(parent->get(), std::move(object->get()));
           parser->pop(object, parent);
         };
       }
     },
     obj_);
  }

  template <std::size_t I>
  static void pop_aggregate_field(parser_state* parser, in_context_base* mapping)
  {
    using type                      = field_type<I, class_type>;
    auto object                     = static_cast<in_context_impl<type, Config>*>(mapping);
    auto parent                     = static_cast<in_context_impl<class_type, Config>*>(object->parent_);
    get_field_ref<I>(parent->get()) = std::move(object->get());
    parser->pop(object, parent);
  };

  template <std::size_t I>
  void read_aggregate_field(parser_state* parser, std::string_view field_key, auto const& field_names)
  {
    if (std::get<I>(field_names) == field_key)
    {
      using type       = field_type<I, class_type>;
      auto mapping     = parser->template push<in_context_impl<type, Config>>(this);
      mapping->pop_fn_ = &pop_aggregate_field<I>;
    }
  }

  void read_aggregate(parser_state* parser, std::string_view field_key)
  {
    constexpr auto field_names = get_field_names<class_type>();

    [&]<std::size_t... I>(std::index_sequence<I...>, std::string_view key)
    {
      (read_aggregate_field<I>(parser, key, field_names), ...);
    }(std::make_index_sequence<std::tuple_size_v<decltype(field_names)>>(), field_key);
  }

private:
  Class obj_;
};
} // namespace acl::detail