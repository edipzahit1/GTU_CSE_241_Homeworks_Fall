#ifndef SHEETHANDLER_H
#define SHEETHANDLER_H

#include "Spreadsheet.h"
#include "FileHandler.h"
#include <string>
#include <unordered_map>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * @class SheetHandler
 * @brief A class responsible for handling multiple spreadsheets, 
 *        including saving, loading, and managing them.
 * 
 * The SheetHandler class allows the user to add, save, load, 
 * and view spreadsheets. It also manages a menu for creating 
 * and running spreadsheets through user interaction.
 * 
 * @note The spreadsheets are stored in a specified directory 
 *       (default is "sheets").
 */
class SheetHandler
{
public:
    /**
     * @brief Constructs a SheetHandler object with an optional directory path.
     * 
     * @param dir_path The path to the directory where sheets will be stored. 
     *                 Defaults to "sheets".
     */
    SheetHandler(const std::string& dir_path = "sheets");

    /**
     * @brief Adds a new spreadsheet to the handler.
     * 
     * @param filename The name of the spreadsheet to add.
     * @param newSheet A pointer to the new Spreadsheet object.
     */
    void add(const std::string& filename, Spreadsheet* newSheet);

    /**
     * @brief Saves a spreadsheet to a file.
     * 
     * @param filename The name of the file to save the spreadsheet as.
     */
    void saveSheet(const std::string& filename);

    /**
     * @brief Loads a spreadsheet from a file.
     * 
     * @param filename The name of the file to load the spreadsheet from.
     */
    void loadSheet(const std::string& filename);

    /**
     * @brief Retrieves a pointer to a spreadsheet by its filename.
     * 
     * @param filename The name of the spreadsheet to retrieve.
     * 
     * @return A pointer to the Spreadsheet object, or nullptr if not found.
     */
    Spreadsheet* getSheet(const std::string& filename) const;

    /**
     * @brief Displays the list of saved spreadsheets.
     */
    void viewSavedSheets() const;

    /**
     * @brief Runs the menu interface for the user to interact with the spreadsheets.
     */
    void runMenu();

    /**
     * @brief Destructor that cleans up any dynamically allocated memory.
     */
    ~SheetHandler();

private:
    /** @brief A map of spreadsheets indexed by their filename. */
    std::unordered_map<std::string, Spreadsheet*> sheets;

    /** @brief A FileHandler object used for managing file operations. */
    FileHandler handler;

    /** @brief The path to the directory where sheets are stored. */
    const std::string directory_path;

    /**
     * @brief Displays the main menu for the user to select options.
     */
    void displayMenu() const;

    /**
     * @brief Handles the creation of a new spreadsheet.
     */
    void handleCreate();

    /**
     * @brief Handles the execution of a selected spreadsheet.
     */
    void handleRun();
};

#endif
