#include <cathedral/gfx/pipeline.hpp>

#include <unordered_set>

namespace cathedral::gfx
{
    vk::Format to_vk_format(vertex_data_type vxdt)
    {
        switch (vxdt)
        {
        case vertex_data_type::FLOAT:
            return vk::Format::eR32Sfloat;
        case vertex_data_type::VEC2F:
            return vk::Format::eR32G32Sfloat;
        case vertex_data_type::VEC3F:
            return vk::Format::eR32G32B32Sfloat;
        case vertex_data_type::VEC4F:
            return vk::Format::eR32G32B32A32Sfloat;
        default:
            CRITICAL_ERROR("Unhandled vertex data type");
        }
        return vk::Format::eUndefined;
    }

    pipeline::pipeline(pipeline_args args)
        : _args(std::move(args))
    {
        CRITICAL_CHECK(_args.vkctx != nullptr);
        CRITICAL_CHECK(_args.vertex_shader != nullptr);
        CRITICAL_CHECK(_args.fragment_shader != nullptr);
        CRITICAL_CHECK(_args.vertex_shader->type() == shader_type::VERTEX);
        CRITICAL_CHECK(_args.fragment_shader->type() == shader_type::FRAGMENT);
        CRITICAL_CHECK(_args.vertex_input.vertex_size > 0);
        CRITICAL_CHECK(_args.vertex_input.attributes.size() > 0);

        const auto& vkctx = *_args.vkctx;

        vk::GraphicsPipelineCreateInfo pipeline_info;

        // Color blend
        vk::PipelineColorBlendAttachmentState color_blend_attachment_state;
        color_blend_attachment_state.blendEnable = _args.color_blend_enable;
        color_blend_attachment_state.colorBlendOp = vk::BlendOp::eAdd;
        color_blend_attachment_state.alphaBlendOp = vk::BlendOp::eAdd;
        color_blend_attachment_state.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
        color_blend_attachment_state.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
        color_blend_attachment_state.srcAlphaBlendFactor = vk::BlendFactor::eOne;
        color_blend_attachment_state.dstAlphaBlendFactor = vk::BlendFactor::eZero;
        color_blend_attachment_state.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

        vk::PipelineColorBlendStateCreateInfo color_blend;
        color_blend.blendConstants = std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f };
        color_blend.attachmentCount = 1;
        color_blend.pAttachments = &color_blend_attachment_state;
        color_blend.logicOpEnable = false;

        pipeline_info.pColorBlendState = &color_blend;

        // Depth stencil
        vk::PipelineDepthStencilStateCreateInfo depth_stencil;
        depth_stencil.depthBoundsTestEnable = false;
        depth_stencil.depthTestEnable = _args.enable_depth;
        depth_stencil.stencilTestEnable = _args.enable_stencil;
        depth_stencil.depthWriteEnable = _args.enable_depth;
        depth_stencil.depthCompareOp = vk::CompareOp::eLess;
        depth_stencil.minDepthBounds = 0.0f;
        depth_stencil.maxDepthBounds = 1.0f;

        pipeline_info.pDepthStencilState = &depth_stencil;

        // Dynamic state
        std::vector<vk::DynamicState> dynamic_states = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
        vk::PipelineDynamicStateCreateInfo dynamic_state;
        dynamic_state.dynamicStateCount = dynamic_states.size();
        dynamic_state.pDynamicStates = dynamic_states.data();

        pipeline_info.pDynamicState = &dynamic_state;

        // Input assembly
        vk::PipelineInputAssemblyStateCreateInfo input_assembly;
        input_assembly.primitiveRestartEnable = false;
        input_assembly.topology = _args.input_topology;

        pipeline_info.pInputAssemblyState = &input_assembly;

        // Multisampling
        vk::PipelineMultisampleStateCreateInfo multisampling;
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
        pipeline_info.pMultisampleState = &multisampling;

        // Raster
        vk::PipelineRasterizationStateCreateInfo raster;
        raster.polygonMode = _args.polygon_mode;
        raster.cullMode = _args.cull_backfaces ? vk::CullModeFlagBits::eBack : vk::CullModeFlagBits::eNone;
        raster.depthBiasEnable = false;
        raster.rasterizerDiscardEnable = false;
        raster.frontFace = vk::FrontFace::eCounterClockwise;
        raster.lineWidth = _args.line_width;

        pipeline_info.pRasterizationState = &raster;

        // Tessellation
        pipeline_info.pTessellationState = nullptr;

        // Viewport
        const auto wsz = vkctx.get_surface_size();
        vk::Viewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = wsz.x;
        viewport.height = wsz.y;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vk::Rect2D scissor;
        scissor.offset = vk::Offset2D{ 0, 0 };
        scissor.extent = vk::Extent2D(wsz.x, wsz.y);

