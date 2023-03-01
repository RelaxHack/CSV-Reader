#include <iostream>
#include <fstream>
#include <string>
#include <algorithm> 
#include <vector>
#include <sstream>
#include <map>
using namespace std;

//The program is written with the expectation that the table can be large and it will be impossible to store it in memory.
static map<string, int> headerMap;
static map<string, int> rowsMap;
struct expression {
    int columnNumber, rowNumber; // self position
    string firstCellColumn, secondCellColumn;
    string firstCellRow, secondCellRow;
    int firstCellValue, secondCellValue;
    bool isFirstCellFull = false;
    bool isSecondCellFull = false;
    int selfValue;
    bool isCompleted = false;
    char action;
    //expression self cells
    string selfCellColumn, selfCellRow;
    int firstCellColumnReal, secondCellColumnReal;
    int firstCellRowReal, secondCellRowReal;

    expression(int& columnNumber, int& rowNumber, string& firstCellColumn, string& secondCellColumn, string& firstCellRow, string& secondCellRow, char& action) {
        this->columnNumber = columnNumber;
        this->rowNumber = rowNumber;
        if (firstCellColumn == "") {
            this->firstCellValue = stoi(firstCellRow);
            isFirstCellFull = true;
        }
        else {
            this->firstCellColumn = firstCellColumn;
        }
        if (secondCellColumn == "") {
            this->secondCellValue = stoi(secondCellRow);
            isSecondCellFull = true;
        }
        else {
            this->secondCellColumn = secondCellColumn;
        }
        this->firstCellRow = firstCellRow;
        this->secondCellRow = secondCellRow;
        this->action = action;
        setSelfCellColumn(columnNumber);
    }

    void setSelfCellColumn(int& columnNumber) {
        for (auto& item : headerMap) {
            if (item.second == columnNumber) {
                this->selfCellColumn = item.first;
                break;
            }
        }
    }

    void setSelfCellRow() {
        for (auto& item : rowsMap) {
            if (item.second == this->rowNumber) {
                this->selfCellRow = item.first;
                break;
            }
        }
    }

    void setSelfCellReal() {
        firstCellColumnReal = headerMap[this->firstCellColumn];
        secondCellColumnReal = headerMap[this->secondCellColumn];
        firstCellRowReal = rowsMap[this->firstCellRow];
        secondCellRowReal = rowsMap[this->secondCellRow];
    }

    bool isRequiredCell(int& columnNumber, int& rowNumber) {
        return ((this->firstCellColumnReal == columnNumber && this->firstCellRowReal == rowNumber)
            || (this->secondCellColumnReal == columnNumber && this->secondCellRowReal == rowNumber));
    }

    void setRequiredCell(int& columnNumber, int& rowNumber, int value) {
        if (this->firstCellColumnReal == columnNumber && this->firstCellRowReal == rowNumber && !isFirstCellFull) {
            this->firstCellValue = value;
            isFirstCellFull = true;
        }
        else if (this->secondCellColumnReal == columnNumber && this->secondCellRowReal == rowNumber && !isSecondCellFull) {
            this->secondCellValue = value;
            isSecondCellFull = true;
        }
    }

    void checkCompletedExpression() {
        if (!isCompleted && isFirstCellFull && isSecondCellFull) {
            switch (action)
            {
            case '+':
                selfValue = firstCellValue + secondCellValue;
                break;
            case '-':
                selfValue = firstCellValue - secondCellValue;
                break;
            case '*':
                selfValue = firstCellValue * secondCellValue;
                break;
            case '/':
                if (secondCellValue != 0) {
                    selfValue = firstCellValue / secondCellValue;
                }
                else {
                    cout << "Cell expression: " + selfCellColumn + selfCellRow << endl;
                    throw "ERROR: Divided by zero!";
                }
                break;
            }
            isCompleted = true;
        }
    }
};

static vector<expression> expressions;

void fillHeaderMap(vector<string>& header) {
    for (int i = 0; i < header.size(); i++) {
        if (headerMap.count(header[i])) {
            throw "ERROR: Duplicate column names!";
        }
        headerMap.insert(make_pair(header[i], i));
    }
}

