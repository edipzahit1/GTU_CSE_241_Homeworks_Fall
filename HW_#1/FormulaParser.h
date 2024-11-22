#ifndef PARSE_EM
#define PARSE_EM

#include <string>
#include <vector>
#include <iostream>
#include <set>
#include "Spreadsheet.h"
#include "Cell.h"

using namespace std;

class Spreadsheet;

enum class FunctionType 
{
    SUM, AVER, STDDEV, MAX, MIN, INVALID
};

class FormulaParser 
{
public:
    FormulaParser(Spreadsheet& spreadsheet) : spreadsheet(spreadsheet) {}

    double parseAndEvaluate(string& formula, pair<int, int> coordinates);
    void autoCalculate(pair<int, int> coordinate);
private:
    Spreadsheet& spreadsheet;

    vector<string> parsePlusAndMinus(const string& formula) const;
    vector<string> parseMultpAndDiv(const string& token) const;

    double evaluateMultpAndDivToken(string& token, set<pair<int, int>>& uniqueDependents) const;
    bool isValidRange(const string& range) const; 
    pair<int, int> getCellReference(const string& token) const;
    FunctionType getFunctionType(string& token) const;
    bool isValue(const string& token) const;
    double evaluateSingleToken(string& singleToken, set<pair<int, int>>& uniqueDependents) const;

    double SUM(pair<int, int> startPos, pair<int, int> endPos, set<pair<int, int>>& uniqueDependents) const;
    double AVER(pair<int, int> startPos, pair<int, int> endPos, set<pair<int, int>>& uniqueDependents) const;
    double STDDEV(pair<int, int> startPos, pair<int, int> endPos, set<pair<int, int>>& uniqueDependents) const;
    double MAX(pair<int, int> startPos, pair<int, int> endPos, set<pair<int, int>>& uniqueDependents) const;
    double MIN(pair<int, int> startPos, pair<int, int> endPos, set<pair<int, int>>& uniqueDependents) const;
};

#endif