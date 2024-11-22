#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <vector>
#include <string>
#include <variant>
#include "AnsiTerminal.h"
#include "Cell.h"
#include "FormulaParser.h"

using namespace std;

class Cell;

using CellContent = variant<monostate, int, double, string>;

class Spreadsheet 
{
public:
    Spreadsheet(int r = 1, int c = 1);
    
    Cell* getCell(int r, int c);
    int getRowCount() const { return cells.size(); }
    int getColCount() const { return cells[0].size(); }
    string getContentAsString(int r, int c) const;
    string getLetterRepresentation(int r, int c) const;
    vector<Cell*> getCellsInRange(pair<int, int> startPos, pair<int, int> endPos);

    void displayScreen(int currentRow, int currentCol, string inputLine = "");
    void run();
private:
    vector<vector<Cell>> cells;
    AnsiTerminal terminal;  

    string getColumnLabel(int columnIndex) const;
    string getCellLabel(int r, int c) const;

    //Helper functions to move through the sheet
    void moveCell(int &currentRow, int &currentCol, const char dir);
};

#endif
