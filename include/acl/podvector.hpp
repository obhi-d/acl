/*
 * podvector.hpp
 *
 *  Created on: 11/2/2018, 10:48:02 AM
 *      Author: obhi
 */

#pragma once
#include "default_allocator.hpp"
#include "type_traits.hpp"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <memory>
// refer to:
// https://en.cppreference.com/w/cpp/header/vector
namespace acl
{
template <typename Ty, typename Allocator = default_allocator<>, typename SizeType = typename Allocator::size_type>
class podvector : public Allocator
{
  static_assert(std::is_trivially_copyable_v<Ty>, "Requires trivially copyable on Ty");
  static_assert(!std::is_same<bool, Ty>::value, "podvector<bool,...> not allowed -> recommended use is via "
                                                "std::vector<bool,...>");

public:
  using value_type                = Ty;
  using allocator_type            = Allocator;
  using size_type                 = SizeType;
  using difference_type           = size_type;
  using reference                 = value_type&;
  using const_reference           = const value_type&;
  using pointer                   = Ty*;
  using const_pointer             = Ty const*;
  using iterator                  = Ty*;
  using const_iterator            = Ty const*;
  using reverse_iterator          = std::reverse_iterator<iterator>;
  using const_reverse_iterator    = std::reverse_iterator<const_iterator>;
  using allocator                 = Allocator;
  using allocator_tag             = typename Allocator::tag;
  using allocator_is_always_equal = typename acl::allocator_traits<allocator_tag>::is_always_equal;
  using propagate_allocator_on_move =
    typename acl::allocator_traits<allocator_tag>::propagate_on_container_move_assignment;
  using propagate_allocator_on_copy =
    typename acl::allocator_traits<allocator_tag>::propagate_on_container_copy_assignment;
  using propagate_allocator_on_swap = typename acl::allocator_traits<allocator_tag>::propagate_on_container_swap;

  explicit podvector(const Allocator& alloc = Allocator()) noexcept
      : Allocator(alloc), data_(nullptr), size_(0), capacity_(0){};

  explicit podvector(size_type n) noexcept : data_(allocate(n)), size_(n), capacity_(n) {}

  podvector(size_type n, const Ty& value, const Allocator& alloc = Allocator())
      : Allocator(alloc), data_(allocate(n)), size_(n), capacity_(n)
  {
    std::uninitialized_fill_n(data_, n, value);
  }

  template <class InputIterator>
  podvector(InputIterator first, InputIterator last, const Allocator& alloc = Allocator()) noexcept : Allocator(alloc)
  {
    construct_from_range(first, last, std::is_integral<InputIterator>());
  }

  podvector(const podvector<Ty, Allocator>& x) noexcept

      : data_(allocate(x.capacity_)), size_(x.size_), capacity_(x.capacity_)
  {
    copy(std::begin(x), std::end(x), data_);
  }
  podvector(podvector&& x) noexcept : size_(x.size_), capacity_(x.capacity_), data_(x.data_)
  {
    std::memset(&x, 0, sizeof(x));
  };
  podvector(const podvector& x, const Allocator& alloc) noexcept
      : Allocator(alloc), data_(allocate(x.capacity_)), size_(x.size_), capacity_(x.capacity_)
  {
    copy(std::begin(x), std::end(x), data_);
  }
  podvector(podvector&& x, const Allocator& alloc) noexcept
      : Allocator(alloc), data_(x.data_), size_(x.size_), capacity_(x.capacity_)
  {
    std::memset(&x, 0, sizeof(x));
  };
  podvector(std::initializer_list<Ty> x, const Allocator& alloc = Allocator()) noexcept
      : Allocator(alloc), size_(static_cast<size_type>(x.size())), capacity_(size_)
  {
    data_ = allocate(static_cast<size_type>(x.size()));
    copy(std::begin(x), std::end(x), data_);
  }

