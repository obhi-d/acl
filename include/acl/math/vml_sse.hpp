#pragma once

#include "deduced_types.hpp"
#include "real.hpp"
#include "types.hpp"
#include <acl/utils/config.hpp>

namespace acl::vml
{

template <FloatingType scalar_t>
using float_to_int_t = std::conditional_t<sizeof(scalar_t) == 4, int, int64_t>;
template <FloatingType scalar_t>
using float_to_uint_t = std::conditional_t<sizeof(scalar_t) == 4, uint32_t, int64_t>;
template <IntegralType scalar_t>
using int_to_float_t = std::conditional_t<sizeof(scalar_t) == 4, float, double>;

template <ScalarType A>
constexpr A get_quad_scalar_type(std::array<A, 4>)
{
  return {};
}
#ifdef ACL_USE_SSE2

constexpr float get_quad_scalar_type(__m128)
{
  return 0.0f;
}

ACL_API __m128 exp_ps(__m128 x);
ACL_API __m128 log_ps(__m128 x);
ACL_API __m128 sin_ps(__m128 x);
ACL_API __m128 cos_ps(__m128 x);
ACL_API void   sincos_ps(__m128 x, __m128*, __m128*);

#else
inline auto exp_ps(auto x)
{
  return x;
}

inline auto log_ps(auto x)
{
  return x;
}

#endif

template <typename iquadv_t>
struct qvscalar_t
{
  using type = decltype(get_quad_scalar_type(iquadv_t()));
};

#ifdef ACL_USE_AVX

ACL_API __m256 exp256_ps(__m256 x);
ACL_API __m256 log256_ps(__m256 x);
ACL_API __m256 sin256_ps(__m256 x);
ACL_API __m256 cos256_ps(__m256 x);
ACL_API void   sincos256_ps(__m256 x, __m256*, __m256*);

#else
inline auto exp256_pd(auto x)
{
  return x;
}
inline auto log256_pd(auto x)
{
  return x;
}
#endif

template <typename iquadv_t>
using qscalar_t = typename qvscalar_t<iquadv_t>::type;

template <typename scalar_t>
union qv_type
{
  quadv_t<scalar_t>       vector;
  std::array<scalar_t, 4> rows;
};

constexpr uint64_t k_highbit_64 = 0x8000000000000000;
constexpr uint32_t k_highbit_32 = 0x80000000;
constexpr uint64_t k_signbit_64 = 0x7fffffffffffffff;
constexpr uint32_t k_signbit_32 = 0x7fffffff;
constexpr uint64_t k_allbits_64 = 0xffffffffffffffff;
constexpr uint32_t k_allbits_32 = 0xffffffff;

template <typename quadvt>
inline qscalar_t<quadvt> get_x(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_cvtss_f32(q);
  }
  else
  {
    return q[0];
  }
}

template <typename quadvt>
inline qscalar_t<quadvt> get_y(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    auto temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(1, 1, 1, 1));
    return _mm_cvtss_f32(temp);
  }
  else
  {
    return q[1];
  }
}

template <typename quadvt>
inline qscalar_t<quadvt> get_z(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    auto temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(2, 2, 2, 2));
    return _mm_cvtss_f32(temp);
  }
  else
  {
    return q[2];
  }
}

template <typename quadvt>
inline qscalar_t<quadvt> get_w(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    auto temp = _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 3, 3, 3));
    return _mm_cvtss_f32(temp);
  }
  else
  {
    return q[3];
  }
}

template <typename quadvt>
inline quadvt add(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_add_ps(a, b);
  }
  else
  {
    return quadvt{a[0] + b[0], a[1] + b[1], a[2] + b[2], a[3] + b[3]};
  }
}

template <typename quadvt>
inline quadvt sub(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_sub_ps(a, b);
  }
  else
  {
    return quadvt{a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3]};
  }
}

template <typename quadvt>
inline bool equals(quadvt const& v1, quadvt const& v2) noexcept
{
  constexpr qscalar_t<quadvt> zero = 0;
  quadvt                      r    = sub(v1, v2);
  return (bool)(acl::equals<qscalar_t<quadvt>>(get_x(r), zero) && acl::equals<qscalar_t<quadvt>>(get_y(r), zero) &&
                acl::equals<qscalar_t<quadvt>>(get_z(r), zero) && acl::equals<qscalar_t<quadvt>>(get_w(r), zero));
}

template <typename quadvt>
inline qscalar_t<quadvt> hadd(quadvt const& v) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {

    __m128 shuf = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 3, 0, 1)); // [ C D | A B ]
    __m128 sums = _mm_add_ps(v, shuf);                           // sums = [ D+C C+D | B+A A+B ]
    shuf        = _mm_movehl_ps(shuf, sums);                     //  [   C   D | D+C C+D ]  // let the
                                                                 //  compiler avoid a mov by reusing shuf
    sums = _mm_add_ss(sums, shuf);
    return _mm_cvtss_f32(sums);
  }
  else
  {
    return v[0] + v[1] + v[2] + v[3];
  }
}

template <typename quadvt>
inline quadvt isnanv(quadvt const& v) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_cmpneq_ps(v, v);
  }
  else
  {
    constexpr qscalar_t<quadvt> zero = 0;
    constexpr qscalar_t<quadvt> one  = 1;
    return quadvt{v[0] != v[0] ? one : zero, v[1] != v[1] ? one : zero, v[2] != v[2] ? one : zero,
                  v[3] != v[3] ? one : zero};
  }
}

template <typename quadvt>
inline quadvt isinfv(quadvt const& v) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    // Mask off the sign bit
    auto vtemp = _mm_and_ps(v, _mm_castsi128_ps(_mm_set_epi32(k_signbit_32, k_signbit_32, k_signbit_32, k_signbit_32)));
    // Compare to infinity
    vtemp = _mm_cmpeq_ps(vtemp, _mm_set1_ps(std::numeric_limits<float>::infinity()));
    // If any are infinity, the signs are true.
    return vtemp;
  }
  else
  {
    constexpr qscalar_t<quadvt> zero = 0;
    constexpr qscalar_t<quadvt> one  = 1;
    return quadvt{isinf(v[0]) ? one : zero, isinf(v[1]) ? one : zero, isinf(v[2]) ? one : zero,
                  isinf(v[3]) ? one : zero};
  }
}

template <typename quadvt>
inline bool isnan(quadvt const& v) noexcept
{
  return hadd(isnanv(v)) != 0;
}

template <typename quadvt>
inline bool isinf(quadvt const& v) noexcept
{
  return hadd(isinfv(v)) != 0;
}

template <typename quadvt>
inline bool isnegative_x(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return (_mm_movemask_ps(q) & 0x1) != 0;
  }
  else
  {
    return q[0] < 0.0f;
  }
}

template <typename quadvt>
inline bool isgreater_x(quadvt const& q1, quadvt const& q2) noexcept
{
  return (get_x(q1)) > (get_x(q2));
}

template <typename quadvt>
inline bool islesser_x(quadvt const& q1, quadvt const& q2) noexcept
{
  return (get_x(q1)) < (get_x(q2));
}

template <typename scalar_t>
inline quadv_t<scalar_t> set(scalar_t v) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
  {
    return _mm_set1_ps(v);
  }
  else
  {
    return {v, v, v, v};
  }
}

template <typename scalar_t>
inline quadv_t<scalar_t> set(const scalar_t* v) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
  {
    return _mm_load_pd(v);
  }
  else
  {
    return {v[0], v[1], v[2], v[3]};
  }
}

template <typename scalar_t>
inline quadv_t<scalar_t> set(scalar_t x, scalar_t y, scalar_t z) noexcept
{
  return set(x, y, z, 0);
}

template <typename scalar_t>
inline quadv_t<scalar_t> set(scalar_t x, scalar_t y, scalar_t z, scalar_t w) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
  {
    return _mm_set_ps(w, z, y, x);
  }
  else
  {
    return {x, y, z, w};
  }
}

template <typename scalar_t>
inline quadv_t<scalar_t> set_unaligned(scalar_t const* v) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
  {
    return _mm_loadu_ps(v);
  }
  else
  {
    return {v[0], v[1], v[2], v[3]};
  }
}

template <typename scalar_t>
inline quadv_t<scalar_t> zero() noexcept
{
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
  {
    return _mm_setzero_ps();
  }
  else
  {
    return {0, 0, 0, 0};
  }
}

