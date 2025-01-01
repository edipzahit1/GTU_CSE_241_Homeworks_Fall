#include "Spreadsheet.h"
#include "FormulaParser.h"
#include "Cell.h"
#include "myvec.h"
#include "myset.h"
#include <cctype>
#include <iostream>
#include <iomanip>
#include <unordered_set>
#include <memory>
#include <stdexcept>
#include <string>

const int COLUMN_WIDTH = 12;    // Width of each column in characters
const int ROW_HEADER_WIDTH = 4; // Width for row headers

Spreadsheet::Spreadsheet(int rows, int cols)
{
    for (int i = 0; i < rows; ++i)
    {
        cells.push_back(spc::myvec<std::unique_ptr<Cell>>(cols)); // Initialize row with column capacity
        for (int j = 0; j < cols; ++j)
        {
            cells[i].push_back(std::make_unique<StringValueCell>(i, j, "")); // Initialize each cell
        }
    }
    parser = std::make_shared<FormulaParser>(this);
}

Cell *Spreadsheet::getCell(int r, int c) const
{
    if (r >= getRowCount() || c >= getColCount())
        throw std::out_of_range("Cell out of range.");
    return cells[r][c].get();
}

void Spreadsheet::setCell(int r, int c, std::unique_ptr<Cell> cell)
{
    if (r >= getRowCount() || c >= getColCount())
        throw std::out_of_range("Cell out of range.");
    cells[r][c] = std::move(cell);
}

void Spreadsheet::enterData(int r, int c, std::string &input)
{
    if (!input.empty() && input[0] == '=')
    {
        try
        {
            spc::myvec<std::pair<int, int>> dependentCells;
            double result = parser.get()->parseAndEvaluate(input, {r, c}, dependentCells);

            setCell(r, c, std::make_unique<FormulaCell>(r, c, input));

            auto formulaCell = dynamic_cast<FormulaCell *>(getCell(r, c));
            formulaCell->setCalculatedValue(result);
            for (auto &pair : dependentCells)
                formulaCell->addDependentCell(pair);
        }
        catch (const std::exception &e)
        {
            setCell(r, c, std::make_unique<StringValueCell>(r, c, input));
        }
    }
    else
    {
        try
        {
            int intValue = std::stoi(input);
            setCell(r, c, std::make_unique<IntValueCell>(r, c, intValue));
        }
        catch (std::invalid_argument &)
        {
            try
            {
                double doubleValue = std::stod(input);
                setCell(r, c, std::make_unique<DoubleValueCell>(r, c, doubleValue));
            }
            catch (std::invalid_argument &)
            {
                setCell(r, c, std::make_unique<StringValueCell>(r, c, input));
            }
        }
    }
}

void Spreadsheet::enterData(int r, int c, std::string &&input)
{
    if (!input.empty() && input[0] == '=')
    {
        try
        {
            spc::myvec<std::pair<int, int>> dependentCells;
            double result = parser.get()->parseAndEvaluate(input, {r, c}, dependentCells);

            setCell(r, c, std::make_unique<FormulaCell>(r, c, input));

            auto formulaCell = dynamic_cast<FormulaCell *>(getCell(r, c));
            formulaCell->setCalculatedValue(result);
            for (auto &pair : dependentCells)
                formulaCell->addDependentCell(pair);
        }
        catch (const std::exception &e)
        {
            setCell(r, c, std::make_unique<StringValueCell>(r, c, input));
        }
    }
    else
    {
        try
        {
            int intValue = std::stoi(input);
            setCell(r, c, std::make_unique<IntValueCell>(r, c, intValue));
        }
        catch (std::invalid_argument &)
        {
            try
            {
                double doubleValue = std::stod(input);
                setCell(r, c, std::make_unique<DoubleValueCell>(r, c, doubleValue));
            }
            catch (std::invalid_argument &)
            {
                setCell(r, c, std::make_unique<StringValueCell>(r, c, input));
            }
        }
    }
}

spc::myvec<Cell *> Spreadsheet::getCellsInRange(std::pair<int, int> startPos, std::pair<int, int> endPos)
{
    spc::myvec<Cell *> cellsInRange;

    int startRow = startPos.first;
    int startCol = startPos.second;
    int endRow = endPos.first;
    int endCol = endPos.second;

    if (startRow > endRow)
        std::swap(startRow, endRow);
    if (startCol > endCol)
        std::swap(startCol, endCol);

    for (int i = startRow; i <= endRow; ++i)
    {
        int colStart = (i == startRow) ? startCol : 0;
        int colEnd = (i == endRow) ? endCol : (getColCount() - 1);

        for (int j = colStart; j <= colEnd; ++j)
        {
            Cell *cell = getCell(i, j);
            if (cell)
            {
                cellsInRange.push_back(cell);
            }
        }
    }
    return cellsInRange;
}

std::string Spreadsheet::getColumnLabel(int columnIndex) const
{
    std::string label;
    while (columnIndex > 0)
    {
        columnIndex--;
        label = static_cast<char>('A' + (columnIndex % 26)) + label;
        columnIndex /= 26;
    }
    return label;
}

std::string Spreadsheet::getCellLabel(int r, int c) const
{
    std::string colLabel = getColumnLabel(c);
    return colLabel + std::to_string(r);
}

