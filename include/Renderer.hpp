#ifndef DATAVIZ_RENDERER_HPP
#define DATAVIZ_RENDERER_HPP

#include <string>

#include "Annotations.hpp"
#include "Plot.hpp"

namespace dataviz {

// Stage 7: builds the SVG text of a plot.
// It uses Axis for scaling and ticks, and Annotations for all text.
class Renderer {
public:
    // Returns a complete SVG document (800 x 600).
    // Throws std::invalid_argument if the plot has nothing to draw, a value
    // is NaN/infinite, or a bar has x0 >= x1.
    std::string render(const Plot& plot, const Annotations& annotations) const;
};

}  // namespace dataviz

#endif  // DATAVIZ_RENDERER_HPP