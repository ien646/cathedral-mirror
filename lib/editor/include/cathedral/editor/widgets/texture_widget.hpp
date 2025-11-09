#pragma once

#include <cathedral/engine/texture.hpp>

#include <memory>

namespace cathedral::editor
{
    class texture_widget
    {
    public:
        explicit texture_widget(std::shared_ptr<engine::texture> texture);
        ~texture_widget();

        void tick();
        void set_texture(std::shared_ptr<engine::texture> texture);

        static std::pair<float, float> size();

    private:
        std::shared_ptr<engine::texture> _texture;
        void* _imgui_texture = nullptr;

        // FIXME: VRAM leaky for edgy use cases
        std::vector<std::shared_ptr<engine::texture>> _trash_textures;
        std::vector<void*> _trash_imgui_textures;

        bool _texture_changed = false;
    };
} // namespace cathedral::editor