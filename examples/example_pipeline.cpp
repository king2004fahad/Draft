// Runs the whole dataviz pipeline on data/input/sample.csv and writes four
// SVG files to data/output/.
//
//   example_pipeline [input.csv] [output_folder]

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "dataviz.hpp"

#ifndef DATAVIZ_SOURCE_DIR
#define DATAVIZ_SOURCE_DIR "."
#endif

using namespace dataviz;


static void save(const Plot& plot, const Annotations& annotations, const std::string& file)
{
    const Renderer renderer;
    const ImageExporter exporter;
    const std::string svg = renderer.render(plot, annotations);
    if (!exporter.exportSVG(svg, file))
    {
        throw std::runtime_error("could not write " + file);
    }
    std::cout << "wrote " << file << "\n";
}

int main(int argc, char** argv)
{
    const std::string root = DATAVIZ_SOURCE_DIR;
    const std::string input = argc > 1 ? argv[1] : root + "/data/input/sample.csv";
    const std::string outDir = argc > 2 ? argv[2] : root + "/data/output";

    try
    {
        // Stage 1: load
        DataLoader loader;
        if (!loader.loadCSV(input))
        {
            std::cerr << "cannot read " << input << "\n";
            return 1;
        }
        std::cout << "loaded " << loader.getRowCount() << " rows\n";

        // Stage 2: process (blank and non-numeric scores are dropped)
        const std::vector<double> scores =
            DataProcessor::cleanData(DataProcessor::toNumbers(loader.getColumn("score")));
        std::cout << "valid scores: " << scores.size() << "\n";

        // Stage 3: statistics
        const Statistics::Summary s = Statistics::summarize(scores);
        std::cout << "mean " << s.mean << ", median " << s.median
                  << ", std " << s.standardDeviation
                  << ", range " << s.min << " to " << s.max << "\n";

        // Stages 4-8 for a count plot
        CountPlot counts;
        counts.count(loader.getColumn("category"));
        Annotations a1;
        a1.setTitle("Students per category");
        a1.setXLabel("Category");
        a1.setYLabel("Count");
        save(counts.toPlot(), a1, outDir + "/category_counts.svg");

        // Histogram, with the mean marked as a note
        const Histogram histogram(scores, 8);
        Annotations a2;
        a2.setTitle("Distribution of scores");
        a2.setXLabel("Score");
        a2.setYLabel("Count");
        a2.addNote(s.mean, static_cast<double>(histogram.maxCount()), "mean");
        save(histogram.toPlot(), a2, outDir + "/score_histogram.svg");

        // KDE curve
        const KDEPlot kde(scores);
        Annotations a3;
        a3.setTitle("Score density (KDE)");
        a3.setXLabel("Score");
        a3.setYLabel("Density");
        save(kde.toPlot(), a3, outDir + "/score_kde.svg");

        // Scatter plot of two columns. Keep only rows where both are valid.
        const std::vector<double> sc = DataProcessor::toNumbers(loader.getColumn("score"));
        const std::vector<double> ht = DataProcessor::toNumbers(loader.getColumn("height"));
        Plot scatter;
        scatter.kind = PlotKind::Scatter;
        scatter.showBestFit = true;
        for (std::size_t i = 0; i < sc.size(); ++i)
        {
            if (std::isfinite(sc[i]) && std::isfinite(ht[i]))
            {
                scatter.points.push_back({sc[i], ht[i]});
            }
        }
        Annotations a4;
        a4.setTitle("Height against score");
        a4.setXLabel("Score");
        a4.setYLabel("Height (cm)");
        save(scatter, a4, outDir + "/height_vs_score.svg");
    }
    catch (const std::exception& e)
    {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}