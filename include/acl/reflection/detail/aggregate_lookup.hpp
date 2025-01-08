#pragma once

#include <type_traits>

// clang-format off
/** Generate the C++ `aggregate_lookup` pattern with a maximum of `N` parameters.
 * @param max_n The maximum number of parameters to support.
 * @return A string containing the generated C++ code.

def generate_aggregate_lookup_pattern(max_n):
    """
    Generate the C++ `aggregate_lookup` pattern with a maximum of `N` parameters.
    Args:
        max_n (int): The maximum number of parameters to support.
    Returns:
        str: A string containing the generated C++ code.
    """
    lines = []
#Add function header
    lines.append("template <class T, class Fn>")
    lines.append("[[nodiscard]] consteval auto aggregate_lookup(Fn&& fn) noexcept -> decltype(auto)")
    lines.append("{")
    lines.append("    using type = std::remove_cvref_t<T>;")
    lines.append("    // Iterate forward so that compiler finds the match quickly")
    lines.append("    if constexpr (!requires {type{anyinit()};})")
    lines.append("    {")
    lines.append("        // No call")
    lines.append("        return fn();")
    lines.append("    }")
    for i in range(1, max_n + 1):
#Construct the condition
        params = ", ".join(["anyinit()" for _ in range(i)])
        requires_condition = f"!requires {{type{{{params}}};}}"
#Construct the arguments and bindings
        bindings = ", ".join([f"arg{j}" for j in range(i)])
        arguments = ", ".join([f"static_cast<decltype(arg{j})&&>(arg{j})" for j in range(i)])
        lines.append(f"    else if constexpr ({requires_condition})")
        lines.append("    {")
        lines.append(f"        // {i}")
        lines.append(f"        auto&& [{bindings}] = type_var<type>;")
        lines.append(f"        return static_cast<decltype(fn)&&>(fn)({arguments});")
        lines.append("    }")
    lines.append("    else")
    lines.append("    {")
    lines.append(f"        static_assert(always_false<T>, \"This type has {max_n} members, for streaming, split it up into multiple classes.\");") 
    lines.append("        return fn();") 
    lines.append("    }")
    lines.append("}")
#Join all lines and return the result
    return "\n".join(lines)
#Example usage
max_n = 64  # Change this to the desired maximum number of parameters
cpp_code = generate_aggregate_lookup_pattern(max_n)
print(cpp_code)

**/
// clang-format on

