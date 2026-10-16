// Stages 1-3: DataLoader, DataProcessor, Statistics
#include <cstdio>
#include <fstream>
#include <limits>

#include "dataviz.hpp"
#include "test_util.hpp"

using namespace dataviz;

static void testLoader()
{
    const std::string path = "dataviz_test_tmp.csv";
    {
        std::ofstream out(path);
        out << "name,score\r\n"
            << "\"Smith, J\",10\r\n"
            << "\r\n"                  // blank line is skipped
            << "Lee,20.5\r\n"
            << "Short\r\n";            // short row is padded
    }

    DataLoader loader;
    CHECK(loader.loadCSV(path));
    CHECK(loader.getHeaders().size() == 2);
    CHECK(loader.getRowCount() == 3);
    CHECK(loader.getColumn("name")[0] == "Smith, J");  // quoted comma survives
    CHECK(loader.getColumn("score")[1] == "20.5");
    CHECK(loader.getColumn("score")[2] == "");          // padded
    CHECK_THROWS(loader.getColumn("missing"));

    CHECK(!loader.loadCSV("no_such_file.csv"));
    CHECK(loader.getRowCount() == 0);                   // failed load clears state

    std::remove(path.c_str());
}

static void testProcessor()
{
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();

    const std::vector<double> parsed =
        DataProcessor::toNumbers({"1.5", " 2 ", "", "abc", "3x", "-4e1"});
    CHECK(parsed.size() == 6);
    CHECK_NEAR(parsed[0], 1.5);
    CHECK_NEAR(parsed[1], 2.0);
    CHECK(std::isnan(parsed[2]));
    CHECK(std::isnan(parsed[3]));
    CHECK(std::isnan(parsed[4]));
    CHECK_NEAR(parsed[5], -40.0);

    const std::vector<double> bad = {1.0, nan, 3.0, inf};
    CHECK(DataProcessor::isEmpty({}));
    CHECK(!DataProcessor::validateData(bad));
    CHECK(DataProcessor::validateData({1.0, 2.0}));

    const std::vector<double> cleaned = DataProcessor::cleanData(bad);
    CHECK(cleaned.size() == 2);
    CHECK(bad.size() == 4);  // input untouched

    CHECK_NEAR(DataProcessor::findMinimum({4.0, -2.0, 9.0}), -2.0);
    CHECK_NEAR(DataProcessor::findMaximum({4.0, -2.0, 9.0}), 9.0);
    CHECK_THROWS(DataProcessor::findMinimum({}));
    CHECK_THROWS(DataProcessor::findMaximum({}));

    const std::vector<double> n = DataProcessor::normalizeData({10.0, 20.0, 30.0});
    CHECK_NEAR(n[0], 0.0);
    CHECK_NEAR(n[1], 0.5);
    CHECK_NEAR(n[2], 1.0);
    CHECK_NEAR(DataProcessor::normalizeData({5.0, 5.0})[1], 0.0);
    CHECK_THROWS(DataProcessor::normalizeData({}));
    CHECK_THROWS(DataProcessor::normalizeData(bad));
}

static void testStatistics()
{
    const std::vector<double> d = {2, 4, 4, 4, 5, 5, 7, 9};
    CHECK_NEAR(Statistics::mean(d), 5.0);
    CHECK_NEAR(Statistics::median(d), 4.5);
    CHECK_NEAR(Statistics::median({3, 1, 2}), 2.0);
    CHECK_NEAR(Statistics::variance(d, false), 4.0);
    CHECK_NEAR(Statistics::standardDeviation(d, false), 2.0);
    CHECK_NEAR(Statistics::variance(d, true), 32.0 / 7.0);
    CHECK_NEAR(Statistics::min(d), 2.0);
    CHECK_NEAR(Statistics::max(d), 9.0);

    const Statistics::Summary s = Statistics::summarize(d);
    CHECK(s.count == 8);
    CHECK_NEAR(s.mean, 5.0);

    CHECK_THROWS(Statistics::mean({}));
    CHECK_THROWS(Statistics::median({}));
    CHECK_THROWS(Statistics::variance({1.0}, true));
    CHECK_NEAR(Statistics::variance({1.0}, false), 0.0);
    CHECK_NEAR(Statistics::silvermanBandwidth({3.0, 3.0, 3.0}), 1.0);
    CHECK(Statistics::silvermanBandwidth(d) > 0.0);
}

int main()
{
    testLoader();
    testProcessor();
    testStatistics();
    return reportResults("test_data");
}