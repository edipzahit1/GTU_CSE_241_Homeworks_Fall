#include "FileHandler.h"
#include "Spreadsheet.h"
#include <string>
#include <fstream>
#include <cctype>
#include <sstream>
#include <iostream>

using namespace std;

FileHandler::FileHandler() {}

void FileHandler::saveToFile(const string &filename, const Spreadsheet &spreadsheet)
{
    ofstream file(filename);
    if (!file.is_open())
        return;

    for (int i = 0; i < spreadsheet.getRowCount(); ++i)
    {
        for (int j = 0; j < spreadsheet.getColCount(); ++j)
        {
            file << spreadsheet.getContentAsString(i, j);
            if (j < spreadsheet.getColCount() - 1)
            {
                file << ",";
            }
        }
        file << "\n";
    }
    file.close();
}

void FileHandler::loadFromFile(const string &filename, Spreadsheet& spreadsheet) 
{
    ifstream file(filename);
    if (!file.is_open()) 
        throw runtime_error("File error");

    string line;
    int row = 0;
    while (getline(file, line)) 
    {
        stringstream ss(line);
        string cellData;
        int col = 0;

        while (getline(ss, cellData, ',')) 
        {
            Cell *cell = spreadsheet.getCell(row, col);
            if (isInteger(cellData))
                cell->setContent(stoi(cellData));
            else if (isDouble(cellData))
                cell->setContent(stod(cellData));
            else 
                cell->setContent(cellData);
            
            ++col;
        }
        ++row;
    }

    file.close();
}

// Conversion made my ChatGPT
//---------------------------------------------------//
//---------------------------------------------------//
bool FileHandler::isInteger(const std::string &str)
{
    if (str.empty())
        return false;

    // Check if there's a leading minus sign
    int start = (str[0] == '-') ? 1 : 0;
    if (start == 1 && str.length() == 1)
        return false; // "-" alone is not an integer

    // Check if every character after the optional '-' is a digit
    for (size_t i = start; i < str.length(); i++)
    {
        if (!std::isdigit(str[i]))
            return false;
    }
    return true;
}

bool FileHandler::isDouble(const std::string &str)
{
    if (str.empty())
        return false;

    int start = (str[0] == '-') ? 1 : 0;
    bool hasDecimal = false;

    for (size_t i = start; i < str.length(); i++)
    {
        if (str[i] == '.')
        {
            if (hasDecimal)
                return false; // More than one decimal point
            hasDecimal = true;
        }
        else if (!std::isdigit(str[i]))
        {
            return false;
        }
    }

    // A valid double should have at least one digit before or after the decimal point
    return hasDecimal;
}
//---------------------------------------------------//
//---------------------------------------------------//