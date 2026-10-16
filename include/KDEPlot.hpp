#ifndef DATAVIZ_KDEPLOT_HPP
#define DATAVIZ_KDEPLOT_HPP

#include <cstddef>
#include <vector>

#include "Plot.hpp"

namespace dataviz {

// Stage 4c: kernel density estimate (smooth curve) with a Gaussian kernel.
class KDEPlot {
public:
    // numPoints: how many x positions the curve is sampled at (>= 2).
    // bandwidth: smoothing width; 0 means "choose automatically" using
    //            Silverman's rule (Statistics::silvermanBandwidth).
    // Throws std::invalid_argument if data is empty or not finite,
    // numPoints < 2, or bandwidth < 0.
    explicit KDEPlot(const std::vector<double>& data,
                     std::size_t numPoints = 200,
                     double bandwidth = 0.0);

    double bandwidth() const;
    const std::vector<Point>& points() const;

    // Line plot of the density curve.
    Plot toPlot() const;

private:
    double bandwidth_;
    std::vector<Point> points_;
};

}  // namespace dataviz

#endif  // DATAVIZ_KDEPLOT_HPP