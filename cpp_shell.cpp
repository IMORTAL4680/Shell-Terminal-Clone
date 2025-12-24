/**
 * C++ Shell / Terminal Emulator - Windows Edition
 * Refactored with proper OOP class structure
 * 
 * Classes: CommandParser, BuiltInCommands, ExternalExecutor, Terminal
 */

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <windows.h>
#include <direct.h>
#include <fstream>
#include <shellapi.h>

// ============================================================================
// CommandParser Class - Handles parsing of user input
// ============================================================================

class CommandParser {
private:
    std::vector<std::string> tokens;
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }

public:
    // Tokenize input string into parts
    void tokenize(const std::string& input) {
        tokens.clear();
        std::istringstream iss(trim(input));
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
    }
    
    // Get the command (first token)
    std::string getCommand() const {
        if (tokens.empty()) return "";
        return tokens[0];
    }
    
    // Get all arguments (tokens after command)
    std::vector<std::string> getArguments() const {
        return tokens;
    }
    
    // Check if input is empty
    bool isEmpty() const {
        return tokens.empty();
    }
};

// ============================================================================
// BuiltInCommands Class - Handles all built-in shell commands
// ============================================================================

class BuiltInCommands {
private:
    std::vector<std::string>& history;
    const std::string VERSION = "1.0.0";

public:
    BuiltInCommands(std::vector<std::string>& hist) : history(hist) {}
    
    // cd - change directory
    void cd(const std::vector<std::string>& args) {
        std::string target;
        
        if (args.size() < 2) {
            const char* home = getenv("USERPROFILE");
            if (home) {
                target = home;
            } else {
                std::cout << "  Error: Cannot find home directory\n";
                return;
            }
        } else {
            target = args[1];
        }
        
        if (_chdir(target.c_str()) != 0) {
            std::cout << "  Error: Cannot find directory '" << target << "'\n";
        }
    }
    
    // ls - list directory contents
    void ls(const std::vector<std::string>& args) {
        std::string path = ".";
        if (args.size() > 1) {
            path = args[1];
        }
        
        WIN32_FIND_DATAA findData;
        std::string searchPath = path + "\\*";
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
        
        if (hFind == INVALID_HANDLE_VALUE) {
            std::cout << "  Error: Cannot access '" << path << "'\n";
            return;
        }
        
        std::cout << "\n  Contents of " << path << ":\n";
        std::cout << "  " << std::string(40, '-') << "\n";
        
        do {
            std::string name = findData.cFileName;
            if (name != "." && name != "..") {
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    std::cout << "  \033[34m[DIR]\033[0m  " << name << "\n";
                } else {
                    std::cout << "  \033[37m[FILE]\033[0m " << name << "\n";
                }
            }
        } while (FindNextFileA(hFind, &findData));
        
