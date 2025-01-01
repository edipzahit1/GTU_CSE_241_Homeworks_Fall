#include <string>
#include <fstream>
#include <stdexcept>
#include "FileHandler.h"
#include "Cell.h"

void FileHandler::saveToFile(const std::string &filename, const Spreadsheet &sheet)
{
    std::ofstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("File could not open.");

    for (int i = 0; i < sheet.getRowCount(); ++i)
    {
        for (int j = 0; j < sheet.getColCount(); ++j)
        {
            Cell *cell = sheet.getCell(i, j);
            if (auto *formulaCell = dynamic_cast<FormulaCell *>(cell))
                file << formulaCell->getFormula();
            else
                file << cell->getValueAsString();
            if (j < sheet.getColCount() - 1)
            {
                file << ",";
            }
        }
        file << "\n";
    }
    file.close();
}

void FileHandler::loadFromFile(const std::string &filename, Spreadsheet &spreadsheet)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("File error");

    std::string line;
    int row = 0;

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string cellData;
        int col = 0;

        while (std::getline(ss, cellData, ','))
        {
            if (row >= spreadsheet.getRowCount() && row < Spreadsheet::MAX_ROWS)
                spreadsheet.expand(row + 1, spreadsheet.getColCount());

            if (col >= spreadsheet.getColCount() && col < Spreadsheet::MAX_COLS)
                spreadsheet.expand(spreadsheet.getRowCount(), col + 1);

            if (row < Spreadsheet::MAX_ROWS && col < Spreadsheet::MAX_COLS)
                spreadsheet.enterData(row, col, cellData);
            ++col;
        }
        ++row;
    }
    file.close();
}
