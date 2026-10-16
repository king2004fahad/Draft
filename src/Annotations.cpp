#include "Annotations.hpp"

#include <cmath>
#include <stdexcept>

namespace dataviz {

void Annotations::setTitle(const std::string& title) { title_ = title; }
void Annotations::setXLabel(const std::string& label) { xLabel_ = label; }
void Annotations::setYLabel(const std::string& label) { yLabel_ = label; }

void Annotations::addNote(double x, double y, const std::string& text)
{
    if (!std::isfinite(x) || !std::isfinite(y))
    {
        throw std::invalid_argument("Annotations::addNote: position must be finite");
    }
    notes_.push_back({x, y, text});
}

void Annotations::clearNotes() { notes_.clear(); }

const std::string& Annotations::title() const { return title_; }
const std::string& Annotations::xLabel() const { return xLabel_; }
const std::string& Annotations::yLabel() const { return yLabel_; }
const std::vector<Note>& Annotations::notes() const { return notes_; }

}  // namespace dataviz