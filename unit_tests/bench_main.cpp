#define ANKERL_NANOBENCH_IMPLEMENT
#include <acl/arena_allocator.hpp>
#include <nanobench.h>
#include <string_view>

struct alloc_mem_manager
{
  using alloc_info = acl::alloc_info<uint32_t>;

  uint32_t arena_nb = 0;

  alloc_mem_manager() {}

  bool drop_arena([[maybe_unused]] acl::uhandle id)
  {
    return true;
  }

  acl::uhandle add_arena([[maybe_unused]] acl::ihandle id, [[maybe_unused]] std::size_t size)
  {
    return arena_nb++;
  }

  void remove_arena(acl::uhandle h) {}

  template <typename Allocator>
  void begin_defragment(Allocator& allocator)
  {}

  template <typename Allocator>
  void end_defragment(Allocator& allocator)
  {}

  void rebind_alloc([[maybe_unused]] acl::uhandle halloc, alloc_info info) {}

  void move_memory([[maybe_unused]] acl::uhandle src_arena, [[maybe_unused]] acl::uhandle dst_arena,
                   [[maybe_unused]] std::size_t from, [[maybe_unused]] std::size_t to, std::size_t size)
  {}
};

struct rand_device
{
  uint32_t update()
  {
    uint32_t x = seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return seed = x;
  }

  uint32_t seed = 2147483647;
};

template <typename T>
void bench_arena(uint32_t size, std::string_view name)
{
  using allocator_t = acl::arena_allocator<T, alloc_mem_manager, uint32_t, false>;
  constexpr uint32_t        nbatch = 100000;
  alloc_mem_manager         mgr;
  std::vector<acl::ihandle> allocations;
  allocations.reserve(nbatch);
  ankerl::nanobench::Bench bench;
  bench.output(&std::cout);
  bench.minEpochIterations(15);
  bench.batch(nbatch).run(std::string{name},
                         [&]
                         {
                           rand_device dev;
                           allocator_t allocator(size, mgr);
                           for (std::uint32_t allocs = 0; allocs < nbatch; ++allocs)
                           {
                             if ((dev.update() & 0x1) || allocations.empty())
                             {
                               acl::fixed_alloc_desc<std::uint32_t, T::min_granularity> desc(
                                 (dev.update() % 100) * T::min_granularity, 0, {});
                               allocations.push_back(allocator.allocate(desc).halloc);
                             }
                             else
                             {
                               allocator.deallocate(allocations.back());
                               allocations.pop_back();
                             }
                           }
                           allocations.clear();
                         });
}

int main(int argc, char* argv[])
{
  constexpr uint32_t size = 256 * 256;


  //  (acl::strat::slotted_v0<uint32_t, 256, 255, 4, acl::strat::best_fit_tree<uint32_t>>),
  //  (acl::strat::slotted_v1<uint32_t, 256, 255, 4, acl::strat::best_fit_tree<uint32_t>>),
  //  (acl::strat::slotted_v2<uint32_t, 256, 255, 8, 4, acl::strat::best_fit_tree<uint32_t>>)
  bench_arena<acl::strat::greedy_v0<uint32_t>>(size, "greedy-v0");
  bench_arena<acl::strat::greedy_v1<uint32_t>>(size, "greedy-v1");
  bench_arena<acl::strat::best_fit_tree<uint32_t>>(size, "bf-tree");
  bench_arena<acl::strat::best_fit_v0<uint32_t>>(size, "bf-v0");
  bench_arena<acl::strat::slotted_v0<uint32_t>>(size, "slot-v0");
  bench_arena<acl::strat::slotted_v1<uint32_t>>(size, "slot-v1");
  bench_arena<acl::strat::slotted_v2<uint32_t>>(size, "slot-v2");
  bench_arena<acl::strat::slotted_v0<uint32_t, 256, 255, 4, acl::strat::best_fit_tree<uint32_t>>>(size, "slot-v0-t");
  bench_arena<acl::strat::slotted_v1<uint32_t, 256, 255, 4, acl::strat::best_fit_tree<uint32_t>>>(size, "slot-v1-t");
  bench_arena<acl::strat::slotted_v2<uint32_t, 256, 255, 8, 4, acl::strat::best_fit_tree<uint32_t>>>(size, "slot-v2-t");

  return 0;
}