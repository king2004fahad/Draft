#include "DataLoader.hpp"

#include <fstream>
#include <stdexcept>

namespace dataviz {

namespace {

// Splits one CSV line into fields. Supports "quoted, fields" and "" escapes.
std::vector<std::string> parseLine(const std::string& line)
{
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i)
    {
        const char c = line[i];
        if (inQuotes)
        {
            if (c == '"')
            {
                if (i + 1 < line.size() && line[i + 1] == '"')
                {
                    field += '"';
                    ++i;
                }
                else
                {
                    inQuotes = false;
                }
            }
            else
            {
                field += c;
            }
        }
        else if (c == '"')
        {
            inQuotes = true;
        }
        else if (c == ',')
        {
            fields.push_back(field);
            field.clear();
        }
        else
        {
            field += c;
        }
    }
    fields.push_back(field);
    return fields;
}

bool isBlank(const std::string& text)
{
    for (char c : text)
    {
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
        {
            return false;
        }
    }
    return true;
}

}  // namespace

bool DataLoader::loadCSV(const std::string& filename)
{
    headers_.clear();
    rows_.clear();

    std::ifstream file(filename);
    if (!file.is_open())
    {
        return false;
    }

    std::string line;
    bool firstLine = true;

    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (isBlank(line))
        {
            continue;
        }

        std::vector<std::string> fields = parseLine(line);

        if (firstLine)
        {
            // Remove a UTF-8 byte-order mark if the file starts with one.
            if (fields[0].compare(0, 3, "\xEF\xBB\xBF") == 0)
            {
                fields[0].erase(0, 3);
            }
            headers_ = fields;
            firstLine = false;
        }
        else
        {
            fields.resize(headers_.size());
            rows_.push_back(fields);
        }
    }

    return !headers_.empty();
}

const std::vector<std::string>& DataLoader::getHeaders() const { return headers_; }

const std::vector<std::vector<std::string>>& DataLoader::getRows() const { return rows_; }

std::size_t DataLoader::getRowCount() const { return rows_.size(); }

std::size_t DataLoader::columnIndex(const std::string& name) const
{
    for (std::size_t i = 0; i < headers_.size(); ++i)
    {
        if (headers_[i] == name)
        {
            return i;
        }
    }
    throw std::invalid_argument("DataLoader: no column named '" + name + "'");
}

std::vector<std::string> DataLoader::getColumn(const std::string& name) const
{
    const std::size_t index = columnIndex(name);
    std::vector<std::string> column;
    column.reserve(rows_.size());
    for (const auto& row : rows_)
    {
        column.push_back(row[index]);
    }
    return column;
}

}  // namespace dataviz