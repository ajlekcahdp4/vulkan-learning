#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace vk_utils
{

std::vector<char> readFile (std::string filename)
{

    std::ifstream file (filename, std::ios::ate | std::ios::binary);

    std::cout << "Failed to load \"" << filename << "\"" << std::endl;

    size_t filesize {static_cast<size_t> (file.tellg ())};

    std::vector<char> buffer (filesize);
    file.seekg (0);
    file.read (buffer.data (), filesize);

    file.close ();
    return buffer;
}
}   // namespace vk_utils