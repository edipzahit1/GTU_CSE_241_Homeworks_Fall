#ifndef CELL_H
#define CELL_H

#include <string>
#include <vector>
#include <variant>
#include "FormulaParser.h"

using namespace std;

class FormulaParser;

// Learned from GPT it is like a evolved union, thought it
// would be accurate for this concept
using CellContent = variant<monostate, int, double, string>;

class Cell
{
public:
    Cell(int r = 0, int c = 0);

    void setContent(const string& value);
    void setContent(const double value);
    void setContent(const int value);
    void setContent(const string &input, FormulaParser &parser, const pair<int, int> &cellCoords);
    CellContent getContent() const { return content; }

    void setFormula(string &formula, vector<pair<int, int>> &dependentCells);
    string getFormula() const { return formula; }

    pair<int, int> getCoordinates() const { return {row, col}; }
    vector<pair<int, int>> &getDependentCells();

    int getRow() const { return row; }
    int getCol() const { return col; }

    string getLetterTransformation() const;
    void setLetterRepresentation(const string &rep);
    string getLetterRepresentation() const { return letterRepresentation; }

    string getContentAsString() const;

private:
    CellContent content;
    string formula;
    string letterRepresentation;
    int row, col;
    vector<pair<int, int>> dependentCells;

    bool tryParseToInt(const std::string &input, int &output);
    bool tryParseToDouble(const std::string &input, double &output);

};

#endif
