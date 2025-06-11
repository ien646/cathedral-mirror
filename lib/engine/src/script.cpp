#include <cathedral/engine/script.hpp>

namespace cathedral::engine
{
    script::script(std::string name)
        : _name(std::move(name))
    {
    }

    const std::string& script::name() const
    {
        return _name;
    }

    void script::set_name(std::string name)
    {
        _name = std::move(name);
    }
} // namespace cathedral::engine