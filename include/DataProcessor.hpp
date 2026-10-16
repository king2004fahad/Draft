#ifndef DATAVIZ_DATAPROCESSOR_HPP
#define DATAVIZ_DATAPROCESSOR_HPP

#include <string>
#include <vector>

namespace dataviz {

// Stage 2: turns raw text into clean numbers.
// Typical use:  cleanData(toNumbers(loader.getColumn("score")))
class DataProcessor {
public:
    // Converts text to numbers. Blank or non-numeric cells become NaN so
    // that cleanData() can remove them. Never throws.
    static std::vector<double> toNumbers(const std::vector<std::string>& text);

    static bool isEmpty(const std::vector<double>& data);

    // True if every value is finite (no NaN, no infinity). An empty
    // dataset is valid; check isEmpty() separately.
    static bool validateData(const std::vector<double>& data);

    // Copy of the data without NaN / infinite values.
    static std::vector<double> cleanData(const std::vector<double>& data);

    // Scales to [0, 1]. Throws std::invalid_argument if the data is empty
    // or not finite. If all values are equal, every result is 0.
    static std::vector<double> normalizeData(const std::vector<double>& data);

    // Throw std::invalid_argument on empty data.
    static double findMinimum(const std::vector<double>& data);
    static double findMaximum(const std::vector<double>& data);
};

}  // namespace dataviz

#endif  // DATAVIZ_DATAPROCESSOR_HPP