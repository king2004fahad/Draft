#include "Statistics.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <string>

namespace dataviz {

namespace {

void requireData(const std::vector<double>& data, const char* where)
{
    if (data.empty())
    {
        throw std::invalid_argument(std::string("Statistics::") + where + ": dataset is empty");
    }
}

}  // namespace

double Statistics::mean(const std::vector<double>& data)
{
    requireData(data, "mean");
    return std::accumulate(data.begin(), data.end(), 0.0) / static_cast<double>(data.size());
}

double Statistics::median(const std::vector<double>& data)
{
    requireData(data, "median");
    std::vector<double> sorted(data);
    std::sort(sorted.begin(), sorted.end());

    const std::size_t n = sorted.size();
    if (n % 2 == 1)
    {
        return sorted[n / 2];
    }
    return (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0;
}

double Statistics::variance(const std::vector<double>& data, bool sample)
{
    requireData(data, "variance");
    if (sample && data.size() < 2)
    {
        throw std::invalid_argument("Statistics::variance: sample variance needs at least 2 values");
    }

    const double average = mean(data);
    double sumSquares = 0.0;
    for (double value : data)
    {
        const double diff = value - average;
        sumSquares += diff * diff;
    }

    const std::size_t divisor = sample ? data.size() - 1 : data.size();
    return sumSquares / static_cast<double>(divisor);
}

double Statistics::standardDeviation(const std::vector<double>& data, bool sample)
{
    return std::sqrt(variance(data, sample));
}

double Statistics::min(const std::vector<double>& data)
{
    requireData(data, "min");
    return *std::min_element(data.begin(), data.end());
}

double Statistics::max(const std::vector<double>& data)
{
    requireData(data, "max");
    return *std::max_element(data.begin(), data.end());
}

Statistics::Summary Statistics::summarize(const std::vector<double>& data)
{
    requireData(data, "summarize");
    Summary s;
    s.count = data.size();
    s.mean = mean(data);
    s.median = median(data);
    s.standardDeviation = data.size() > 1 ? standardDeviation(data, true) : 0.0;
    s.min = min(data);
    s.max = max(data);
    return s;
}

double Statistics::silvermanBandwidth(const std::vector<double>& data)
{
    requireData(data, "silvermanBandwidth");
    if (data.size() < 2)
    {
        return 1.0;
    }
    const double sigma = standardDeviation(data, true);
    if (sigma == 0.0)
    {
        return 1.0;
    }
    return 0.9 * sigma * std::pow(static_cast<double>(data.size()), -0.2);
}

}  // namespace dataviz