template <typename quadvt>
inline qscalar_t<quadvt> get(quadvt const& q, std::uint32_t idx) noexcept
{
  qv_type<qscalar_t<quadvt>> value;
  value.vector = q;
  return value[idx];
}

template <typename quadvt>
inline quadvt min(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_min_ps(a, b);
  }
  else
  {
    quadvt r;
    for (std::uint32_t i = 0; i < a.size(); ++i)
      r[i] = std::min(a[i], b[i]);
    return r;
  }
}

template <typename quadvt>
inline quadvt max(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_max_ps(a, b);
  }
  else
  {
    quadvt r;
    for (std::uint32_t i = 0; i < a.size(); ++i)
      r[i] = std::max(a[i], b[i]);
    return r;
  }
}

template <typename scalar_t>
inline quadv_t<scalar_t> set_x(scalar_t val) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
  {
    return _mm_set_ss(val);
  }
  else
  {
    return {val, 0, 0, 0};
  }
}

template <typename quadvt>
inline quadvt set_x(quadvt const& q, qscalar_t<quadvt> val) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    auto v = _mm_set_ss(val);
    return _mm_move_ss(q, v);
  }
  else
  {
    return {val, q[1], q[2], q[3]};
  }
}

template <typename quadvt>
inline quadvt set_y(quadvt const& q, qscalar_t<quadvt> val) noexcept
{
  qv_type<qscalar_t<quadvt>> qt;
  qt.vector  = q;
  qt.rows[1] = val;
  return qt.vector;
}

template <typename quadvt>
inline quadvt set_z(quadvt const& q, qscalar_t<quadvt> val) noexcept
{
  qv_type<qscalar_t<quadvt>> qt;
  qt.vector  = q;
  qt.rows[2] = val;
  return qt.vector;
}

template <typename quadvt>
inline quadvt set_w(quadvt const& q, qscalar_t<quadvt> val) noexcept
{
  qv_type<qscalar_t<quadvt>> qt;
  qt.vector  = q;
  qt.rows[3] = val;
  return qt.vector;
}

template <typename quadvt>
inline quadvt set_x(quadvt const& q, quadvt const& v) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_move_ss(q, v);
  }
  else
  {
    return {v[0], q[1], q[2], q[3]};
  }
}

template <typename quadvt>
inline quadvt set_y(quadvt const& q, quadvt const& v) noexcept
{
  qv_type<qscalar_t<quadvt>> qq;
  qq.vector  = q;
  qq.rows[1] = get_x(v);
  return qq.vector;
}

template <typename quadvt>
inline quadvt set_z(quadvt const& q, quadvt const& v) noexcept
{
  qv_type<qscalar_t<quadvt>> qq;
  qq.vector  = q;
  qq.rows[2] = get_x(v);
  return qq.vector;
}

template <typename quadvt>
inline quadvt set_w(quadvt const& q, quadvt const& v) noexcept
{
  qv_type<qscalar_t<quadvt>> qq;
  qq.vector  = q;
  qq.rows[3] = get_x(v);
  return qq.vector;
}

template <typename quadvt>
inline quadvt half_x(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return quadvt(_mm_mul_ss(_mm_set_ss(0.5f), q));
  }
  else
  {
    qv_type<qscalar_t<quadvt>> qq;
    qq.vector = q;
    qq.rows[0] *= 0.5f;
    return qq.vector;
  }
}

template <typename quadvt>
inline quadvt splat_x(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return quadvt(_mm_shuffle_ps(q, q, _MM_SHUFFLE(0, 0, 0, 0)));
  }
  else
  {
    return {q[0], q[0], q[0], q[0]};
  }
}

template <typename quadvt>
inline quadvt splat_y(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return quadvt(_mm_shuffle_ps(q, q, _MM_SHUFFLE(1, 1, 1, 1)));
  }
  else
  {
    return {q[1], q[1], q[1], q[1]};
  }
}

template <typename quadvt>
inline quadvt splat_z(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return quadvt(_mm_shuffle_ps(q, q, _MM_SHUFFLE(2, 2, 2, 2)));
  }
  else
  {
    return {q[2], q[2], q[2], q[2]};
  }
}

template <typename quadvt>
inline quadvt splat_w(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return quadvt(_mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 3, 3, 3)));
  }
  else
  {
    return {q[3], q[3], q[3], q[3]};
  }
}

template <typename quadvt>
inline bool greater_any(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return (_mm_movemask_ps(_mm_cmpgt_ps(a, b))) != 0;
  }
  else
  {
    return a[0] > b[0] || a[1] > b[1] || a[2] > b[2] || a[3] > b[3];
  }
}

template <typename quadvt>
inline bool greater_all(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return (_mm_movemask_ps(_mm_cmpgt_ps(a, b)) == 0xF);
  }
  else
  {
    return a[0] > b[0] && a[1] > b[1] && a[2] > b[2] && a[3] > b[3];
  }
}

template <typename quadvt>
inline bool greater_all_3(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return (_mm_movemask_ps(_mm_cmpgt_ps(a, b)) == 0x7);
  }
  else
  {
    return a[0] > b[0] && a[1] > b[1] && a[2] > b[2];
  }
}

template <typename quadvt>
inline bool lesser_any(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return (_mm_movemask_ps(_mm_cmplt_ps(a, b))) != 0;
  }
  else
  {
    return a[0] < b[0] || a[1] < b[1] || a[2] < b[2] || a[3] < b[3];
  }
}

template <typename quadvt>
inline bool lesser_all(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return (_mm_movemask_ps(_mm_cmplt_ps(a, b)) == 0xF);
  }
  else
  {
    return a[0] < b[0] && a[1] < b[1] && a[2] < b[2] && a[3] < b[3];
  }
}

template <typename quadvt>
inline bool lesser_all_3(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return (_mm_movemask_ps(_mm_cmplt_ps(a, b)) == 0x7);
  }
  else
  {
    return a[0] < b[0] && a[1] < b[1] && a[2] < b[2];
  }
}

template <typename quadvt>
inline quadvt abs(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {

    return _mm_and_ps(q, _mm_castsi128_ps(_mm_set_epi32(k_signbit_32, k_signbit_32, k_signbit_32, k_signbit_32)));
  }
  else
  {
    return quadvt{std::abs(q[0]), std::abs(q[1]), std::abs(q[2]), std::abs(q[3])};
  }
}

template <typename quadvt>
inline quadvt negate(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    const __m128i k_sign = _mm_set_epi32(k_highbit_32, k_highbit_32, k_highbit_32, k_highbit_32);
    return _mm_xor_ps(q, _mm_castsi128_ps(k_sign));
  }
  else
  {
    return quadvt{-(q[0]), -(q[1]), -(q[2]), -(q[3])};
  }
}

template <typename quadvt>
inline quadvt negate_w(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    const __m128i k_sign = _mm_set_epi32(k_highbit_32, 0, 0, 0);
    return _mm_xor_ps(q, _mm_castsi128_ps(k_sign));
  }
  else
  {
    return quadvt{(q[0]), (q[1]), (q[2]), -(q[3])};
  }
}

template <typename quadvt>
inline quadvt mul(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_mul_ps(a, b);
  }
  else
  {
    return {a[0] * b[0], a[1] * b[1], a[2] * b[2], a[3] * b[3]};
  }
}

template <typename quadvt>
inline quadvt add_x(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_add_ss(a, b);
  }
  else
  {
    return {a[0] + b[0], 0, 0, 0};
  }
}

template <typename quadvt>
inline quadvt sub_x(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_sub_ss(a, b);
  }
  else
  {
    return {a[0] - b[0], 0, 0, 0};
  }
}

template <typename quadvt>
inline quadvt mul_x(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_mul_ss(a, b);
  }
  else
  {
    return {a[0] * b[0], 0, 0, 0};
  }
}

template <typename quadvt>
inline quadvt sqrt_x(quadvt const& a) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_sqrt_ss(a);
  }
  else
  {
    return {std::sqrt(a[0]), 0, 0, 0};
  }
}

template <typename quadvt>
inline quadvt recip_sqrt_x(quadvt const& a) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_rsqrt_ss(a);
  }
  else
  {
    return {acl::recip_sqrt(a[0]), 0, 0, 0};
  }
}

template <typename quadvt>
inline quadvt div(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_div_ps(a, b);
  }
  else
  {
    return {a[0] / b[0], a[1] / b[1], a[2] / b[2], a[3] / b[3]};
  }
}

