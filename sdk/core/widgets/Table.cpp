#include "Table.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Table::Table(Widget *parent)
    : Widget(tools::LvObjFactory::createLvglTable, parent)
{
    setFocusable(true);
}

void Table::setCellValue(uint32_t row, uint32_t col, const std::string &text)
{
    lv_table_set_cell_value(lvObj(), row, col, text.c_str());
}

std::string Table::cellValue(uint32_t row, uint32_t col) const
{
    const char *v = lv_table_get_cell_value(lvObj(), row, col);
    return v ? std::string(v) : std::string();
}

void Table::setRowCount(uint32_t rowCount)
{
    lv_table_set_row_count(lvObj(), rowCount);
}

uint32_t Table::rowCount() const
{
    return lv_table_get_row_count(lvObj());
}

void Table::setColumnCount(uint32_t colCount)
{
    lv_table_set_column_count(lvObj(), colCount);
}

uint32_t Table::columnCount() const
{
    return lv_table_get_column_count(lvObj());
}

void Table::setColumnWidth(uint32_t col, int32_t width)
{
    lv_table_set_column_width(lvObj(), col, width);
}

int32_t Table::columnWidth(uint32_t col) const
{
    return lv_table_get_column_width(lvObj(), col);
}

void Table::setCellMergeRight(uint32_t row, uint32_t col, bool enabled)
{
    if (enabled) {
        lv_table_set_cell_ctrl(lvObj(), row, col, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    } else {
        lv_table_clear_cell_ctrl(lvObj(), row, col, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    }
}

void Table::setSelectedCell(uint32_t row, uint32_t col)
{
    lv_table_set_selected_cell(lvObj(), row, col);
}

bool Table::getSelectedCell(uint32_t &row, uint32_t &col) const
{
    lv_table_get_selected_cell(lvObj(), &row, &col);
    return true; // LVGL always returns valid row/col
}

} // namespace lvglpp::widgets
