#ifndef HEADERS_H
#define HEADERS_H

#include <iostream>
#include <string>
#include <vector>
#include <pwd.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <cstring>
#include <cstring>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <iomanip>
#include <queue>
#include <fstream>
#include <deque>
#include <regex>
#include <fcntl.h> 
#include <libproc.h>
#include <sys/proc_info.h>
#include <sys/proc.h>
#include <signal.h>

using namespace std;

struct CommandNode {
    string command;              
    vector<string> args;         
    CommandNode* next;           

    CommandNode(string cmd = "") : command(cmd), next(nullptr) {}
};

string getUserName();
string getCWD();
string getSysName();
string removeSpaces(string input);
bool searchFile(const string& filename, const string& startDir);
void processCommand(string command);
void executeCommandTree(CommandNode* root);
CommandNode* parseCommandTree(const string& input);

#endif