template <typename quadvt, typename scalar_t>
inline quadvt mul_quad_scalar(quadvt const& q, scalar_t val) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    __m128 res = _mm_set_ps1(val);
    return _mm_mul_ps(q, res);
  }
  else
  {
    return {q[0] * val, q[1] * val, q[2] * val, q[3] * val};
  }
}

template <typename quadvt>
inline quadvt madd(quadvt const& a, quadvt const& v, quadvt const& c) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    quadvt t = _mm_mul_ps(a, v);
    return _mm_add_ps(t, c);
  }
  else
  {
    return add(mul(a, v), c);
  }
}

template <typename quadvt>
inline quadvt vhadd(quadvt const& v) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    if constexpr (has_sse3)
    {
      __m128 shuf = _mm_movehdup_ps(v); // broadcast elements 3,1 to 2,0
      __m128 sums = _mm_add_ps(v, shuf);
      shuf        = _mm_movehl_ps(shuf, sums); // high half -> low half
      sums        = _mm_add_ss(sums, shuf);
      return quadvt{sums};
    }
    else
    {
      __m128 shuf = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 3, 0, 1)); // [ C D | A B ]
      __m128 sums = _mm_add_ps(v, shuf);                           // sums = [ D+C C+D | B+A A+B ]
      shuf        = _mm_movehl_ps(shuf, sums);                     //  [   C   D | D+C C+D ]  // let the
                                                                   //  compiler avoid a mov by reusing shuf
      sums = _mm_add_ss(sums, shuf);
      return quadvt{sums};
    }
  }
  else
  {
    return {v[0] + v[1] + v[2] + v[3], 0, 0, 0};
  }
}

template <typename quadvt>
inline quadvt recip_sqrt(quadvt const& qpf) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    const __m128 approx = _mm_rsqrt_ps(qpf);
    const __m128 muls   = _mm_mul_ps(_mm_mul_ps(qpf, approx), approx);
    return _mm_mul_ps(_mm_mul_ps(_mm_set_ps1(0.5f), approx), _mm_sub_ps(_mm_set_ps1(3.0f), muls));
  }
  else
  {
    return {acl::recip_sqrt(qpf[0]), acl::recip_sqrt(qpf[1]), acl::recip_sqrt(qpf[2]), acl::recip_sqrt(qpf[3])};
  }
}

template <typename quadvt>
inline quadvt select(quadvt const& a, quadvt const& b, quadvt const& c) noexcept
{
  using scalar_t = qscalar_t<quadvt>;
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
  {
    quadvt vtemp1 = _mm_andnot_ps(c, a);
    quadvt vtemp2 = _mm_and_ps(b, c);
    return _mm_or_ps(vtemp1, vtemp2);
  }
  else
  {
    union
    {
      std::array<float_to_int_t<scalar_t>, 4> ival;
      std::array<scalar_t, 4>                 val;
    } ua, ub, uc, uret;

    ua.val = a;
    ub.val = b;
    uc.val = c;
    for (int i = 0; i < 4; ++i)
      uret.ival[i] = ((~uc.ival[i]) & ua.ival[i]) | (uc.ival[i] & ub.ival[i]);
    return uret.val;
  }
}

template <typename quadvt>
inline quadvt vdot(quadvt const& a, quadvt const& b) noexcept;
template <typename quadvt>
inline qscalar_t<quadvt> dot(quadvt const& a, quadvt const& b) noexcept;

template <typename quadvt>
inline quadvt vdot(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    if constexpr (has_sse41)
      return quadvt{_mm_dp_ps(a, b, 0xFF)};
    else if constexpr (has_sse3)
    {
      auto v    = _mm_mul_ps(a, b);
      auto shuf = _mm_movehdup_ps(v); // broadcast elements 3,1 to 2,0
      auto sums = _mm_add_ps(v, shuf);
      shuf      = _mm_movehl_ps(shuf, sums); // high half -> low half
      sums      = _mm_add_ss(sums, shuf);
      return quadvt{sums};
    }
    else
    {
      auto v    = _mm_mul_ps(a, b);
      auto shuf = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 3, 0, 1)); // [ C D | A B ]
      auto sums = _mm_add_ps(v, shuf);                           // sums = [ D+C C+D | B+A A+B ]
      shuf      = _mm_movehl_ps(shuf, sums);                     //  [   C   D | D+C C+D ]  // let the
                                                                 //  compiler avoid a mov by reusing shuf
      sums = _mm_add_ss(sums, shuf);
      return quadvt{sums};
    }
  }
  else
  {
    return {dot(a, b), 0, 0, 0};
  }
}

template <typename quadvt>
inline qscalar_t<quadvt> dot(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return get_x(vdot(a, b));
  }
  else
  {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
  }
}

template <typename quadvt>
inline quadvt normalize(quadvt const& v, quadvt const& l) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    if constexpr (has_sse41)
    {
      auto q = _mm_dp_ps(l, l, 0xFF);
      // get the reciprocal
      q = _mm_sqrt_ps(q);
      return _mm_div_ps(v, q);
    }
    else
    {
      auto q = vdot(l, l);
      q      = _mm_sqrt_ss(q);
      q      = _mm_shuffle_ps(q, q, _MM_SHUFFLE(0, 0, 0, 0));
      return _mm_div_ps(v, q);
    }
  }
  else
  {
    qscalar_t<quadvt> res = qscalar_t<quadvt>(1.0f) / std::sqrt(dot(l, l));
    return set(v[0] * res, v[1] * res, v[2] * res, v[3] * res);
  }
}

template <typename quadvt>
inline quadvt normalize(quadvt const& v) noexcept
{
  return normalize(v, v);
}

template <typename scalar_t>
inline quadv_t<scalar_t> clear_w_mask() noexcept
{
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
    return _mm_castsi128_ps(_mm_set_epi32(0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));
  else
  {
    return {uint_to_float(std::numeric_limits<float_to_uint_t<scalar_t>>::max()),
            uint_to_float(std::numeric_limits<float_to_uint_t<scalar_t>>::max()),
            uint_to_float(std::numeric_limits<float_to_uint_t<scalar_t>>::max()), 0};
  }
}

template <typename scalar_t>
inline quadv_t<scalar_t> xyz0_w1() noexcept
{
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
    return _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
  else
  {
    return {0, 0, 0, static_cast<scalar_t>(1.0)};
  }
}

template <typename scalar_t>
inline quadv_t<scalar_t> clear_xyz() noexcept
{
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
    return _mm_castsi128_ps(_mm_set_epi32(std::numeric_limits<uint32_t>::max(), 0, 0, 0));
  else
  {
    return {uint_to_float(std::numeric_limits<float_to_uint_t<scalar_t>>::max()), 0, 0, 0};
  }
}

template <typename quadvt>
inline quadvt lerp(quadvt const& src, quadvt const& dst, qscalar_t<quadvt> t) noexcept
{
  return madd(set(t), sub(dst, src), src);
}

template <typename quadvt>
inline qscalar_t<quadvt> length(quadvt const& vec) noexcept
{
  return std::sqrt(dot(vec, vec));
}

template <typename quadvt>
inline qscalar_t<quadvt> sqlength(quadvt const& vec) noexcept
{
  return dot(vec, vec);
}

template <typename quadvt>
inline qscalar_t<quadvt> distance(quadvt const& a, quadvt const& b) noexcept
{
  return length(sub(a, b));
}

template <typename quadvt>
inline qscalar_t<quadvt> sqdistance(quadvt const& a, quadvt const& b) noexcept
{
  return sqlength(sub(a, b));
}
template <typename quadvt>
inline quadvt half(quadvt const& a) noexcept
{
  return mul_quad_scalar(a, 0.5f);
}
template <typename quadvt>
inline quadvt set_000w(quadvt const& a, std::uint8_t select) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    switch (select)
    {
    case 0:
      return _mm_and_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(0, 2, 1, 0)), clear_xyz<qscalar_t<quadvt>>());
    case 1:
      return _mm_and_ps(_mm_movelh_ps(a, a), clear_xyz<qscalar_t<quadvt>>());
    case 2:
      return _mm_and_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(2, 2, 1, 0)), clear_xyz<qscalar_t<quadvt>>());
    case 3:
      return _mm_and_ps(a, clear_xyz<qscalar_t<quadvt>>());
    }
    assert(0 && "Not allowed!");
    return _mm_setzero_ps();
  }
  else
  {
    return {0, 0, 0, a[select]};
  }
}
template <typename quadvt>
inline quadvt set_111w(quadvt const& a, std::uint8_t select) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_or_ps(_mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f), set_000w(a, select));
  }
  else
  {
    return {1.0, 1.0, 1.0, a[select]};
  }
}

