#ifndef __UTILS_CONSTANT__
#define __UTILS_CONSTANT__

#include "utils/define.hpp"
#include <Windows.h>
#include <fstream>
#include <array>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION

namespace TutoVulkan
{
// ------------------------------------- VALIDATION LAYER -------------------------------------
static const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

#ifdef _DEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

// -------------------------------------------- PATH ------------------------------------------
static Path getExecutableFile()
{
    wchar_t path[MAX_PATH] = {0};
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    Path exe(path);
    return exe;
}

static const Path EXECUTABLE_FILE = getExecutableFile();
static const Path EXECUTABLE_DIR = EXECUTABLE_FILE.parent_path();
static const Path SHADERS_DIR = Path(EXECUTABLE_DIR.string() + "/../../../src/shaders/");
static const Path LIBS_DIR = Path(EXECUTABLE_DIR.string() + "/..");
static const std::string IMGUI_INI_FILE = Path(EXECUTABLE_DIR.string() + "/../../imgui.ini").string();

inline const Path getShadersPath(const std::string &p_filename)
{
    return Path(SHADERS_DIR) /= p_filename;
}

inline std::vector<char> readPathBinary(const Path &p_path)
{
    std::ifstream file;
    file.open(p_path, std::ios::binary | std::ios::in | std::ios::ate);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file " + p_path.string());
    }

    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> result(size);
    file.read(result.data(), size);
    file.close();

    assert(size > 0);

    return result;
}

// --------------------------------------- VULKAN CONST ---------------------------------------
static const std::vector<const char *> EXTENSIONS = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
static const int MAX_FRAMES_IN_FLIGHT = 2;
static Vec3f CLEAR_COLOR = Vec3f(0.f, 0.f, 0.f);
} // namespace TutoVulkan

#endif
