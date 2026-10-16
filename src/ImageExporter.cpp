#include "ImageExporter.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <system_error>

namespace dataviz {

bool ImageExporter::exportSVG(const std::string& svg, const std::string& filename) const
{
    if (svg.find("<svg") == std::string::npos)
    {
        throw std::invalid_argument("ImageExporter: content is not an SVG document");
    }

    const std::filesystem::path path(filename);
    if (path.extension() != ".svg")
    {
        throw std::invalid_argument("ImageExporter: filename must end with .svg");
    }

    if (path.has_parent_path())
    {
        std::error_code ignored;
        std::filesystem::create_directories(path.parent_path(), ignored);
    }

    std::ofstream file(filename);
    if (!file.is_open())
    {
        return false;
    }
    file << svg;
    return file.good();
}

}  // namespace dataviz