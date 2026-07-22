#ifndef LVGLPP_TABLE_H
#define LVGLPP_TABLE_H

#include "BaseItem.h"
#include <string>

namespace lvglpp::widgets {

class Table : public BaseItem
{
public:
    explicit Table(BaseItem *parent = nullptr);

    /// Set the cell value at [row, col]
    void setCellValue(uint32_t row, uint32_t col, const std::string &text);

    /// Get the cell value at [row, col]
    std::string cellValue(uint32_t row, uint32_t col) const;

    /// Set row count
    void setRowCount(uint32_t rowCount);

    /// Get row count
    uint32_t rowCount() const;

    /// Set column count
    void setColumnCount(uint32_t colCount);

    /// Get column count
    uint32_t columnCount() const;

    /// Set column width
    void setColumnWidth(uint32_t col, int32_t width);

    /// Get column width
    int32_t columnWidth(uint32_t col) const;

    /// Add merge-right ctrl to a cell (to span across columns)
    void setCellMergeRight(uint32_t row, uint32_t col, bool enabled);

    /// Set selected cell
    void setSelectedCell(uint32_t row, uint32_t col);

    /// Get selected cell (returns false if no selection)
    bool getSelectedCell(uint32_t &row, uint32_t &col) const;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_TABLE_H
