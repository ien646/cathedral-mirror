#include <cathedral/script/error.hpp>

#include <cathedral/core.hpp>

namespace cathedral::script
{
    namespace
    {
        const std::string annotations;

        void error_handler(std::optional<std::string> err)
        {
            log_error("Script error!");
            if (err.has_value())
            {
                log_error(err.value());
            }
        }
    } // namespace

    void error_initializer::initialize(state& s)
    {
        s.set_function("__cathedral_error_handler__", &error_handler);
    }

    const std::string& error_initializer::get_annotations()
    {
        return annotations;
    }
} // namespace cathedral::script