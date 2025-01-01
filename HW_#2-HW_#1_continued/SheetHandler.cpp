#include "SheetHandler.h"
#include <filesystem>

SheetHandler::SheetHandler(const std::string &dirPath)
    : directory_path(dirPath)
{
    std::cout << "Initializing SheetHandler...\n";

    if (!fs::exists(directory_path) || !fs::is_directory(directory_path))
    {
        std::cout << "Directory " << directory_path << " does not exist. Creating it...\n";
        fs::create_directory(directory_path);
        return;
    }

    for (const auto &entry : fs::directory_iterator(directory_path))
    {
        if (entry.is_regular_file())
        {
            const std::string filePath = entry.path().filename().string();
            try
            {
                std::cout << "Loading file: " << filePath << "\n";
                Spreadsheet* sheet = new Spreadsheet();  // Using raw pointer
                handler.loadFromFile(entry.path().string(), *sheet);
                sheets[filePath] = sheet;  // Store raw pointer in the map
            }
            catch (const std::exception &e)
            {
                std::cout << "Error loading file " << filePath << ": " << e.what() << "\n";
            }
        }
    }
    std::cout << "SheetHandler initialized with " << sheets.size() << " spreadsheets.\n";
}

SheetHandler::~SheetHandler() {
    for (auto &entry : sheets) {
        delete entry.second;
    }
}

void SheetHandler::add(const std::string &filename, Spreadsheet* newSheet)
{
    if (newSheet)
    {
        sheets[filename] = newSheet;  // Store raw pointer
        handler.saveToFile(directory_path + "/" + filename, *newSheet);
    }
}

void SheetHandler::saveSheet(const std::string &filename)
{
    if (sheets.find(filename) == sheets.end())
        throw std::invalid_argument("No spreadsheet found with the given filename.");

    Spreadsheet* sheet = sheets[filename];
    std::string fullPath = directory_path + "/" + filename;
    handler.saveToFile(fullPath, *sheet);
    std::cout << "Spreadsheet saved successfully as " << filename << "\n";
}

void SheetHandler::loadSheet(const std::string &filename)
{
    if (sheets.find(filename) != sheets.end())
    {
        std::cout << "A spreadsheet with this filename is already loaded.\n";
        return;
    }

    Spreadsheet* newSheet = new Spreadsheet();  // Using raw pointer
    std::string fullPath = directory_path + "/" + filename;
    handler.loadFromFile(fullPath, *newSheet);
    sheets[filename] = newSheet;  // Store raw pointer
    std::cout << "Spreadsheet loaded successfully from " << filename << "\n";
}

Spreadsheet* SheetHandler::getSheet(const std::string &filename) const
{
    auto it = sheets.find(filename);
    if (it == sheets.end())
        throw std::invalid_argument("No spreadsheet found with the given filename.");
    return it->second;  // Return raw pointer
}

void SheetHandler::viewSavedSheets() const
{
    std::cout << "\n--- Saved Sheets ---\n";
    for (const auto &[filename, sheet] : sheets)
    {
        std::cout << filename << "\n";
    }
}

void SheetHandler::displayMenu() const
{
    std::cout << "\n--- Spreadsheet Manager Menu ---\n";
    std::cout << "1. Create a new spreadsheet\n";
    std::cout << "2. Run sheet\n";
    std::cout << "3. View saved sheets\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter your choice: ";
}

void SheetHandler::runMenu()
{
    std::string choice;

    do
    {
        displayMenu();
        std::getline(std::cin, choice);

        switch (std::stoi(choice))
        {
        case 1:
            handleCreate();
            break;
        case 2:
            handleRun();
            break;
        case 3:
            viewSavedSheets();
            break;
        case 4:
            std::cout << "Exiting the Spreadsheet Manager. Goodbye!\n";
            break;
        default:
            std::cout << "Invalid choice! Please try again.\n";
            break;
        }
    } while (choice != "4");
}

void SheetHandler::handleCreate()
{
    std::string rows, cols;
    std::cout << "Enter the number of rows: \n";
    std::getline(std::cin, rows);
    std::cout << "Enter the number of columns: \n";
    std::getline(std::cin, cols);

    std::string filename;
    std::cout << "Enter a filename for the new spreadsheet: \n";
    std::getline(std::cin, filename);

    Spreadsheet* newSheet = new Spreadsheet(std::stoi(rows), std::stoi(cols));  // Using raw pointer
    add(filename, newSheet);  // Store raw pointer in map
    std::cout << "Spreadsheet added successfully!\n";
}

void SheetHandler::handleRun()
{
    std::string filename;
    std::cout << "Enter the filename of the spreadsheet to run: \n";
    std::getline(std::cin, filename);

    try
    {
        Spreadsheet* sheet = getSheet(filename);
        sheet->run();

        std::string choice;
        std::cout << "Do you want to save this sheet? (y/n): ";
        std::getline(std::cin, choice);

        if (choice == "y" || choice == "Y")
        {
            try
            {
                handler.saveToFile(directory_path + "/" + filename, *sheet);  // Save the raw pointer's contents
                std::cout << "Sheet saved successfully!\n";
            }
            catch (const std::exception &e)
            {
                std::cout << "Error saving the sheet: " << e.what() << "\n";
            }
        }
        else
        {
            std::cout << "Sheet was not saved.\n";
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << "\n";
    }
}
