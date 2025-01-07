#include "FormulaParser.h"
#include "Spreadsheet.h"
#include "Cell.h"
#include <string>
#include <stdexcept>
#include <algorithm>
#include <set>
#include <cmath>
#include <memory>
#include <iostream>
#include "myset.h"
#include "myvec.h"

// From GPT-------------------------
//----------------------------------
std::string removeSpaces(const std::string &str)
{
    std::string newStr = str;
    newStr.erase(remove(newStr.begin(), newStr.end(), ' '), newStr.end());
    return newStr;
}

bool FormulaParser::isValue(const std::string &token) const
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

double FormulaParser::parseAndEvaluate(std::string &formula, std::pair<int, int> coordinates, spc::myvec<std::pair<int, int>> &dependentCells)
{
    if (formula.empty())
        throw std::runtime_error("Empty formula input.\n");

    spc::myvec<std::string> tokens = parsePlusAndMinus(formula.substr(1));

    if (tokens.empty())
        throw std::runtime_error("Empty tokens at parseAndEvaluate.\n");

    spc::myset<std::pair<int, int>> uniqueDependents;

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
            double tokenResult = evaluateMultpAndDivToken(token, uniqueDependents);
            result = isAddition == true ? result + tokenResult : result - tokenResult;
        }
    }

    for (const auto &dependent : uniqueDependents)
        dependentCells.push_back(dependent);

    return result;
}

double FormulaParser::evaluateMultpAndDivToken(std::string &token, spc::myset<std::pair<int, int>> &uniqueDependents) const
{
    spc::myvec<std::string> tokens = parseMultpAndDiv(token);

    double result = evaluateSingleToken(tokens[0], uniqueDependents);

    bool isMultp = true;

    for (int i = 1; i < tokens.get_size(); ++i)
    {
        if (tokens[i] == "*")
            isMultp = true;
        else if (tokens[i] == "/")
            isMultp = false;
        else
        {
            double nextVal = evaluateSingleToken(tokens[i], uniqueDependents);
            if (nextVal == 0.0 && isMultp == false) // divide 0 check
                nextVal = 1;
            result = isMultp ? result * nextVal : result / nextVal;
        }
    }
    return result;
}

spc::myvec<std::string> FormulaParser::parseMultpAndDiv(const std::string &token) const
{
    spc::myvec<std::string> tokens;
    std::string subToken;
    for (size_t i = 0; i < token.size(); i++)
    {
        switch (token[i])
        {
        case '/':
        case '*':
            if (!subToken.empty())
                tokens.push_back(subToken);

            // string(1, formula[i]) creates a string with length one, simply convert char to string
            tokens.push_back(std::string(1, token[i]));
            subToken.clear();
            break;
        default:
            subToken += token[i];
            break;
        }
    }
    if (token.empty())
        throw std::invalid_argument("Not a valid formula there is a / or * sign at the end"); // throws to ParseAndEvaluate

    tokens.push_back(subToken);
    return tokens;
}

