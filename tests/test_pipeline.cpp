// The whole pipeline, end to end, on a small CSV written by the test.
#include <cstdio>
#include <fstream>
#include <sstream>

#include "dataviz.hpp"
#include "test_util.hpp"

using namespace dataviz;

int main()
{
    const std::string csv = "dataviz_pipeline_tmp.csv";
    const std::string svgPath = "dataviz_pipeline_tmp.svg";
    {
        std::ofstream out(csv);
        out << "group,value\n"
            << "A,1\nA,2\nB,3\nB,4\nB,oops\nC,5\nC,\nA,6\n";  // two bad cells
    }

    // 1. DataLoader
    DataLoader loader;
    CHECK(loader.loadCSV(csv));

    // 2. DataProcessor
    const std::vector<double> values =
        DataProcessor::cleanData(DataProcessor::toNumbers(loader.getColumn("value")));
    CHECK(values.size() == 6);  // the two bad cells were removed

    // 3. Statistics
    const Statistics::Summary stats = Statistics::summarize(values);
    CHECK_NEAR(stats.mean, 3.5);

    // 4. Plot data
    const Histogram histogram(values, 3);
    Annotations annotations;
    annotations.setTitle("Pipeline test");
    annotations.setXLabel("value");
    annotations.setYLabel("count");

    // 5-6. Axis is used inside Renderer; Annotations carries the text
    annotations.addNote(stats.mean, static_cast<double>(histogram.maxCount()), "mean");

    // 7. Renderer
    Renderer renderer;
    const std::string svg = renderer.render(histogram.toPlot(), annotations);
    CHECK(svg.find("<rect") != std::string::npos);

    // 8. ImageExporter
    ImageExporter exporter;
    CHECK(exporter.exportSVG(svg, svgPath));

    std::ifstream in(svgPath);
    CHECK(in.good());

    std::remove(csv.c_str());
    std::remove(svgPath.c_str());
    return reportResults("test_pipeline");
}