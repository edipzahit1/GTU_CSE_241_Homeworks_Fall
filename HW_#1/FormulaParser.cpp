#include "FormulaParser.h"
#include "Spreadsheet.h"
#include "Cell.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <set>
#include <cmath>

using namespace std;

// From GPT-------------------------
//----------------------------------
string removeSpaces(const string &str)
{
    string newStr = str;
    newStr.erase(remove(newStr.begin(), newStr.end(), ' '), newStr.end());
    return newStr;
}

bool FormulaParser::isValue(const string &token) const
{
    if (token.empty())
        return false;

    // Check if it's a pure number (no letters or invalid characters)
    bool decimalPointSeen = false;
    for (char c : token)
    {
        if (!isdigit(c))
        {
            if (c == '.' && !decimalPointSeen)
            {
                decimalPointSeen = true; // First decimal point is allowed
            }
            else
            {
                return false; // Invalid character
            }
        }
    }

    return true;
}
//----------------------------------
//----------------------------------

double FormulaParser::parseAndEvaluate(string &formula, pair<int, int> coordinates)
{
    if (formula.empty())
        return 0.0;
    
    vector<string> tokens = parsePlusAndMinus(formula.substr(1)); // may throw to SpreadSheet we ll catch there
    
    if (tokens.empty())
        return 0.0;
    set<pair<int, int>> uniqueDependents;

    bool isAddition = true;
    double result = 0.0;

    for (auto &token : tokens)
    {
        if (token == "+")
            isAddition = true;
        else if (token == "-")
            isAddition = false;
        else
        {
            double tokenResult;
            try
            {
                tokenResult = evaluateMultpAndDivToken(token, uniqueDependents);
            }
            catch (const invalid_argument &e)
            {
                cerr << "Thrown from parseAndEvaluate\n";
                throw e; // throwing to Spreadsheet class to take care of invalid formula and dont change the value of it
            }
            result = isAddition == true ? result + tokenResult : result - tokenResult;
        }
    }
    vector<pair<int, int>> dependentCells(uniqueDependents.begin(), uniqueDependents.end());
    Cell *cell = spreadsheet.getCell(coordinates.first, coordinates.second);
    
    cell->setFormula(formula, dependentCells);
    cell->setContent(result);
    return result;
}

double FormulaParser::evaluateMultpAndDivToken(string &token, set<pair<int, int>> &uniqueDependents) const
{
    vector<string> tokens = parseMultpAndDiv(token);
    double result = evaluateSingleToken(tokens[0], uniqueDependents);
    bool isMultp = true;

    for (size_t i = 1; i < tokens.size(); ++i)
    {
        if (tokens[i] == "*")
            isMultp = true;
        else if (tokens[i] == "/")
            isMultp = false;
        else
        {
            double nextVal = evaluateSingleToken(tokens[i], uniqueDependents);
            if (nextVal == 0.0 && isMultp == false)  //divide 0 check
                nextVal = 1; 
            result = isMultp ? result * nextVal : result / nextVal;
        }
    }
    return result;
}

double FormulaParser::evaluateSingleToken(string &singleToken, set<pair<int, int>> &uniqueDependents) const
{
    cout << "Evaluating token: " << singleToken << endl;
    pair<int, int> coordinates;
    coordinates = getCellReference(singleToken);
    double result = 0.0;
    if (coordinates != pair<int, int>(-1, -1))
    {
        Cell *cell = spreadsheet.getCell(coordinates.first, coordinates.second);
        auto content = cell->getContent();

        uniqueDependents.insert({cell->getRow(), cell->getCol()});

        if (const double *val = get_if<double>(&content))
            return *val;
        else if (const int *val = get_if<int>(&content))
            return static_cast<double>(*val);
        else
            return 0.0;
    }

    FunctionType func = getFunctionType(singleToken); // also checks if range is valid et cetera
    if (func != FunctionType::INVALID)
    {
        string range = singleToken.substr(singleToken.find('(') + 1, singleToken.find(')') - singleToken.find('(') - 1);

        int dotdotPos = range.find("..");

        string startCell = range.substr(0, dotdotPos);
        string endCell = range.substr(dotdotPos + 2);

        pair<int, int> startPos = getCellReference(startCell);
        pair<int, int> endPos = getCellReference(endCell);

        if (startPos == pair<int, int>(-1, -1) || endPos == pair<int, int>(-1, -1))
        {
            throw invalid_argument("Invalid cell reference in function range.");
        }
        switch (func)
        {
        case FunctionType::SUM:
            result = SUM(startPos, endPos, uniqueDependents);
            break;
        case FunctionType::AVER:
            result = AVER(startPos, endPos, uniqueDependents);
            break;
        case FunctionType::STDDEV:
            result = STDDEV(startPos, endPos, uniqueDependents);
            break;
        case FunctionType::MAX:
            result = MAX(startPos, endPos, uniqueDependents);
            break;
        case FunctionType::MIN:
            result = MIN(startPos, endPos, uniqueDependents);
            break;
        case FunctionType::INVALID:
            break;
        }
        return result;
    }
    else if (isValue(singleToken))
    {
        try
        {
            return stod(singleToken);
        }
        catch (const invalid_argument &)
        {
            throw invalid_argument("token is not a valid");
        }
    }
    else
    {
        cerr << "Thrown from evaLuateSingleToken\n";
        throw invalid_argument("token is not a valid cell reference function or numeric value");
        ;
    }
}

