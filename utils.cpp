#include "headers.h"


using namespace std;

void appendToHistory(const string& command) {
    static const size_t maxHistorySize = 20;
    static const string historyFile = "/Users/priyanshusharma/Documents/AOS_Assignment2/history.txt";

    deque<string> history;
    ifstream inFile(historyFile);
    string line;
    while (getline(inFile, line)) {
        history.push_back(line);
    }
    inFile.close();

    if (!history.empty() && history.back() == command) {
        return; 
    }

    if (history.size() >= maxHistorySize) {
        history.pop_front(); 
    }
    history.push_back(command);

    std::ofstream outFile(historyFile);
    for (const auto& entry : history) {
        outFile << entry << std::endl;
    }
    outFile.close();
}

void printHistory(size_t num) {
    static const string historyFile = "/Users/priyanshusharma/Documents/AOS_Assignment2/history.txt";
    ifstream inFile(historyFile);
    string line;
    deque<string> history;
    while (getline(inFile, line)) {
        history.push_back(line);
    }
    inFile.close();

    size_t count = 0;
    for (auto it = history.rbegin(); it != history.rend(); ++it) {
        if (count >= num) break;
        cout << *it << endl;
        ++count;
    }
}

string getUserName() {
    struct passwd *pw = getpwuid(getuid());
    return string(pw->pw_name);
}

string getCWD() {
    char *cwd = getcwd(NULL, 0);
    string currentDir(cwd);
    free(cwd);
    return currentDir;
}

string getSysName() {
    struct utsname sys_info;
    if (uname(&sys_info) == 0) {
        return string(sys_info.nodename);
    } else {
        return string();
    }
}

string removeSpaces(string input) {
    size_t start = 0;
    while (start < input.size() && (isspace(input[start]) || input[start] == '\n')) {
        start++;
    }

    size_t end = input.size();
    while (end > start && (isspace(input[end - 1]) || input[end - 1] == '\n')) {
        end--;
    }

    return input.substr(start, end - start);
}

bool searchFile(const string& filename, const string& startDir) {
    queue<string> dirs;
    dirs.push(startDir);

    while (!dirs.empty()) {
        string currentDir = dirs.front();
        dirs.pop();

        DIR* dir = opendir(currentDir.c_str());
        if (!dir) {
            perror(("search: cannot access " + currentDir).c_str());
            continue;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            string entryName = entry->d_name;

            if (entryName == "." || entryName == "..") {
                continue;
            }

            string fullPath = currentDir + "/" + entryName;

            if (entryName == filename) {
                closedir(dir);
                return true;
            }

            if (entry->d_type == DT_DIR) {
                dirs.push(fullPath);
            }
        }

        closedir(dir);
    }

    return false;
}

void listDirectory(const string& path, bool showAll, bool showLong) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        perror(("ls: cannot access " + path).c_str());
        return;
    }

    struct dirent* entry;
    vector<string> files;

    while ((entry = readdir(dir)) != nullptr) {
        if (!showAll && entry->d_name[0] == '.') {
            continue;
        }
        files.push_back(entry->d_name);
    }

    closedir(dir);

    if (showLong) {
        struct stat fileStat;
        for (const auto& file : files) {
            string fullPath = path + "/" + file;
            if (stat(fullPath.c_str(), &fileStat) < 0) {
                perror(("stat error for " + file).c_str());
                continue;
            }

            // File type and permissions
            cout << ((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
            cout << ((fileStat.st_mode & S_IRUSR) ? "r" : "-");
            cout << ((fileStat.st_mode & S_IWUSR) ? "w" : "-");
            cout << ((fileStat.st_mode & S_IXUSR) ? "x" : "-");
            cout << ((fileStat.st_mode & S_IRGRP) ? "r" : "-");
            cout << ((fileStat.st_mode & S_IWGRP) ? "w" : "-");
            cout << ((fileStat.st_mode & S_IXGRP) ? "x" : "-");
            cout << ((fileStat.st_mode & S_IROTH) ? "r" : "-");
            cout << ((fileStat.st_mode & S_IWOTH) ? "w" : "-");
            cout << ((fileStat.st_mode & S_IXOTH) ? "x" : "-");

            cout << " " << fileStat.st_nlink;

            cout << " " << setw(8) << fileStat.st_uid;
            cout << " " << setw(8) << fileStat.st_gid;

            cout << " " << setw(8) << fileStat.st_size;

            cout << " " << ctime(&fileStat.st_mtime);

            cout << " " << file << endl;
        }
    } else {
        for (const auto& file : files) {
            cout << file << "  ";
        }
        cout << endl;
    }
}

void processEchoCommand(string& command) {
    regex rgx(R"((\"[^\"]*\")|(\S+))");
    smatch match;
    string toPrint = "";
    string::const_iterator searchStart(command.cbegin());

    while (regex_search(searchStart, command.cend(), match, rgx)) {
        string part = match[1];

        if (part.front() == '"' && part.back() == '"') {
            part = part.substr(1, part.size() - 2);
        }
        toPrint += part + " ";

        searchStart = match.suffix().first;
    }

    if (!toPrint.empty()) {
        toPrint.pop_back();
    }

    cout << toPrint << endl;
}

string getMemoryUsage(pid_t pid) {
    proc_taskinfo taskInfo;
    int result = proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &taskInfo, sizeof(taskInfo));

    if (result <= 0) {
        return "Unable to get memory usage";
    }

    long long memoryUsageKB = taskInfo.pti_virtual_size / 1024;

    return to_string(memoryUsageKB) + " KB";
}

