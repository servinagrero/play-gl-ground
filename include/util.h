#include <fstream>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

const fs::path shader_dir = fs::current_path() / "include/shader";

std::string load_shader(const fs::path shader_path)
{
        std::ifstream file;

        file.open(shader_dir / shader_path);
        if(!file.good())
                throw std::invalid_argument("Shader " + shader_path.string() + " does not exists");

        std::string content( (std::istreambuf_iterator<char>(file) ),
                             (std::istreambuf_iterator<char>()    ) );

        return content;
}