template <typename quadvt>
inline quadvt exp(quadvt const& a) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return exp_ps(a);
  }
  else
  {
    return {std::exp(a[0]), std::exp(a[1]), std::exp(a[2]), std::exp(a[3])};
  }
}

template <typename quadvt>
inline quadvt log(quadvt const& a) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return log_ps(a);
  }
  else
  {
    return {std::log(a[0]), std::log(a[1]), std::log(a[2]), std::log(a[3])};
  }
}

/**
 * @brief Power function for positive numbers only
 */
template <typename quadvt>
inline quadvt ppow(quadvt const& a, qscalar_t<quadvt> exp) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return exp_ps(_mm_mul_ps(_mm_set1_ps(exp), log_ps(a)));
  }
  else
  {
    return {std::pow(a[0], exp), std::pow(a[1], exp), std::pow(a[2], exp), std::pow(a[3], exp)};
  }
}

template <int idx, typename quadvt>
inline quadvt set_x(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_shuffle_ps(q, q, _MM_SHUFFLE(3, 2, 1, idx));
  }
  else
  {
    return quadvt{q[idx], static_cast<qscalar_t<quadvt>>(0), static_cast<qscalar_t<quadvt>>(0),
                  static_cast<qscalar_t<quadvt>>(0)};
  }
}

template <typename quadvt>
inline quadvt clear_w(quadvt const& q) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    return _mm_and_ps(q, clear_w_mask<qscalar_t<quadvt>>());
  }
  else
  {
    return quadvt{q[0], q[1], q[2], 0};
  }
}

template <typename quadvt>
inline quadvt cross(quadvt const& a, quadvt const& b) noexcept
{
  if constexpr (has_sse && std::is_same_v<qscalar_t<quadvt>, float>)
  {
    // set w to zero
    return _mm_sub_ps(
      _mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2))),
      _mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
  }
  else
    return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
}

template <typename quadvt>
inline quadvt rotate(quadvt const& v, quadvt const& rowx, quadvt const& rowy, quadvt const& rowz) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    auto v_res  = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
    v_res       = _mm_mul_ps(v_res, rowx);
    auto v_temp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
    v_temp      = _mm_mul_ps(v_temp, rowy);
    v_res       = _mm_add_ps(v_res, v_temp);
    v_temp      = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
    v_temp      = _mm_mul_ps(v_temp, rowz);
    v_res       = _mm_add_ps(v_res, v_temp);
    return v_res;
  }
  else
  {
    auto r = mul(splat_z(v), rowz);
    r      = madd(splat_y(v), rowy, r);
    r      = madd(splat_x(v), rowx, r);
    return r;
  }
}

template <typename quadvt>
inline quadvt mul_quad_mat4(quadvt const& v, quadv_array_t<qscalar_t<quadvt>, 4> const& m) noexcept
{
  if constexpr (has_sse && std::is_same_v<qscalar_t<quadvt>, float>)
  {
    auto ret    = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
    ret         = _mm_mul_ps(ret, m[0]);
    auto v_temp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
    v_temp      = _mm_mul_ps(v_temp, m[1]);
    ret         = _mm_add_ps(ret, v_temp);
    v_temp      = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
    v_temp      = _mm_mul_ps(v_temp, m[2]);
    ret         = _mm_add_ps(ret, v_temp);
    v_temp      = _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3));
    v_temp      = _mm_mul_ps(v_temp, m[3]);
    ret         = _mm_add_ps(ret, v_temp);
    return ret;
  }
  else
  {
    auto z = splat_z(v);
    auto y = splat_y(v);
    auto x = splat_x(v);
    auto w = splat_w(v);

    auto r = mul(w, m[3]);
    r      = madd(z, m[2], r);
    r      = madd(y, m[1], r);
    r      = madd(x, m[0], r);

    return r;
  }
}

template <typename quadvt>
inline quadvt mul_quad_mat3(quadvt const& v, quadv_array_t<qscalar_t<quadvt>, 3> const& m) noexcept
{
  if constexpr (has_sse && std::is_same_v<qscalar_t<quadvt>, float>)
  {
    auto ret    = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
    ret         = _mm_mul_ps(ret, m[0]);
    auto v_temp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
    v_temp      = _mm_mul_ps(v_temp, m[1]);
    ret         = _mm_add_ps(ret, v_temp);
    v_temp      = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
    v_temp      = _mm_mul_ps(v_temp, m[2]);
    ret         = _mm_add_ps(ret, v_temp);
    return ret;
  }
  else
  {
    auto x = splat_x(v);
    auto y = splat_y(v);
    auto z = splat_z(v);

    auto r = mul(x, m[0]);
    r      = madd(y, m[1], r);
    r      = madd(z, m[2], r);

    return r;
  }
}

template <typename quadvt>
inline quadvt conjugate_quat(quadvt const& v) noexcept
{
  if constexpr (has_sse && std::is_same_v<qscalar_t<quadvt>, float>)
  {
    const __m128i k_sign = _mm_set_epi32(0, k_highbit_32, k_highbit_32, k_highbit_32);
    return _mm_xor_ps(v, _mm_castsi128_ps(k_sign));
  }
  else
  {
    return quat_t<qscalar_t<quadvt>>(-(v[0]), -(v[1]), -(v[2]), (v[3]));
  }
}

