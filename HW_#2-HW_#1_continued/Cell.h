#ifndef CELL_H
#define CELL_H

#include <string>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include "myvec.h"

/**
 * Abstract base class representing a generic spreadsheet cell.
 * Provides common functionality for all cell types, including row and column management
 * and letter representation for cell coordinates.
 */
class Cell
{
public:
    /**
     * Constructor for Cell.
     * @param r Row index (must be non-negative).
     * @param c Column index (must be non-negative).
     * @throws std::runtime_error if row or column is negative.
     */
    Cell(int r, int c) : row(r), col(c)
    {
        if (row < 0 || col < 0)
            throw std::runtime_error("Invalid initializer for Cell instance.");
        setLetterRepresentation(r, c);
    }

    /**
     * Virtual destructor for polymorphic behavior.
     */
    virtual ~Cell() = default;

    /**
     * Sets the letter representation for the cell based on its coordinates.
     * @param r Row index.
     * @param c Column index.
     */
    void setLetterRepresentation(int r, int c);

    /**
     * Retrieves the letter representation of the cell.
     * @return String representing the cell's location (e.g., "A1").
     */
    std::string getLetterRepresentation() const { return letter_rep; }

    /**
     * Retrieves the row index of the cell.
     * @return Row index.
     */
    int getRow() const { return row; }

    /**
     * Retrieves the column index of the cell.
     * @return Column index.
     */
    int getCol() const { return col; }

    /**
     * Retrieves the cell's value as a double.
     * This method may throw if the value is non-numeric.
     * @return Cell value as double.
     */
    double getCellValueAsDouble();

    /**
     * Pure virtual method to retrieve the cell's value as a string.
     * @return Cell value as a string.
     */
    virtual std::string getValueAsString() const = 0;

private:
    std::string letter_rep; ///< String representation of the cell's location.
    int row, col; ///< Row and column indices.
};

/**
 * Class representing a formula-based cell.
 * Stores a formula and its calculated value, and tracks dependent cells.
 */
class FormulaCell : public Cell
{
public:
    /**
     * Constructor for FormulaCell.
     * @param r Row index.
     * @param c Column index.
     * @param f Formula string.
     */
    FormulaCell(int r, int c, const std::string &f)
        : Cell(r, c), formula(f), calculatedValue(0) {}

    /**
     * Sets the calculated value for the formula.
     * @param value The new calculated value.
     */
    void setCalculatedValue(double value) { calculatedValue = value; }

    /**
     * Retrieves the calculated value of the formula.
     * @return Calculated value as a double.
     */
    double getCalculatedValue() const { return calculatedValue; }

    /**
     * Retrieves the formula string.
     * @return Formula string.
     */
    const std::string &getFormula() const { return formula; }

    /**
     * Adds a dependent cell to the list.
     * @param coor Pair representing the dependent cell's coordinates.
     */
    void addDependentCell(const std::pair<int, int> &coor) { dependentCells.push_back(coor); }

    /**
     * Retrieves the list of dependent cells.
     * @return Vector of dependent cell coordinates.
     */
    const spc::myvec<std::pair<int, int>> &fetchDependentCells() const { return dependentCells; }

    /**
     * Clears the list of dependent cells.
     */
    void clearDependentCells() { dependentCells.clear(); }

    /**
     * Retrieves the cell's value as a string.
     * Formats the value as an integer or double based on precision.
     * @return Formatted value string.
     */
    std::string getValueAsString() const override
    {
        std::ostringstream oss;

        if (isInteger(calculatedValue))
        {
            oss << std::fixed << std::setprecision(0) << calculatedValue;
        }
        else
        {
            oss << std::fixed << std::setprecision(2) << calculatedValue;
        }

        return oss.str();
    }

private:
    /**
     * Checks if a double value is an integer.
     * @param calculatedValue The value to check.
     * @return True if the value is an integer, false otherwise.
     */
    bool isInteger(double calculatedValue) const
    {
        return calculatedValue == static_cast<int>(calculatedValue);
    }

    std::string formula; ///< The formula string.
    double calculatedValue; ///< The calculated value of the formula.
    spc::myvec<std::pair<int, int>> dependentCells; ///< List of dependent cell coordinates.
};

/**
 * Abstract base class for value-based cells.
 * Provides common functionality for cells storing direct values.
 */
class ValueCell : public Cell
{
public:
    /**
     * Constructor for ValueCell.
     * @param r Row index.
     * @param c Column index.
     */
    ValueCell(int r, int c) : Cell(r, c) {}

    /**
     * Virtual destructor for polymorphic behavior.
     */
    virtual ~ValueCell() = default;

    /**
     * Pure virtual method to set the cell's value.
     * @param v New value as a string.
     */
    virtual void setValue(const std::string &v) = 0;
};

/**
 * Class representing a cell that stores an integer value.
 */
class IntValueCell : public ValueCell
{
public:
    /**
     * Constructor for IntValueCell.
     * @param r Row index.
     * @param c Column index.
     * @param value Initial integer value.
     */
    IntValueCell(int r, int c, int value)
        : ValueCell(r, c), val(value) {}

    /**
     * Retrieves the cell's value as a string.
     * @return Integer value as a string.
     */
    std::string getValueAsString() const override
    {
        return std::to_string(val);
    }

    /**
     * Retrieves the integer value of the cell.
     * @return Integer value.
     */
    int getValue() const { return val; }

    /**
     * Sets the integer value of the cell.
     * @param v New value as a string.
     */
    void setValue(const std::string &v) { val = std::stoi(v); }

private:
    int val; ///< Integer value stored in the cell.
};

/**
 * Class representing a cell that stores a string value.
 */
class StringValueCell : public ValueCell
{
public:
    /**
     * Constructor for StringValueCell.
     * @param r Row index.
     * @param c Column index.
     * @param value Initial string value.
     */
    StringValueCell(int r, int c, const std::string &value)
        : ValueCell(r, c), val(value) {}

    /**
     * Retrieves the cell's value as a string.
     * @return String value.
     */
    std::string getValueAsString() const override
    {
        return val;
    }

    /**
     * Retrieves the string value of the cell.
     * @return String value.
     */
    std::string getValue() const { return val; }

    /**
     * Sets the string value of the cell.
     * @param v New value as a string.
     */
    void setValue(const std::string &v) { val = v; }

private:
    std::string val; ///< String value stored in the cell.
};

/**
 * Class representing a cell that stores a double value.
 */
class DoubleValueCell : public ValueCell
{
public:
    /**
     * Constructor for DoubleValueCell.
     * @param r Row index.
     * @param c Column index.
     * @param value Initial double value.
     */
    DoubleValueCell(int r, int c, double value)
        : ValueCell(r, c), val(value) {}

    /**
     * Retrieves the cell's value as a string.
     * @return Double value as a string.
     */
    std::string getValueAsString() const override
    {
        std::ostringstream oss;
        oss << val;
        return oss.str();
    }

    /**
     * Retrieves the double value of the cell.
     * @return Double value.
     */
    double getValue() const { return val; }

    /**
     * Sets the double value of the cell.
     * @param v New value as a string.
     */
    void setValue(const std::string &v) { val = std::stod(v); }

private:
    double val; ///< Double value stored in the cell.
};

#endif
