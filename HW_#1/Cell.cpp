#include <string>
#include <iostream>
#include <iomanip>
#include "Cell.h"
#include "FormulaParser.h"

using namespace std;

Cell::Cell(int r, int c) : content(monostate()), row(r), col(c)
{}

string Cell::getLetterTransformation() const
{
    int c = col;
    string letter;

    while (c >= 0)
    {
        letter = static_cast<char>('A' + (c % 26)) + letter;
        c = c / 26 - 1;
    }
    return letter;
}

string Cell::getContentAsString() const
{
    const CellContent &content = this->content;

    if (const double *val = get_if<double>(&content))
    {
        std::ostringstream oss;
        oss.precision(2); 
        oss << std::fixed << *val;
        return oss.str();
    }
    else if (const string *strVal = get_if<string>(&content))
    {
        return *strVal;
    }
    else if (const int *val = get_if<int>(&content))
    {
        return to_string(*val);
    }
    else
    {
        return "";
    }
}

void Cell::setLetterRepresentation(const string &rep)
{
    letterRepresentation = rep + to_string(row + 1);
}

//i have changed this to this name because it cost me 2 hour to find the bug i was assigning dependentCells to itself (dependentCell_____SS) 
void Cell::setFormula(string &formula_____SS, vector<pair<int, int>> &dependentCell_____SS)
{
    formula = formula_____SS;
    dependentCells = dependentCell_____SS;
}

vector<pair<int, int>> &Cell::getDependentCells()
{
    return dependentCells;
}

void Cell::setContent(const string& value)
{
    content = value;
}

void Cell::setContent(const double value)
{
    content = value;
}

void Cell::setContent(const int value)
{
    content = value;
}

void Cell::setContent(const string &input, FormulaParser &parser, const pair<int, int> &cellCoords)
{
    if (!input.empty() && input.at(0) == '=')
    {
        // It's a formula
        formula = input;
        try
        {
            double result = parser.parseAndEvaluate(formula, cellCoords);
            content = result; // Store the result of formula evaluation
        }
        catch (const std::exception &e)
        {
            std::cerr << "Formula error in cell (" << cellCoords.first << ", " << cellCoords.second << "): " << e.what() << std::endl;
            content = formula; // Store the formula itself if there's an error
        }
    }
    else
    {
        formula.clear();

        int intValue;
        double doubleValue;

        if (tryParseToInt(input, intValue) && input.find('.') == string::npos)
        {
            cout << "Parsed as integer";
            content = intValue;
        }
        else if (tryParseToDouble(input, doubleValue))
        {
            content = doubleValue;
        }
        else
        {
            content = input; // If parsing fail store as a string
        }
    }
}

bool Cell::tryParseToInt(const string &input, int &output)
{
    try
    {
        size_t pos;
        output = stoi(input, &pos);
        return pos == input.length(); 
    }
    catch (const std::invalid_argument &)
    {
        return false;
    }
    catch (const std::out_of_range &)
    {
        return false;
    }
}

bool Cell::tryParseToDouble(const string &input, double &output)
{
    try
    {
        size_t pos;
        output = stod(input, &pos);
        return pos == input.length(); // Ensure no extra characters
    }
    catch (...)
    {
        return false;
    }
}