void Spreadsheet::moveCell(int &currentRow, int &currentCol, const char dir)
{
    if (dir == (char)1) // Up arrow
    {
        currentRow = std::max(0, currentRow - 1); // Move up, ensuring we stay within bounds
    }
    else if (dir == (char)2) // Down arrow
    {
        if (currentRow + 1 >= getRowCount())
        {
            if (getRowCount() < MAX_ROWS)
            {
                expand(currentRow + 2, getColCount()); // Expand by 1 row beyond current position
                currentRow++;
            }
            else
                currentRow = MAX_ROWS - 1;
        }
        else
        {
            currentRow++;
        }
    }
    else if (dir == (char)4) // Left arrow
    {
        currentCol = std::max(0, currentCol - 1); // Move left, ensuring we stay within bounds
    }
    else if (dir == (char)3) // Right arrow
    {
        if (currentCol + 1 >= getColCount())
        {
            if (getColCount() < MAX_COLS)
            {
                expand(getRowCount(), currentCol + 2); // Expand by 1 column beyond current position
                currentCol++;
            }
            else
                currentCol = MAX_COLS - 1;
        }
        else
        {
            currentCol++;
        }
    }
}

void Spreadsheet::displayScreen(int currentRow, int currentCol, AnsiTerminal& terminal, std::string inputLine)
{
    terminal.clearScreen();

    // Get the formula from the current cell if it is a FormulaCell
    std::string cellFormula = "";
    if (auto formulaCell = dynamic_cast<FormulaCell *>(cells[currentRow][currentCol].get()))
    {
        cellFormula = formulaCell->getFormula();
    }

    std::string letterRep = cells[currentRow][currentCol]->getLetterRepresentation();
    terminal.printAt(1, 1, letterRep + " | Formula: " + cellFormula);

    std::ostringstream headerLine;
    headerLine << std::setw(ROW_HEADER_WIDTH) << " ";
    for (int j = 0; j < getColCount(); ++j)
    {
        std::string colLabel = getColumnLabel(j + 1);
        headerLine << "|" << std::setw(COLUMN_WIDTH - 1) << colLabel;
    }
    headerLine << "|";
    terminal.printAt(2, 1, inputLine);
    terminal.printAt(3, 1, headerLine.str());

    for (int i = 0; i < getRowCount(); ++i)
    {
        std::ostringstream rowStream;
        rowStream << std::setw(ROW_HEADER_WIDTH - 1) << (i + 1) << "|";

        for (int j = 0; j < getColCount(); ++j)
        {
            std::string cellText = cells[i][j]->getValueAsString();
            cellText = formatCellText(cellText, COLUMN_WIDTH);

            if (i == currentRow && j == currentCol)
                rowStream << "|" << "\033[7m" << cellText << "\033[0m";
            else
                rowStream << "|" << cellText;
        }
        rowStream << "|";
        terminal.printAt(4 + i, 1, rowStream.str());
    }
}

std::string Spreadsheet::formatCellText(const std::string &cellText, int width)
{
    if (static_cast<int>(cellText.length()) >= width)
    {
        return cellText.substr(0, width - 2) + ">";
    }
    else
    {
        std::ostringstream oss;
        oss << std::setw(width - 1) << std::right << cellText;
        return oss.str();
    }
}

void Spreadsheet::run()
{
    AnsiTerminal terminal;
    int currentRow = 0, currentCol = 0;
    std::pair<int, int> oldLoc;
    std::string input;
    while (true)
    {
        input = cells[currentRow][currentCol]->getValueAsString();
        displayScreen(currentRow, currentCol, terminal, input);

        char command = terminal.getSpecialKey();

        if (command == 'q')
        {
            std::cout << "Exiting spreadsheet...\n";
            return;
        }

        if (terminal.isArrowKey(command))
        {
            moveCell(currentRow, currentCol, command);
            continue;
        }

        if (!input.empty() && command == 127)
        {
            input.pop_back();
        }

        if (isprint(command) || command == '=' || command == 127)
        {
            if (command != 127)
                input.push_back(command);

            bool editing = true;
            oldLoc = {currentRow, currentCol};

            while (editing)
            {
                displayScreen(currentRow, currentCol, terminal, input);

                char editCommand = terminal.getSpecialKey();

                if (editCommand == '\n')
                {
                    editing = false;
                }
                else if (editCommand == 127 && !input.empty())
                {
                    input.pop_back();
                }
                else if (terminal.isArrowKey(editCommand))
                {
                    editing = false;
                    moveCell(currentRow, currentCol, editCommand);
                }
                else if (isprint(editCommand))
                {
                    input.push_back(editCommand);
                }
            }

            enterData(oldLoc.first, oldLoc.second, input);

            parser.get()->autoCalculate(oldLoc);

            input.clear();
        }
    }
}

void Spreadsheet::expand(int newRowCount, int newColCount)
{
    int currentRowCount = getRowCount();
    int currentColCount = getColCount();

    if (newRowCount > currentRowCount)
    {
        for (int i = currentRowCount; i < newRowCount; ++i)
        {
            cells.push_back(spc::myvec<std::unique_ptr<Cell>>(currentColCount));
            for (int j = 0; j < currentColCount; ++j)
            {
                cells[i].push_back(std::make_unique<StringValueCell>(i, j, ""));
            }
        }
    }

    if (newColCount > currentColCount)
    {
        for (int i = 0; i < getRowCount(); ++i)
        {
            for (int j = currentColCount; j < newColCount; ++j)
            {
                cells[i].push_back(std::make_unique<StringValueCell>(i, j, ""));
            }
        }
    }
}
