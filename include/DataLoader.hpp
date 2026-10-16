#ifndef DATAVIZ_DATALOADER_HPP
#define DATAVIZ_DATALOADER_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace dataviz {

// Stage 1: reads a CSV file into memory as text.
// Conversion to numbers is the job of DataProcessor.
class DataLoader {
public:
    // Returns false if the file cannot be opened or has no header line.
    // Blank lines are skipped. Rows are padded/cut to the header width.
    bool loadCSV(const std::string& filename);

    const std::vector<std::string>& getHeaders() const;
    const std::vector<std::vector<std::string>>& getRows() const;
    std::size_t getRowCount() const;

    // Index of a column. Throws std::invalid_argument if it does not exist.
    std::size_t columnIndex(const std::string& name) const;

    // All values of one column. Throws std::invalid_argument if unknown.
    std::vector<std::string> getColumn(const std::string& name) const;

private:
    std::vector<std::string> headers_;
    std::vector<std::vector<std::string>> rows_;
};

}  // namespace dataviz

#endif  // DATAVIZ_DATALOADER_HPP