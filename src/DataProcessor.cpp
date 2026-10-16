#include "DataProcessor.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <stdexcept>

namespace dataviz {

std::vector<double> DataProcessor::toNumbers(const std::vector<std::string>& text)
{
    const double notANumber = std::numeric_limits<double>::quiet_NaN();
    std::vector<double> numbers;
    numbers.reserve(text.size());

    for (const std::string& cell : text)
    {
        const char* begin = cell.c_str();
        while (*begin != '\0' && std::isspace(static_cast<unsigned char>(*begin)))
        {
            ++begin;
        }
        if (*begin == '\0')
        {
            numbers.push_back(notANumber);
            continue;
        }

        char* end = nullptr;
        const double value = std::strtod(begin, &end);
        while (*end != '\0' && std::isspace(static_cast<unsigned char>(*end)))
        {
            ++end;
        }
        // Anything left over means the cell was not purely a number.
        numbers.push_back(*end == '\0' ? value : notANumber);
    }
    return numbers;
}

bool DataProcessor::isEmpty(const std::vector<double>& data)
{
    return data.empty();
}

bool DataProcessor::validateData(const std::vector<double>& data)
{
    return std::all_of(data.begin(), data.end(),
                       [](double v) { return std::isfinite(v); });
}

std::vector<double> DataProcessor::cleanData(const std::vector<double>& data)
{
    std::vector<double> cleaned;
    cleaned.reserve(data.size());
    for (double value : data)
    {
        if (std::isfinite(value))
        {
            cleaned.push_back(value);
        }
    }
    return cleaned;
}

double DataProcessor::findMinimum(const std::vector<double>& data)
{
    if (data.empty())
    {
        throw std::invalid_argument("findMinimum: dataset is empty");
    }
    return *std::min_element(data.begin(), data.end());
}

double DataProcessor::findMaximum(const std::vector<double>& data)
{
    if (data.empty())
    {
        throw std::invalid_argument("findMaximum: dataset is empty");
    }
    return *std::max_element(data.begin(), data.end());
}

std::vector<double> DataProcessor::normalizeData(const std::vector<double>& data)
{
    if (data.empty())
    {
        throw std::invalid_argument("normalizeData: dataset is empty");
    }
    if (!validateData(data))
    {
        throw std::invalid_argument("normalizeData: dataset contains NaN or infinity");
    }

    const double minimum = findMinimum(data);
    const double range = findMaximum(data) - minimum;

    if (range == 0.0)
    {
        return std::vector<double>(data.size(), 0.0);
    }

    std::vector<double> result;
    result.reserve(data.size());
    for (double value : data)
    {
        result.push_back((value - minimum) / range);
    }
    return result;
}

}  // namespace dataviz