#pragma once

#include <cathedral/ds.hpp>
#include <cathedral/engine/native_script.hpp>

#include <memory>
#include <string>

namespace cathedral::engine
{
    const unordered_map<std::string, std::shared_ptr<script>>& get_native_script_registry();

    std::shared_ptr<script> get_native_script(const std::string& name);

    void register_native_script(std::string name, std::shared_ptr<script> script);

    void unregister_native_script(const std::string& name);
} // namespace cathedral::engine