string getExecutablePath(pid_t pid) {
    char path[PATH_MAX];
    int result = proc_pidpath(pid, path, sizeof(path));

    if (result <= 0) {
        return "Unable to get executable path";
    }

    return string(path);
}

string getProcessStatus(pid_t pid) {
    struct proc_bsdinfo proc;
    std::string status;

    if (proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &proc, sizeof(proc)) > 0) {
        cout << PROC_PIDTBSDINFO << endl;
        switch (proc.pbi_status) {
            case 1:
                status = "R";
                break;
            case 2:
                status = "S";
                break;
            case 4:
                status = "Z";
                break;
            case 8:
                status = "T";
                break;
            default:
                status = "Unknown";
        }

        pid_t foreground = tcgetpgrp(STDIN_FILENO);  
        if (foreground == proc.pbi_pgid) {          
            status += "+";  
        }

    } else {
        status = "Unable to get process status";
    }

    return status;
}


void printProcessInfo(pid_t pid) {
    proc_taskinfo taskInfo;
    int result = proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &taskInfo, sizeof(taskInfo));
    
    if (result <= 0) {
        perror("Failed to get process info");
        return;
    }
    
    string status = getProcessStatus(pid);
    string memoryUsage = getMemoryUsage(pid);
    string executablePath = getExecutablePath(pid);
    
    cout << "PID: " << pid << endl;
    cout << "STATUS: " << status << endl;
    cout << "MEMORY USAGE:" << memoryUsage << endl;
    cout << "EXECUTABLE PATH: " << executablePath << endl;
}


void bringToForeground(pid_t pid) {
    if (kill(pid, SIGCONT) == -1) {
        perror("Failed to bring process to foreground");
        return;
    }
    int status;
    waitpid(pid, &status, WUNTRACED);
    if (WIFSTOPPED(status)) {
        printf("Process %d stopped\n", pid);
    } else if (WIFEXITED(status)) {
        printf("Process %d exited\n", pid);
    }
}

void executeCommand(vector<string> tokens) {
    int background = 0;

    // Check if the last token is "&"
    if (tokens.back() == "&") {
        background = 1;  // Set the background flag
        tokens.pop_back();  // Remove the "&" from the tokens
    }

    // Convert vector of tokens to array of C strings
    vector<char*> args;
    for (auto &token : tokens) {
        args.push_back(&token[0]);
    }
    args.push_back(nullptr);

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process: Execute the command
        if (execvp(args[0], args.data()) == -1) {
            perror("Command execution failed");
            exit(1);
        }
    } else {
        // Parent process
        if (!background) {
            // Wait for the child process to complete if not running in the background
            waitpid(pid, NULL, 0);
        } else {
            cout << "Process running in background priyanshu with PID: " << pid << endl;
        }
    }
}

