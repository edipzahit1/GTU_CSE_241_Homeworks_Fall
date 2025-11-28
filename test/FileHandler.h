#ifndef HANDLE_EM
#define HANDLE_EM

#include <string>
#include "Spreadsheet.h"

class Spreadsheet;

/**
 * @class FileHandler
 * @brief A utility class for handling file operations for a Spreadsheet.
 */
class FileHandler 
{
public:
    /**
     * @brief Saves the current state of the spreadsheet to a file.
     * @param filename The name of the file to save to.
     * @param spreadsheet The Spreadsheet object to save.
     */
    void saveToFile(const std::string &filename, const Spreadsheet &spreadsheet);

    /**
     * @brief Loads the state of the spreadsheet from a file.
     * @param filename The name of the file to load from.
     * @param spreadsheet The Spreadsheet object to populate.
     */
    void loadFromFile(const std::string &filename, Spreadsheet &spreadsheet);

private:
    /**
     * @brief Checks if a given string represents an integer value.
     * @param str The string to check.
     * @return True if the string represents an integer, false otherwise.
     */
    bool isInteger(const std::string& str);

    /**
     * @brief Checks if a given string represents a double value.
     * @param str The string to check.
     * @return True if the string represents a double, false otherwise.
     */
    bool isDouble(const std::string& str);
};

#endif
