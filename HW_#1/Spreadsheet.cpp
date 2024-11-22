#include "Spreadsheet.h"
#include "FormulaParser.h"
#include "Cell.h"
#include <vector>
#include <cctype>
#include <iostream>
#include <iomanip>

using namespace std;

const int COLUMN_WIDTH = 12;    // Width of each column in characters
const int ROW_HEADER_WIDTH = 4; // Width for row headers

Spreadsheet::Spreadsheet(int r, int c)
{
    if (r < 1 || c < 1)
        throw invalid_argument("Invalid size for spreadsheet");
    cells = vector<vector<Cell>>(r, vector<Cell>(c, Cell()));

    for (int i = 0; i < r; ++i)
    {
        for (int j = 0; j < c; ++j)
        {
            cells[i][j] = Cell(i, j);
            cells[i][j].setLetterRepresentation(cells[i][j].getLetterTransformation());
        }
    }
}

string Spreadsheet::getColumnLabel(int columnIndex) const
{
    string label;
    while (columnIndex > 0)
    {
        columnIndex--;
        label = static_cast<char>('A' + (columnIndex % 26)) + label;
        columnIndex /= 26;
    }
    return label;
}

string Spreadsheet::getCellLabel(int r, int c) const
{
    string colLabel = getColumnLabel(c);
    return colLabel + to_string(r);
}

//----------------------------------------------------------//
// Precisions and visual interface adjustments made by ChatGPT
string formatCellText(const string &cellText, int width)
{
    if (static_cast<int>(cellText.length()) >= width)
    {
        return cellText.substr(0, width - 2) + ">"; // Truncate and add '>' at the end to indicate overflow
    }
    else
    {
        ostringstream oss;
        oss << setw(width - 1) << right << cellText;
        return oss.str();
    }
}

void Spreadsheet::displayScreen(int currentRow, int currentCol, string inputLine)
{
    terminal.clearScreen();
    string cellFormula = cells[currentRow][currentCol].getFormula();
    string letterRep = cells[currentRow][currentCol].getLetterRepresentation();
    terminal.printAt(1, 1, letterRep + " | Formula: " + cellFormula);

    ostringstream headerLine;
    headerLine << setw(ROW_HEADER_WIDTH) << " "; // Space for row labels
    for (size_t j = 0; j < cells[0].size(); ++j)
    {
        string colLabel = getColumnLabel(j + 1);                 // Convert column index to label
        headerLine << "|" << setw(COLUMN_WIDTH - 1) << colLabel; // Column header with padding
    }
    headerLine << "|"; // End the header row with a separator
    terminal.printAt(2, 1, inputLine);
    terminal.printAt(3, 1, headerLine.str());

    for (size_t i = 0; i < cells.size(); ++i)
    {
        // Print the row label (numbers)
        ostringstream rowStream;
        rowStream << setw(ROW_HEADER_WIDTH - 1) << (i + 1) << "|"; // Row label with separator

        for (size_t j = 0; j < cells[i].size(); ++j)
        {
            // Format the cell content as a string and fit within the column width
            string cellText = cells[i][j].getContentAsString();

            // Truncate or pad cell content to fit within COLUMN_WIDTH
            if (cellText.length() >= COLUMN_WIDTH)
            {
                cellText = cellText.substr(0, COLUMN_WIDTH - 2) + ">"; // Truncate with ">"
            }
            else
            {
                cellText = formatCellText(cellText, COLUMN_WIDTH); // Right-align within cell
            }

            // Truncate or pad cell content to fit within COLUMN_WIDTH
            if (cellText.length() >= COLUMN_WIDTH)
            {
                cellText = cellText.substr(0, COLUMN_WIDTH - 2) + ">"; // Truncate with ">"
            }
            else
            {
                cellText = formatCellText(cellText, COLUMN_WIDTH); // Right-align within cell
            }

            // Highlight the current cell
            if (static_cast<int>(i) == currentRow && static_cast<int>(j) == currentCol)
            {
                rowStream << "|" << "\033[7m" << cellText << "\033[0m"; // Invert colors for highlight
            }
            else
            {
                rowStream << "|" << cellText; // Regular cells
            }
        }

        rowStream << "|"; // Close row with separator
        terminal.printAt(4 + i, 1, rowStream.str());
    }
}
//----------------------------------------------------------//
//----------------------------------------------------------//

void Spreadsheet::moveCell(int &currentRow, int &currentCol, const char dir)
{
    if (dir == (char)1) // Up arrow
    {
        currentRow = max(0, currentRow - 1); // Move up, ensuring we stay within bounds
    }
    else if (dir == (char)2) // Down arrow
    {
        currentRow = min(static_cast<int>(cells.size() - 1), currentRow + 1); // Move down, ensuring we stay within bounds
    }
    else if (dir == (char)4) // Left arrow
    {
        currentCol = max(0, currentCol - 1); // Move left, ensuring we stay within bounds
    }
    else if (dir == (char)3) // Right arrow
    {
        currentCol = min(static_cast<int>(cells[0].size() - 1), currentCol + 1); // Move right, ensuring we stay within bounds
    }
}

Cell *Spreadsheet::getCell(int r, int c)
{
    // Check if the row and column are within the valid bounds
    if (r >= 0 && r < getRowCount() && c >= 0 && c < getColCount())
    {
        return &cells[r][c]; // Return the pointer to the cell
    }
    else
    {
        throw out_of_range("Cell coordinates are out of range.");
    }
}

string Spreadsheet::getContentAsString(int r, int c) const
{
    return cells[r][c].getContentAsString();
}

string Spreadsheet::getLetterRepresentation(int r, int c) const
{
    return cells[r][c].getLetterRepresentation();
}

vector<Cell *> Spreadsheet::getCellsInRange(pair<int, int> startPos, pair<int, int> endPos)
{
    vector<Cell *> cellsInRange;

    int startRow = startPos.first;
    int startCol = startPos.second;
    int endRow = endPos.first;
    int endCol = endPos.second;

    if (startRow > endRow)
        swap(startRow, endRow);
    if (startCol > endCol)
        swap(startCol, endCol);

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

void Spreadsheet::run()
{
    FormulaParser parser(*this);
    int currentRow = 0, currentCol = 0;
    pair<int, int> oldLoc;
    string input;

    while (true)
    {
        input = cells[currentRow][currentCol].getContentAsString();
        displayScreen(currentRow, currentCol, input);

        char command = terminal.getSpecialKey();

        if (command == 'q')
        {
            cout << "Exiting spreadsheet...\n";
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
                displayScreen(currentRow, currentCol, input);

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

            if (!input.empty())
            {
                cells[oldLoc.first][oldLoc.second].setContent(input, parser, {oldLoc.first, oldLoc.second});
            }
            else
            {
                string empty = "";
                cells[oldLoc.first][oldLoc.second].setContent(empty);
            }

            // Recalculate the dependent cells scan all sheet that check every cells dependent
            // cells if current cell in there calculate that cells value again if it contains a
            // formula
            parser.autoCalculate({oldLoc.first, oldLoc.second});
            input.clear();
        }
    }
}
