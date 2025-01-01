#ifndef PARSE_EM
#define PARSE_EM

#include "myvec.h"
#include "myset.h"
#include <string>
#include <vector>
#include <set>
#include <iostream>

/**
 * @enum FunctionType
 * @brief Represents the types of functions that can be parsed and evaluated.
 */
enum class FunctionType
{
    SUM,    ///< Sum of values in a range.
    AVER,   ///< Average of values in a range.
    STDDEV, ///< Standard deviation of values in a range.
    MAX,    ///< Maximum value in a range.
    MIN,    ///< Minimum value in a range.
    INVALID ///< Invalid function type.
};

class Spreadsheet;

/**
 * @class FormulaParser
 * @brief Responsible for parsing and evaluating formulas in the spreadsheet.
 */
class FormulaParser
{
public:
    /**
     * @brief Constructs a FormulaParser with a reference to the Spreadsheet.
     * @param sheet Pointer to the Spreadsheet object.
     */
    FormulaParser(Spreadsheet *sheet) : spreadsheet(sheet) {}

    /**
     * @brief Parses and evaluates a formula, updating dependent cells as needed.
     * @param formula The formula string to parse and evaluate.
     * @param coordinates The coordinates of the cell containing the formula.
     * @param dependentCells A vector to store dependent cell coordinates.
     * @return The calculated result of the formula.
     */
    double parseAndEvaluate(std::string &formula, std::pair<int, int> coordinates, spc::myvec<std::pair<int, int>> &dependentCells);

    /**
     * @brief Automatically recalculates cells dependent on a specified cell.
     * @param coordinate The coordinates of the cell whose dependents need recalculating.
     */
    void autoCalculate(std::pair<int, int> coordinate);

private:
    Spreadsheet *spreadsheet; ///< Pointer to the associated Spreadsheet object.

    /**
     * @brief Splits a formula into tokens based on addition and subtraction operators.
     * @param formula The formula string to parse.
     * @return A vector of tokens split by addition and subtraction operators.
     */
    spc::myvec<std::string> parsePlusAndMinus(const std::string &formula) const;

    /**
     * @brief Splits a token into sub-tokens based on multiplication and division operators.
     * @param token The token to parse.
     * @return A vector of sub-tokens split by multiplication and division operators.
     */
    spc::myvec<std::string> parseMultpAndDiv(const std::string &token) const;

    /**
     * @brief Evaluates a token containing multiplication and division operations.
     * @param token The token to evaluate.
     * @param uniqueDependents A set to store unique dependent cell coordinates.
     * @return The evaluated result of the token.
     */
    double evaluateMultpAndDivToken(std::string &token, spc::myset<std::pair<int, int>> &uniqueDependents) const;

    /**
     * @brief Checks if a given range string is valid.
     * @param range The range string to validate.
     * @return True if the range is valid, false otherwise.
     */
    bool isValidRange(const std::string &range) const;

    /**
     * @brief Converts a cell reference string to its row and column coordinates.
     * @param token The cell reference string.
     * @return A pair representing the row and column coordinates.
     */
    std::pair<int, int> getCellReference(const std::string &token) const;

    /**
     * @brief Determines the type of function represented by a token.
     * @param token The token to analyze.
     * @return The corresponding FunctionType.
     */
    FunctionType getFunctionType(std::string &token) const;

    /**
     * @brief Checks if a token represents a numerical value.
     * @param token The token to check.
     * @return True if the token is a value, false otherwise.
     */
    bool isValue(const std::string &token) const;

    /**
     * @brief Evaluates a single token and updates dependent cells.
     * @param singleToken The token to evaluate.
     * @param uniqueDependents A set to store unique dependent cell coordinates.
     * @return The evaluated result of the token.
     */
    double evaluateSingleToken(std::string &singleToken, spc::myset<std::pair<int, int>> &uniqueDependents) const;

    /**
     * @brief Calculates the sum of values in a specified range.
     * @param startPos The starting coordinates of the range.
     * @param endPos The ending coordinates of the range.
     * @param uniqueDependents A set to store unique dependent cell coordinates.
     * @return The sum of values in the range.
     */
    double SUM(std::pair<int, int> startPos, std::pair<int, int> endPos, spc::myset<std::pair<int, int>> &uniqueDependents) const;

    /**
     * @brief Calculates the average of values in a specified range.
     * @param startPos The starting coordinates of the range.
     * @param endPos The ending coordinates of the range.
     * @param uniqueDependents A set to store unique dependent cell coordinates.
     * @return The average of values in the range.
     */
    double AVER(std::pair<int, int> startPos, std::pair<int, int> endPos, spc::myset<std::pair<int, int>> &uniqueDependents) const;

    /**
     * @brief Calculates the standard deviation of values in a specified range.
     * @param startPos The starting coordinates of the range.
     * @param endPos The ending coordinates of the range.
     * @param uniqueDependents A set to store unique dependent cell coordinates.
     * @return The standard deviation of values in the range.
     */
    double STDDEV(std::pair<int, int> startPos, std::pair<int, int> endPos, spc::myset<std::pair<int, int>> &uniqueDependents) const;

    /**
     * @brief Finds the maximum value in a specified range.
     * @param startPos The starting coordinates of the range.
     * @param endPos The ending coordinates of the range.
     * @param uniqueDependents A set to store unique dependent cell coordinates.
     * @return The maximum value in the range.
     */
    double MAX(std::pair<int, int> startPos, std::pair<int, int> endPos, spc::myset<std::pair<int, int>> &uniqueDependents) const;

    /**
     * @brief Finds the minimum value in a specified range.
     * @param startPos The starting coordinates of the range.
     * @param endPos The ending coordinates of the range.
     * @param uniqueDependents A set to store unique dependent cell coordinates.
     * @return The minimum value in the range.
     */
    double MIN(std::pair<int, int> startPos, std::pair<int, int> endPos, spc::myset<std::pair<int, int>> &uniqueDependents) const;
};

#endif