namespace acl::detail
{

struct anyinit
{
  template <typename T>
  operator T() const noexcept;
};

template <typename T>
extern const T type_var{};

template <class T, class Fn>
// NOLINTNEXTLINE
[[nodiscard]] consteval auto aggregate_lookup(Fn&& fn) noexcept -> decltype(auto)
{
  using type = std::remove_cvref_t<T>;
  // Iterate forward so that compiler finds the match quickly
  if constexpr (!requires { type{anyinit()}; })
  {
    // No call
    return fn();
  }
  else if constexpr (!requires { type{anyinit()}; })
  {
    // 1
    auto&& [arg0] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(static_cast<decltype(arg0)&&>(arg0));
  }
  else if constexpr (!requires { type{anyinit(), anyinit()}; })
  {
    // 2
    auto&& [arg0, arg1] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1));
  }
  else if constexpr (!requires { type{anyinit(), anyinit(), anyinit()}; })
  {
    // 3
    auto&& [arg0, arg1, arg2] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1),
                                           static_cast<decltype(arg2)&&>(arg2));
  }
  else if constexpr (!requires { type{anyinit(), anyinit(), anyinit(), anyinit()}; })
  {
    // 4
    auto&& [arg0, arg1, arg2, arg3] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1),
                                           static_cast<decltype(arg2)&&>(arg2), static_cast<decltype(arg3)&&>(arg3));
  }
  else if constexpr (!requires { type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit()}; })
  {
    // 5
    auto&& [arg0, arg1, arg2, arg3, arg4] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1),
                                           static_cast<decltype(arg2)&&>(arg2), static_cast<decltype(arg3)&&>(arg3),
                                           static_cast<decltype(arg4)&&>(arg4));
  }
  else if constexpr (!requires { type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()}; })
  {
    // 6
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1),
                                           static_cast<decltype(arg2)&&>(arg2), static_cast<decltype(arg3)&&>(arg3),
                                           static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5));
  }
  else if constexpr (!requires { type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()}; })
  {
    // 7
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1),
                                           static_cast<decltype(arg2)&&>(arg2), static_cast<decltype(arg3)&&>(arg3),
                                           static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
                                           static_cast<decltype(arg6)&&>(arg6));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 8
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1),
                                           static_cast<decltype(arg2)&&>(arg2), static_cast<decltype(arg3)&&>(arg3),
                                           static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
                                           static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 9
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 10
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1),
                                           static_cast<decltype(arg2)&&>(arg2), static_cast<decltype(arg3)&&>(arg3),
                                           static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
                                           static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7),
                                           static_cast<decltype(arg8)&&>(arg8), static_cast<decltype(arg9)&&>(arg9));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 11
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 12
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 13
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 14
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 15
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14] =
     type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 16
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15] =
     type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 17
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15,
            arg16] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 18
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 19
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 20
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 21
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 22
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 23
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 24
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 25
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 26
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 27
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 28
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 29
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 30
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 31
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30] =
     type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 32
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31] =
     type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 33
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 34
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 35
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 36
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 37
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 38
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 39
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 40
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 41
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 42
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit()};
                     })
  {
    // 43
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 44
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 45
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 46
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45] =
     type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 47
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46] =
     type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 48
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 49
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit()};
                     })
  {
    // 50
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit()};
                     })
  {
    // 51
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 52
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 53
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51, arg52] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51),
     static_cast<decltype(arg52)&&>(arg52));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 54
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51, arg52, arg53] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51),
     static_cast<decltype(arg52)&&>(arg52), static_cast<decltype(arg53)&&>(arg53));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 55
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51, arg52, arg53, arg54] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51),
     static_cast<decltype(arg52)&&>(arg52), static_cast<decltype(arg53)&&>(arg53),
     static_cast<decltype(arg54)&&>(arg54));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 56
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51, arg52, arg53, arg54, arg55] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51),
     static_cast<decltype(arg52)&&>(arg52), static_cast<decltype(arg53)&&>(arg53),
     static_cast<decltype(arg54)&&>(arg54), static_cast<decltype(arg55)&&>(arg55));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit()};
                     })
  {
    // 57
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51, arg52, arg53, arg54, arg55, arg56] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51),
     static_cast<decltype(arg52)&&>(arg52), static_cast<decltype(arg53)&&>(arg53),
     static_cast<decltype(arg54)&&>(arg54), static_cast<decltype(arg55)&&>(arg55),
     static_cast<decltype(arg56)&&>(arg56));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit()};
                     })
  {
    // 58
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51, arg52, arg53, arg54, arg55, arg56, arg57] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51),
     static_cast<decltype(arg52)&&>(arg52), static_cast<decltype(arg53)&&>(arg53),
     static_cast<decltype(arg54)&&>(arg54), static_cast<decltype(arg55)&&>(arg55),
     static_cast<decltype(arg56)&&>(arg56), static_cast<decltype(arg57)&&>(arg57));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit()};
                     })
  {
    // 59
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51, arg52, arg53, arg54, arg55, arg56, arg57, arg58] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51),
     static_cast<decltype(arg52)&&>(arg52), static_cast<decltype(arg53)&&>(arg53),
     static_cast<decltype(arg54)&&>(arg54), static_cast<decltype(arg55)&&>(arg55),
     static_cast<decltype(arg56)&&>(arg56), static_cast<decltype(arg57)&&>(arg57),
     static_cast<decltype(arg58)&&>(arg58));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 60
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51, arg52, arg53, arg54, arg55, arg56, arg57, arg58, arg59] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51),
     static_cast<decltype(arg52)&&>(arg52), static_cast<decltype(arg53)&&>(arg53),
     static_cast<decltype(arg54)&&>(arg54), static_cast<decltype(arg55)&&>(arg55),
     static_cast<decltype(arg56)&&>(arg56), static_cast<decltype(arg57)&&>(arg57),
     static_cast<decltype(arg58)&&>(arg58), static_cast<decltype(arg59)&&>(arg59));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 61
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51, arg52, arg53, arg54, arg55, arg56, arg57, arg58, arg59, arg60] =
     type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51),
     static_cast<decltype(arg52)&&>(arg52), static_cast<decltype(arg53)&&>(arg53),
     static_cast<decltype(arg54)&&>(arg54), static_cast<decltype(arg55)&&>(arg55),
     static_cast<decltype(arg56)&&>(arg56), static_cast<decltype(arg57)&&>(arg57),
     static_cast<decltype(arg58)&&>(arg58), static_cast<decltype(arg59)&&>(arg59),
     static_cast<decltype(arg60)&&>(arg60));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 62
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51, arg52, arg53, arg54, arg55, arg56, arg57, arg58, arg59, arg60, arg61] =
     type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51),
     static_cast<decltype(arg52)&&>(arg52), static_cast<decltype(arg53)&&>(arg53),
     static_cast<decltype(arg54)&&>(arg54), static_cast<decltype(arg55)&&>(arg55),
     static_cast<decltype(arg56)&&>(arg56), static_cast<decltype(arg57)&&>(arg57),
     static_cast<decltype(arg58)&&>(arg58), static_cast<decltype(arg59)&&>(arg59),
     static_cast<decltype(arg60)&&>(arg60), static_cast<decltype(arg61)&&>(arg61));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 63
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51, arg52, arg53, arg54, arg55, arg56, arg57, arg58, arg59, arg60, arg61,
            arg62] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51),
     static_cast<decltype(arg52)&&>(arg52), static_cast<decltype(arg53)&&>(arg53),
     static_cast<decltype(arg54)&&>(arg54), static_cast<decltype(arg55)&&>(arg55),
     static_cast<decltype(arg56)&&>(arg56), static_cast<decltype(arg57)&&>(arg57),
     static_cast<decltype(arg58)&&>(arg58), static_cast<decltype(arg59)&&>(arg59),
     static_cast<decltype(arg60)&&>(arg60), static_cast<decltype(arg61)&&>(arg61),
     static_cast<decltype(arg62)&&>(arg62));
  }
  else if constexpr (!requires {
                       type{anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(),
                            anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit(), anyinit()};
                     })
  {
    // 64
    auto&& [arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15, arg16,
            arg17, arg18, arg19, arg20, arg21, arg22, arg23, arg24, arg25, arg26, arg27, arg28, arg29, arg30, arg31,
            arg32, arg33, arg34, arg35, arg36, arg37, arg38, arg39, arg40, arg41, arg42, arg43, arg44, arg45, arg46,
            arg47, arg48, arg49, arg50, arg51, arg52, arg53, arg54, arg55, arg56, arg57, arg58, arg59, arg60, arg61,
            arg62, arg63] = type_var<type>;
    return static_cast<decltype(fn)&&>(fn)(
     static_cast<decltype(arg0)&&>(arg0), static_cast<decltype(arg1)&&>(arg1), static_cast<decltype(arg2)&&>(arg2),
     static_cast<decltype(arg3)&&>(arg3), static_cast<decltype(arg4)&&>(arg4), static_cast<decltype(arg5)&&>(arg5),
     static_cast<decltype(arg6)&&>(arg6), static_cast<decltype(arg7)&&>(arg7), static_cast<decltype(arg8)&&>(arg8),
     static_cast<decltype(arg9)&&>(arg9), static_cast<decltype(arg10)&&>(arg10), static_cast<decltype(arg11)&&>(arg11),
     static_cast<decltype(arg12)&&>(arg12), static_cast<decltype(arg13)&&>(arg13),
     static_cast<decltype(arg14)&&>(arg14), static_cast<decltype(arg15)&&>(arg15),
     static_cast<decltype(arg16)&&>(arg16), static_cast<decltype(arg17)&&>(arg17),
     static_cast<decltype(arg18)&&>(arg18), static_cast<decltype(arg19)&&>(arg19),
     static_cast<decltype(arg20)&&>(arg20), static_cast<decltype(arg21)&&>(arg21),
     static_cast<decltype(arg22)&&>(arg22), static_cast<decltype(arg23)&&>(arg23),
     static_cast<decltype(arg24)&&>(arg24), static_cast<decltype(arg25)&&>(arg25),
     static_cast<decltype(arg26)&&>(arg26), static_cast<decltype(arg27)&&>(arg27),
     static_cast<decltype(arg28)&&>(arg28), static_cast<decltype(arg29)&&>(arg29),
     static_cast<decltype(arg30)&&>(arg30), static_cast<decltype(arg31)&&>(arg31),
     static_cast<decltype(arg32)&&>(arg32), static_cast<decltype(arg33)&&>(arg33),
     static_cast<decltype(arg34)&&>(arg34), static_cast<decltype(arg35)&&>(arg35),
     static_cast<decltype(arg36)&&>(arg36), static_cast<decltype(arg37)&&>(arg37),
     static_cast<decltype(arg38)&&>(arg38), static_cast<decltype(arg39)&&>(arg39),
     static_cast<decltype(arg40)&&>(arg40), static_cast<decltype(arg41)&&>(arg41),
     static_cast<decltype(arg42)&&>(arg42), static_cast<decltype(arg43)&&>(arg43),
     static_cast<decltype(arg44)&&>(arg44), static_cast<decltype(arg45)&&>(arg45),
     static_cast<decltype(arg46)&&>(arg46), static_cast<decltype(arg47)&&>(arg47),
     static_cast<decltype(arg48)&&>(arg48), static_cast<decltype(arg49)&&>(arg49),
     static_cast<decltype(arg50)&&>(arg50), static_cast<decltype(arg51)&&>(arg51),
     static_cast<decltype(arg52)&&>(arg52), static_cast<decltype(arg53)&&>(arg53),
     static_cast<decltype(arg54)&&>(arg54), static_cast<decltype(arg55)&&>(arg55),
     static_cast<decltype(arg56)&&>(arg56), static_cast<decltype(arg57)&&>(arg57),
     static_cast<decltype(arg58)&&>(arg58), static_cast<decltype(arg59)&&>(arg59),
     static_cast<decltype(arg60)&&>(arg60), static_cast<decltype(arg61)&&>(arg61),
     static_cast<decltype(arg62)&&>(arg62), static_cast<decltype(arg63)&&>(arg63));
  }
  else
  {
    static_assert(always_false<T>, "This type has 64 members, for streaming, split it up into multiple classes.");
    return fn();
  }
}

} // namespace acl::detail