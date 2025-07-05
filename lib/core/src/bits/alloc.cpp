#include <cathedral/bits/alloc.hpp>

#include <cstdlib>

namespace
{
    size_t period_allocations = 0;
    size_t period_allocated_memory = 0;
} // namespace

void* operator new(const size_t sz)
{
    ++period_allocations;
    period_allocated_memory += sz;
    return malloc(sz);
}

namespace cathedral
{
    size_t period_allocations()
    {
        return ::period_allocations;
    }

    size_t period_allocated_memory()
    {
        return ::period_allocated_memory;
    }

    void flush_allocation_period()
    {
        ::period_allocations = 0;
        ::period_allocated_memory = 0;
    }
} // namespace cathedral