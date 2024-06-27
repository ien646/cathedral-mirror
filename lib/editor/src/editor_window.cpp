#include <cathedral/editor/editor_window.hpp>

#include <QMouseEvent>
#include <QVulkanInstance>
#include <QWindow>

#include <ien/platform.hpp>

#include <thread>

// clang-format off
#if defined(IEN_OS_WIN)
    #include <vulkan/vulkan_win32.h>
    const std::vector<const char*> instance_extensions = {
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };
#elif defined(IEN_OS_LINUX)
    #include <xcb/xcb.h>
    #include <vulkan/vulkan_xcb.h>
    #include <vulkan/vulkan_wayland.h>
    const std::vector<const char*> instance_extensions = {
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
    };
#endif
// clang-format on

namespace cathedral::editor
{
    editor_window::editor_window()
    {
        editor_window_menubar* menubar = new editor_window_menubar(this);
        setMenuBar(menubar);

        _vk_window = new QWindow();
        _vk_window->setFlags(Qt::WindowTransparentForInput);
        _vk_window->setSurfaceType(QWindow::SurfaceType::VulkanSurface);
        _vk_window->show();

        _vk_widget = QWidget::createWindowContainer(_vk_window, this, Qt::WindowDoesNotAcceptFocus);
        _vk_widget->setMouseTracking(false);
        _vk_widget->setMinimumSize(200, 200);
        setCentralWidget(_vk_widget);

        cathedral::gfx::vulkan_context_args vkctx_args;
        vkctx_args.instance_extensions = instance_extensions;
        vkctx_args.surface_retriever = [&](vk::Instance inst) {
            QVulkanInstance* vkinst = new QVulkanInstance();
            vkinst->setVkInstance(inst);
            vkinst->create();
            _vk_window->setVulkanInstance(vkinst);
            return QVulkanInstance::surfaceForWindow(_vk_window);
        };
        vkctx_args.surface_size_retriever = [&]() { return glm::ivec2{ _vk_widget->width(), _vk_widget->height() }; };
        vkctx_args.validation_layers = true;

        _vkctx = std::make_unique<cathedral::gfx::vulkan_context>(vkctx_args);
        _swapchain = std::make_unique<cathedral::gfx::swapchain>(*_vkctx, vk::PresentModeKHR::eFifo);

        _scene_dock = new QDockWidget("SceneTree", this);
        _scene_dock->setAllowedAreas(Qt::DockWidgetArea::LeftDockWidgetArea);
        _scene_dock->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
        addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, _scene_dock);

        _props_dock = new QDockWidget("Properties", this);
        _props_dock->setAllowedAreas(Qt::DockWidgetArea::RightDockWidgetArea);
        _props_dock->setFeatures(QDockWidget::DockWidgetFeature::NoDockWidgetFeatures);
        addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, _props_dock);

        // On some platforms, the widget/window has no position until the first
        // render/present is submitted (i.e. Wayland). Delay the initial resize
        // so that the widget has correct geometry on start
        std::thread delayed_resize([this] {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            QMetaObject::invokeMethod(this, [this] { resize(800, 600); });
        });
        delayed_resize.detach();

        connect(_vk_window, &QWindow::widthChanged, this, [this](int w) { _swapchain->recreate(); });
        connect(_vk_window, &QWindow::heightChanged, this, [this](int h) { _swapchain->recreate(); });
    }

    void editor_window::tick()
    {
        uint32_t swapchain_image_index = _swapchain->acquire_next_image();

        vk::CommandBuffer transition_color_cmdbuff = _swapchain->transition_cmdbuff_undefined_color(swapchain_image_index);
        vk::CommandBuffer transition_present_cmdbuff = _swapchain->transition_cmdbuff_color_present(swapchain_image_index);

        auto image_ready_semaphore = _swapchain->image_ready_semaphore();
        vk::PipelineStageFlags wait_stage = vk::PipelineStageFlagBits::eAllGraphics;

        vk::SubmitInfo submit_transition0;
        submit_transition0.commandBufferCount = 1;
        submit_transition0.pCommandBuffers = &transition_color_cmdbuff;
        submit_transition0.signalSemaphoreCount = 0;
        submit_transition0.waitSemaphoreCount = 1;
        submit_transition0.pWaitSemaphores = &image_ready_semaphore;
        submit_transition0.pWaitDstStageMask = &wait_stage;
        _vkctx->graphics_queue().submit(submit_transition0);
        _vkctx->graphics_queue().waitIdle();

        vk::UniqueCommandBuffer cmdbuff = _vkctx->create_primary_commandbuffer();
        cmdbuff->begin(vk::CommandBufferBeginInfo{});
        {
            vk::RenderingAttachmentInfo color_att_info;
            color_att_info.clearValue.color
                .float32 = std::array<float, 4>{ (float)rand() / (float)RAND_MAX, 0.0f, 0.0f, 1.0f };
            color_att_info.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
            color_att_info.imageView = _swapchain->imageview(swapchain_image_index);
            color_att_info.loadOp = vk::AttachmentLoadOp::eClear;
            color_att_info.resolveMode = vk::ResolveModeFlagBits::eNone;
            color_att_info.storeOp = vk::AttachmentStoreOp::eStore;

            auto surfsize = _vkctx->get_surface_size();

            vk::RenderingInfo render_info;
            render_info.colorAttachmentCount = 1;
            render_info.pColorAttachments = &color_att_info;
            render_info.pDepthAttachment = nullptr;
            render_info.pStencilAttachment = nullptr;
            render_info.renderArea.offset = vk::Offset2D(0, 0);
            render_info.renderArea.extent = vk::Extent2D(surfsize.x, surfsize.y);
            render_info.layerCount = 1;
            render_info.viewMask = 0;

            cmdbuff->beginRendering(render_info);
            {
                //...
            }
            cmdbuff->endRendering();
        }
        cmdbuff->end();
        _vkctx->submit_commandbuffer_sync(*cmdbuff);

        vk::SubmitInfo submit_transition1;
        submit_transition1.commandBufferCount = 1;
        submit_transition1.pCommandBuffers = &transition_present_cmdbuff;
        submit_transition1.signalSemaphoreCount = 0;
        submit_transition1.waitSemaphoreCount = 0;
        _vkctx->graphics_queue().submit(submit_transition1);
        _vkctx->graphics_queue().waitIdle();

        vk::SwapchainKHR swapchain = _swapchain->get();

        vk::PresentInfoKHR present;
        present.pImageIndices = &swapchain_image_index;
        present.pSwapchains = &swapchain;
        present.swapchainCount = 1;
        present.waitSemaphoreCount = 0;

        auto _x = _vkctx->graphics_queue().presentKHR(present);
        _vkctx->device().waitIdle();
    }
} // namespace cathedral::editor