template <typename quadvt>
inline quadvt mul_quat(quadvt const& q1, quadvt const& q2) noexcept
{
  if constexpr (has_sse && std::is_same_v<qscalar_t<quadvt>, float>)
  {
    if constexpr (has_sse3)
    {
      // @link
      // http://momchil-velikov.blogspot.com/2013/10/fast-sse-quternion-multiplication.html
      // Copy to SSE registers and use as few as possible for x86
      __m128 t0 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 3, 3, 3)); /* 1, 0.5 */
      __m128 t1 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(2, 3, 0, 1)); /* 1, 0.5 */

      __m128 t3 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(0, 0, 0, 0)); /* 1, 0.5 */
      __m128 t4 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(1, 0, 3, 2)); /* 1, 0.5 */

      __m128 t5 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 1, 1, 1)); /* 1, 0.5 */
      __m128 t6 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(2, 0, 3, 1)); /* 1, 0.5 */

      /* [d,d,d,d]*[z,w,x,y] = [dz,dw,dx,dy] */
      __m128 m0 = _mm_mul_ps(t0, t1); /* 5/4, 1 */

      /* [a,a,a,a]*[y,x,w,z] = [ay,ax,aw,az]*/
      __m128 m1 = _mm_mul_ps(t3, t4); /* 5/4, 1 */

      /* [b,b,b,b]*[z,x,w,y] = [bz,bx,bw,by]*/
      __m128 m2 = _mm_mul_ps(t5, t6); /* 5/4, 1 */

      /* [c,c,c,c]*[w,z,x,y] = [cw,cz,cx,cy] */
      __m128 t7 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(2, 2, 2, 2)); /* 1, 0.5 */
      __m128 t8 = _mm_shuffle_ps(q2, q2, _MM_SHUFFLE(3, 2, 0, 1)); /* 1, 0.5 */

      __m128 m3 = _mm_mul_ps(t7, t8); /* 5/4, 1 */

      /* 1 */
      /* [dz,dw,dx,dy]+-[ay,ax,aw,az] = [dz+ay,dw-ax,dx+aw,dy-az] */
      __m128 e = _mm_addsub_ps(m0, m1); /* 3, 1 */

      /* 2 */
      /* [dx+aw,dz+ay,dy-az,dw-ax] */
      e = _mm_shuffle_ps(e, e, _MM_SHUFFLE(1, 3, 0, 2)); /* 1, 0.5 */

      /* [dx+aw,dz+ay,dy-az,dw-ax]+-[bz,bx,bw,by] =
       * [dx+aw+bz,dz+ay-bx,dy-az+bw,dw-ax-by]*/
      e = _mm_addsub_ps(e, m2); /* 3, 1 */

      /* 2 */
      /* [dz+ay-bx,dw-ax-by,dy-az+bw,dx+aw+bz] */
      e = _mm_shuffle_ps(e, e, _MM_SHUFFLE(2, 0, 1, 3)); /* 1, 0.5 */

      /* [dz+ay-bx,dw-ax-by,dy-az+bw,dx+aw+bz]+-[cw,cz,cx,cy]
         = [dz+ay-bx+cw,dw-ax-by-cz,dy-az+bw+cx,dx+aw+bz-cy] */
      e = _mm_addsub_ps(e, m3); /* 3, 1 */

      /* 2 */
      /* [dw-ax-by-cz,dz+ay-bx+cw,dy-az+bw+cx,dx+aw+bz-cy] */
      e = _mm_shuffle_ps(e, e, _MM_SHUFFLE(2, 3, 1, 0)); /* 1, 0.5 */
      return e;
    }
    else
    {

      // Copy to SSE registers and use as few as possible for x86
      __m128 result;
      {
        result = mul(splat_w(q2), q1);
      }
      {
        const __m128i k_sign = _mm_set_epi32(0x80000000, 0x80000000, 0x00000000, 0x00000000);
        __m128        t      = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(0, 1, 2, 3));
        t                    = mul(splat_x(q2), t);
        t                    = _mm_xor_ps(t, _mm_castsi128_ps(k_sign));
        result               = _mm_add_ps(t, result);
      }
      {
        const __m128i k_sign = _mm_set_epi32(0x80000000, 0x00000000, 0x00000000, 0x80000000);
        __m128        t      = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 0, 3, 2));
        t                    = mul(splat_y(q2), t);
        t                    = _mm_xor_ps(t, _mm_castsi128_ps(k_sign));
        result               = _mm_add_ps(t, result);
      }
      {
        const __m128i k_sign = _mm_set_epi32(0x80000000, 0x00000000, 0x80000000, 0x00000000);
        __m128        t      = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(2, 3, 0, 1));
        t                    = mul(splat_z(q2), t);
        t                    = _mm_xor_ps(t, _mm_castsi128_ps(k_sign));
        result               = _mm_add_ps(t, result);
      }
      return result;
    }
  }
  else
    return quadvt{(q2[3] * q1[0]) + (q2[0] * q1[3]) - (q2[1] * q1[2]) + (q2[2] * q1[1]),
                  (q2[3] * q1[1]) + (q2[0] * q1[2]) + (q2[1] * q1[3]) - (q2[2] * q1[0]),
                  (q2[3] * q1[2]) - (q2[0] * q1[1]) + (q2[1] * q1[0]) + (q2[2] * q1[3]),
                  (q2[3] * q1[3]) - (q2[0] * q1[0]) - (q2[1] * q1[1]) - (q2[2] * q1[2])};
}

template <typename quadvt>
inline quadvt mul_vec3a_quat(quadvt const& v, quadvt const& q) noexcept
{
  return vml::mul_quat(vml::mul_quat(q, v), vml::conjugate_quat(q));
}

template <typename quadvt>
inline quadvt mul_extends_quat(quadvt const& v, quadvt const& rot) noexcept
{
  if constexpr (has_sse && std::is_same_v<qscalar_t<quadvt>, float>)
  {
    auto fff0 = _mm_castsi128_ps(_mm_set_epi32(0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));

    auto q0 = _mm_add_ps(rot, rot);
    auto q1 = _mm_mul_ps(rot, q0);

    auto v0 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 0, 0, 1));
    auto v1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 1, 2, 2));
    auto r0 = _mm_sub_ps(_mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f), v0);
    r0      = _mm_sub_ps(r0, v1);

    v0 = _mm_shuffle_ps(rot, rot, _MM_SHUFFLE(3, 1, 0, 0));
    v1 = _mm_shuffle_ps(q0, q0, _MM_SHUFFLE(3, 2, 1, 2));
    v0 = _mm_mul_ps(v0, v1);

    v1      = _mm_shuffle_ps(rot, rot, _MM_SHUFFLE(3, 3, 3, 3));
    auto v2 = _mm_shuffle_ps(q0, q0, _MM_SHUFFLE(3, 0, 2, 1));
    v1      = _mm_mul_ps(v1, v2);

    auto r1 = _mm_add_ps(v0, v1);
    auto r2 = _mm_sub_ps(v0, v1);

    v0 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(1, 0, 2, 1));
    v0 = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(1, 3, 2, 0));
    v1 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(2, 2, 0, 0));
    v1 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(2, 0, 2, 0));

    q1 = _mm_shuffle_ps(r0, v0, _MM_SHUFFLE(1, 0, 3, 0));
    q1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 3, 2, 0));

    auto t0 = abs(_mm_mul_ps(splat_x(v), q1));
    q1      = _mm_shuffle_ps(r0, v0, _MM_SHUFFLE(3, 2, 3, 1));
    q1      = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 3, 0, 2));
    auto t1 = abs(_mm_mul_ps(splat_y(v), q1));
    q1      = _mm_shuffle_ps(v1, r0, _MM_SHUFFLE(3, 2, 1, 0));
    auto t2 = abs(_mm_mul_ps(splat_z(v), q1));
    return clear_w(add(t0, add(t1, t2)));
  }
  else
  {
    auto xx = rot[0] * rot[0];
    auto yy = rot[1] * rot[1];
    auto zz = rot[2] * rot[2];
    auto xy = rot[0] * rot[1];
    auto xz = rot[0] * rot[2];
    auto yz = rot[1] * rot[2];
    auto wx = rot[3] * rot[0];
    auto wy = rot[3] * rot[1];
    auto wz = rot[3] * rot[2];

    auto t0 = abs(quadvt{v[0] * (1 - 2 * (yy + zz)), v[0] * (2 * (xy + wz)), v[0] * (2 * (xz - wy))});

    auto t1 = abs(quadvt{v[1] * (2 * (xy - wz)), v[1] * (1 - 2 * (xx + zz)), v[1] * (2 * (yz + wx))});

    auto t2 = abs(quadvt{v[2] * (2 * (xz + wy)), v[2] * (2 * (yz - wx)), v[2] * (1 - 2 * (xx + yy))});

    return clear_w(add(t0, add(t1, t2)));
  }
}

template <typename scalar_t>
inline quadv_array_t<scalar_t, 3> transpose(quadv_array_t<scalar_t, 3> const& m) noexcept
{
  quadv_array_t<scalar_t, 3> ret;
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
  {
    ret[0] = _mm_move_ss(_mm_shuffle_ps(m[1], m[2], _MM_SHUFFLE(3, 0, 0, 3)), m[0]);
    ret[1] = _mm_shuffle_ps(_mm_shuffle_ps(m[0], m[1], _MM_SHUFFLE(3, 1, 3, 1)), m[2], _MM_SHUFFLE(3, 1, 2, 0));
    ret[2] = _mm_shuffle_ps(_mm_shuffle_ps(m[0], m[1], _MM_SHUFFLE(3, 2, 3, 2)), m[2], _MM_SHUFFLE(3, 2, 2, 0));
    return ret;
  }
  else
  {
    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
        ret[i][j] = m[j][i];
      ret[i][3] = 0;
    }
    return ret;
  }
}

template <typename scalar_t>
inline quadv_array_t<scalar_t, 4> transpose(quadv_array_t<scalar_t, 4> const& m) noexcept
{
  quadv_array_t<scalar_t, 4> ret;
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
  {
    auto tmp0 = _mm_unpacklo_ps(m[0], m[1]);
    auto tmp2 = _mm_unpacklo_ps(m[2], m[3]);
    auto tmp1 = _mm_unpackhi_ps(m[0], m[1]);
    auto tmp3 = _mm_unpackhi_ps(m[2], m[3]);
    ret[0]    = _mm_movelh_ps(tmp0, tmp2);
    ret[1]    = _mm_movehl_ps(tmp2, tmp0);
    ret[2]    = _mm_movelh_ps(tmp1, tmp3);
    ret[3]    = _mm_movehl_ps(tmp3, tmp1);
  }
  else
  {
    for (int i = 0; i < 4; ++i)
      for (int j = 0; j < 4; ++j)
        ret[i][j] = m[j][i];
  }
  return ret;
}

/**
 * @brief Full matrix multiplication
 */
