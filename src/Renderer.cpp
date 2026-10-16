#include "Renderer.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

#include "Axis.hpp"

namespace dataviz {

namespace {

const double WIDTH = 800.0;
const double HEIGHT = 600.0;
const double MARGIN_LEFT = 80.0;
const double MARGIN_RIGHT = 40.0;
const double MARGIN_TOP = 60.0;
const double MARGIN_BOTTOM = 80.0;
const std::size_t TICK_COUNT = 6;

const char* const FONT = "font-family=\"sans-serif\"";
const char* const DATA_COLOR = "#4682b4";
const char* const GRID_COLOR = "#e0e0e0";
const char* const NOTE_COLOR = "#c0392b";
const char* const BEST_FIT_COLOR = "#c0392b";

// Replaces characters that have a special meaning in XML.
std::string escapeXml(const std::string& text)
{
    std::string out;
    for (char c : text)
    {
        switch (c)
        {
            case '&':  out += "&amp;";  break;
            case '<':  out += "&lt;";   break;
            case '>':  out += "&gt;";   break;
            case '"':  out += "&quot;"; break;
            case '\'': out += "&apos;"; break;
            default:   out += c;        break;
        }
    }
    return out;
}

// Number for SVG attributes.
std::string num(double value)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value;
    return out.str();
}

// Number for human-readable tick labels.
std::string tickText(double value)
{
    std::ostringstream out;
    out << std::setprecision(4) << value;
    return out.str();
}

void requireFinite(double value, const char* what)
{
    if (!std::isfinite(value))
    {
        throw std::invalid_argument(std::string("Renderer: ") + what + " is NaN or infinite");
    }
}

bool bestFitLine(const std::vector<Point>& points, double xMin, double xMax,
                 Point& start, Point& end)
{
    if (points.size() < 2 || xMin == xMax)
    {
        return false;
    }

    long double meanX = 0.0L;
    long double meanY = 0.0L;
    for (const Point& point : points)
    {
        meanX += point.x;
        meanY += point.y;
    }
    meanX /= points.size();
    meanY /= points.size();

    long double covariance = 0.0L;
    long double varianceX = 0.0L;
    for (const Point& point : points)
    {
        const long double dx = point.x - meanX;
        covariance += dx * (point.y - meanY);
        varianceX += dx * dx;
    }
    if (varianceX == 0.0L)
    {
        return false;
    }

    const long double slope = covariance / varianceX;
    const long double intercept = meanY - slope * meanX;
    const long double startY = slope * xMin + intercept;
    const long double endY = slope * xMax + intercept;
    if (!std::isfinite(slope) || !std::isfinite(startY) || !std::isfinite(endY) ||
        std::fabs(startY) > std::numeric_limits<double>::max() ||
        std::fabs(endY) > std::numeric_limits<double>::max())
    {
        throw std::invalid_argument("Renderer: best-fit line cannot be computed for this data");
    }

    start = {xMin, static_cast<double>(startY)};
    end = {xMax, static_cast<double>(endY)};
    return true;
}

}  // namespace

