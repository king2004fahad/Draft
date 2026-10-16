// Stage 4: CountPlot, Histogram, KDEPlot
#include <numeric>

#include "dataviz.hpp"
#include "test_util.hpp"

using namespace dataviz;

static void testCountPlot()
{
    CountPlot plot;
    plot.count({"b", "a", "b", "c", "b", "a"});

    CHECK(plot.getCounts().size() == 3);
    CHECK(plot.categories()[0] == "b");  // first-appearance order
    CHECK(plot.frequencies()[0] == 3);
    CHECK(plot.maxFrequency() == 3);
    CHECK(plot.total() == 6);

    const Plot p = plot.toPlot();
    CHECK(p.kind == PlotKind::Bar);
    CHECK(p.bars.size() == 3);
    CHECK(p.bars[1].label == "a");
    CHECK_NEAR(p.bars[1].x0, 1.0);
    CHECK_NEAR(p.bars[1].x1, 2.0);
    CHECK_NEAR(p.bars[1].height, 2.0);

    plot.count({});  // clears the previous result
    CHECK(plot.total() == 0);
    CHECK_THROWS(plot.toPlot());
}

static void testHistogram()
{
    const Histogram h({1, 2, 2, 3, 3, 3, 4, 4, 4, 4}, 4);
    CHECK(h.numBins() == 4);
    CHECK_NEAR(h.rangeMin(), 1.0);
    CHECK_NEAR(h.rangeMax(), 4.0);
    CHECK_NEAR(h.binWidth(), 0.75);
    CHECK(h.totalCount() == 10);  // no value lost
    CHECK(h.counts().back() == 4);  // maximum lands in the last bin
    CHECK(h.binEdges().size() == 5);
    CHECK(h.maxCount() == 4);

    const Plot p = h.toPlot();
    CHECK(p.kind == PlotKind::Bar);
    CHECK(p.bars.size() == 4);
    CHECK(p.bars[0].label.empty());
    CHECK_NEAR(p.bars[0].x0, 1.0);

    const Histogram same({5, 5, 5}, 3);  // all-equal data still works
    CHECK(same.totalCount() == 3);

    CHECK_THROWS(Histogram({}, 3));
    CHECK_THROWS(Histogram({1.0, 2.0}, 0));
    CHECK_THROWS(Histogram({1.0, std::nan("")}, 3));
}

static void testKDE()
{
    const std::vector<double> data = {1, 2, 2, 3, 3, 3, 4, 4, 5};
    const KDEPlot kde(data, 400);

    CHECK(kde.bandwidth() > 0.0);
    CHECK(kde.points().size() == 400);

    // The area under a density curve should be about 1 (trapezoid rule).
    double area = 0.0;
    const std::vector<Point>& pts = kde.points();
    for (std::size_t i = 1; i < pts.size(); ++i)
    {
        area += 0.5 * (pts[i].y + pts[i - 1].y) * (pts[i].x - pts[i - 1].x);
    }
    CHECK(std::fabs(area - 1.0) < 0.01);

    const Plot p = kde.toPlot();
    CHECK(p.kind == PlotKind::Line);
    CHECK(p.points.size() == 400);

    CHECK_NEAR(KDEPlot(data, 50, 0.7).bandwidth(), 0.7);  // manual bandwidth
    CHECK_THROWS(KDEPlot({}));
    CHECK_THROWS(KDEPlot(data, 1));
    CHECK_THROWS(KDEPlot(data, 50, -1.0));
    CHECK_THROWS(KDEPlot({1.0, std::nan("")}));
}

int main()
{
    testCountPlot();
    testHistogram();
    testKDE();
    return reportResults("test_plots");
}