void processCommand(string command) {
    static string previousDir = getCWD();

    vector<string> tokens;
    string token;
    stringstream ss(command);
    
    while (ss >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        return;
    }

    bool runInBackground = false;
    if (tokens.back() == "&") {
        runInBackground = true;
        tokens.pop_back();
    }

    bool commandSuccess = false;

    if (tokens[0] == "cd") {
        string newDir;
        if (tokens.size() == 1 || tokens[1] == "~") {
            newDir = "/Users/priyanshusharma/Documents/AOS_Assignment2";
        } else if (tokens[1] == "-") {
            newDir = previousDir;
        } else {
            newDir = tokens[1];
        }

        string currentDir = getCWD();
        if (chdir(newDir.c_str()) != 0) {
            perror("chdir failed");
        } else {
            previousDir = currentDir;
            commandSuccess = true;
        }

    } else if (tokens[0] == "ls") {
        bool showAll = false;
        bool showLong = false;
        vector<string> directories;

        for (size_t i = 1; i < tokens.size(); ++i) {
            if (tokens[i] == "-a") {
                showAll = true;
            } else if (tokens[i] == "-l") {
                showLong = true;
            } else if(tokens[i] == "-la" || tokens[i] == "-al"){
                showAll = true;
                showLong = true;
            } else {
                directories.push_back(tokens[i]);
            }
        }

        if (directories.empty()) {
            directories.push_back(".");
        }

        for (const auto& dir : directories) {
            cout << dir << ":\n";
            listDirectory(dir, showAll, showLong);
        }
        commandSuccess = true;

    } else if (tokens[0] == "echo") {
        processEchoCommand(command);
        commandSuccess = true;

    } else if (tokens[0] == "pwd") {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            cout << cwd << endl;
            commandSuccess = true;
        } else {
            perror("pwd failed");
        }

    } else if (tokens[0] == "search") {
        if (tokens.size() > 1) {
            bool found = searchFile(tokens[1], ".");
            if (found) {
                cout << "True" << endl;
            } else {
                cout << "False" << endl;
            }
            commandSuccess = true;
        } else {
            cerr << "search: missing argument\n";
        }

    } else if (tokens[0] == "history") {
        if (tokens.size() == 1) {
            printHistory(10);
        } else if (tokens.size() == 2) {
            size_t num = std::stoul(tokens[1]);
            printHistory(num);
        } else {
            cerr << "history: invalid arguments\n";
        }
        commandSuccess = true;

    } else if (tokens[0] == "pinfo") {
        if (tokens.size() == 1) {
            pid_t pid = getpid(); // Get current process ID
            printProcessInfo(pid);
        } else if (tokens.size() == 2) {
            pid_t pid = stoi(tokens[1]);
            printProcessInfo(pid);
        } else {
            cerr << "pinfo: invalid arguments\n";
        }
        commandSuccess = true;

    } else if(tokens[0] == "fg"){
        if(tokens.size() == 2) {
            pid_t pid = stoi(tokens[1]);
            bringToForeground(pid);
        } else {
            cerr << "fg: invalid arguments\n";
        }
        
    } else {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
        } else if (pid == 0) {
            int inFd = -1, outFd = -1;
            setsid();
            auto it = find(tokens.begin(), tokens.end(), "<");
            if (it != tokens.end()) {
                string inputFile = *(it + 1);
                inFd = open(inputFile.c_str(), O_RDONLY);
                if (inFd < 0) {
                    perror("Failed to open input file");
                    exit(EXIT_FAILURE);
                }
                dup2(inFd, STDIN_FILENO);
                close(inFd);
                tokens.erase(it, it + 2);
            }
            it = find(tokens.begin(), tokens.end(), ">");
            if (it != tokens.end()) {
                string outputFile = *(it + 1);
                outFd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                if (outFd < 0) {
                    perror("Failed to open output file");
                    exit(EXIT_FAILURE);
                }
                dup2(outFd, STDOUT_FILENO);
                close(outFd);
                tokens.erase(it, it + 2);
            }

            vector<char*> args;
            for (const auto& arg : tokens) {
                args.push_back(const_cast<char*>(arg.c_str()));
            }
            args.push_back(nullptr);

            if (execvp(args[0], args.data()) == -1) {
                perror("Command execution failed");
                exit(EXIT_FAILURE);
            }
        } else {
            if (runInBackground) {
                cout << "Started background process with PID: " << pid << endl;
            } else {
                int status;
                waitpid(pid, &status, 0);
                if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    commandSuccess = true;
                }
            }
        }
    }

    if (commandSuccess) {
        appendToHistory(command);
    }
}

CommandNode* parseCommandTree(const string& input) {
    stringstream ss(input);
    string segment;
    CommandNode* root = nullptr;
    CommandNode* current = nullptr;

    while (getline(ss, segment, '|')) {
        CommandNode* node = new CommandNode();
        
        stringstream segmentStream(segment);
        segmentStream >> node->command;
        string arg;
        while (segmentStream >> arg) {
            node->args.push_back(arg);
        }

        if (root == nullptr) {
            root = node;
        } else {
            current->next = node;
        }
        current = node;
    }

    return root;
}

void executeCommandTree(CommandNode* root) {
    if (!root) return;

    int pipefd[2];
    pid_t pid;
    int in_fd = 0;

    while (root != nullptr) {
        pipe(pipefd);

        if ((pid = fork()) == -1) {
            perror("Fork failed");
            return;
        } else if (pid == 0) {
            dup2(in_fd, 0); 
            if (root->next != nullptr) {
                dup2(pipefd[1], 1); 
            }
            close(pipefd[0]);

            vector<char*> args;
            args.push_back(const_cast<char*>(root->command.c_str()));
            for (const string& arg : root->args) {
                args.push_back(const_cast<char*>(arg.c_str()));
            }
            args.push_back(nullptr);

            if (execvp(args[0], args.data()) == -1) {
                perror("Command execution failed");
                exit(EXIT_FAILURE);
            }
        } else {
            wait(nullptr); 
            close(pipefd[1]);
            in_fd = pipefd[0]; 
            root = root->next; 
        }
    }
}



