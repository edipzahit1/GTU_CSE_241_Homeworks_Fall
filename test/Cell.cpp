#include "Cell.h"
#include <string>

void Cell::setLetterRepresentation(int row, int col)
{
    int c = col;
    std::string letter;

    while (c >= 0)
    {
        letter = static_cast<char>('A' + (c % 26)) + letter;
        c = c / 26 - 1;
    }
    
    letter_rep = letter + std::to_string(row + 1);
}

double Cell::getCellValueAsDouble()
{
    if (auto *intCell = dynamic_cast<IntValueCell *>(this))
    {
        return static_cast<double>(intCell->getValue());
    }
    else if (auto *doubleCell = dynamic_cast<DoubleValueCell *>(this))
    {
        return doubleCell->getValue();
    }
    else if (auto *formulaCell = dynamic_cast<FormulaCell *>(this))
    {
        return formulaCell->getCalculatedValue();
    }
    else
    {
        return 0.0;
    }
}