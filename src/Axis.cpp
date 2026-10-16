#include "Axis.hpp"

#include <cmath>
#include <stdexcept>

namespace dataviz {

namespace {

void checkScreen(double start, double end)
{
    if (!std::isfinite(start) || !std::isfinite(end))
    {
        throw std::invalid_argument("Axis: screen range must be finite");
    }
}

}  // namespace

Axis::Axis(double minValue, double maxValue, double screenStart, double screenEnd)
    : minValue_(0.0), maxValue_(1.0), screenStart_(0.0), screenEnd_(1.0)
{
    setRange(minValue, maxValue);
    setScreenRange(screenStart, screenEnd);
}

double Axis::scale(double value) const
{
    return screenStart_ +
           (value - minValue_) * (screenEnd_ - screenStart_) / (maxValue_ - minValue_);
}

std::vector<double> Axis::ticks(std::size_t count) const
{
    if (count < 2)
    {
        throw std::invalid_argument("Axis::ticks: need at least two ticks");
    }

    const double step = (maxValue_ - minValue_) / static_cast<double>(count - 1);

    std::vector<double> values;
    values.reserve(count);
    for (std::size_t i = 0; i < count; ++i)
    {
        values.push_back(minValue_ + static_cast<double>(i) * step);
    }
    values.back() = maxValue_;  // avoid floating-point drift on the last tick
    return values;
}

double Axis::getMin() const { return minValue_; }
double Axis::getMax() const { return maxValue_; }
double Axis::getScreenStart() const { return screenStart_; }
double Axis::getScreenEnd() const { return screenEnd_; }

void Axis::setRange(double newMinValue, double newMaxValue)
{
    if (!std::isfinite(newMinValue) || !std::isfinite(newMaxValue))
    {
        throw std::invalid_argument("Axis: range must be finite");
    }
    if (newMinValue > newMaxValue)
    {
        throw std::invalid_argument("Axis: minimum is larger than maximum");
    }
    if (newMinValue == newMaxValue)
    {
        newMinValue -= 1.0;
        newMaxValue += 1.0;
    }
    minValue_ = newMinValue;
    maxValue_ = newMaxValue;
}

void Axis::setScreenRange(double newScreenStart, double newScreenEnd)
{
    checkScreen(newScreenStart, newScreenEnd);
    screenStart_ = newScreenStart;
    screenEnd_ = newScreenEnd;
}

}  // namespace dataviz