template <typename scalar_t>
inline quadv_array_t<scalar_t, 4> mul_mat4(quadv_array_t<scalar_t, 4> const& m1,
                                           quadv_array_t<scalar_t, 4> const& m2) noexcept
{
  quadv_array_t<scalar_t, 4> ret;

  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
  {
    // Use vw to hold the original row
    __m128 vw = m1[0];
    // Splat the component x,y,Z then W
    __m128 vx = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 vy = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 vz = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(2, 2, 2, 2));
    vw        = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(3, 3, 3, 3));
    // Perform the opertion on the first row
    vx = _mm_mul_ps(vx, m2[0]);
    vy = _mm_mul_ps(vy, m2[1]);
    vz = _mm_mul_ps(vz, m2[2]);
    vw = _mm_mul_ps(vw, m2[3]);
    // Perform a binary add to reduce cumulative errors
    vx     = _mm_add_ps(vx, vz);
    vy     = _mm_add_ps(vy, vw);
    vx     = _mm_add_ps(vx, vy);
    ret[0] = vx;
    // Repeat for the other 3 rows
    vw     = m1[1];
    vx     = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(0, 0, 0, 0));
    vy     = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(1, 1, 1, 1));
    vz     = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(2, 2, 2, 2));
    vw     = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(3, 3, 3, 3));
    vx     = _mm_mul_ps(vx, m2[0]);
    vy     = _mm_mul_ps(vy, m2[1]);
    vz     = _mm_mul_ps(vz, m2[2]);
    vw     = _mm_mul_ps(vw, m2[3]);
    vx     = _mm_add_ps(vx, vz);
    vy     = _mm_add_ps(vy, vw);
    vx     = _mm_add_ps(vx, vy);
    ret[1] = vx;
    vw     = m1[2];
    vx     = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(0, 0, 0, 0));
    vy     = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(1, 1, 1, 1));
    vz     = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(2, 2, 2, 2));
    vw     = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(3, 3, 3, 3));
    vx     = _mm_mul_ps(vx, m2[0]);
    vy     = _mm_mul_ps(vy, m2[1]);
    vz     = _mm_mul_ps(vz, m2[2]);
    vw     = _mm_mul_ps(vw, m2[3]);
    vx     = _mm_add_ps(vx, vz);
    vy     = _mm_add_ps(vy, vw);
    vx     = _mm_add_ps(vx, vy);
    ret[2] = vx;
    vw     = m1[3];
    vx     = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(0, 0, 0, 0));
    vy     = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(1, 1, 1, 1));
    vz     = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(2, 2, 2, 2));
    vw     = _mm_shuffle_ps(vw, vw, _MM_SHUFFLE(3, 3, 3, 3));
    vx     = _mm_mul_ps(vx, m2[0]);
    vy     = _mm_mul_ps(vy, m2[1]);
    vz     = _mm_mul_ps(vz, m2[2]);
    vw     = _mm_mul_ps(vw, m2[3]);
    vx     = _mm_add_ps(vx, vz);
    vy     = _mm_add_ps(vy, vw);
    vx     = _mm_add_ps(vx, vy);
    ret[3] = vx;
  }
  else
  {
    for (int i = 0; i < 4; ++i)
    {
      auto x    = m1[i][0];
      auto y    = m1[i][1];
      auto z    = m1[i][2];
      auto w    = m1[i][3];
      ret[i][0] = (m2[0][0] * x) + (m2[1][0] * y) + (m2[2][0] * z) + (m2[3][0] * w);
      ret[i][1] = (m2[0][1] * x) + (m2[1][1] * y) + (m2[2][1] * z) + (m2[3][1] * w);
      ret[i][2] = (m2[0][2] * x) + (m2[1][2] * y) + (m2[2][2] * z) + (m2[3][2] * w);
      ret[i][3] = (m2[0][3] * x) + (m2[1][3] * y) + (m2[2][3] * z) + (m2[3][3] * w);
    }
  }

  return ret;
}

/**
 * @brief Full matrix multiplication
 */
template <typename quadvt>
inline quadvt mul_transform(quadvt const& v, quadv_array_t<qscalar_t<quadvt>, 4> const& m) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    auto ret    = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
    ret         = _mm_mul_ps(ret, m[0]);
    auto v_temp = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
    v_temp      = _mm_mul_ps(v_temp, m[1]);
    ret         = _mm_add_ps(ret, v_temp);
    v_temp      = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
    v_temp      = _mm_mul_ps(v_temp, m[2]);
    ret         = _mm_add_ps(ret, v_temp);
    ret         = _mm_add_ps(ret, m[3]);
    return clear_w(ret);
  }
  else
  {
    auto x = splat_x(v);
    auto y = splat_y(v);
    auto z = splat_z(v);

    auto r = madd(z, m[2], m[3]);
    r      = madd(y, m[1], r);
    r      = madd(x, m[0], r);
    return clear_w(r);
  }
}

/**
 * @brief Full matrix multiplication
 */
template <typename quadvt>
inline quadvt mul_extends_mat4(quadvt const& v, quadv_array_t<qscalar_t<quadvt>, 4> const& m) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    auto ret        = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0));
    auto clear_sign = _mm_castsi128_ps(_mm_set1_epi32(0x7fffffff));
    ret             = _mm_mul_ps(ret, m[0]);
    ret             = _mm_and_ps(ret, clear_sign);
    auto v_temp     = _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1));
    v_temp          = _mm_mul_ps(v_temp, m[1]);
    v_temp          = _mm_and_ps(v_temp, clear_sign);
    ret             = _mm_add_ps(ret, v_temp);
    v_temp          = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2));
    v_temp          = _mm_mul_ps(v_temp, m[2]);
    v_temp          = _mm_and_ps(v_temp, clear_sign);
    ret             = _mm_add_ps(ret, v_temp);
    return ret;
  }
  else
  {
    quadvt ret{0, 0, 0, 0};
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        ret[i] += v[j] * std::abs(m[j][i]);
    return ret;
  }
}

template <typename scalar_t>
inline quadv_array_t<scalar_t, 2> mul_aabb_mat4(quadv_array_t<scalar_t, 2> const& box,
                                                quadv_array_t<scalar_t, 4> const& m) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
  {
    auto max0 = _mm_shuffle_ps(box[1], box[1], _MM_SHUFFLE(0, 0, 0, 0));
    max0      = _mm_mul_ps(max0, m[0]);
    auto min0 = _mm_shuffle_ps(box[0], box[0], _MM_SHUFFLE(0, 0, 0, 0));
    min0      = _mm_mul_ps(min0, m[0]);
    auto max1 = _mm_shuffle_ps(box[1], box[1], _MM_SHUFFLE(1, 1, 1, 1));
    max1      = _mm_mul_ps(max1, m[1]);
    auto min1 = _mm_shuffle_ps(box[0], box[0], _MM_SHUFFLE(1, 1, 1, 1));
    min1      = _mm_mul_ps(min1, m[1]);
    auto max2 = _mm_shuffle_ps(box[1], box[1], _MM_SHUFFLE(2, 2, 2, 2));
    max2      = _mm_mul_ps(max2, m[2]);
    auto min2 = _mm_shuffle_ps(box[0], box[0], _MM_SHUFFLE(2, 2, 2, 2));
    min2      = _mm_mul_ps(min2, m[2]);

    quadv_array_t<scalar_t, 2> ret;
    ret[0] = clear_w(
      _mm_add_ps(_mm_add_ps(_mm_min_ps(max0, min0), _mm_add_ps(_mm_min_ps(max1, min1), _mm_min_ps(max2, min2))), m[3]));
    ret[1] = clear_w(
      _mm_add_ps(_mm_add_ps(_mm_max_ps(max0, min0), _mm_add_ps(_mm_max_ps(max1, min1), _mm_max_ps(max2, min2))), m[3]));
    return ret;
  }
  else
  {
    quadv_array_t<scalar_t, 2> ret;
    for (int i = 0; i < 3; i++)
    {
      ret[0][i] = std::min(box[0][0] * m[0][i], box[1][0] * m[0][i]) +
                  std::min(box[0][1] * m[1][i], box[1][1] * m[1][i]) +
                  std::min(box[0][2] * m[2][i], box[1][2] * m[2][i]) + m[3][i];
    }
    ret[0][3] = 0;
    for (int i = 0; i < 3; i++)
    {
      ret[1][i] = std::max(box[0][0] * m[0][i], box[1][0] * m[0][i]) +
                  std::max(box[0][1] * m[1][i], box[1][1] * m[1][i]) +
                  std::max(box[0][2] * m[2][i], box[1][2] * m[2][i]) + m[3][i];
    }
    ret[1][3] = 0;
    return ret;
  }
}

