# dataviz

`dataviz` is a small C++17 data-visualization project. It loads tabular data
from CSV, processes numeric columns, calculates summary statistics, creates
plot data, renders charts as SVG, and writes the SVG files to disk.

## Features

- Load CSV files and retrieve columns by header name.
- Convert values to numbers and clean invalid or non-finite data.
- Calculate mean, median, variance, standard deviation, minimum, and maximum.
- Create category-count plots, histograms, kernel density estimates (KDE), line
  plots, and scatter plots.
- Add titles, axis labels, and notes to charts.
- Optionally draw an ordinary least-squares best-fit line on scatter plots.
- Export charts as SVG files without third-party dependencies.

## Requirements

- CMake 3.14 or newer
- A C++17 compiler (for example, GCC/MinGW, Clang, or MSVC)

## Build

From the project root, configure and build with:

```powershell
cmake -S . -B build
cmake --build build
```

The tests and example program are enabled by default. To run the tests:

```powershell
ctest --test-dir build --output-on-failure
```

## Run the example

The example reads `data/input/sample.csv`, prints summary statistics, and
writes four SVG charts to `data/output/`.

On Windows with a single-configuration generator such as MinGW Makefiles:

```powershell
.\build\example_pipeline.exe
```

For a multi-configuration generator such as Visual Studio, build and run the
Debug executable:

```powershell
cmake --build build --config Debug
.\build\Debug\example_pipeline.exe
```

The example also accepts an input CSV path and output directory:

```powershell
.\build\example_pipeline.exe "data\input\sample.csv" "data\output"
```

Open the generated `.svg` files in a browser or an SVG-compatible image viewer:

- `data/output/category_counts.svg` — number of rows in each category.
- `data/output/score_histogram.svg` — distribution of valid scores, with the
  mean marked.
- `data/output/score_kde.svg` — estimated score density.
- `data/output/height_vs_score.svg` — height plotted against score, with a
  dashed best-fit line.

The sample CSV includes some missing or non-numeric score values to demonstrate
data cleaning. The pipeline excludes invalid numeric values from statistics
and plots that require them.

## Use the library

Link an application to the `dataviz` CMake target and include the convenience
header:

```cpp
#include "dataviz.hpp"

int main()
{
    dataviz::CountPlot counts;
    counts.count({"Engineering", "Science", "Engineering"});

    dataviz::Annotations annotations;
    annotations.setTitle("Rows per category");
    annotations.setXLabel("Category");
    annotations.setYLabel("Count");

    const dataviz::Renderer renderer;
    const std::string svg = renderer.render(counts.toPlot(), annotations);

    const dataviz::ImageExporter exporter;
    return exporter.exportSVG(svg, "category_counts.svg") ? 0 : 1;
}
```

To enable a best-fit line, set `showBestFit` on a scatter `Plot` before
rendering it:

```cpp
dataviz::Plot scatter;
scatter.kind = dataviz::PlotKind::Scatter;
scatter.points = {{1.0, 2.0}, {2.0, 2.5}, {3.0, 4.0}};
scatter.showBestFit = true;
```

The best-fit line is disabled by default. It is not drawn when the data has
fewer than two points or all points have the same x-coordinate.

## Project layout

```text
include/       Public headers
src/           Library implementation
examples/      Example CSV-to-SVG pipeline
tests/         Unit and pipeline tests
data/input/    Sample input data
data/output/   Generated SVG charts
```
