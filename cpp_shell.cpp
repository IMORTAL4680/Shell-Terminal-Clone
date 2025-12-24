#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <windows.h>
#include <direct.h>
#include <fstream>
#include <shellapi.h>


class Shell {
private:
    std::vector<std::string> history;   // Command history storage
    std::string username;                // Current username
    std::string currentDir;              // Current working directory
    bool running;                        // Shell running state
    const std::string VERSION = "1.0.0"; // Shell version

    // Get Windows username
    std::string getUsername() {
        char buffer[256];
        DWORD size = sizeof(buffer);
        if (GetUserNameA(buffer, &size)) {
            return std::string(buffer);
        }
        return "user";
    }

    // Get current working directory
    std::string getCurrentDirectory() {
        char buffer[MAX_PATH];
        if (_getcwd(buffer, MAX_PATH) != nullptr) {
            return std::string(buffer);
        }
        return "C:\\";
    }

    // Trim whitespace from string
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }

    // Parse input into command and arguments
    std::vector<std::string> parseCommand(const std::string& input) {
        std::vector<std::string> tokens;
        std::istringstream iss(trim(input));
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }

public:
    // Constructor
    Shell() : running(true) {
        username = getUsername();
        currentDir = getCurrentDirectory();
    }

    // Initialize terminal window
    void initialize() {
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
        currentDir = getCurrentDirectory();
        std::cout << "\033[36m" << username << "@cpp-shell \033[33m" << currentDir << "\033[0m\n";
        std::cout << "\033[32m>>> \033[0m";
    }

    // Save command to history
    void saveToHistory(const std::string& cmd) {
        std::string trimmed = trim(cmd);
        if (!trimmed.empty()) {
            history.push_back(trimmed);
        }
    }

    // -help command
    void cmdHelp() {
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

    // -version command
    void cmdVersion() {
        std::cout << "\n  cpp-shell version " << VERSION << "\n";
        std::cout << "  Windows Edition\n";
        std::cout << "  Built with C++\n\n";
    }

    // -history command
    void cmdHistory() {
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

    // ls command - list directory contents
    void cmdLs(const std::vector<std::string>& args) {
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

    // cd command - change directory
    void cmdCd(const std::vector<std::string>& args) {
        std::string target;
        
        if (args.size() < 2) {
            // Go to user profile directory
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
        } else {
            currentDir = getCurrentDirectory();
        }
    }

    // cat command - display file contents
    void cmdCat(const std::vector<std::string>& args) {
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

    // mkdir command - create directory
    void cmdMkdir(const std::vector<std::string>& args) {
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

    // clear command - clear screen
    void cmdClear() {
        system("cls");
    }

    // rm command - delete file or folder
    void cmdRm(const std::vector<std::string>& args) {
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
            // Remove directory
            if (RemoveDirectoryA(target.c_str())) {
                std::cout << "  Directory '" << target << "' deleted.\n";
            } else {
                std::cout << "  Error: Cannot delete '" << target << "' (folder may not be empty)\n";
            }
        } else {
            // Remove file
            if (DeleteFileA(target.c_str())) {
                std::cout << "  File '" << target << "' deleted.\n";
            } else {
                std::cout << "  Error: Cannot delete '" << target << "'\n";
            }
        }
    }

    // notepad command - open notepad
    void cmdNotepad(const std::vector<std::string>& args) {
        std::string file = "";
        if (args.size() > 1) {
            file = args[1];
        }
        
        ShellExecuteA(NULL, "open", "notepad.exe", file.c_str(), NULL, SW_SHOWNORMAL);
        std::cout << "  Notepad opened.\n";
    }

    // code command - open VS Code
    void cmdCode(const std::vector<std::string>& args) {
        std::string path = ".";
        if (args.size() > 1) {
            path = args[1];
        }
        
        ShellExecuteA(NULL, "open", "code", path.c_str(), NULL, SW_SHOWNORMAL);
        std::cout << "  VS Code opened.\n";
    }

    // wd command - show working directory (custom)
    void cmdWd() {
        std::cout << "  " << getCurrentDirectory() << "\n";
    }

    // rename command - rename file or folder
    void cmdRename(const std::vector<std::string>& args) {
        if (args.size() < 3) {
            std::cout << "  Usage: rename <old_name> <new_name>\n";
            return;
        }
        
        std::string oldName = args[1];
        std::string newName = args[2];
        
        if (MoveFileA(oldName.c_str(), newName.c_str())) {
            std::cout << "  Renamed '" << oldName << "' to '" << newName << "'\n";
        } else {
            std::cout << "  Error: Cannot rename '" << oldName << "'\n";
        }
    }

    // move command - move file or folder
    void cmdMove(const std::vector<std::string>& args) {
        if (args.size() < 3) {
            std::cout << "  Usage: move <source> <destination>\n";
            return;
        }
        
        std::string source = args[1];
        std::string dest = args[2];
        
        // Check if destination is a directory
        DWORD destAttrs = GetFileAttributesA(dest.c_str());
        if (destAttrs != INVALID_FILE_ATTRIBUTES && (destAttrs & FILE_ATTRIBUTE_DIRECTORY)) {
            // Extract filename from source and append to dest
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

    // Process and execute command
    bool processCommand(const std::string& input) {
        std::vector<std::string> args = parseCommand(input);
        
        if (args.empty()) return true;

        std::string cmd = args[0];

        // Exit command
        if (cmd == "exit") {
            std::cout << "  Goodbye!\n";
            return false;
        }

        // Save to history
        saveToHistory(input);

        // Custom commands (with dash prefix)
        if (cmd == "-help") {
            cmdHelp();
        }
        else if (cmd == "-version") {
            cmdVersion();
        }
        else if (cmd == "-history") {
            cmdHistory();
        }
        // Built-in commands
        else if (cmd == "ls" || cmd == "dir") {
            cmdLs(args);
        }
        else if (cmd == "cd") {
            cmdCd(args);
        }
        else if (cmd == "cat" || cmd == "type") {
            cmdCat(args);
        }
        else if (cmd == "mkdir") {
            cmdMkdir(args);
        }
        else if (cmd == "clear" || cmd == "cls") {
            cmdClear();
        }
        else if (cmd == "rm" || cmd == "del" || cmd == "delete") {
            cmdRm(args);
        }
        else if (cmd == "notepad") {
            cmdNotepad(args);
        }
        else if (cmd == "code") {
            cmdCode(args);
        }
        else if (cmd == "wd" || cmd == "pwd") {
            cmdWd();
        }
        else if (cmd == "rename" || cmd == "ren") {
            cmdRename(args);
        }
        else if (cmd == "move" || cmd == "mv") {
            cmdMove(args);
        }
        // Unknown command
        else {
            std::cout << "  '" << cmd << "' is not recognized.\n";
            std::cout << "  Type '-help' for available commands.\n";
        }

        return true;
    }

    // Main shell loop
    void run() {
        initialize();
        std::string input;

        while (running) {
            displayPrompt();
            
            if (!std::getline(std::cin, input)) {
                std::cout << "\n";
                break;
            }

            running = processCommand(input);
        }
    }
};

// ============================================================================
// Main Function - Creates popup console window
// ============================================================================

int main() {
    // Allocate a new console if not attached (for popup window behavior)
    if (GetConsoleWindow() == NULL) {
        AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
    
    // Set console window size
    HWND console = GetConsoleWindow();
    if (console != NULL) {
        // Move window to center of screen
        RECT rect;
        GetWindowRect(console, &rect);
        int width = 800;
        int height = 500;
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        int x = (screenWidth - width) / 2;
        int y = (screenHeight - height) / 2;
        MoveWindow(console, x, y, width, height, TRUE);
    }
    
    Shell shell;
    shell.run();
    return 0;
}
