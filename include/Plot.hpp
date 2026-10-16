#ifndef DATAVIZ_PLOT_HPP
#define DATAVIZ_PLOT_HPP

#include <string>
#include <vector>

namespace dataviz {

// What kind of picture the Renderer should draw.
enum class PlotKind { Line, Scatter, Bar };

struct Point {
    double x;
    double y;
};

// One bar, spanning [x0, x1] on the x-axis.
// Histogram bars use real data edges and an empty label.
// Count-plot bars use x0 = i, x1 = i + 1 and the category name as label.
struct Bar {
    double x0;
    double x1;
    double height;
    std::string label;
};

// Pure data to be drawn. Titles, labels and notes live in Annotations.
// Line and Scatter plots use `points`; Bar plots use `bars`.
struct Plot {
    PlotKind kind = PlotKind::Line;
    std::vector<Point> points;
    std::vector<Bar> bars;
};

}  // namespace dataviz

#endif  // DATAVIZ_PLOT_HPP