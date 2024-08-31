#pragma once

#include <cathedral/gfx/image.hpp>
#include <cathedral/gfx/sampler.hpp>

namespace cathedral::engine
{
    class upload_queue;

    struct texture_args
    {
        const ien::image* pimage = nullptr;
        gfx::sampler_args sampler_args;
        uint32_t mipmap_levels = 1;
        vk::ImageAspectFlagBits image_aspect_flags = vk::ImageAspectFlagBits::eColor;
        vk::Filter mipmap_generation_filter = vk::Filter::eLinear;
        std::optional<std::string> path = std::nullopt;
    };

    class texture
    {
    public:
        texture(texture_args args, upload_queue& queue);

        const gfx::sampler& sampler() const { return *_sampler; }
        const gfx::image& image() const { return *_image; }
        const vk::ImageView imageview() const { return *_imageview; }
        const std::optional<std::string>& path() const { return _path; }

    private:
        std::unique_ptr<gfx::image> _image;
        vk::UniqueImageView _imageview;
        std::unique_ptr<gfx::sampler> _sampler;
        std::optional<std::string> _path;
    };
}