  ~podvector() noexcept
  {
    deallocate();
  }
  podvector<Ty, Allocator>& operator=(const podvector<Ty, Allocator>& x) noexcept
  {
    return assign_copy(x, propagate_allocator_on_copy());
  }
  podvector<Ty, Allocator>& operator=(podvector<Ty, Allocator>&& x) noexcept
  {
    return assign_move(std::move(x), propagate_allocator_on_move());
  }
  podvector& operator=(std::initializer_list<Ty> x) noexcept
  {
    if (capacity_ < x.size())
    {
      capacity_ = static_cast<size_type>(x.size());
      deallocate();
      data_ = allocate(static_cast<size_type>(x.size()));
    }
    size_ = static_cast<size_type>(x.size());
    copy(std::begin(x), std::end(x), data_);
    return *this;
  }
  template <class InputIterator>
  void assign(InputIterator first, InputIterator last) noexcept
  {
    size_type s = static_cast<size_type>(std::distance(first, last));
    if (capacity_ < s)
    {
      capacity_ = static_cast<size_type>(s);
      deallocate();
      data_ = allocate(s);
    }
    size_ = static_cast<size_type>(s);
    copy(first, last, data_);
  }
  void assign(size_type n, const Ty& value) noexcept
  {
    if (capacity_ < n)
    {
      capacity_ = n;
      deallocate();
      data_ = allocate(n);
    }
    size_ = static_cast<size_type>(n);
    std::uninitialized_fill_n(data_, n, value);
  }
  void assign(std::initializer_list<Ty> x) noexcept
  {
    if (capacity_ < x.size())
    {
      deallocate();
      capacity_ = static_cast<size_type>(x.size());
      data_     = allocate(capacity_);
    }
    size_ = static_cast<size_type>(x.size());
    copy(std::begin(x), std::end(x), data_);
  }
  allocator_type get_allocator() const noexcept
  {
    return *this;
  }

  // iterators:
  iterator begin() noexcept
  {
    return data_;
  }
  const_iterator begin() const noexcept
  {
    return data_;
  }
  iterator end() noexcept
  {
    return data_ + size_;
  }
  const_iterator end() const noexcept
  {
    return data_ + size_;
  }

  reverse_iterator rbegin() noexcept
  {
    return reverse_iterator(end());
  }
  const_reverse_iterator rbegin() const noexcept
  {
    return const_reverse_iterator(end());
  }
  reverse_iterator rend() noexcept
  {
    return reverse_iterator(begin());
  }
  const_reverse_iterator rend() const noexcept
  {
    return const_reverse_iterator(begin());
  }

  const_iterator cbegin() const noexcept
  {
    return begin();
  }
  const_iterator cend() const noexcept
  {
    return end();
  }
  const_reverse_iterator crbegin() const noexcept
  {
    return rbegin();
  }
  const_reverse_iterator crend() const noexcept
  {
    return rend();
  }

  // capacity:
  size_type size() const noexcept
  {
    return size_;
  }
  size_type max_size() const noexcept
  {
    return Allocator::max_size();
  }
  void resize(size_type sz) noexcept
  {
    reserve(sz);
    size_ = sz;
  }
  void resize(size_type sz, const Ty& c) noexcept
  {
    reserve(sz);
    if (sz > size_)
      std::uninitialized_fill_n(data_ + size_, sz - size_, c);
    size_ = sz;
  }
  size_type capacity() const noexcept
  {
    return capacity_;
  }
  bool empty() const noexcept
  {
    return size_ != 0;
  }
  void reserve(size_type n) noexcept
  {
    if (capacity_ < n)
    {
      unchecked_reserve(n);
    }
  }
  void shrink_to_fit() noexcept
  {
    if (capacity_ != size_)
    {
      unchecked_reserve(size_);
    }
  }

  // element access:
  reference operator[](size_type n) noexcept
  {
    assert(n < size_);
    return data_[n];
  }
  const_reference operator[](size_type n) const noexcept
  {
    assert(n < size_);
    return data_[n];
  }
  reference at(size_type n)
  {
    assert(n < size_);
    return data_[n];
  }
  const_reference at(size_type n) const noexcept
  {
    assert(n < size_);
    return data_[n];
  }
  reference front()
  {
    assert(0 < size_);
    return data_[0];
  }
  const_reference front() const noexcept
  {
    assert(0 < size_);
    return data_[0];
  }
  reference back()
  {
    assert(0 < size_);
    return data_[size_ - 1];
  }
  const_reference back() const noexcept
  {
    assert(0 < size_);
    return data_[size_ - 1];
  }

  // data access
  Ty* data() noexcept
  {
    return data_;
  }
  const Ty* data() const noexcept
  {
    return data_;
  }

  // modifiers:
  template <class... Args>
  void emplace_back(Args&&... args) noexcept
  {
    if (capacity_ < size_ + 1)
      unchecked_reserve(size_ + std::max<size_type>(size_ >> 1, 1));
    data_[size_++] = Ty(std::forward<Args>(args)...);
  }
  void push_back(const Ty& x) noexcept
  {
    if (capacity_ < size_ + 1)
      unchecked_reserve(size_ + std::max<size_type>(size_ >> 1, 1));
    data_[size_++] = x;
  }
  void push_back(Ty&& x) noexcept
  {
    if (capacity_ < size_ + 1)
      unchecked_reserve(size_ + std::max<size_type>(size_ >> 1, 1));
    data_[size_++] = std::move(x);
  }
  void pop_back() noexcept
  {
    assert(size_);
    size_--;
  }

