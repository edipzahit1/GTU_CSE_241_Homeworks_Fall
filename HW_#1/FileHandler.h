#ifndef HANDLE_EM
#define HANDLE_EM

#include <vector>
#include <string>
#include "Spreadsheet.h"

using namespace std;

class Spreadsheet;

class FileHandler 
{
public:
    FileHandler();

    void saveToFile(const string &filename, const Spreadsheet &spreadsheet);
    void loadFromFile(const string &filename, Spreadsheet &spreadsheet);

private:
    bool isInteger(const string& str);
    bool isDouble(const string& str);
};

#endif
