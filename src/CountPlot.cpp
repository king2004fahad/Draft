
#include "CountPlot.hpp"

#include <algorithm>
#include <stdexcept>
#include <unordered_map>

namespace dataviz {

void CountPlot::count(const std::vector<std::string>& data)
{
    counts_.clear();

    // category -> position in counts_ (fast lookup, keeps first-seen order)
    std::unordered_map<std::string, std::size_t> position;

    for (const std::string& category : data)
    {
        auto it = position.find(category);
        if (it == position.end())
        {
            position[category] = counts_.size();
            counts_.push_back({category, 1});
        }
        else
        {
            ++counts_[it->second].frequency;
        }
    }
}

const std::vector<Count>& CountPlot::getCounts() const { return counts_; }

std::vector<std::string> CountPlot::categories() const
{
    std::vector<std::string> result;
    result.reserve(counts_.size());
    for (const Count& item : counts_)
    {
        result.push_back(item.category);
    }
    return result;
}

std::vector<std::size_t> CountPlot::frequencies() const
{
    std::vector<std::size_t> result;
    result.reserve(counts_.size());
    for (const Count& item : counts_)
    {
        result.push_back(item.frequency);
    }
    return result;
}

std::size_t CountPlot::maxFrequency() const
{
    std::size_t highest = 0;
    for (const Count& item : counts_)
    {
        highest = std::max(highest, item.frequency);
    }
    return highest;
}

std::size_t CountPlot::total() const
{
    std::size_t sum = 0;
    for (const Count& item : counts_)
    {
        sum += item.frequency;
    }
    return sum;
}

Plot CountPlot::toPlot() const
{
    if (counts_.empty())
    {
        throw std::invalid_argument("CountPlot::toPlot: nothing has been counted");
    }

    Plot plot;
    plot.kind = PlotKind::Bar;
    for (std::size_t i = 0; i < counts_.size(); ++i)
    {
        plot.bars.push_back({static_cast<double>(i),
                             static_cast<double>(i + 1),
                             static_cast<double>(counts_[i].frequency),
                             counts_[i].category});
    }
    return plot;
}

}  // namespace dataviz