        FindClose(hFind);
        std::cout << "\n";
    }
    
    // clear - clear screen
    void clear() {
        system("cls");
    }
    
    // help - display help message
    void help() {
        std::cout << "\n  Custom Commands:\n";
        std::cout << "  ----------------\n";
        std::cout << "  -help          Show this help message\n";
        std::cout << "  -version       Show shell version\n";
        std::cout << "  -history       Show command history\n";
        std::cout << "  wd             Show current working directory\n";
        std::cout << "  rename <old> <new>  Rename a file or folder\n";
        std::cout << "  move <src> <dest>   Move a file or folder\n";
        std::cout << "\n  Built-in Commands:\n";
        std::cout << "  ------------------\n";
        std::cout << "  ls [dir]       List directory contents\n";
        std::cout << "  cd <dir>       Change directory\n";
        std::cout << "  cat <file>     Display file contents\n";
        std::cout << "  mkdir <dir>    Create a new directory\n";
        std::cout << "  rm <path>      Delete a file or folder\n";
        std::cout << "  clear          Clear the screen\n";
        std::cout << "  notepad [file] Open Notepad\n";
        std::cout << "  code [path]    Open VS Code\n";
        std::cout << "  exit           Exit the shell\n\n";
    }
    
    // exit - handled in Terminal class, returns false
    bool exit() {
        std::cout << "  Goodbye!\n";
        return false;
    }
    
    // version - display version
    void version() {
        std::cout << "\n  cpp-shell version " << VERSION << "\n";
        std::cout << "  Windows Edition\n";
        std::cout << "  Built with C++\n\n";
    }
    
    // showHistory - display command history
    void showHistory() {
        if (history.empty()) {
            std::cout << "  No commands in history.\n";
            return;
        }
        std::cout << "\n  Command History:\n";
        std::cout << "  ----------------\n";
        for (size_t i = 0; i < history.size(); i++) {
            std::cout << "  " << (i + 1) << ". " << history[i] << "\n";
        }
        std::cout << "\n";
    }
    
    // cat - display file contents
    void cat(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "  Usage: cat <filename>\n";
            return;
        }
        
        std::ifstream file(args[1]);
        if (!file.is_open()) {
            std::cout << "  Error: Cannot open file '" << args[1] << "'\n";
            return;
        }
        
        std::cout << "\n";
        std::string line;
        while (std::getline(file, line)) {
            std::cout << "  " << line << "\n";
        }
        std::cout << "\n";
        file.close();
    }
    
    // mkdir - create directory
    void mkdir(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "  Usage: mkdir <directory>\n";
            return;
        }
        
        if (CreateDirectoryA(args[1].c_str(), NULL)) {
            std::cout << "  Directory '" << args[1] << "' created.\n";
        } else {
            DWORD error = GetLastError();
            if (error == ERROR_ALREADY_EXISTS) {
                std::cout << "  Error: Directory '" << args[1] << "' already exists.\n";
            } else {
                std::cout << "  Error: Cannot create directory '" << args[1] << "'\n";
            }
        }
    }
    
    // rm - delete file or folder
    void rm(const std::vector<std::string>& args) {
        if (args.size() < 2) {
            std::cout << "  Usage: rm <file_or_folder>\n";
            return;
        }
        
        std::string target = args[1];
        DWORD attrs = GetFileAttributesA(target.c_str());
        
        if (attrs == INVALID_FILE_ATTRIBUTES) {
            std::cout << "  Error: '" << target << "' not found.\n";
            return;
        }
        
        if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
            if (RemoveDirectoryA(target.c_str())) {
                std::cout << "  Directory '" << target << "' deleted.\n";
            } else {
                std::cout << "  Error: Cannot delete '" << target << "' (folder may not be empty)\n";
            }
        } else {
            if (DeleteFileA(target.c_str())) {
                std::cout << "  File '" << target << "' deleted.\n";
            } else {
                std::cout << "  Error: Cannot delete '" << target << "'\n";
            }
        }
    }
    
    // wd - show working directory
    void wd() {
        char buffer[MAX_PATH];
        if (_getcwd(buffer, MAX_PATH) != nullptr) {
            std::cout << "  " << buffer << "\n";
        }
    }
    
    // rename - rename file or folder
    void rename(const std::vector<std::string>& args) {
        if (args.size() < 3) {
            std::cout << "  Usage: rename <old_name> <new_name>\n";
            return;
        }
        
        if (MoveFileA(args[1].c_str(), args[2].c_str())) {
            std::cout << "  Renamed '" << args[1] << "' to '" << args[2] << "'\n";
        } else {
            std::cout << "  Error: Cannot rename '" << args[1] << "'\n";
        }
    }
    
    // move - move file or folder
    void move(const std::vector<std::string>& args) {
        if (args.size() < 3) {
            std::cout << "  Usage: move <source> <destination>\n";
            return;
        }
        
        std::string source = args[1];
        std::string dest = args[2];
        
        DWORD destAttrs = GetFileAttributesA(dest.c_str());
        if (destAttrs != INVALID_FILE_ATTRIBUTES && (destAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
            size_t pos = source.find_last_of("\\/");
            std::string filename = (pos != std::string::npos) ? source.substr(pos + 1) : source;
            dest += "\\" + filename;
        }
        
        if (MoveFileA(source.c_str(), dest.c_str())) {
            std::cout << "  Moved '" << source << "' to '" << dest << "'\n";
        } else {
            std::cout << "  Error: Cannot move '" << source << "'\n";
        }
    }
};

// ============================================================================
// ExternalExecutor Class - Handles external program execution
// ============================================================================

class ExternalExecutor {
public:
    // Run external system command
    void runSystemCommand(const std::string& cmd, const std::vector<std::string>& args) {
        if (cmd == "notepad") {
            std::string file = (args.size() > 1) ? args[1] : "";
            ShellExecuteA(NULL, "open", "notepad.exe", file.c_str(), NULL, SW_SHOWNORMAL);
            std::cout << "  Notepad opened.\n";
        }
        else if (cmd == "code") {
            std::string path = (args.size() > 1) ? args[1] : ".";
            ShellExecuteA(NULL, "open", "code", path.c_str(), NULL, SW_SHOWNORMAL);
            std::cout << "  VS Code opened.\n";
        }
        else {
            std::cout << "  '" << cmd << "' is not recognized.\n";
            std::cout << "  Type '-help' for available commands.\n";
        }
    }
};

// ============================================================================
// Terminal Class - Main terminal controller
// ============================================================================

