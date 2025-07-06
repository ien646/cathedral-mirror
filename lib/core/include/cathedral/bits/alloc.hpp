#pragma once

#include <cstdint>

namespace cathedral
{
    std::size_t period_allocations();
    std::size_t period_allocated_memory();
    void flush_allocation_period();
}