        vk::PipelineViewportStateCreateInfo viewport_state;
        viewport_state.viewportCount = 1;
        viewport_state.pViewports = &viewport;
        viewport_state.scissorCount = 1;
        viewport_state.pScissors = &scissor;
        pipeline_info.pViewportState = &viewport_state;

        // Renderpass
        pipeline_info.renderPass = nullptr; // dynamic rendering

        // Vertex input
        vk::PipelineVertexInputStateCreateInfo vertex_input;
        vk::VertexInputBindingDescription vertex_binding;
        vertex_binding.binding = 0;
        vertex_binding.inputRate = vk::VertexInputRate::eVertex;
        vertex_binding.stride = _args.vertex_input.vertex_size;
        std::vector<vk::VertexInputAttributeDescription> vertex_attrs;
        for (const auto& attr : _args.vertex_input.attributes)
        {
            vk::VertexInputAttributeDescription desc;
            desc.binding = 0;
            desc.format = to_vk_format(attr.type);
            desc.location = attr.location;
            desc.offset = attr.offset;
            vertex_attrs.push_back(desc);
        }

        vertex_input.pVertexBindingDescriptions = &vertex_binding;
        vertex_input.pVertexAttributeDescriptions = vertex_attrs.data();
        vertex_input.vertexBindingDescriptionCount = 1;
        vertex_input.vertexAttributeDescriptionCount = vertex_attrs.size();

        pipeline_info.pVertexInputState = &vertex_input;

        // Shader stages
        CRITICAL_CHECK(_args.vertex_shader->get_module(vkctx).has_value());
        CRITICAL_CHECK(_args.fragment_shader->get_module(vkctx).has_value());

        vk::PipelineShaderStageCreateInfo vertex_shader_stage;
        vertex_shader_stage.stage = vk::ShaderStageFlagBits::eVertex;
        vertex_shader_stage.module = *_args.vertex_shader->get_module(vkctx);
        vertex_shader_stage.pName = "main";

        vk::PipelineShaderStageCreateInfo fragment_shader_stage;
        fragment_shader_stage.stage = vk::ShaderStageFlagBits::eFragment;
        fragment_shader_stage.module = *_args.fragment_shader->get_module(vkctx);
        fragment_shader_stage.pName = "main";

        std::array<vk::PipelineShaderStageCreateInfo, 2> shader_stages = { vertex_shader_stage, fragment_shader_stage };

        pipeline_info.pStages = shader_stages.data();
        pipeline_info.stageCount = shader_stages.size();

        // Dynamic rendering
        vk::PipelineRenderingCreateInfo dynamic_render_info;
        dynamic_render_info.colorAttachmentCount = _args.color_attachment_formats.size();
        dynamic_render_info.pColorAttachmentFormats = _args.color_attachment_formats.data();
        if (_args.enable_depth)
        {
            dynamic_render_info.depthAttachmentFormat = _args.depth_stencil_format;
            dynamic_render_info.stencilAttachmentFormat = _args.depth_stencil_format;
        }

        pipeline_info.pNext = &dynamic_render_info;

        // Layout
        std::vector<vk::DescriptorSetLayout> layouts;

        std::unordered_set<uint32_t> used_set_indices;
        for (const auto& def : _args.descriptor_sets) // Create pipeline descriptor set layouts
        {
            if (!def.definition.validate())
            {
                CRITICAL_ERROR("Invalid descriptor set definition");
            }
            if (used_set_indices.count(def.set_index))
            {
                CRITICAL_ERROR("Duplicated descriptor set index");
            }

            used_set_indices.emplace(def.set_index);

            std::vector<vk::DescriptorSetLayoutBinding> bindings;
            for (const auto& entry : def.definition.entries)
            {
                vk::DescriptorSetLayoutBinding b;
                b.binding = entry.binding;
                b.descriptorCount = entry.count;
                b.descriptorType = to_vk_descriptor_type(entry.type);
                b.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment;
                bindings.push_back(b);
            }

            vk::DescriptorSetLayoutCreateInfo info;
            info.bindingCount = bindings.size();
            info.pBindings = bindings.data();

            auto emplaced =
                _descriptor_set_layouts.emplace(def.set_index, vkctx.device().createDescriptorSetLayoutUnique(info));
            layouts.push_back(*emplaced.first->second);
        }

        vk::PipelineLayoutCreateInfo layout_info;
        layout_info.pushConstantRangeCount = 0;
        layout_info.pSetLayouts = layouts.data();
        layout_info.setLayoutCount = layouts.size();
        _layout = vkctx.device().createPipelineLayoutUnique(layout_info);

        pipeline_info.layout = *_layout;

        auto created_pipeline = vkctx.device().createGraphicsPipelineUnique(vkctx.pipeline_cache(), pipeline_info);
        CRITICAL_CHECK(created_pipeline.result == vk::Result::eSuccess);
        _pipeline = std::move(created_pipeline.value);
    }
} // namespace cathedral::gfx