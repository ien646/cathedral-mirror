#pragma once

#include <cathedral/core.hpp>
#include <cathedral/editor2/resource_managers/resource_filter.hpp>
#include <cathedral/editor2/resource_managers/resource_manager_base.hpp>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor2
{
    class script_manager final : public resource_manager_base
    {
    public:
        explicit script_manager(project::project& pro);

        void tick() override;

    private:
        resource_filter _resource_filter;

        void tick_gui();
    };
} // namespace cathedral::editor2