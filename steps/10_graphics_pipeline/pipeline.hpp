#pragma once

#include "shaders.hpp"

namespace graphics
{
namespace vkinit
{
struct graphics_pipeline_bundle_create_info
{
    vk::raii::Device &device;
    std::string vertex_file_path;
    std::string fragment_file_path;
    vk::Extent2D swapchain_extent;
    vk::Format swapchain_image_format;
};

inline vk::raii::PipelineLayout make_pipeline_layout (vk::raii::Device &device)
{

    vk::PipelineLayoutCreateInfo layout_info;
    layout_info.flags                  = vk::PipelineLayoutCreateFlags ();
    layout_info.setLayoutCount         = 0;
    layout_info.pushConstantRangeCount = 0;
    return device.createPipelineLayout (layout_info);
}

inline vk::raii::RenderPass make_renderpass (vk::raii::Device &device, vk::Format swapchain_image_format)
{
    // define a general attachment, with its load/store operations
    vk::AttachmentDescription color_attachment = {};
    color_attachment.flags                     = vk::AttachmentDescriptionFlags ();
    color_attachment.format                    = swapchain_image_format;
    color_attachment.samples                   = vk::SampleCountFlagBits::e1;
    color_attachment.loadOp                    = vk::AttachmentLoadOp::eClear;
    color_attachment.storeOp                   = vk::AttachmentStoreOp::eStore;
    color_attachment.stencilLoadOp             = vk::AttachmentLoadOp::eDontCare;
    color_attachment.stencilStoreOp            = vk::AttachmentStoreOp::eDontCare;
    color_attachment.initialLayout             = vk::ImageLayout::eUndefined;
    color_attachment.finalLayout               = vk::ImageLayout::ePresentSrcKHR;

    // declare that attachment to be color buffer 0 of the framebuffer
    vk::AttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment              = 0;
    color_attachment_ref.layout                  = vk::ImageLayout::eColorAttachmentOptimal;

    // renderpasses are broken down into subpasses, there's always at least one.
    vk::SubpassDescription subpass = {};
    subpass.flags                  = vk::SubpassDescriptionFlags ();
    subpass.pipelineBindPoint      = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount   = 1;
    subpass.pColorAttachments      = &color_attachment_ref;

    // create the renderpass
    vk::RenderPassCreateInfo renderpassInfo = {};
    renderpassInfo.flags                    = vk::RenderPassCreateFlags ();
    renderpassInfo.attachmentCount          = 1;
    renderpassInfo.pAttachments             = &color_attachment;
    renderpassInfo.subpassCount             = 1;
    renderpassInfo.pSubpasses               = &subpass;
    return device.createRenderPass (renderpassInfo);
}

struct graphics_pipeline_bundle
{
    graphics_pipeline_bundle () {}
    graphics_pipeline_bundle (const graphics_pipeline_bundle_create_info &specification)
    {
        vk::GraphicsPipelineCreateInfo pipeline_info {};
        pipeline_info.flags = vk::PipelineCreateFlags ();

        std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;

        // vertex input
        vk::PipelineVertexInputStateCreateInfo vertex_input_info {};
        vertex_input_info.flags                           = vk::PipelineVertexInputStateCreateFlags ();
        vertex_input_info.vertexBindingDescriptionCount   = 0;
        vertex_input_info.vertexAttributeDescriptionCount = 0;
        pipeline_info.pVertexInputState                   = &vertex_input_info;

        // input assembly
        vk::PipelineInputAssemblyStateCreateInfo input_asm_info {};
        input_asm_info.flags              = vk::PipelineInputAssemblyStateCreateFlags ();
        input_asm_info.topology           = vk::PrimitiveTopology::eTriangleList;
        pipeline_info.pInputAssemblyState = &input_asm_info;

        // vertex shader
        std::cout << "Create vertex shader module" << std::endl;
        auto vertex_shader = vk_utils::create_module (specification.vertex_file_path, specification.device);
        vk::PipelineShaderStageCreateInfo vertex_shader_info {};
        vertex_shader_info.flags  = vk::PipelineShaderStageCreateFlags ();
        vertex_shader_info.stage  = vk::ShaderStageFlagBits::eVertex;
        vertex_shader_info.module = *vertex_shader;
        vertex_shader_info.pName  = "main";
        shader_stages.push_back (vertex_shader_info);

        // viewport and scissor
        vk::Viewport viewport = {};

        viewport.x         = 0.0f;
        viewport.y         = 0.0f;
        viewport.width     = (float)specification.swapchain_extent.width;
        viewport.height    = (float)specification.swapchain_extent.height;
        viewport.minDepth  = 0.0f;
        viewport.maxDepth  = 1.0f;
        vk::Rect2D scissor = {};
        scissor.offset.x   = 0.0f;
        scissor.offset.y   = 0.0f;
        scissor.extent     = specification.swapchain_extent;

        vk::PipelineViewportStateCreateInfo viewport_info = {};

        viewport_info.flags          = vk::PipelineViewportStateCreateFlags ();
        viewport_info.viewportCount  = 1;
        viewport_info.pViewports     = &viewport;
        viewport_info.scissorCount   = 1;
        viewport_info.pScissors      = &scissor;
        pipeline_info.pViewportState = &viewport_info;

        // rasterisation
        vk::PipelineRasterizationStateCreateInfo rasterizer = {};

        rasterizer.flags                   = vk::PipelineRasterizationStateCreateFlags ();
        rasterizer.depthClampEnable        = VK_FALSE;   // discard out of bounds fragments, don't clamp them
        rasterizer.rasterizerDiscardEnable = VK_FALSE;   // This flag would disable fragment output
        rasterizer.polygonMode             = vk::PolygonMode::eFill;
        rasterizer.lineWidth               = 1.0f;
        rasterizer.cullMode                = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace               = vk::FrontFace::eClockwise;
        rasterizer.depthBiasEnable         = VK_FALSE;   // Depth bias can be useful in shadow maps.
        pipeline_info.pRasterizationState  = &rasterizer;

        // fragment shader
        std::cout << "Create fragment shader module" << std::endl;

        auto fragment_shader = vk_utils::create_module (specification.fragment_file_path, specification.device);
        vk::PipelineShaderStageCreateInfo fragment_shader_info = {};
        fragment_shader_info.flags                             = vk::PipelineShaderStageCreateFlags ();
        fragment_shader_info.stage                             = vk::ShaderStageFlagBits::eFragment;
        fragment_shader_info.module                            = *fragment_shader;
        fragment_shader_info.pName                             = "main";
        shader_stages.push_back (fragment_shader_info);

        // declare shaders to the pipeline info
        pipeline_info.stageCount = shader_stages.size ();
        pipeline_info.pStages    = shader_stages.data ();

        // multisampling
        vk::PipelineMultisampleStateCreateInfo multisampling = {};

        multisampling.flags                = vk::PipelineMultisampleStateCreateFlags ();
        multisampling.sampleShadingEnable  = VK_FALSE;
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
        pipeline_info.pMultisampleState    = &multisampling;

        // color blend
        vk::PipelineColorBlendAttachmentState color_blend_attachments = {};
        color_blend_attachments.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        color_blend_attachments.blendEnable = VK_FALSE;

        vk::PipelineColorBlendStateCreateInfo color_blending = {};
        color_blending.flags                                 = vk::PipelineColorBlendStateCreateFlags ();
        color_blending.logicOpEnable                         = VK_FALSE;
        color_blending.logicOp                               = vk::LogicOp::eCopy;
        color_blending.attachmentCount                       = 1;
        color_blending.pAttachments                          = &color_blend_attachments;
        color_blending.blendConstants[0]                     = 0.0f;
        color_blending.blendConstants[1]                     = 0.0f;
        color_blending.blendConstants[2]                     = 0.0f;
        color_blending.blendConstants[3]                     = 0.0f;
        pipeline_info.pColorBlendState                       = &color_blending;

        // pipeline layout
        std::cout << "Create Pipeline Layout" << std::endl;
        m_layout             = make_pipeline_layout (specification.device);
        pipeline_info.layout = *m_layout;

        // renderpass
        std::cout << "Create RenderPass" << std::endl;
        m_renderpass = make_renderpass (specification.device, specification.swapchain_image_format);

        // make the pipeline
        std::cout << "Create Graphics Pipeline" << std::endl;
        pipeline_info.renderPass         = *m_renderpass;
        pipeline_info.subpass            = 0;
        pipeline_info.basePipelineHandle = nullptr;
        m_pipeline                       = specification.device.createGraphicsPipeline (nullptr, pipeline_info);
    }
    vk::raii::PipelineLayout m_layout {nullptr};
    vk::raii::RenderPass m_renderpass {nullptr};
    vk::raii::Pipeline m_pipeline {nullptr};
};

}   // namespace vkinit
}   // namespace graphics