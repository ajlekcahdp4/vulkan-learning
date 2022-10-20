#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

namespace graphics
{
namespace vk_utils
{

std::vector<char> read_file (std::string filename)
{

    std::ifstream file (filename, std::ios::ate | std::ios::binary);

    assert (file.is_open ());
    assert (file.tellg () > 0);

    std::size_t filesize {static_cast<std::size_t> (file.tellg ())};

    std::vector<char> buffer (filesize);
    file.seekg (0);
    file.read (buffer.data (), filesize);

    file.close ();
    return buffer;
}

vk::raii::ShaderModule create_module (const std::string &filename, vk::raii::Device &device)
{
    auto source_code = read_file (filename);
    vk::ShaderModuleCreateInfo module_info {};
    module_info.flags    = vk::ShaderModuleCreateFlags {};
    module_info.codeSize = source_code.size ();
    module_info.pCode    = reinterpret_cast<uint32_t *> (source_code.data ());
    return device.createShaderModule (module_info);
}

}   // namespace vk_utils
}   // namespace graphics