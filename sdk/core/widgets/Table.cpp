#include "Table.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Table::Table(BaseItem *parent)
    : BaseItem(tools::LvObjFactory::createLvglTable, parent)
{}

void Table::setCellValue(uint32_t row, uint32_t col, const std::string &text)
{
    lv_table_set_cell_value(getLvglItem(), row, col, text.c_str());
}

std::string Table::cellValue(uint32_t row, uint32_t col) const
{
    const char *v = lv_table_get_cell_value(getLvglItem(), row, col);
    return v ? std::string(v) : std::string();
}

void Table::setRowCount(uint32_t rowCount)
{
    lv_table_set_row_count(getLvglItem(), rowCount);
}

uint32_t Table::rowCount() const
{
    return lv_table_get_row_count(getLvglItem());
}

void Table::setColumnCount(uint32_t colCount)
{
    lv_table_set_column_count(getLvglItem(), colCount);
}

uint32_t Table::columnCount() const
{
    return lv_table_get_column_count(getLvglItem());
}

void Table::setColumnWidth(uint32_t col, int32_t width)
{
    lv_table_set_column_width(getLvglItem(), col, width);
}

int32_t Table::columnWidth(uint32_t col) const
{
    return lv_table_get_column_width(getLvglItem(), col);
}

void Table::setCellMergeRight(uint32_t row, uint32_t col, bool enabled)
{
    if (enabled) {
        lv_table_set_cell_ctrl(getLvglItem(), row, col, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    } else {
        lv_table_clear_cell_ctrl(getLvglItem(), row, col, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    }
}

void Table::setSelectedCell(uint32_t row, uint32_t col)
{
    lv_table_set_selected_cell(getLvglItem(), row, col);
}

bool Table::getSelectedCell(uint32_t &row, uint32_t &col) const
{
    lv_table_get_selected_cell(getLvglItem(), &row, &col);
    return true; // LVGL always returns valid row/col
}

} // namespace lvglpp::widgets
