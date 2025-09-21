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
        }

        CATHEDRAL_NON_COPYABLE(renderer_resource<T>);
        CATHEDRAL_DEFAULT_MOVABLE(renderer_resource<T>);

        ~renderer_resource() override { _renderer->enqueue_resource_for_deletion(std::shared_ptr<T>(_obj.release())); }

        T& value() const { return *_obj; }

        T& operator*() const { return *_obj; }

        T* operator->() const { return _obj.get(); }

        bool operator==(std::nullptr_t) const { return _obj == nullptr; }

        explicit(false) operator bool() const { return static_cast<bool>(_obj); }

    private:
        std::unique_ptr<T> _obj = {};
        renderer* _renderer = nullptr;
    };
} // namespace cathedral::engine