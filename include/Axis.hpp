#ifndef DATAVIZ_AXIS_HPP
#define DATAVIZ_AXIS_HPP

#include <cstddef>
#include <vector>

namespace dataviz {

// Stage 5: maps data values to positions on the drawing and chooses ticks.
class Axis {
public:
    // minValue..maxValue is the data range; screenStart..screenEnd is where
    // that range is drawn (screenStart may be larger than screenEnd, which
    // is how a y-axis is flipped for SVG).
    // Throws std::invalid_argument if any value is NaN/infinite or
    // minValue > maxValue. If minValue == maxValue the range is widened by
    // 1 on each side so there is always something to scale.
    Axis(double minValue, double maxValue, double screenStart, double screenEnd);

    // Linear mapping of a data value to a screen position.
    double scale(double value) const;

    // `count` evenly spaced values from min to max, inclusive.
    // Throws std::invalid_argument if count < 2.
    std::vector<double> ticks(std::size_t count = 6) const;

    double getMin() const;
    double getMax() const;
    double getScreenStart() const;
    double getScreenEnd() const;

    // Same validation as the constructor.
    void setRange(double newMinValue, double newMaxValue);
    void setScreenRange(double newScreenStart, double newScreenEnd);

private:
    double minValue_;
    double maxValue_;
    double screenStart_;
    double screenEnd_;
};

}  // namespace dataviz

#endif  // DATAVIZ_AXIS_HPP