  template <class... Args>
  iterator emplace(const_iterator position, Args&&... args) noexcept
  {
    size_type p = insert_hole(position);
    data_[p]    = Ty(std::forward<Args>(args)...);
    return data_ + p;
  }
  iterator insert(const_iterator position, const Ty& x) noexcept
  {
    size_type p = insert_hole(position);
    data_[p]    = x;
    return data_ + p;
  }
  iterator insert(const_iterator position, Ty&& x) noexcept
  {
    size_type p = insert_hole(position);
    data_[p]    = std::move(x);
    return data_ + p;
  }
  iterator insert(const_iterator position, size_type n, const Ty& x) noexcept
  {
    return insert_range(position, n, x, std::true_type());
  }
  template <class InputIterator>
  iterator insert(const_iterator position, InputIterator first, InputIterator last) noexcept
  {
    return insert_range(position, first, last, std::is_integral<InputIterator>());
  }

  iterator insert(const_iterator position, std::initializer_list<Ty> x) noexcept
  {
    size_type p = insert_hole(position, static_cast<size_type>(x.size()));
    copy(std::begin(x), std::end(x), data_ + p);
    return data_ + p;
  }

  iterator erase(const_iterator position) noexcept
  {
    assert(position < end());
    std::memmove(const_cast<iterator>(position), position + 1,
                 static_cast<std::size_t>((data_ + size_) - (position + 1)) * sizeof(Ty));
    size_--;
    return const_cast<iterator>(position);
  }
  iterator erase(const_iterator first, const_iterator last) noexcept
  {
    assert(last < end());
    std::uint32_t n = static_cast<std::uint32_t>(std::distance(first, last));
    std::memmove(const_cast<iterator>(first), last, static_cast<std::size_t>((data_ + size_) - (last)) * sizeof(Ty));
    size_ -= n;
    return const_cast<iterator>(first);
  }
  void swap(podvector<Ty, Allocator>& x) noexcept
  {
    swap(x, propagate_allocator_on_swap());
  }
  void clear() noexcept
  {
    size_ = 0;
  }

private:
  inline size_type insert_hole(const_iterator it, size_type n = 1) noexcept
  {
    size_type p   = static_cast<size_type>(std::distance<const Ty*>(data_, it));
    size_type nsz = size_ + n;
    if (capacity_ < nsz)
    {
      unchecked_reserve(size_ + std::max(size_ >> 1, n), p, n);
    }
    else
    {
      pointer src = const_cast<iterator>(it);
      std::memmove(src + n, src, (size_ - p) * sizeof(Ty));
    }
    size_ = nsz;
    return p;
  }
  inline podvector& assign_copy(const podvector& x, std::false_type) noexcept
  {
    if (capacity_ < x.size_)
    {
      deallocate();
      data_     = allocate(x.size_);
      capacity_ = x.size_;
    }
    size_ = x.size_;
    std::memcpy(data_, x.data_, x.size_ * sizeof(Ty));
    return *this;
  }

  inline podvector& assign_copy(const podvector& x, std::true_type) noexcept
  {
    if (allocator_is_always_equal::value || static_cast<const Allocator&>(x) == static_cast<const Allocator&>(*this))
      assign(x, std::false_type());
    else
    {
      deallocate();
      Allocator::operator=(static_cast<const Allocator&>(x));
      data_              = allocate(x.size_);
      capacity_          = x.size_;
      size_              = x.size_;
      std::memcpy(data_, x.data_, x.size_ * sizeof(Ty));
    }
    return *this;
  }

  inline podvector& assign_move(podvector&& x, std::false_type) noexcept
  {
    if (allocator_is_always_equal::value || static_cast<const Allocator&>(x) == static_cast<const Allocator&>(*this))
    {
      deallocate();
      data_       = x.data_;
      size_       = x.size_;
      capacity_   = x.capacity_;
      x.data_     = nullptr;
      x.capacity_ = x.size_ = 0;
    }
    else
      assign_copy(x, std::false_type());
    return *this;
  }

  inline podvector& assign_move(podvector&& x, std::true_type) noexcept
  {
    deallocate();
    Allocator::operator=(std::move(static_cast<Allocator&>(x)));
    data_              = x.data_;
    size_              = x.size_;
    capacity_          = x.capacity_;
    x.data_            = nullptr;
    x.capacity_ = x.size_ = 0;
    return *this;
  }