spc::myvec<std::string> FormulaParser::parsePlusAndMinus(const std::string &formula) const
{
    spc::myvec<std::string> tokens;
    std::string token;
    for (size_t i = 0; i < formula.size(); i++)
    {
        switch (formula[i])
        {
        case '+':
        case '-':
            if (!token.empty())
                tokens.push_back(token);

            // string(1, formula[i]) creates a string with length one, simply convert char to string
            tokens.push_back(std::string(1, formula[i]));
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
        std::cerr << "Thrown from parsePlusAndMinus\n";
        throw std::invalid_argument("Not a valid formula there is a + or - sign at the end"); // throws to ParseAndEvaluate
    }

    tokens.push_back(token);
    return tokens;
}

double FormulaParser::evaluateSingleToken(std::string &singleToken, spc::myset<std::pair<int, int>> &uniqueDependents) const
{
    std::pair<int, int> coordinates;
    coordinates = getCellReference(singleToken);

    if (coordinates != std::pair<int, int>(-1, -1))
    {
        Cell *cell = spreadsheet->getCell(coordinates.first, coordinates.second);
        uniqueDependents.insert({cell->getRow(), cell->getCol()});

        return cell->getCellValueAsDouble();
    }

    FunctionType func = getFunctionType(singleToken); // also checks if range is valid et cetera
    if (func != FunctionType::INVALID)
    {
        std::string range = singleToken.substr(singleToken.find('(') + 1, singleToken.find(')') - singleToken.find('(') - 1);

        int dotdotPos = range.find("..");

        std::string startCell = range.substr(0, dotdotPos);
        std::string endCell = range.substr(dotdotPos + 2);

        std::pair<int, int> startPos = getCellReference(startCell);
        std::pair<int, int> endPos = getCellReference(endCell);

        if (startPos == std::pair<int, int>(-1, -1) || endPos == std::pair<int, int>(-1, -1))
        {
            throw std::invalid_argument("Invalid cell reference in function range.");
        }
        switch (func)
        {
        case FunctionType::SUM:
            return SUM(startPos, endPos, uniqueDependents);
        case FunctionType::AVER:
            return AVER(startPos, endPos, uniqueDependents);
        case FunctionType::STDDEV:
            return STDDEV(startPos, endPos, uniqueDependents);
        case FunctionType::MAX:
            return MAX(startPos, endPos, uniqueDependents);
        case FunctionType::MIN:
            return MIN(startPos, endPos, uniqueDependents);
        case FunctionType::INVALID:
            throw std::runtime_error("Invalid function type.\n");
        }
    }
    else if (isValue(singleToken))
    {
        try
        {
            return stod(singleToken);
        }
        catch (const std::invalid_argument &)
        {
            throw std::invalid_argument("token is not a valid");
        }
    }
    else
    {
        throw std::invalid_argument("token is not a valid cell reference function or numeric value");
        ;
    }
    return 0.0;
}

std::pair<int, int> FormulaParser::getCellReference(const std::string &token) const
{
    std::string str = removeSpaces(token);
    for (int i = 0; i < spreadsheet->getRowCount(); ++i)
        for (int j = 0; j < spreadsheet->getColCount(); ++j)
            if (str == spreadsheet->getCell(i, j)->getLetterRepresentation())
                return std::pair<int, int>(i, j);
    return std::pair<int, int>(-1, -1);
}

bool FormulaParser::isValidRange(const std::string &range) const
{
    size_t dotPos = range.find("..");
    if (dotPos == std::string::npos) // npos indicates fail
        return false;

    std::string startCell = range.substr(0, dotPos);
    std::string endCell = range.substr(dotPos + 2);

    return getCellReference(startCell) != std::pair<int, int>(-1, -1) &&
           getCellReference(endCell) != std::pair<int, int>(-1, -1);
}

FunctionType FormulaParser::getFunctionType(std::string &token) const
{
    std::string str = removeSpaces(token);

    size_t openParenPos = str.find('(');
    size_t closeParenPos = str.find(')');

    if (openParenPos != std::string::npos && closeParenPos != std::string::npos && closeParenPos > openParenPos)
    {
        std::string range = str.substr(openParenPos + 1, closeParenPos - openParenPos - 1);

        if (isValidRange(range))
        {
            std::string funcName = str.substr(0, openParenPos);
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

double FormulaParser::SUM(std::pair<int, int> startPos, std::pair<int, int> endPos, spc::myset<std::pair<int, int>> &uniqueDependents) const
{
    double result = 0.0;
    spc::myvec<Cell *> cellsInRange = spreadsheet->getCellsInRange(startPos, endPos);

    for (Cell *cell : cellsInRange)
    {
        uniqueDependents.insert({cell->getRow(), cell->getCol()});
        result += cell->getCellValueAsDouble();
    }
    return result;
}

double FormulaParser::AVER(std::pair<int, int> startPos, std::pair<int, int> endPos, spc::myset<std::pair<int, int>> &uniqueDependents) const
{
    spc::myvec<Cell *> cellsInRange = spreadsheet->getCellsInRange(startPos, endPos);

    if (cellsInRange.empty())
    {
        return 0.0;
    }

    double sum = 0.0;
    int validCount = 0;
    for (Cell *cell : cellsInRange)
    {
        std::pair<int, int> cellCoords = {cell->getRow(), cell->getCol()};
        uniqueDependents.insert(cellCoords);
        sum += cell->getCellValueAsDouble();
        validCount++;
    }
    if (validCount == 0)
        return 0.0;

    return sum / validCount;
}

double FormulaParser::STDDEV(std::pair<int, int> startPos, std::pair<int, int> endPos, spc::myset<std::pair<int, int>> &uniqueDependents) const
{
    double sum = 0.0;
    int count = 0;
    spc::myvec<double> values;

    for (int row = startPos.first; row <= endPos.first; ++row)
    {
        for (int col = startPos.second; col <= endPos.second; ++col)
        {
            std::pair<int, int> pos(row, col);
            uniqueDependents.insert(pos);
            double numericValue = spreadsheet->getCell(row, col)->getCellValueAsDouble();
            values.push_back(numericValue);
            sum += numericValue;
            ++count;
        }
    }
    if (count == 0)
        return 0.0;

    double mean = sum / count;
    double varianceSum = 0.0;

    for (double value : values)
    {
        varianceSum += (value - mean) * (value - mean);
    }
    double variance = varianceSum / count;
    return sqrt(variance);
}

double FormulaParser::MAX(std::pair<int, int> startPos, std::pair<int, int> endPos, spc::myset<std::pair<int, int>> &uniqueDependents) const
{
    double maxValue = -std::numeric_limits<double>::infinity(); // GPT s idea to get the lowest limit

    for (int row = startPos.first; row <= endPos.first; ++row)
    {
        for (int col = startPos.second; col <= endPos.second; ++col)
        {
            std::pair<int, int> pos(row, col);
            uniqueDependents.insert(pos);
            double content = spreadsheet->getCell(row, col)->getCellValueAsDouble();
            maxValue = std::max(maxValue, content);
        }
    }
    return maxValue;
}

double FormulaParser::MIN(std::pair<int, int> startPos, std::pair<int, int> endPos, spc::myset<std::pair<int, int>> &uniqueDependents) const
{
    double minValue = std::numeric_limits<double>::infinity();

    for (int row = startPos.first; row <= endPos.first; ++row)
    {
        for (int col = startPos.second; col <= endPos.second; ++col)
        {
            std::pair<int, int> pos(row, col);
            uniqueDependents.insert(pos);
            double content = spreadsheet->getCell(row, col)->getCellValueAsDouble();
            minValue = std::min(minValue, content);
        }
    }
    return minValue;
}

void FormulaParser::autoCalculate(std::pair<int, int> coordinate)
{
    static std::set<std::pair<int, int>> visited; // Tracks already processed cells
    if (visited.find(coordinate) != visited.end())
        return; // Skip if already processed

    visited.insert(coordinate); // Mark this cell as visited

    for (int i = 0; i < spreadsheet->getRowCount(); i++)
    {
        for (int j = 0; j < spreadsheet->getColCount(); j++)
        {
            if (auto formulaCell = dynamic_cast<FormulaCell *>(spreadsheet->getCell(i, j)))
            {
                spc::myvec<std::pair<int, int>> dependentCells = formulaCell->fetchDependentCells();
                if (std::find(dependentCells.begin(), dependentCells.end(), coordinate) != dependentCells.end())
                {
                    std::string formula = formulaCell->getFormula();
                    if (!formula.empty() && formula[0] == '=')
                    {
                        try
                        {
                            spc::myvec<std::pair<int, int>> newDependentCells;
                            double newValue = parseAndEvaluate(formula, {i, j}, newDependentCells);
                            formulaCell->setCalculatedValue(newValue);
                            for (auto &pair : newDependentCells)
                                formulaCell->addDependentCell(pair);

                            autoCalculate({i, j}); // Recursively process this cell's dependencies
                        }
                        catch (const std::exception &e)
                        {
                            std::cerr << "Error recalculating cell (" << i << ", " << j << "): " << e.what() << std::endl;
                        }
                    }
                }
            }
        }
    }
    visited.erase(coordinate); // Remove this cell from visited after processing
}
