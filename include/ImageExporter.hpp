#ifndef DATAVIZ_IMAGEEXPORTER_HPP
#define DATAVIZ_IMAGEEXPORTER_HPP

#include <string>

namespace dataviz {

// Stage 8: writes the finished SVG text to a file.
class ImageExporter {
public:
    // Throws std::invalid_argument if `svg` does not look like an SVG
    // document or `filename` does not end in ".svg".
    // Creates missing parent folders. Returns false if the file cannot be
    // written, true on success.
    bool exportSVG(const std::string& svg, const std::string& filename) const;
};

}  // namespace dataviz

#endif  // DATAVIZ_IMAGEEXPORTER_HPP