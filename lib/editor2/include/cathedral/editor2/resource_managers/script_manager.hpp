#pragma once

#include <cathedral/core.hpp>
#include <cathedral/editor2/resource_managers/resource_filter.hpp>

FORWARD_CLASS(cathedral::engine, scene);
FORWARD_CLASS(cathedral::project, project);

namespace cathedral::editor2
{
    class script_manager
    {
    public:
        explicit script_manager(project::project& pro);
        void tick(engine::scene& scene);

    private:
        project::project& _project;
        resource_filter _resource_filter;
    };
} // namespace cathedral::editor2