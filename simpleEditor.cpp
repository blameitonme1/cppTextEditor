#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;
# define BUFFERSIZE 100
class TextEditor {
private:
    vector<string> buffer; // 缓冲区，储存正在编辑器操作的文本
    int currentLine; // 储存当前行，也就是正在编辑的行数
    vector<pair<vector<string>,int>> undoStack; // 实现undo操作, 思路就是保存当前状态，包括缓冲区内容和cursor的位置
    vector<pair<vector<string>,int>> redoStack; // 实现redo，就是undo的时候将状态存在这个stack里即可
public:
    TextEditor() : currentLine(0) {}
    // 输出帮助信息
    void displayHelp() {
        cout << "Command List:\n";
        cout << "R: Read a file\n";
        cout << "s: show current line\n";
        cout << "W: Write to a file\n";
        cout << "I: Insert a new line\n";
        cout << "D: Delete current line\n";
        cout << "F: Find a target string\n";
        cout << "C: Replace a string\n";
        cout << "Q: Quit the editor\n";
        cout << "N: Move to the next line\n";
        cout << "P: Move to the previous line\n";
        cout << "B: Move to the beginning of the buffer\n";
        cout << "E: Move to the end of the buffer\n";
        cout << "G: Go to a specific line\n";
        cout << "V: View the entire buffer content\n";
        cout << "U: Undo\n";
        cout << "Z: Redo\n";
        cout << "Help or ?: Display help messages\n";
    }
    // 将当前状态保存在栈中
    void pushUndoStack() {
        undoStack.push_back({buffer, currentLine});
        redoStack.clear(); // 清空redo的状态
    }
    // 实现撤销功能，从stack取出上一个状态然后回溯
    void undo() {
        if (!undoStack.empty()) {
            pair<vector<string>, int> previousState = undoStack.back();
            undoStack.pop_back();
            // redo stack要加入当前状态
            redoStack.push_back({buffer, currentLine});

            buffer = previousState.first;
            currentLine = previousState.second;

            cout << "Undo successful :) \n";
        } else {
            cout << "Nothing to undo :( \n";
        }
    }
    // redo的函数，思路就是读取redostack的top值
    void redo() {
        if (!redoStack.empty()) {
            pair<vector<string>, int> nextState = redoStack.back();
            redoStack.pop_back();
            // 记得要存储当前
            undoStack.push_back({buffer, currentLine});

            buffer = nextState.first;
            currentLine = nextState.second;

            cout << "Redo successful :) \n";
        } else {
            cout << "Nothing to redo :( \n";
        }
    }
    // 读取文件的内容进入缓冲区buffer
    void readFromFile(const string& filename) {
        ifstream inFile(filename);
        buffer.clear();
        int count = 0; // 默认cursor存在末尾
        if (inFile.is_open()) {
            string line;
            cout << "content of " << filename << " is as follows: \n\n";
            while (getline(inFile, line)) {
                buffer.push_back(line);
                ++count;
                // 输出读取的内容到terminal上
                cout<<line<<endl;
            }
            inFile.close();
            if(buffer.empty()){
                // 防止没有内容的时候，buffersize导致 M 出现错误
                buffer.push_back("");
            }
            cout << "\n File has been read :) \n";
            currentLine = count;
        } else {
            cout << "Unable to open the file\n :( ";
        }
    }
    // 将缓冲区的内容写入到文件中
    void writeToFile(const string& filename) {
        ofstream outFile(filename);
        if (outFile.is_open()) {
            for (const string& line : buffer) {
                outFile << line << "\n";
            }
            outFile.close();
            cout << "File has been successfully written! :) \n";
        } else {
            cout << "Unable to save the file :( \n";
        }
        undoStack.clear();
    }
    // 在编辑器里面插入新的一行
    void insertLine(int lineNumber, const string& newLine) {
        // 检查 line number 是否 valid
        if (lineNumber >= 0 && lineNumber <= buffer.size()) {
            buffer.insert(buffer.begin() + lineNumber, newLine);
            cout << "New line has been inserted :) \n";
            currentLine = lineNumber;
        } else {
            cout << "Invalid line number :( \n";
        }
    }
    // 删除当前行，移动到下一行
    void deleteCurrentLine() {
        if (!buffer.empty()) {
            buffer.erase(buffer.begin() + currentLine);
            // 考虑最后一行的情况
            if (currentLine >= buffer.size()) {
                currentLine = buffer.size() - 1;
            }
            cout << "Current line has been deleted :)\n";
        } else {
            cout << "Buffer is empty :( \n";
        }
    }
    // 替换成replacement，仅限于当前行使用
    void findAndReplace(const string& target, const string& replacement) {
        if (!buffer.empty()) {
            string& currentLineText = buffer[currentLine];
            // 查找目标字符串
            size_t pos = currentLineText.find(target);
            while (pos != string::npos) {
                currentLineText.replace(pos, target.length(), replacement);
                pos = currentLineText.find(target, pos + replacement.length());
            }
            cout << "String has been replaced :) \n";
        } else {
            cout << "Buffer is empty :( \n";
        }
    }
    // 显示缓冲区的内容
    void displayBuffer() {
        cout << "\nbuffer content is :\n\n";
        for (const string& line : buffer) {
            cout << line << "\n";
        }
    }
    // 移动到指定的一行去
    void moveCursorToLine(int lineNumber) {
        if (lineNumber >= 0 && lineNumber < buffer.size()) {
            currentLine = lineNumber;
            cout << "Cursor has been moved to line " << lineNumber << " :) \n";
        } else {
            cout << "Invalid line number :(\n";
        }
    }
    // 新的成员函数，用于修改指定行的内容
    void modifyLine(int lineNumber, const string& newContent) {
        // 检查行号是否有效
        if (lineNumber >= 0 && lineNumber < buffer.size()) {
            // 保存当前状态到 Undo 栈
            pushUndoStack();
            buffer[lineNumber] = newContent;
            cout << "Line " << lineNumber << " has been modified :) \n";
        } else {
            cout << "Invalid line number :( \n";
        }
    }
    // 当前行开始查找目标字符串
    int find(const string& target) {
    // 检查当前行是否在合法范围内
    if (currentLine < 0 || currentLine >= buffer.size()) {
        cout << "Invalid current line number :( \n";
        return -1; // 返回 -1 表示未找到
    }
    // 从当前行开始查找目标串
    size_t pos = buffer[currentLine].find(target);

    if (pos != string::npos) {
        cout << "Target string found in current line " << currentLine << " at position " << pos << " :) \n";
        return currentLine;
    } else {
            // 如果在当前行没找到，可以在之后的行中查找
            for (int i = currentLine + 1; i < buffer.size(); ++i) {
                pos = buffer[i].find(target);
                if (pos != string::npos) {
                    cout << "Target string found in line " << i << " at position " << pos << " :) \n";
                    currentLine = i; // 更新当前行
                    return i;
                }
            }
            // 如果整个缓冲区都没找到目标串
            cout << "Target string not found in the buffer :( \n";
            return -1; // 返回 -1 表示未找到
        }
    }
    // 移到下一行
    void moveToNextLine() {
        if (currentLine < buffer.size() - 1) {
            ++currentLine;
            cout << "Moved to the next line (Line " << currentLine << ") :) \n";
        } else {
            cout << "Already at the last line :( \n";
        }
    }
    // 移到上一行
    void moveToPreviousLine() {
        if (currentLine > 0) {
            --currentLine;
            cout << "Moved to the previous line (Line " << currentLine << ") :) \n";
        } else {
            cout << "Already at the first line :( \n";
        }
    }
    // 移到缓冲区最开始
    void moveToBeginningOfBuffer() {
        currentLine = 0;
        cout << "Moved to the beginning of the buffer (Line 0) :) \n";
    }
    // 移到缓冲区最后一行
    void moveToEndOfBuffer() {
        currentLine = buffer.size() - 1;
        cout << "Moved to the end of the buffer (Line " << currentLine << ") :) \n";
    }
    void showCurrentLine(){
        cout << "current line is " << currentLine << endl;
    }
};