pair<int, int> FormulaParser::getCellReference(const string &token) const
{
    string str = removeSpaces(token);
    for (int i = 0; i < spreadsheet.getRowCount(); ++i)
        for (int j = 0; j < spreadsheet.getColCount(); ++j)
            if (str == spreadsheet.getLetterRepresentation(i, j))
                return pair<int, int>(i, j);

    return pair<int, int>(-1, -1);
}

FunctionType FormulaParser::getFunctionType(string &token) const
{
    string str = removeSpaces(token);

    size_t openParenPos = str.find('(');
    size_t closeParenPos = str.find(')');

    if (openParenPos != string::npos && closeParenPos != string::npos && closeParenPos > openParenPos)
    {
        string range = str.substr(openParenPos + 1, closeParenPos - openParenPos - 1);

        if (isValidRange(range))
        {
            string funcName = str.substr(0, openParenPos);
            if (funcName == "SUM")
                return FunctionType::SUM;
            else if (funcName == "AVER")
                return FunctionType::AVER;
            else if (funcName == "STDDEV")
                return FunctionType::STDDEV;
            else if (funcName == "MAX")
                return FunctionType::MAX;
            else if (funcName == "MIN")
                return FunctionType::MIN;
        }
    }
    return FunctionType::INVALID;
}

bool FormulaParser::isValidRange(const string &range) const
{
    size_t dotPos = range.find("..");
    if (dotPos == string::npos) // npos indicates fail
        return false;

    string startCell = range.substr(0, dotPos);
    string endCell = range.substr(dotPos + 2);

    return getCellReference(startCell) != pair<int, int>(-1, -1) &&
           getCellReference(endCell) != pair<int, int>(-1, -1);
}

vector<string> FormulaParser::parseMultpAndDiv(const string &token) const
{
    vector<string> tokens;
    string subToken;
    for (size_t i = 0; i < token.size(); i++)
    {
        switch (token[i])
        {
        case '/':
        case '*':
            if (!subToken.empty())
                tokens.push_back(subToken);

            // string(1, formula[i]) creates a string with length one, simply convert char to string
            tokens.push_back(string(1, token[i]));
            subToken.clear();
            break;
        default:
            subToken += token[i];
            break;
        }
    }
    if (token.empty())
    {
        cerr << "Thrown from parseMultpAndDiv\n";
        throw invalid_argument("Not a valid formula there is a / or * sign at the end"); // throws to ParseAndEvaluate
    }

    tokens.push_back(subToken);
    return tokens;
}

vector<string> FormulaParser::parsePlusAndMinus(const string &formula) const
{
    vector<string> tokens;
    string token;
    for (size_t i = 0; i < formula.size(); i++)
    {
        switch (formula[i])
        {
        case '+':
        case '-':
            if (!token.empty())
                tokens.push_back(token);

            // string(1, formula[i]) creates a string with length one, simply convert char to string
            tokens.push_back(string(1, formula[i]));
            token.clear();
            break;
        case ' ':
            break;
        default:
            token += formula[i];
            break;
        }
    }
    if (token.empty())
    {
        cerr << "Thrown from parsePlusAndMinus\n";
        throw invalid_argument("Not a valid formula there is a + or - sign at the end"); // throws to ParseAndEvaluate
    }

    tokens.push_back(token);
    return tokens;
}

double FormulaParser::SUM(pair<int, int> startPos, pair<int, int> endPos, set<pair<int, int>>& uniqueDependents) const
{
    double result = 0.0;
    vector<Cell *> cellsInRange = spreadsheet.getCellsInRange(startPos, endPos);

    for (Cell *cell : cellsInRange)
    {
        pair<int, int> cellCoords = cell->getCoordinates();
        uniqueDependents.insert(cellCoords);
        auto content = cell->getContent();
        if (const double *val = get_if<double>(&content))
        {
            result += *val;
        }
        else if (const int *val = get_if<int>(&content))
        {
            result += static_cast<double>(*val);
        }
    }
    return result;
}

