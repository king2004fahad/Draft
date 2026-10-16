#ifndef DATAVIZ_ANNOTATIONS_HPP
#define DATAVIZ_ANNOTATIONS_HPP

#include <string>
#include <vector>

namespace dataviz {

// A text note placed at a position given in DATA coordinates.
struct Note {
    double x;
    double y;
    std::string text;
};

// Stage 6: everything written on a plot (title, axis labels, notes).
class Annotations {
public:
    void setTitle(const std::string& title);
    void setXLabel(const std::string& label);
    void setYLabel(const std::string& label);

    // Throws std::invalid_argument if x or y is NaN/infinite.
    void addNote(double x, double y, const std::string& text);
    void clearNotes();

    const std::string& title() const;
    const std::string& xLabel() const;
    const std::string& yLabel() const;
    const std::vector<Note>& notes() const;

private:
    std::string title_;
    std::string xLabel_;
    std::string yLabel_;
    std::vector<Note> notes_;
};

}  // namespace dataviz

#endif  // DATAVIZ_ANNOTATIONS_HPP