template <typename scalar_t, typename quadvt>
inline quadv_array_t<scalar_t, 4> make_mat4(scalar_t scale, quadvt const& rot, quadvt const& pos) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, qscalar_t<quadvt>>)
  {
    const auto fff0 = _mm_castsi128_ps(_mm_set_epi32(0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF));

    auto q0 = _mm_add_ps(rot, rot);
    auto q1 = _mm_mul_ps(rot, q0);

    auto v0 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 0, 0, 1));
    auto v1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(3, 1, 2, 2));
    auto r0 = _mm_sub_ps(_mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f), v0);
    r0      = _mm_and_ps(_mm_sub_ps(r0, v1), fff0);

    v0 = _mm_shuffle_ps(rot, rot, _MM_SHUFFLE(3, 1, 0, 0));
    v1 = _mm_shuffle_ps(q0, q0, _MM_SHUFFLE(3, 2, 1, 2));
    v0 = _mm_mul_ps(v0, v1);

    v1      = _mm_shuffle_ps(rot, rot, _MM_SHUFFLE(3, 3, 3, 3));
    auto v2 = _mm_shuffle_ps(q0, q0, _MM_SHUFFLE(3, 0, 2, 1));
    v1      = _mm_mul_ps(v1, v2);

    auto r1 = _mm_add_ps(v0, v1);
    auto r2 = _mm_sub_ps(v0, v1);

    v0 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(1, 0, 2, 1));
    v0 = _mm_shuffle_ps(v0, v0, _MM_SHUFFLE(1, 3, 2, 0));
    v1 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(2, 2, 0, 0));
    v1 = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(2, 0, 2, 0));

    q1 = _mm_shuffle_ps(r0, v0, _MM_SHUFFLE(1, 0, 3, 0));
    q1 = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 3, 2, 0));

    auto scale_q = _mm_set1_ps(scale);

    quadv_array_t<qscalar_t<quadvt>, 4> ret;
    ret[0] = _mm_mul_ps(scale_q, q1);
    q1     = _mm_shuffle_ps(r0, v0, _MM_SHUFFLE(3, 2, 3, 1));
    q1     = _mm_shuffle_ps(q1, q1, _MM_SHUFFLE(1, 3, 0, 2));
    ret[1] = _mm_mul_ps(scale_q, q1);
    q1     = _mm_shuffle_ps(v1, r0, _MM_SHUFFLE(3, 2, 1, 0));
    ret[2] = _mm_mul_ps(scale_q, q1);
    ret[3] = _mm_or_ps(_mm_and_ps(pos, fff0), _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f));
    return ret;
  }
  else
  {
    quadv_array_t<qscalar_t<quadvt>, 4> ret;
    auto                                xx = rot[0] * rot[0];
    auto                                yy = rot[1] * rot[1];
    auto                                zz = rot[2] * rot[2];
    auto                                xy = rot[0] * rot[1];
    auto                                xz = rot[0] * rot[2];
    auto                                yz = rot[1] * rot[2];
    auto                                wx = rot[3] * rot[0];
    auto                                wy = rot[3] * rot[1];
    auto                                wz = rot[3] * rot[2];

    ret[0][0] = scale * (1 - 2 * (yy + zz));
    ret[0][1] = scale * (2 * (xy + wz));
    ret[0][2] = scale * (2 * (xz - wy));

    ret[1][0] = scale * (2 * (xy - wz));
    ret[1][1] = scale * (1 - 2 * (xx + zz));
    ret[1][2] = scale * (2 * (yz + wx));

    ret[2][0] = scale * (2 * (xz + wy));
    ret[2][1] = scale * (2 * (yz - wx));
    ret[2][2] = scale * (1 - 2 * (xx + yy));

    ret[0][3] = ret[1][3] = ret[2][3] = 0.0f;
    ret[3][0]                         = pos[0];
    ret[3][1]                         = pos[1];
    ret[3][2]                         = pos[2];
    ret[3][3]                         = 1.0f;

    return ret;
  }
}

/**
 * @brief Matrix full inverse computation
 */
