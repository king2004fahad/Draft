#include "KDEPlot.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "Statistics.hpp"

namespace dataviz {

namespace {

// Gaussian kernel: standard normal density evaluated at (x - xi) / h.
double gaussianKernel(double x, double xi, double h)
{
    const double invSqrt2Pi = 0.3989422804014327;  // 1 / sqrt(2 * pi)
    const double u = (x - xi) / h;
    return invSqrt2Pi * std::exp(-0.5 * u * u) / h;
}

}  // namespace

KDEPlot::KDEPlot(const std::vector<double>& data, std::size_t numPoints, double bandwidth)
    : bandwidth_(bandwidth)
{
    if (data.empty())
    {
        throw std::invalid_argument("KDEPlot: data is empty");
    }
    if (numPoints < 2)
    {
        throw std::invalid_argument("KDEPlot: numPoints must be at least 2");
    }
    if (bandwidth < 0.0 || !std::isfinite(bandwidth))
    {
        throw std::invalid_argument("KDEPlot: bandwidth must be >= 0");
    }
    for (double x : data)
    {
        if (!std::isfinite(x))
        {
            throw std::invalid_argument("KDEPlot: data contains NaN or infinity");
        }
    }

    if (bandwidth_ == 0.0)
    {
        bandwidth_ = Statistics::silvermanBandwidth(data);
    }

    // Extend 3 bandwidths past the data so the tails fade out.
    const double start = Statistics::min(data) - 3.0 * bandwidth_;
    const double end = Statistics::max(data) + 3.0 * bandwidth_;
    const double step = (end - start) / static_cast<double>(numPoints - 1);

    points_.reserve(numPoints);
    for (std::size_t i = 0; i < numPoints; ++i)
    {
        const double x = start + static_cast<double>(i) * step;
        double sum = 0.0;
        for (double value : data)
        {
            sum += gaussianKernel(x, value, bandwidth_);
        }
        points_.push_back({x, sum / static_cast<double>(data.size())});
    }
}

double KDEPlot::bandwidth() const { return bandwidth_; }

const std::vector<Point>& KDEPlot::points() const { return points_; }

Plot KDEPlot::toPlot() const
{
    Plot plot;
    plot.kind = PlotKind::Line;
    plot.points = points_;
    return plot;
}

}  // namespace dataviz