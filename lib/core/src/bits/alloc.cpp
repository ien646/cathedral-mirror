#include <cathedral/bits/alloc.hpp>

#include <ien/platform.hpp>

#include <cstdlib>

#ifdef IEN_PLATFORM_WINDOWS
    #include <Windows.h>
#else
    #include <dlfcn.h>
#endif

namespace
{
    size_t period_allocations = 0;
    size_t period_allocated_memory = 0;
} // namespace

void* malloc(const size_t size)
{
    ++period_allocations;
    period_allocated_memory += size;

#ifdef IEN_PLATFORM_WINDOWS
    return HeapAlloc(GetProcessHeap(), NULL, size);
#else
    static const auto real_malloc = reinterpret_cast<void* (*)(size_t)>(dlsym(RTLD_NEXT, "malloc"));
    return real_malloc(size);
#endif
}

void free(void* ptr)
{
#ifdef IEN_PLATFORM_WINDOWS
    return HeapFree(GetProcessHeap(), NULL, ptr);
#else
    static const auto real_free = reinterpret_cast<void (*)(void*)>(dlsym(RTLD_NEXT, "free"));
    return real_free(ptr);
#endif
}

void* operator new(const size_t sz)
{
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