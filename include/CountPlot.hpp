#ifndef DATAVIZ_COUNTPLOT_HPP
#define DATAVIZ_COUNTPLOT_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "Plot.hpp"

namespace dataviz {

// One category and how many times it appeared.
struct Count {
    std::string category;
    std::size_t frequency;
};

// Stage 4a: counts how often each category appears.
class CountPlot {
public:
    // Counts each distinct category. Clears any previous result.
    // Categories keep the order of their first appearance.
    // Empty input gives an empty result (no error).
    void count(const std::vector<std::string>& data);

    const std::vector<Count>& getCounts() const;
    std::vector<std::string> categories() const;
    std::vector<std::size_t> frequencies() const;

    std::size_t maxFrequency() const;  // 0 if nothing counted
    std::size_t total() const;

    // Bar plot: bar i spans [i, i + 1], carries the category as its label.
    // Throws std::invalid_argument if nothing has been counted.
    Plot toPlot() const;

private:
    std::vector<Count> counts_;
};

}  // namespace dataviz

#endif  // DATAVIZ_COUNTPLOT_HPP