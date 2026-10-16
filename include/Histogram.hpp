#ifndef DATAVIZ_HISTOGRAM_HPP
#define DATAVIZ_HISTOGRAM_HPP

#include <cstddef>
#include <vector>

#include "Plot.hpp"

namespace dataviz {

// Stage 4b: divides numeric data into equal-width bins and counts them.
class Histogram {
public:
    // Throws std::invalid_argument if data is empty, numBins is 0,
    // or data contains NaN/infinity.
    Histogram(const std::vector<double>& data, std::size_t numBins);

    std::size_t numBins() const;
    double rangeMin() const;
    double rangeMax() const;
    double binWidth() const;
    std::size_t totalCount() const;
    std::size_t maxCount() const;

    // counts()[i] is the number of values in bin i.
    const std::vector<std::size_t>& counts() const;

    // numBins + 1 edges: bin i covers [edges[i], edges[i+1]).
    // The last bin also includes its upper edge.
    const std::vector<double>& binEdges() const;

    // Bar plot with one bar per bin, using the real bin edges.
    Plot toPlot() const;

private:
    std::size_t numBins_;
    double rangeMin_;
    double rangeMax_;
    double binWidth_;
    std::vector<std::size_t> counts_;
    std::vector<double> binEdges_;
};

}  // namespace dataviz

#endif  // DATAVIZ_HISTOGRAM_HPP