double FormulaParser::AVER(pair<int, int> startPos, pair<int, int> endPos, set<pair<int, int>>& uniqueDependents) const
{
    vector<Cell *> cellsInRange = spreadsheet.getCellsInRange(startPos, endPos);

    if (cellsInRange.empty())
    {
        return 0.0;
    }

    double sum = 0.0;
    int validCount = 0;
    for (Cell *cell : cellsInRange)
    {
        pair<int, int> cellCoords = cell->getCoordinates();
        uniqueDependents.insert(cellCoords);
        auto content = cell->getContent();
        if (const double *val = get_if<double>(&content))
        {
            sum += *val;
            validCount++;
        }
        else if (const int *val = get_if<int>(&content))
        {
            sum += static_cast<double>(*val);
            validCount++;
        }
    }
    if (validCount == 0)
        return 0.0;

    return sum / validCount;
}

double FormulaParser::STDDEV(pair<int, int> startPos, pair<int, int> endPos, set<pair<int, int>>& uniqueDependents) const
{
    double sum = 0.0;
    int count = 0;
    vector<double> values;

    for (int row = startPos.first; row <= endPos.first; ++row) {
        for (int col = startPos.second; col <= endPos.second; ++col) {
            pair<int, int> pos(row, col);
            uniqueDependents.insert(pos);

            auto content = spreadsheet.getCell(row, col)->getContent(); 

            double numericValue = 0.0;
            if (const double* val = get_if<double>(&content)) {
                numericValue = *val;
            }
            else if (const int* val = get_if<int>(&content)) {
                numericValue = static_cast<double>(*val);
            }
            
            values.push_back(numericValue);
            sum += numericValue;
            ++count;
        }
    }

    if (count == 0) return 0.0;

    double mean = sum / count;
    double varianceSum = 0.0;

    for (double value : values) {
        varianceSum += (value - mean) * (value - mean);
    }

    double variance = varianceSum / count;
    return sqrt(variance);
}

double FormulaParser::MAX(pair<int, int> startPos, pair<int, int> endPos, set<pair<int, int>>& uniqueDependents) const
{
    double maxValue = -std::numeric_limits<double>::infinity(); //GPT s idea to get the lowest limit

    for (int row = startPos.first; row <= endPos.first; ++row) {
        for (int col = startPos.second; col <= endPos.second; ++col) {
            pair<int, int> pos(row, col);
            uniqueDependents.insert(pos);

            auto content = spreadsheet.getCell(row, col)->getContent();

            if (const double* val = get_if<double>(&content)) {
                maxValue = max(maxValue, *val);
            }
            else if (const int* val = get_if<int>(&content)) {
                maxValue = max(maxValue, static_cast<double>(*val));
            }
        }
    }

    return maxValue;
}

double FormulaParser::MIN(pair<int, int> startPos, pair<int, int> endPos, set<pair<int, int>>& uniqueDependents) const
{
    double minValue = std::numeric_limits<double>::infinity();

    for (int row = startPos.first; row <= endPos.first; ++row) 
    {
        for (int col = startPos.second; col <= endPos.second; ++col) 
        {
            pair<int, int> pos(row, col);
            uniqueDependents.insert(pos);

            auto content = spreadsheet.getCell(row, col)->getContent();

            if (const double* val = get_if<double>(&content)) 
            {
                minValue = min(minValue, *val);
            }
            else if (const int* val = get_if<int>(&content)) 
            {
                minValue = min(minValue, static_cast<double>(*val));
            }
        }
    }
    return minValue;
}

void FormulaParser::autoCalculate(pair<int, int> coordinate)
{
    for (int i = 0; i < spreadsheet.getRowCount(); i++)
    {
        for (int j = 0; j < spreadsheet.getColCount(); j++)
        {
            Cell *cell = spreadsheet.getCell(i, j);
            if (!cell)
                continue;

            vector<pair<int, int>> dependentCells = cell->getDependentCells();
            if (find(dependentCells.begin(), dependentCells.end(), coordinate) != dependentCells.end())
            {
                string formula = cell->getFormula();
                if (!formula.empty() && formula[0] == '=')
                {
                    try
                    {
                        double newValue = parseAndEvaluate(formula, {i, j});
                        cell->setContent(newValue);
                        
                        autoCalculate({i, j}); //recursion because that changing cell can have dependents too
                    }
                    catch (const exception &e)
                    {
                        cerr << "Error recalculating cell (" << i << ", " << j << "): " << e.what() << endl;
                    }
                }
            }
        }
    }
}