class Terminal {
private:
    std::string currentDirectory;
    std::vector<std::string> history;
    std::string username;
    bool running;
    const std::string VERSION = "1.0.0";
    
    CommandParser parser;
    BuiltInCommands* builtIn;
    ExternalExecutor executor;

    std::string getUsername() {
        char buffer[256];
        DWORD size = sizeof(buffer);
        if (GetUserNameA(buffer, &size)) {
            return std::string(buffer);
        }
        return "user";
    }
    
    std::string getCurrentDirectory() {
        char buffer[MAX_PATH];
        if (_getcwd(buffer, MAX_PATH) != nullptr) {
            return std::string(buffer);
        }
        return "C:\\";
    }

public:
    Terminal() : running(true) {
        username = getUsername();
        currentDirectory = getCurrentDirectory();
        builtIn = new BuiltInCommands(history);
    }
    
    ~Terminal() {
        delete builtIn;
    }
    
    // Start terminal - initialize window
    void start() {
        SetConsoleTitleA("C++ Shell / Terminal — cpp-shell");
        system("cls");

        std::cout << "\n";
        std::cout << "  --------------------------------------------\n";
        std::cout << "    C++ Shell / Terminal Emulator (cpp-shell)\n";
        std::cout << "    Version: " << VERSION << "\n";
        std::cout << "    Type '-help' to list available commands\n";
        std::cout << "    **Created by Ojaskar and Sushant**\n";
        std::cout << "  --------------------------------------------\n\n";
    }
    
    // Display prompt
    void displayPrompt() {
        currentDirectory = getCurrentDirectory();
        std::cout << "\033[36m" << username << "@cpp-shell \033[33m" << currentDirectory << "\033[0m\n";
        std::cout << "\033[32m>>> \033[0m";
    }
    
    // Read input from user
    std::string readInput() {
        std::string input;
        if (!std::getline(std::cin, input)) {
            std::cout << "\n";
            running = false;
            return "";
        }
        return input;
    }
    
    // Parse command using CommandParser
    void parseCommand(const std::string& input) {
        parser.tokenize(input);
    }
    
    // Execute command
    void executeCommand() {
        if (parser.isEmpty()) return;
        
        std::string cmd = parser.getCommand();
        std::vector<std::string> args = parser.getArguments();
        
        // Check for exit first
        if (cmd == "exit") {
            running = builtIn->exit();
            return;
        }
        
        // Save to history (trim trailing space)
        std::string fullCmd;
        for (const auto& arg : args) fullCmd += arg + " ";
        if (!fullCmd.empty()) {
            fullCmd.pop_back(); // Remove trailing space
            history.push_back(fullCmd);
        }
        
        // Route to appropriate handler
        if (cmd == "-help") {
            builtIn->help();
        }
        else if (cmd == "-version") {
            builtIn->version();
        }
        else if (cmd == "-history") {
            builtIn->showHistory();
        }
        else if (cmd == "ls" || cmd == "dir") {
            builtIn->ls(args);
        }
        else if (cmd == "cd") {
            builtIn->cd(args);
        }
        else if (cmd == "cat" || cmd == "type") {
            builtIn->cat(args);
        }
        else if (cmd == "mkdir") {
            builtIn->mkdir(args);
        }
        else if (cmd == "clear" || cmd == "cls") {
            builtIn->clear();
        }
        else if (cmd == "rm" || cmd == "del" || cmd == "delete") {
            builtIn->rm(args);
        }
        else if (cmd == "wd" || cmd == "pwd") {
            builtIn->wd();
        }
        else if (cmd == "rename" || cmd == "ren") {
            builtIn->rename(args);
        }
        else if (cmd == "move" || cmd == "mv") {
            builtIn->move(args);
        }
        else if (cmd == "notepad") {
            executor.runSystemCommand(cmd, args);
        }
        else if (cmd == "code") {
            executor.runSystemCommand(cmd, args);
        }
        // Unknown command
        else {
            std::cout << "  '" << cmd << "' is not recognized.\n";
            std::cout << "  Type '-help' for available commands.\n";
        }
    }
    
    // Main run loop
    void runLoop() {
        start();
        
        while (running) {
            displayPrompt();
            std::string input = readInput();
            parseCommand(input);
            executeCommand();
        }
    }
};

// ============================================================================
// Main Function - Creates popup console window
// ============================================================================

int main() {
    if (GetConsoleWindow() == NULL) {
        AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
    
    HWND console = GetConsoleWindow();
    if (console != NULL) {
        int width = 800;
        int height = 500;
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        int x = (screenWidth - width) / 2;
        int y = (screenHeight - height) / 2;
        MoveWindow(console, x, y, width, height, TRUE);
    }
    
    Terminal terminal;
    terminal.runLoop();
    return 0;
}
