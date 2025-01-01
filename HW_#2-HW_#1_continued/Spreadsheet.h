#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "Cell.h"
#include "AnsiTerminal.h"
#include "myvec.h"
#include "FormulaParser.h"
#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>

/**
 * @class Spreadsheet
 * @brief A class representing a spreadsheet consisting of cells arranged 
 *        in rows and columns.
 * 
 * The Spreadsheet class allows you to manage a grid of cells, 
 * input data into individual cells, display the content of cells, 
 * and perform operations like navigating and expanding the grid. 
 * It supports formulas and provides an interface for interacting with 
 * the content of the spreadsheet.
 */
class Spreadsheet
{
public:
    /** @brief Maximum number of rows in the spreadsheet. */
    static const int MAX_ROWS = 100; 
    
    /** @brief Maximum number of columns in the spreadsheet. */
    static const int MAX_COLS = 50;
    
    /**
     * @brief Constructs a Spreadsheet object with a specified number of rows and columns.
     * 
     * @param rows The number of rows in the spreadsheet.
     * @param cols The number of columns in the spreadsheet.
     */
    Spreadsheet(int rows, int cols);
    
    /**
     * @brief Default constructor that creates a 3x3 spreadsheet.
     */
    Spreadsheet() : Spreadsheet(3, 3) {}

    /**
     * @brief Retrieves a pointer to the cell at the specified row and column.
     * 
     * @param r The row index of the cell.
     * @param c The column index of the cell.
     * 
     * @return A pointer to the Cell object at the specified position.
     */
    Cell *getCell(int r, int c) const;

    /**
     * @brief Sets the cell at the specified row and column with a given cell object.
     * 
     * @param r The row index of the cell.
     * @param c The column index of the cell.
     * @param cell A unique pointer to the Cell object to set at the specified position.
     */
    void setCell(int r, int c, std::unique_ptr<Cell> cell);

    /**
     * @brief Enters data into a specified cell in the spreadsheet by taking a reference to the input string.
     * 
     * @param r The row index of the cell.
     * @param c The column index of the cell.
     * @param input The input string containing the data to be entered into the cell.
     */
    void enterData(int r, int c, std::string& input);

    /**
     * @brief Enters data into a specified cell in the spreadsheet by taking an r-value reference to the input string.
     * 
     * @param r The row index of the cell.
     * @param c The column index of the cell.
     * @param input The input string containing the data to be entered into the cell.
     */
    void enterData(int r, int c, std::string&& input);

    /**
     * @brief Returns the total number of rows in the spreadsheet.
     * 
     * @return The number of rows in the spreadsheet.
     */
    int getRowCount() const { return cells.get_size(); }

    /**
     * @brief Returns the total number of columns in the spreadsheet.
     * 
     * @return The number of columns in the spreadsheet.
     */
    int getColCount() const { return cells[0].get_size(); }

    /**
     * @brief Retrieves a list of cells within a specified range.
     * 
     * @param startPos A pair representing the starting row and column.
     * @param endPos A pair representing the ending row and column.
     * 
     * @return A vector of pointers to the cells within the specified range.
     */
    spc::myvec<Cell *> getCellsInRange(std::pair<int, int> startPos, std::pair<int, int> endPos);

    /**
     * @brief Displays the contents of the spreadsheet on the terminal.
     * 
     * @param currentRow The current row index to highlight.
     * @param currentCol The current column index to highlight.
     * @param terminal A reference to the AnsiTerminal object used for display.
     * @param inputLine The input line to be displayed, if any.
     */
    void displayScreen(int currentRow, int currentCol, AnsiTerminal& terminal, std::string inputLine = "");

    /**
     * @brief Runs the spreadsheet, initiating the user interface and interactive features.
     */
    void run();
    
    /**
     * @brief Friend class FileHandler, allowing it to access private members of Spreadsheet.
     */
    friend class FileHandler;

private:
    /** @brief A dynamic 2D array (vector of vectors) holding the cells in the spreadsheet. */
    spc::myvec<spc::myvec<std::unique_ptr<Cell>>> cells;

    /** @brief A shared pointer to the FormulaParser object used for parsing formulas. */
    std::shared_ptr<FormulaParser> parser;

    /**
     * @brief Expands the spreadsheet to accommodate more rows and columns.
     * 
     * @param newRowCount The new number of rows.
     * @param newColCount The new number of columns.
     */
    void expand(int newRowCount, int newColCount);

    /**
     * @brief Returns the column label for a given column index (e.g., "A", "B", "C").
     * 
     * @param columnIndex The index of the column.
     * 
     * @return The label corresponding to the given column index.
     */
    std::string getColumnLabel(int columnIndex) const;

    /**
     * @brief Returns the label for a specific cell, formatted as "A1", "B2", etc.
     * 
     * @param r The row index of the cell.
     * @param c The column index of the cell.
     * 
     * @return The label for the cell at the specified position.
     */
    std::string getCellLabel(int r, int c) const;

    /**
     * @brief Formats the text of a cell to fit within a specified width.
     * 
     * @param cellText The text to format.
     * @param width The width to which the text should be formatted.
     * 
     * @return The formatted cell text.
     */
    std::string formatCellText(const std::string &cellText, int width);

    /**
     * @brief Moves the current cell cursor based on the direction input.
     * 
     * @param currentRow The current row index to update.
     * @param currentCol The current column index to update.
     * @param dir A character representing the direction (e.g., 'U', 'D', 'L', 'R').
     */
    void moveCell(int &currentRow, int &currentCol, const char dir);
};

#endif