  template <typename InputIt>
  inline void copy(InputIt first, InputIt last, pointer dest) noexcept
  {
    if constexpr (std::is_same<pointer, InputIt>::value)
    {
      std::memcpy(dest, first, static_cast<std::size_t>(std::distance(first, last)) * sizeof(Ty));
    }
    else
    {
      std::copy(first, last, dest);
    }
  }

  inline pointer allocate(size_type n) noexcept
  {
    return acl::allocate<Ty>(static_cast<Allocator&>(*this),
                             static_cast<typename allocator::size_type>(n * sizeof(Ty)));
  }

  inline void deallocate() noexcept
  {
    acl::deallocate(static_cast<Allocator&>(*this), data_, capacity_ * sizeof(Ty));
  }

  inline void unchecked_reserve(size_type n) noexcept
  {
    pointer d = allocate(n);
    if (data_)
    {
      std::memcpy(d, data_, size_ * sizeof(Ty));
      deallocate();
    }
    data_     = d;
    capacity_ = n;
  }

  inline void unchecked_reserve(size_type n, size_type at, size_type holes) noexcept
  {
    pointer d = allocate(n);
    if (data_)
    {
      std::memcpy(d, data_, at * sizeof(Ty));
      std::memcpy(d + at + holes, data_ + at, (size_ - at) * sizeof(Ty));
      deallocate();
    }
    data_     = d;
    capacity_ = n;
  }

  void swap(podvector<Ty, Allocator>& x, std::false_type) noexcept
  {
    std::swap(capacity_, x.capacity_);
    std::swap(size_, x.size_);
    std::swap(data_, x.data_);
  }

  void swap(podvector<Ty, Allocator>& x, std::true_type) noexcept
  {
    std::swap(capacity_, x.capacity_);
    std::swap(size_, x.size_);
    std::swap(data_, x.data_);
    std::swap<Allocator>(this, x);
  }

  friend void swap(podvector<Ty, Allocator>& lhs, podvector<Ty, Allocator>& rhs) noexcept
  {
    lhs.swap(rhs);
  }

  friend bool operator==(const podvector<Ty, Allocator>& x, const podvector<Ty, Allocator>& y) noexcept
  {
    return x.size_ == y.size_ && std::memcmp(x.data_, y.data_, x.size_) == 0;
  }

  friend bool operator<(const podvector<Ty, Allocator>& x, const podvector<Ty, Allocator>& y) noexcept
  {
    return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
  }

  friend bool operator!=(const podvector<Ty, Allocator>& x, const podvector<Ty, Allocator>& y) noexcept
  {
    return !(x == y);
  }

  friend bool operator>(const podvector<Ty, Allocator>& x, const podvector<Ty, Allocator>& y) noexcept
  {
    return std::lexicographical_compare(y.begin(), y.end(), x.begin(), x.end());
  }

  friend bool operator>=(const podvector<Ty, Allocator>& x, const podvector<Ty, Allocator>& y) noexcept
  {
    return !std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
  }

  friend bool operator<=(const podvector<Ty, Allocator>& x, const podvector<Ty, Allocator>& y) noexcept
  {
    return !std::lexicographical_compare(y.begin(), y.end(), x.begin(), x.end());
  }

  template <class InputIterator>
  inline iterator insert_range(const_iterator position, InputIterator first, InputIterator last,
                               std::false_type) noexcept
  {
    size_type p = insert_hole(position, static_cast<size_type>(std::distance(first, last)));
    copy(first, last, data_ + p);
    return data_ + p;
  }

  inline iterator insert_range(const_iterator position, size_type n, const Ty& x, std::true_type) noexcept
  {
    size_type p = insert_hole(position, n);
    std::uninitialized_fill_n(data_ + p, n, x);
    return data_ + p;
  }

  template <class InputIterator>
  inline void construct_from_range(InputIterator first, InputIterator last, std::false_type) noexcept
  {
    size_     = static_cast<size_type>(std::distance(first, last));
    data_     = allocate(size_);
    capacity_ = size_;
    copy(first, last, data_);
  }

  inline void construct_from_range(size_type n, const Ty& value, std::true_type) noexcept
  {
    size_     = n;
    data_     = allocate(size_);
    capacity_ = size_;
    std::uninitialized_fill_n(data_, n, value);
  }

  pointer   data_     = nullptr;
  size_type size_     = 0;
  size_type capacity_ = 0;
};
} // namespace acl