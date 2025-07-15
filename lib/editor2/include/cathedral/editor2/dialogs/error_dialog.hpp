#pragma once

#include <cathedral/editor2/widget.hpp>

#include <string>

namespace cathedral::editor2
{
    class error_dialog final : public widget<>
    {
    public:
        void tick() override;

        void open(std::string title, std::string message);

    private:
        bool _open = false;
        std::string _title;
        std::string _message;
    };
} // namespace cathedral::editor2