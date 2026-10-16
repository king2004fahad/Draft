#include "Histogram.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace dataviz {

Histogram::Histogram(const std::vector<double>& data, std::size_t numBins)
    : numBins_(numBins), rangeMin_(0.0), rangeMax_(0.0), binWidth_(0.0)
{
    if (data.empty())
    {
        throw std::invalid_argument("Histogram: data cannot be empty");
    }
    if (numBins == 0)
    {
        throw std::invalid_argument("Histogram: Number of bins must be greater than zero");
    }
    for (double x : data)
    {
        if (!std::isfinite(x))
        {
            throw std::invalid_argument("Histogram: data cannot contain NaN or infinity");
        }
    }

    const auto minmax = std::minmax_element(data.begin(), data.end());
    rangeMin_ = *minmax.first;
    rangeMax_ = *minmax.second;

    // All values equal: widen the range so bins have a width.
    if (rangeMin_ == rangeMax_)
    {
        rangeMin_ -= 0.5;
        rangeMax_ += 0.5;
    }

    binWidth_ = (rangeMax_ - rangeMin_) / static_cast<double>(numBins_);

    binEdges_.reserve(numBins_ + 1);
    for (std::size_t i = 0; i <= numBins_; ++i)
    {
        binEdges_.push_back(rangeMin_ + static_cast<double>(i) * binWidth_);
    }
    binEdges_.back() = rangeMax_;  // avoid floating-point drift

    counts_.assign(numBins_, 0);
    for (double x : data)
    {
        auto index = static_cast<std::size_t>((x - rangeMin_) / binWidth_);
        if (index >= numBins_)
        {
            index = numBins_ - 1;  // the maximum value goes in the last bin
        }
        ++counts_[index];
    }
}

std::size_t Histogram::numBins() const { return numBins_; }
double Histogram::rangeMin() const { return rangeMin_; }
double Histogram::rangeMax() const { return rangeMax_; }
double Histogram::binWidth() const { return binWidth_; }

std::size_t Histogram::totalCount() const
{
    std::size_t total = 0;
    for (std::size_t c : counts_)
    {
        total += c;
    }
    return total;
}

std::size_t Histogram::maxCount() const
{
    return *std::max_element(counts_.begin(), counts_.end());
}

const std::vector<std::size_t>& Histogram::counts() const { return counts_; }
const std::vector<double>& Histogram::binEdges() const { return binEdges_; }

Plot Histogram::toPlot() const
{
    Plot plot;
    plot.kind = PlotKind::Bar;
    for (std::size_t i = 0; i < numBins_; ++i)
    {
        plot.bars.push_back({binEdges_[i], binEdges_[i + 1],
                             static_cast<double>(counts_[i]), ""});
    }
    return plot;
}

}  // namespace dataviz