void fillRowsMap(string& rowName, int& i) {
    if (rowsMap.count(rowName)) {
        throw "ERROR: Duplicate row names!";
    }
    rowsMap.insert(make_pair(rowName, i));
}

bool isLetter(char& ch) {
    return ch >= 'A' && ch <= 'Z' || ch >= 'a' && ch <= 'z';
}

bool isAction(char& ch) {
    return ch == '+' || ch == '-' || ch == '/' || ch == '*';
}

bool isOnlyLetters(string& str) {
    for (char ch : str) {
        if (!isLetter(ch)) {
            return false;
        }
    }
    return true;
}

bool isOnlyDigits(string& str) {
    if (str[0] == '+' || str[0] == '-') {
        str.erase(0, 1);
    }
    for (int i = 0; i < str.length(); i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

vector<string> cutStrToVector(string& fileLine, char& delim) {
    vector<string> elems;
    stringstream ss(fileLine);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

void removeSpacesAtBeginningAndEnd(string& str) {
    char space = ' ';
    while (str[0] == space) {
        str.erase(0, 1);
    }
    reverse(str.begin(), str.end());
    while (str[0] == space) {
        str.erase(0, 1);
    }
    reverse(str.begin(), str.end());
}

void headerAnalysis(vector<string>& elemsHeader) {
    if (elemsHeader.empty()) throw "ERROR: Header not on the first line or is missing!";
    if (elemsHeader[0] != "") throw "ERROR: First header element must contain an empty value or is missing!";
    for (string elem : elemsHeader) {
        if (!isOnlyLetters(elem)) {
            throw "ERROR: Header names contain invalid symbols!";
        }
    }
}

expression createExpression(string str, int& lineNumber, int& columnNumber) {
    str.erase(str.begin());
    int i = lineNumber;
    int j = columnNumber;
    string firstCellColumn = "";
    string secondCellColumn = "";
    string firstCellRow = "";
    string secondCellRow = "";
    char action;
    short processingStage = 1;
    for (char ch : str) {
        switch (processingStage)
        {
        case 1:
            if (isLetter(ch)) {
                firstCellColumn += ch;
                break;
            }
            else if (isdigit(ch)) {
                firstCellRow += ch;
                processingStage = 2;
                break;
            }
        case 2:
            if (isdigit(ch)) {
                firstCellRow += ch;
                break;
            }
            else if (isAction(ch)) {
                action = ch;
                processingStage = 3;
                break;
            }
        case 3:
            if (isLetter(ch)) {
                secondCellColumn += ch;
                break;
            }
            else if (isdigit(ch)) {
                secondCellRow += ch;
                processingStage = 4;
                break;
            }
        case 4:
            if (isdigit(ch)) {
                secondCellRow += ch;
                break;
            }
            else {
                break;
            }
        }
    }
    expression exp = expression(i, j, firstCellColumn, secondCellColumn, firstCellRow, secondCellRow, action);
    return exp;
}

bool isCorrectExpression(string str) {
    if (str[0] != '=') return false;
    str.erase(str.begin());
    short verificationStage = 0;
    for (char ch : str) {
        switch (verificationStage)
        {
        case 0:
            if (isLetter(ch)) {
                verificationStage = 1;
                break;
            }
            else if (isdigit(ch)) {
                verificationStage = 2;
            }
            else {
                return false;
            }
            break;
        case 1:
            if (isLetter(ch)) {
                break;
            }
            else if (isdigit(ch)) {
                verificationStage = 2;
                break;
            }
            else {
                return false;
            }
        case 2:
            if (isdigit(ch)) {
                break;
            }
            else if (isAction(ch)) {
                verificationStage = 3;
                break;
            }
            else {
                return false;
            }
        case 3:
            if (isLetter(ch)) {
                break;
            }
            else if (isdigit(ch)) {
                verificationStage = 4;
                break;
            }
            else {
                return false;
            }
        case 4:
            if (isdigit(ch)) {
                break;
            }
            else {
                return false;
            }
        }
    }
    if (verificationStage == 4) {
        return true;
    }
    else {
        return false;
    }
}

bool isSpacesInMiddleOfNumber(string& str) {
    removeSpacesAtBeginningAndEnd(str);
    for (char ch : str) {
        if (ch == ' ') {
            return true;
        }
    }
    return false;
}

void lineNumberCheck(string str) {
    removeSpacesAtBeginningAndEnd(str);
    if (str[0] == '0' || str[0] == '-' || str[0] == '=') {
        throw "ERROR: Line numbering must be a non-zero positive integer!";
    }
}

void elemsAnalysis(vector<string>& elemsRow, int& countRows) {
    for (int countColumns = 0; countColumns < elemsRow.size(); countColumns++) {
        if (countColumns == 0) {
            lineNumberCheck(elemsRow[0]);
        }
        if (elemsRow[countColumns].empty()) {
            throw "ERROR: Cell or row number is empty!"; // according to conditions, cells cannot be empty
        }
        if (elemsRow[countColumns][0] == '0' && elemsRow[countColumns].size() != 1) {
            throw "An integer cannot start from zero!";
        }
        if (elemsRow[countColumns][0] == '=') {
            if (isCorrectExpression(elemsRow[countColumns])) {
                expressions.push_back(createExpression(elemsRow[countColumns], countColumns, countRows));
            }
            else {
                throw "ERROR: Cell contains incorrect expression!";
            }
        }
        else {
            if (isSpacesInMiddleOfNumber(elemsRow[countColumns]) || !isOnlyDigits(elemsRow[countColumns])) {
                throw "ERROR: Cell or row number contains incorrect data!";
            }
            try {
                stoi(elemsRow[countColumns]);
            }
            catch (...) {
                throw "ERROR: Cell or row number contains incorrect data or out of range int!";
            }
        }
    }
}

void setRowsAndRealForExpressions() {
    for (expression& item : expressions) {
        item.setSelfCellRow();
    }
    for (expression& item : expressions) {
        item.setSelfCellReal();
    }
}

void throwRequiredCellMissingError(string& cellColumn, string& cellRow) {
    cout << "Cell " << cellColumn << cellRow << " is not exist" << endl;
    throw "ERROR: Required cell to calculate expression is missing!";
}

void checkForExistenceOfRequiredCells() {
    for (expression& item : expressions) {
        if (item.firstCellColumn != "" && !(headerMap.count(item.firstCellColumn) && rowsMap.count(item.firstCellRow))) {
            throwRequiredCellMissingError(item.firstCellColumn, item.firstCellRow);
        }
        if (item.secondCellColumn != "" && !(headerMap.count(item.secondCellColumn) && rowsMap.count(item.secondCellRow))) {
            throwRequiredCellMissingError(item.secondCellColumn, item.secondCellRow);
        }
    }
}

void searchLoopingInExpression(expression& parent, string& progenitorCellColumn, string& progenitorCellRow) {
    for (expression possibleChild : expressions) {
        if ((progenitorCellColumn == parent.firstCellColumn) && (progenitorCellRow == parent.firstCellRow)
            || ((progenitorCellColumn == parent.secondCellColumn) && (progenitorCellRow == parent.secondCellRow))) {
            throw "ERROR: Found cyclicity in expressions!";
        }
        else if ((possibleChild.selfCellColumn == parent.firstCellColumn) && (possibleChild.selfCellRow == parent.firstCellRow)
            || (possibleChild.selfCellColumn == parent.secondCellColumn) && (possibleChild.selfCellRow == parent.secondCellRow)) {
            searchLoopingInExpression(possibleChild, progenitorCellColumn, progenitorCellRow);
        }
    }
}

void checkingLoopedExpressions() {
    string progenitorCellColumn;
    string progenitorCellRow;
    for (expression& progenitor : expressions) {
        // transfer your cell to 
        progenitorCellColumn = progenitor.selfCellColumn;
        progenitorCellRow = progenitor.selfCellRow;
        searchLoopingInExpression(progenitor, progenitorCellColumn, progenitorCellRow);
    }
}

void tableAnalysis(ifstream& csvFile, char& delim) {
    int countRows = 1;
    string fileLine;
    getline(csvFile, fileLine);
    vector<string> elemsHeader = cutStrToVector(fileLine, delim);
    headerAnalysis(elemsHeader);
    fillHeaderMap(elemsHeader);
    int number_columns = elemsHeader.size();
    vector<string> elemsRow;
    while (getline(csvFile, fileLine)) {
        if (fileLine == "") {
            continue;
        }
        elemsRow = cutStrToVector(fileLine, delim);
        if (number_columns != elemsRow.size()) throw "ERROR: Wrong number of columns or last element not set!";
        elemsAnalysis(elemsRow, countRows);
        fillRowsMap(elemsRow[0], countRows);
        countRows++;
    }
    checkForExistenceOfRequiredCells();
    setRowsAndRealForExpressions();
    checkingLoopedExpressions();
}

bool checkCompletenessExpressions() {
    int countCompleteExpression = 0;
    for (expression& item : expressions) {
        item.checkCompletedExpression();
        if (item.isCompleted) {
            countCompleteExpression++;
        }
    }
    return countCompleteExpression == expressions.size();
}

void fillRequiredCell(string& rowElement, int& countColumns, int& countRows) {
    if (rowElement[0] == '=') {
        for (expression& item : expressions) {
            if (item.columnNumber == countColumns && item.rowNumber == countRows) { // self search
                if (item.isCompleted) {
                    for (expression& item2 : expressions) {
                        if (item2.isRequiredCell(countColumns, countRows)) {
                            item2.setRequiredCell(countColumns, countRows, item.selfValue); //the expression looks for its cell in the required cells of other expressions, in the case when it is calculated
                        }
                    }
                }
            }
        }
    }
    else {
        for (expression& item : expressions) {
            if (item.isRequiredCell(countColumns, countRows)) {
                item.setRequiredCell(countColumns, countRows, stoi(rowElement));
            }
        }
    }
}

void expressionEvaluation(ifstream& csvFile, char& delim) {
    int i = 0;
    int countRows;
    string fileLine;
    vector<string> elemsRow;
    while (!checkCompletenessExpressions()) {
        countRows = 1;
        csvFile.clear();
        csvFile.seekg(0);
        getline(csvFile, fileLine);
        while (getline(csvFile, fileLine)) {
            if (fileLine == "") {
                continue;
            }
            elemsRow = cutStrToVector(fileLine, delim);
            for (int countColumns = 1; countColumns < elemsRow.size(); countColumns++) {
                fillRequiredCell(elemsRow[countColumns], countColumns, countRows);
            }
            countRows++;
        }
    }

}

void printElement(string& rowElement, int& countColumns, int& countRows) {
    if (rowElement[0] == '=') {
        for (expression& item : expressions) {
            if (item.columnNumber == countColumns && item.rowNumber == countRows) {
                cout << item.selfValue;
            }
        }
    }
    else {
        if (rowElement[0] == '+' || rowElement[0] == ' ' || rowElement[rowElement.size() - 1] == ' ') {
            cout << stoi(rowElement);
        }
        else {
            cout << rowElement;
        }

    }
}

void printTable(ifstream& csvFile, char& delim) {
    csvFile.clear();
    csvFile.seekg(0);
    int i = 0;
    int countRows = 1;
    string fileLine;
    vector<string> elemsRow;
    int elemsRowSize;
    getline(csvFile, fileLine);
    cout << fileLine << endl;
    while (getline(csvFile, fileLine)) {
        if (fileLine == "") {
            continue;
        }
        elemsRow = cutStrToVector(fileLine, delim);
        elemsRowSize = elemsRow.size();
        for (int countColumns = 0; countColumns < elemsRowSize; countColumns++) {
            printElement(elemsRow[countColumns], countColumns, countRows);
            if (countColumns != (elemsRowSize - 1)) {
                cout << delim;
            }
        }
        cout << endl;
        countRows++;
    }
}

int main(int argc, char* argv[])
{
    if (argc == 1) {
        cout << "Pass the name of the csv file as an argument!" << endl;
        return -1;
    }
    string fileName = argv[1];
    char delim = ','; // Comma-Separated Values
    ifstream csvFile(fileName);
    if (!csvFile.is_open()) {
        cout << "File cannot be opened!";
        return -2;
    }
    try {
        tableAnalysis(csvFile, delim);
        expressionEvaluation(csvFile, delim);
        printTable(csvFile, delim);
    }
    catch (const char* msg) {
        csvFile.close();
        cout << msg << endl;
        return -3;
    }
    csvFile.close();
    return 0;
}