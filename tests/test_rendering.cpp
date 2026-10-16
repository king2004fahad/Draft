// Stages 5-8: Axis, Annotations, Renderer, ImageExporter
#include <cstdio>
#include <fstream>
#include <sstream>

#include "dataviz.hpp"
#include "test_util.hpp"

using namespace dataviz;

static bool contains(const std::string& text, const std::string& part)
{
    return text.find(part) != std::string::npos;
}

static void testAxis()
{
    const Axis a(0.0, 100.0, 0.0, 500.0);
    CHECK_NEAR(a.scale(50.0), 250.0);
    CHECK_NEAR(a.scale(0.0), 0.0);
    CHECK_NEAR(a.scale(100.0), 500.0);

    const Axis flipped(0.0, 10.0, 400.0, 0.0);  // y-axis style
    CHECK_NEAR(flipped.scale(0.0), 400.0);
    CHECK_NEAR(flipped.scale(10.0), 0.0);

    const std::vector<double> t = a.ticks(6);
    CHECK(t.size() == 6);
    CHECK_NEAR(t[0], 0.0);
    CHECK_NEAR(t[1], 20.0);
    CHECK_NEAR(t[5], 100.0);

    const Axis same(3.0, 3.0, 0.0, 100.0);  // widened, no division by zero
    CHECK_NEAR(same.getMin(), 2.0);
    CHECK_NEAR(same.getMax(), 4.0);
    CHECK_NEAR(same.scale(3.0), 50.0);

    CHECK_THROWS(a.ticks(1));
    CHECK_THROWS(Axis(5.0, 1.0, 0.0, 1.0));
    CHECK_THROWS(Axis(std::nan(""), 1.0, 0.0, 1.0));

    Axis b(0.0, 1.0, 0.0, 1.0);
    b.setRange(10.0, 20.0);
    b.setScreenRange(0.0, 50.0);
    CHECK_NEAR(b.scale(15.0), 25.0);
}

static void testAnnotations()
{
    Annotations a;
    CHECK(a.title().empty());
    a.setTitle("T");
    a.setXLabel("X");
    a.setYLabel("Y");
    a.addNote(1.0, 2.0, "note");
    CHECK(a.title() == "T");
    CHECK(a.xLabel() == "X");
    CHECK(a.yLabel() == "Y");
    CHECK(a.notes().size() == 1);
    CHECK_THROWS(a.addNote(std::nan(""), 0.0, "bad"));
    a.clearNotes();
    CHECK(a.notes().empty());
}

static void testRenderer()
{
    Renderer renderer;
    Annotations ann;
    ann.setTitle("Squares & <Cubes>");
    ann.setXLabel("x");
    ann.setYLabel("y");
    ann.addNote(2.0, 4.0, "look here");

    Plot line;
    line.kind = PlotKind::Line;
    line.points = {{1, 1}, {2, 4}, {3, 9}, {4, 16}};
    std::string svg = renderer.render(line, ann);
    CHECK(svg.find("<svg") == 0);
    CHECK(contains(svg, "</svg>"));
    CHECK(contains(svg, "<polyline"));
    CHECK(contains(svg, "Squares &amp; &lt;Cubes&gt;"));  // text is escaped
    CHECK(contains(svg, "look here"));

    Plot scatter = line;
    scatter.kind = PlotKind::Scatter;
    svg = renderer.render(scatter, ann);
    CHECK(!contains(svg, "<polyline"));
    CHECK(contains(svg, "fill-opacity"));
    CHECK(!contains(svg, "stroke-dasharray"));

    scatter.showBestFit = true;
    svg = renderer.render(scatter, ann);
    CHECK(contains(svg, "stroke-dasharray=\"6,4\""));
    CHECK(contains(svg, "stroke=\"#c0392b\""));

    Plot verticalScatter;
    verticalScatter.kind = PlotKind::Scatter;
    verticalScatter.showBestFit = true;
    verticalScatter.points = {{2, 1}, {2, 3}};
    svg = renderer.render(verticalScatter, ann);
    CHECK(!contains(svg, "stroke-dasharray"));

    CountPlot counts;
    counts.count({"a", "b", "a"});
    svg = renderer.render(counts.toPlot(), ann);
    CHECK(contains(svg, "<rect x="));
    CHECK(contains(svg, ">a</text>"));  // category label drawn

    Plot single;
    single.points = {{2, 2}};  // zero range must not crash
    CHECK(!renderer.render(single, ann).empty());

    CHECK_THROWS(renderer.render(Plot{}, ann));  // empty line plot
    Plot emptyBars;
    emptyBars.kind = PlotKind::Bar;
    CHECK_THROWS(renderer.render(emptyBars, ann));
    Plot nan;
    nan.points = {{1.0, std::nan("")}};
    CHECK_THROWS(renderer.render(nan, ann));
    Plot badBar;
    badBar.kind = PlotKind::Bar;
    badBar.bars = {{2.0, 1.0, 3.0, ""}};
    CHECK_THROWS(renderer.render(badBar, ann));
}

static void testExporter()
{
    ImageExporter exporter;
    const std::string svg = "<svg xmlns=\"http://www.w3.org/2000/svg\"></svg>\n";
    const std::string path = "dataviz_test_out/nested/test.svg";

    CHECK(exporter.exportSVG(svg, path));  // also creates the folders
    std::ifstream in(path);
    std::stringstream buffer;
    buffer << in.rdbuf();
    CHECK(buffer.str() == svg);

    CHECK_THROWS(exporter.exportSVG("not svg", path));
    CHECK_THROWS(exporter.exportSVG(svg, "image.png"));

    std::remove(path.c_str());
    std::remove("dataviz_test_out/nested");
    std::remove("dataviz_test_out");
}

int main()
{
    testAxis();
    testAnnotations();
    testRenderer();
    testExporter();
    return reportResults("test_rendering");
}