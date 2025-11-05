#include <cathedral/engine/native_script_registry.hpp>

namespace cathedral::engine
{
    std::unordered_map<std::string, std::shared_ptr<script>> _registry;

    const std::unordered_map<std::string, std::shared_ptr<script>>& get_native_script_registry()
    {
        return _registry;
    }

    std::shared_ptr<script> get_native_script(const std::string& name)
    {
        if (_registry.contains(name))
        {
            return _registry[name];
        }
        return {};
    }

    void register_native_script(std::string name, std::shared_ptr<script> script)
    {
        _registry.emplace(MOVE(name), MOVE(script));
    }

    void unregister_native_script(const std::string& name)
    {
        _registry.erase(name);
    }
} // namespace cathedral::engine