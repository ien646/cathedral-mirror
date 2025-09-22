#pragma once

#include <cathedral/engine/renderer.hpp>

namespace cathedral::engine
{
    template <typename T>
    class renderer_resource final : public renderer_resource_base
    {
    public:
        renderer_resource() = default;

        renderer_resource(T&& obj, renderer* renderer)
            : _obj(std::make_unique<T>(std::move(obj)))
            , _renderer(renderer)
        {
            CRITICAL_CHECK_NOTNULL(_renderer);
        }

        CATHEDRAL_NON_COPYABLE(renderer_resource<T>);

        renderer_resource(renderer_resource&& mv_src) noexcept { *this = std::move(mv_src); }

        renderer_resource& operator=(renderer_resource&& mv_src) noexcept
        {
            // If this is a valid resource, destruction of held object must be handled properly
            if (_obj != nullptr)
            {
                _renderer->enqueue_resource_for_deletion(std::shared_ptr<T>(_obj.release()));
                _obj = {};
            }

            _obj = std::move(mv_src._obj);
            _renderer = mv_src._renderer;
            mv_src._renderer = nullptr;

            return *this;
        }

        ~renderer_resource() override
        {
            if (_renderer != nullptr)
            {
                _renderer->enqueue_resource_for_deletion(std::shared_ptr<T>(_obj.release()));
                _renderer = nullptr;
                _obj = {};
            }
        }

        T& value() const { return *_obj; }

        T& operator*() const { return *_obj; }

        T* operator->() const { return _obj.get(); }

        bool operator==(std::nullptr_t) const { return _obj == nullptr; }

        explicit(false) operator bool() const { return _obj != nullptr; }

    private:
        std::unique_ptr<T> _obj = nullptr;
        renderer* _renderer = nullptr;
    };
} // namespace cathedral::engine