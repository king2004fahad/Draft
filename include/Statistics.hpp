#ifndef DATAVIZ_STATISTICS_HPP
#define DATAVIZ_STATISTICS_HPP

#include <cstddef>
#include <vector>

namespace dataviz {

// Stage 3: numerical summaries of a dataset.
// All functions throw std::invalid_argument on empty data.
class Statistics {
public:
    struct Summary {
        std::size_t count;
        double mean;
        double median;
        double standardDeviation;  // sample standard deviation (0 if count == 1)
        double min;
        double max;
    };

    static double mean(const std::vector<double>& data);
    static double median(const std::vector<double>& data);

    // sample = true divides by n - 1 and needs at least 2 values.
    static double variance(const std::vector<double>& data, bool sample = true);
    static double standardDeviation(const std::vector<double>& data, bool sample = true);

    static double min(const std::vector<double>& data);
    static double max(const std::vector<double>& data);

    static Summary summarize(const std::vector<double>& data);

    // Silverman's rule of thumb, used by KDEPlot:
    // h = 0.9 * sigma * n^(-1/5). Returns 1.0 when sigma cannot be
    // estimated (fewer than 2 values, or all values equal).
    static double silvermanBandwidth(const std::vector<double>& data);
};

}  // namespace dataviz

#endif  // DATAVIZ_STATISTICS_HPP