#include "headers.h"

using namespace std;

string getRelativePath(string cwd) {
    string root = "/Users/priyanshusharma/Documents/AOS_Assignment2";
    if (cwd.find(root) == 0) {
        return "~" + cwd.substr(root.length());
    }
    return cwd;
}

vector<string> splitCommands(string& input) {
    vector<string> commands;
    stringstream ss(input);
    string command;
    while (getline(ss, command, ';')) {
        command = removeSpaces(command);
        if (!command.empty()) {
            commands.push_back(command);
        }
    }
    return commands;
}



int main() {
    while (1) {
        string cwd = getCWD();
        string relativeCWD = getRelativePath(cwd);
        cout << getUserName() << "@" << getSysName() << ":" << relativeCWD << "> ";

        string input;
        getline(cin, input);

        input = removeSpaces(input);

        if (input == "exit") {
            break;
        }

        vector<string> commands = splitCommands(input);

        for (string& command : commands) {
            if (command.find('|') != string::npos) {
                CommandNode* root = parseCommandTree(command);
                executeCommandTree(root);
                delete root;
            } else {
                processCommand(command);
            }
        }

        
    }
    return 0;
}
