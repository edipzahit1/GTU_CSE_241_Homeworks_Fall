#include <iostream>
#include <string>
#include <sstream>
#include "Spreadsheet.h"
#include "FileHandler.h"
#include "FormulaParser.h"
#include "AnsiTerminal.h"

using namespace std;

int main() {
    // Create a new Spreadsheet with 5 rows and 5 columns
    Spreadsheet spreadsheet(5, 5); 

    // Create a FileHandler for saving/loading files
    FileHandler fileHandler;

    fileHandler.loadFromFile("spreadsheet.csv", spreadsheet);
    spreadsheet.run();

    FormulaParser parser(spreadsheet);


    // Populate some cells with example data and formulas
    spreadsheet.getCell(0, 0)->setContent(10); // Set A1 to 10
    spreadsheet.getCell(0, 1)->setContent(5);  // Set B1 to 5
    spreadsheet.getCell(1, 0)->setContent("Hello");  // Set A2 to "Hello"
    string formula = "=A1 + B1";
    spreadsheet.getCell(2, 0)->setContent(formula, parser, {2, 0});  // Formula in C1 (A1 + B1)

    // View some initial values
    cout << "Initial values:" << endl;
    cout << "A1: " << spreadsheet.getContentAsString(0, 0) << endl;
    cout << "B1: " << spreadsheet.getContentAsString(0, 1) << endl;
    cout << "C1: " << spreadsheet.getContentAsString(2, 0) << endl;

    char command;
    cout << "Enter 'r' to run the sheet.\n";
    cin >> command;
    if (command == 'r')
        spreadsheet.run();  // Calls the run function to interact with the user

    fileHandler.saveToFile("spreadsheet.csv", spreadsheet);

    //Spreadsheet anotherSheet(10, 10);
    
    //fileHandler.loadFromFile("spreadsheet.csv", anotherSheet); 

    //anotherSheet.displayScreen(1,1);

    return 0;
}