int main() {

    TextEditor editor;
    string filename;

    cout << "Enter the file name: ";
    cin >> filename;
    editor.readFromFile(filename);

    string command;
    // 除非输入Q 或者 q，一直执行指令
    do {
        cout << "Enter a command :) (type Help or ? for help): ";
        cin >> command;

        if (command == "R") {
            cout << "Enter the new file name: ";
            cin >> filename;
            editor.readFromFile(filename);
        }
        else if(command == "s") {
            editor.showCurrentLine();
        } 
        else if (command == "W") {
            cout << "Enter the new file name: ";
            cin >> filename;
            editor.writeToFile(filename);
        } else if (command == "I") {
            int lineNumber;
            cout << "Enter the line number for the new line: ";
            cin >> lineNumber;
            cin.ignore();
            string newLine;
            cout << "Enter the content for the new line: ";
            getline(cin, newLine);
            editor.insertLine(lineNumber, newLine);
        } else if (command == "D") {
            editor.deleteCurrentLine();
        } else if (command == "F") {
            string target;
            cout << "Enter the target string: ";
            cin >> target;
            // 调用find函数
            int foundLine = editor.find(target);
            if (foundLine != -1) {
                cout << "Target string found in line " << foundLine << " :) \n";
            } else {
                cout << "Target string not found :( \n";
            }
        } else if (command == "C") {
            string target, replacement;
            cout << "Enter the string to replace: ";
            cin >> target;
            cout << "Enter the replacement string: ";
            cin >> replacement;
            editor.findAndReplace(target, replacement);
        } else if (command == "Q") {
            // 退出
            break;
        } else if (command == "Help" || command == "?") {
            editor.displayHelp();
        } else if (command == "N") {
            editor.moveToNextLine();
            
        } else if (command == "P") {
            editor.moveToPreviousLine();
        } else if (command == "B") {
            // Move to the beginning of the buffer
            editor.moveToBeginningOfBuffer();
        } else if (command == "E") {
            // Move to the end of the buffer
            editor.moveToEndOfBuffer();
        } else if (command == "G") {
            int lineNumber;
            cout << "Enter the line number to move to: ";
            cin >> lineNumber;
            editor.moveCursorToLine(lineNumber);
        } else if (command == "V") {
            editor.displayBuffer();
        } else if (command == "U") {
            editor.undo();
        } else if (command == "Z") {
            editor.redo();
        }
        else if (command == "M") {
            int lineNumber;
            cout << "Enter the line number to modify: ";
            cin >> lineNumber;
            cin.ignore();
            string newContent;
            cout << "Enter the new content for the line: ";
            getline(cin, newContent);
            editor.modifyLine(lineNumber, newContent);}
        else {
            cout << "Unknown command, please try again :( \n";
        }

    } while (command != "Q" || command != "q");

    return 0;
}

