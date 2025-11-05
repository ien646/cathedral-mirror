#include <cathedral/engine/script.hpp>

namespace cathedral::engine
{
    script::script(std::string name)
        : _name(MOVE(name))
    {
    }

    const std::string& script::name() const
    {
        return _name;
    }

    void script::set_name(std::string name)
    {
        _name = MOVE(name);
    }
} // namespace cathedral::engine