std::string Renderer::render(const Plot& plot, const Annotations& annotations) const
{
    const bool isBar = (plot.kind == PlotKind::Bar);

    // ---- 1. validate and find the data ranges ---------------------------
    double xMin = 0.0, xMax = 0.0, yMin = 0.0, yMax = 0.0;

    if (isBar)
    {
        if (plot.bars.empty())
        {
            throw std::invalid_argument("Renderer: bar plot has no bars");
        }
        xMin = plot.bars.front().x0;
        xMax = plot.bars.front().x1;
        for (const Bar& b : plot.bars)
        {
            requireFinite(b.x0, "bar x0");
            requireFinite(b.x1, "bar x1");
            requireFinite(b.height, "bar height");
            if (b.x0 >= b.x1)
            {
                throw std::invalid_argument("Renderer: bar needs x0 < x1");
            }
            xMin = std::min(xMin, b.x0);
            xMax = std::max(xMax, b.x1);
            yMin = std::min(yMin, b.height);  // bars always include y = 0
            yMax = std::max(yMax, b.height);
        }
    }
    else
    {
        if (plot.points.empty())
        {
            throw std::invalid_argument("Renderer: plot has no points");
        }
        xMin = xMax = plot.points.front().x;
        yMin = yMax = plot.points.front().y;
        for (const Point& p : plot.points)
        {
            requireFinite(p.x, "point x");
            requireFinite(p.y, "point y");
            xMin = std::min(xMin, p.x);
            xMax = std::max(xMax, p.x);
            yMin = std::min(yMin, p.y);
            yMax = std::max(yMax, p.y);
        }
    }

    Point fitStart{};
    Point fitEnd{};
    const bool hasBestFit =
        plot.kind == PlotKind::Scatter && plot.showBestFit &&
        bestFitLine(plot.points, xMin, xMax, fitStart, fitEnd);
    if (hasBestFit)
    {
        yMin = std::min(yMin, std::min(fitStart.y, fitEnd.y));
        yMax = std::max(yMax, std::max(fitStart.y, fitEnd.y));
    }

    // ---- 2. axes: data range -> pixel range -----------------------------
    const double left = MARGIN_LEFT;
    const double right = WIDTH - MARGIN_RIGHT;
    const double top = MARGIN_TOP;
    const double bottom = HEIGHT - MARGIN_BOTTOM;

    const Axis xAxis(xMin, xMax, left, right);
    const Axis yAxis(yMin, yMax, bottom, top);  // flipped: SVG y grows downward

    // Bars labelled with category names get one label per bar, not numbers.
    const bool categorical =
        isBar && std::all_of(plot.bars.begin(), plot.bars.end(),
                             [](const Bar& b) { return !b.label.empty(); });

    std::ostringstream svg;
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << num(WIDTH)
        << "\" height=\"" << num(HEIGHT) << "\" viewBox=\"0 0 " << num(WIDTH)
        << " " << num(HEIGHT) << "\">\n";
    svg << "  <rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n";

    // ---- 3. grid, ticks and tick labels ---------------------------------
    for (double value : yAxis.ticks(TICK_COUNT))
    {
        const double y = yAxis.scale(value);
        svg << "  <line x1=\"" << num(left) << "\" y1=\"" << num(y) << "\" x2=\""
            << num(right) << "\" y2=\"" << num(y) << "\" stroke=\"" << GRID_COLOR << "\"/>\n";
        svg << "  <text x=\"" << num(left - 10) << "\" y=\"" << num(y + 4)
            << "\" text-anchor=\"end\" font-size=\"12\" " << FONT << ">"
            << tickText(value) << "</text>\n";
    }

    if (categorical)
    {
        const bool crowded = plot.bars.size() > 6;
        for (const Bar& b : plot.bars)
        {
            const double centre = xAxis.scale((b.x0 + b.x1) / 2.0);
            svg << "  <text x=\"" << num(centre) << "\" y=\"" << num(bottom + 20)
                << "\" text-anchor=\"" << (crowded ? "end" : "middle")
                << "\" font-size=\"12\" " << FONT;
            if (crowded)
            {
                svg << " transform=\"rotate(-35 " << num(centre) << " " << num(bottom + 20) << ")\"";
            }
            svg << ">" << escapeXml(b.label) << "</text>\n";
        }
    }
    else
    {
        for (double value : xAxis.ticks(TICK_COUNT))
        {
            const double x = xAxis.scale(value);
            svg << "  <line x1=\"" << num(x) << "\" y1=\"" << num(top) << "\" x2=\""
                << num(x) << "\" y2=\"" << num(bottom) << "\" stroke=\"" << GRID_COLOR << "\"/>\n";
            svg << "  <text x=\"" << num(x) << "\" y=\"" << num(bottom + 20)
                << "\" text-anchor=\"middle\" font-size=\"12\" " << FONT << ">"
                << tickText(value) << "</text>\n";
        }
    }

    // ---- 4. the data itself ---------------------------------------------
    if (isBar)
    {
        const double baseline = yAxis.scale(0.0);
        for (const Bar& b : plot.bars)
        {
            double x0 = xAxis.scale(b.x0);
            double x1 = xAxis.scale(b.x1);
            if (categorical)
            {
                const double gap = 0.1 * (x1 - x0);  // space between bars
                x0 += gap;
                x1 -= gap;
            }
            const double tip = yAxis.scale(b.height);
            svg << "  <rect x=\"" << num(x0) << "\" y=\"" << num(std::min(tip, baseline))
                << "\" width=\"" << num(x1 - x0) << "\" height=\""
                << num(std::fabs(baseline - tip)) << "\" fill=\"" << DATA_COLOR
                << "\" stroke=\"white\"/>\n";
        }
    }
    else
    {
        if (plot.kind == PlotKind::Line)
        {
            svg << "  <polyline fill=\"none\" stroke=\"" << DATA_COLOR
                << "\" stroke-width=\"2\" points=\"";
            for (const Point& p : plot.points)
            {
                svg << num(xAxis.scale(p.x)) << "," << num(yAxis.scale(p.y)) << " ";
            }
            svg << "\"/>\n";
        }
        else
        {
            if (hasBestFit)
            {
                svg << "  <line x1=\"" << num(xAxis.scale(fitStart.x)) << "\" y1=\""
                    << num(yAxis.scale(fitStart.y)) << "\" x2=\""
                    << num(xAxis.scale(fitEnd.x)) << "\" y2=\""
                    << num(yAxis.scale(fitEnd.y)) << "\" stroke=\"" << BEST_FIT_COLOR
                    << "\" stroke-width=\"2\" stroke-dasharray=\"6,4\"/>\n";
            }
            for (const Point& p : plot.points)
            {
                svg << "  <circle cx=\"" << num(xAxis.scale(p.x)) << "\" cy=\""
                    << num(yAxis.scale(p.y)) << "\" r=\"4\" fill=\"" << DATA_COLOR
                    << "\" fill-opacity=\"0.7\"/>\n";
            }
        }
    }

    // ---- 5. axis lines (drawn after the data so they stay visible) ------
    svg << "  <line x1=\"" << num(left) << "\" y1=\"" << num(bottom) << "\" x2=\""
        << num(right) << "\" y2=\"" << num(bottom) << "\" stroke=\"black\"/>\n";
    svg << "  <line x1=\"" << num(left) << "\" y1=\"" << num(top) << "\" x2=\""
        << num(left) << "\" y2=\"" << num(bottom) << "\" stroke=\"black\"/>\n";

    // ---- 6. annotations: title, axis labels, notes ----------------------
    svg << "  <text x=\"" << num(WIDTH / 2) << "\" y=\"" << num(MARGIN_TOP / 2 + 6)
        << "\" text-anchor=\"middle\" font-size=\"20\" " << FONT << ">"
        << escapeXml(annotations.title()) << "</text>\n";
    svg << "  <text x=\"" << num((left + right) / 2) << "\" y=\"" << num(HEIGHT - 15)
        << "\" text-anchor=\"middle\" font-size=\"14\" " << FONT << ">"
        << escapeXml(annotations.xLabel()) << "</text>\n";
    svg << "  <text x=\"22\" y=\"" << num((top + bottom) / 2)
        << "\" text-anchor=\"middle\" font-size=\"14\" " << FONT
        << " transform=\"rotate(-90 22 " << num((top + bottom) / 2) << ")\">"
        << escapeXml(annotations.yLabel()) << "</text>\n";

    for (const Note& note : annotations.notes())
    {
        const double x = xAxis.scale(note.x);
        const double y = yAxis.scale(note.y);
        svg << "  <circle cx=\"" << num(x) << "\" cy=\"" << num(y) << "\" r=\"4\" fill=\""
            << NOTE_COLOR << "\"/>\n";
        svg << "  <text x=\"" << num(x + 8) << "\" y=\"" << num(y - 8)
            << "\" font-size=\"12\" fill=\"" << NOTE_COLOR << "\" " << FONT << ">"
            << escapeXml(note.text) << "</text>\n";
    }

    svg << "</svg>\n";
    return svg.str();
}

}  // namespace dataviz