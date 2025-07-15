#include <cathedral/editor2/widget.hpp>

#include <atomic>

namespace cathedral::editor2
{
    namespace
    {
        std::atomic<uint64_t> widget_id_counter = 0;
    }

    widget_base::widget_base()
        : _id(widget_id_counter++)
    {
    }

    uint64_t widget_base::id() const
    {
        return _id;
    }
    bool widget_base::operator==(const widget_base& other) const
    {
        return _id == other._id;
    }
} // namespace cathedral::editor2