template <typename scalar_t>
inline quadv_array_t<scalar_t, 4> inverse(quadv_array_t<scalar_t, 4> const& m) noexcept
{
  if constexpr (has_sse && std::is_same_v<float, scalar_t>)
  {
    auto mt  = transpose(m);
    auto v00 = _mm_shuffle_ps(mt[2], mt[2], _MM_SHUFFLE(1, 1, 0, 0));
    auto v10 = _mm_shuffle_ps(mt[3], mt[3], _MM_SHUFFLE(3, 2, 3, 2));
    auto v01 = _mm_shuffle_ps(mt[0], mt[0], _MM_SHUFFLE(1, 1, 0, 0));
    auto v11 = _mm_shuffle_ps(mt[1], mt[1], _MM_SHUFFLE(3, 2, 3, 2));
    auto v02 = _mm_shuffle_ps(mt[2], mt[0], _MM_SHUFFLE(2, 0, 2, 0));
    auto v12 = _mm_shuffle_ps(mt[3], mt[1], _MM_SHUFFLE(3, 1, 3, 1));

    auto D0 = _mm_mul_ps(v00, v10);
    auto D1 = _mm_mul_ps(v01, v11);
    auto D2 = _mm_mul_ps(v02, v12);

    v00 = _mm_shuffle_ps(mt[2], mt[2], _MM_SHUFFLE(3, 2, 3, 2));
    v10 = _mm_shuffle_ps(mt[3], mt[3], _MM_SHUFFLE(1, 1, 0, 0));
    v01 = _mm_shuffle_ps(mt[0], mt[0], _MM_SHUFFLE(3, 2, 3, 2));
    v11 = _mm_shuffle_ps(mt[1], mt[1], _MM_SHUFFLE(1, 1, 0, 0));
    v02 = _mm_shuffle_ps(mt[2], mt[0], _MM_SHUFFLE(3, 1, 3, 1));
    v12 = _mm_shuffle_ps(mt[3], mt[1], _MM_SHUFFLE(2, 0, 2, 0));

    v00 = _mm_mul_ps(v00, v10);
    v01 = _mm_mul_ps(v01, v11);
    v02 = _mm_mul_ps(v02, v12);
    D0  = _mm_sub_ps(D0, v00);
    D1  = _mm_sub_ps(D1, v01);
    D2  = _mm_sub_ps(D2, v02);
    // v11 = D0Y,D0W,D2Y,D2Y
    v11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 1, 3, 1));
    v00 = _mm_shuffle_ps(mt[1], mt[1], _MM_SHUFFLE(1, 0, 2, 1));
    v10 = _mm_shuffle_ps(v11, D0, _MM_SHUFFLE(0, 3, 0, 2));
    v01 = _mm_shuffle_ps(mt[0], mt[0], _MM_SHUFFLE(0, 1, 0, 2));
    v11 = _mm_shuffle_ps(v11, D0, _MM_SHUFFLE(2, 1, 2, 1));
    // v13 = D1Y,D1W,D2W,D2W
    auto v13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 3, 3, 1));
    v02      = _mm_shuffle_ps(mt[3], mt[3], _MM_SHUFFLE(1, 0, 2, 1));
    v12      = _mm_shuffle_ps(v13, D1, _MM_SHUFFLE(0, 3, 0, 2));
    auto v03 = _mm_shuffle_ps(mt[2], mt[2], _MM_SHUFFLE(0, 1, 0, 2));
    v13      = _mm_shuffle_ps(v13, D1, _MM_SHUFFLE(2, 1, 2, 1));

    auto c0 = _mm_mul_ps(v00, v10);
    auto c2 = _mm_mul_ps(v01, v11);
    auto c4 = _mm_mul_ps(v02, v12);
    auto c6 = _mm_mul_ps(v03, v13);

    // v11 = D0X,D0Y,D2X,D2X
    v11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(0, 0, 1, 0));
    v00 = _mm_shuffle_ps(mt[1], mt[1], _MM_SHUFFLE(2, 1, 3, 2));
    v10 = _mm_shuffle_ps(D0, v11, _MM_SHUFFLE(2, 1, 0, 3));
    v01 = _mm_shuffle_ps(mt[0], mt[0], _MM_SHUFFLE(1, 3, 2, 3));
    v11 = _mm_shuffle_ps(D0, v11, _MM_SHUFFLE(0, 2, 1, 2));
    // v13 = D1X,D1Y,D2Z,D2Z
    v13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(2, 2, 1, 0));
    v02 = _mm_shuffle_ps(mt[3], mt[3], _MM_SHUFFLE(2, 1, 3, 2));
    v12 = _mm_shuffle_ps(D1, v13, _MM_SHUFFLE(2, 1, 0, 3));
    v03 = _mm_shuffle_ps(mt[2], mt[2], _MM_SHUFFLE(1, 3, 2, 3));
    v13 = _mm_shuffle_ps(D1, v13, _MM_SHUFFLE(0, 2, 1, 2));

    v00 = _mm_mul_ps(v00, v10);
    v01 = _mm_mul_ps(v01, v11);
    v02 = _mm_mul_ps(v02, v12);
    v03 = _mm_mul_ps(v03, v13);
    c0  = _mm_sub_ps(c0, v00);
    c2  = _mm_sub_ps(c2, v01);
    c4  = _mm_sub_ps(c4, v02);
    c6  = _mm_sub_ps(c6, v03);

    v00 = _mm_shuffle_ps(mt[1], mt[1], _MM_SHUFFLE(0, 3, 0, 3));
    // v10 = D0Z,D0Z,D2X,D2Y
    v10 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 2, 2));
    v10 = _mm_shuffle_ps(v10, v10, _MM_SHUFFLE(0, 2, 3, 0));
    v01 = _mm_shuffle_ps(mt[0], mt[0], _MM_SHUFFLE(2, 0, 3, 1));
    // v11 = D0X,D0W,D2X,D2Y
    v11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 3, 0));
    v11 = _mm_shuffle_ps(v11, v11, _MM_SHUFFLE(2, 1, 0, 3));
    v02 = _mm_shuffle_ps(mt[3], mt[3], _MM_SHUFFLE(0, 3, 0, 3));
    // v12 = D1Z,D1Z,D2Z,D2W
    v12 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 2, 2));
    v12 = _mm_shuffle_ps(v12, v12, _MM_SHUFFLE(0, 2, 3, 0));
    v03 = _mm_shuffle_ps(mt[2], mt[2], _MM_SHUFFLE(2, 0, 3, 1));
    // v13 = D1X,D1W,D2Z,D2W
    v13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 3, 0));
    v13 = _mm_shuffle_ps(v13, v13, _MM_SHUFFLE(2, 1, 0, 3));

    v00     = _mm_mul_ps(v00, v10);
    v01     = _mm_mul_ps(v01, v11);
    v02     = _mm_mul_ps(v02, v12);
    v03     = _mm_mul_ps(v03, v13);
    auto c1 = _mm_sub_ps(c0, v00);
    c0      = _mm_add_ps(c0, v00);
    auto c3 = _mm_add_ps(c2, v01);
    c2      = _mm_sub_ps(c2, v01);
    auto c5 = _mm_sub_ps(c4, v02);
    c4      = _mm_add_ps(c4, v02);
    auto c7 = _mm_add_ps(c6, v03);
    c6      = _mm_sub_ps(c6, v03);

    c0 = _mm_shuffle_ps(c0, c1, _MM_SHUFFLE(3, 1, 2, 0));
    c2 = _mm_shuffle_ps(c2, c3, _MM_SHUFFLE(3, 1, 2, 0));
    c4 = _mm_shuffle_ps(c4, c5, _MM_SHUFFLE(3, 1, 2, 0));
    c6 = _mm_shuffle_ps(c6, c7, _MM_SHUFFLE(3, 1, 2, 0));
    c0 = _mm_shuffle_ps(c0, c0, _MM_SHUFFLE(3, 1, 2, 0));
    c2 = _mm_shuffle_ps(c2, c2, _MM_SHUFFLE(3, 1, 2, 0));
    c4 = _mm_shuffle_ps(c4, c4, _MM_SHUFFLE(3, 1, 2, 0));
    c6 = _mm_shuffle_ps(c6, c6, _MM_SHUFFLE(3, 1, 2, 0));
    // get the determinate
    auto                       v_temp = splat_x(_mm_div_ss(_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f), vdot(c0, mt[0])));
    quadv_array_t<scalar_t, 4> result;
    result[0] = _mm_mul_ps(c0, v_temp);
    result[1] = _mm_mul_ps(c2, v_temp);
    result[2] = _mm_mul_ps(c4, v_temp);
    result[3] = _mm_mul_ps(c6, v_temp);
    return result;
  }
  else
  {
    auto m2323 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
    auto m1323 = m[2][1] * m[3][3] - m[2][3] * m[3][1];
    auto m1223 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
    auto m0323 = m[2][0] * m[3][3] - m[2][3] * m[3][0];
    auto m0223 = m[2][0] * m[3][2] - m[2][2] * m[3][0];
    auto m0123 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
    auto m2313 = m[1][2] * m[3][3] - m[1][3] * m[3][2];
    auto m1313 = m[1][1] * m[3][3] - m[1][3] * m[3][1];
    auto m1213 = m[1][1] * m[3][2] - m[1][2] * m[3][1];
    auto m2312 = m[1][2] * m[2][3] - m[1][3] * m[2][2];
    auto m1312 = m[1][1] * m[2][3] - m[1][3] * m[2][1];
    auto m1212 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
    auto m0313 = m[1][0] * m[3][3] - m[1][3] * m[3][0];
    auto m0213 = m[1][0] * m[3][2] - m[1][2] * m[3][0];
    auto m0312 = m[1][0] * m[2][3] - m[1][3] * m[2][0];
    auto m0212 = m[1][0] * m[2][2] - m[1][2] * m[2][0];
    auto m0113 = m[1][0] * m[3][1] - m[1][1] * m[3][0];
    auto m0112 = m[1][0] * m[2][1] - m[1][1] * m[2][0];

    auto det = m[0][0] * (m[1][1] * m2323 - m[1][2] * m1323 + m[1][3] * m1223) -
               m[0][1] * (m[1][0] * m2323 - m[1][2] * m0323 + m[1][3] * m0223) +
               m[0][2] * (m[1][0] * m1323 - m[1][1] * m0323 + m[1][3] * m0123) -
               m[0][3] * (m[1][0] * m1223 - m[1][1] * m0223 + m[1][2] * m0123);
    det = 1 / det;

    return quadv_array_t<scalar_t, 4>{
  // clang-format off
      quadv_t<scalar_t>{
       det * (m[1][1] * m2323 - m[1][2] * m1323 + m[1][3] * m1223),
       det * -(m[0][1] * m2323 - m[0][2] * m1323 + m[0][3] * m1223),
       det * (m[0][1] * m2313 - m[0][2] * m1313 + m[0][3] * m1213),
       det * -(m[0][1] * m2312 - m[0][2] * m1312 + m[0][3] * m1212) 
      }, 
      quadv_t<scalar_t>{
       det * -(m[1][0] * m2323 - m[1][2] * m0323 + m[1][3] * m0223),
       det * (m[0][0] * m2323 - m[0][2] * m0323 + m[0][3] * m0223),
       det * -(m[0][0] * m2313 - m[0][2] * m0313 + m[0][3] * m0213),
       det * (m[0][0] * m2312 - m[0][2] * m0312 + m[0][3] * m0212)
      }, 
      quadv_t<scalar_t>{
       det * (m[1][0] * m1323 - m[1][1] * m0323 + m[1][3] * m0123),
       det * -(m[0][0] * m1323 - m[0][1] * m0323 + m[0][3] * m0123),
       det * (m[0][0] * m1313 - m[0][1] * m0313 + m[0][3] * m0113),
       det * -(m[0][0] * m1312 - m[0][1] * m0312 + m[0][3] * m0112)
      }, 
      quadv_t<scalar_t>{
       det * -(m[1][0] * m1223 - m[1][1] * m0223 + m[1][2] * m0123),
       det * (m[0][0] * m1223 - m[0][1] * m0223 + m[0][2] * m0123),
       det * -(m[0][0] * m1213 - m[0][1] * m0213 + m[0][2] * m0113),
       det * (m[0][0] * m1212 - m[0][1] * m0212 + m[0][2] * m0112)
      }
  // clang-format on
    };
  }
}

} // namespace acl::vml
