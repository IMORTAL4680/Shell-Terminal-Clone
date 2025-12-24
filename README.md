# C++ Terminal Emulator (cpp-shell)

A zsh-style terminal (shell) emulator written in C++17 that supports built-in commands, external command execution, and command history.

## Features

- **zsh-style Prompt**: Beautiful two-line prompt with username, hostname, and current directory
- **Built-in Commands**: `exit`, `history`, `pwd`, `cd`, `clear`, `help`
- **External Command Execution**: Execute any system command via fork/exec
- **Command History**: All commands are saved during runtime
- **Color Support**: ANSI color-coded prompt for better readability
- **Error Handling**: Clear error messages for failed commands

## Prompt Format

```
╭─[username@cpp-shell]─(current_directory)
╰─❯ 
```

## Requirements

- **Compiler**: g++ with C++17 support (GCC 7+ or Clang 5+)
- **Operating System**: Linux or macOS
- **Libraries**: Standard C++ library, POSIX system calls

## Compilation

### Using Make

```bash
# Build the shell
make

# Run the shell
make run

# Clean build files
make clean
```

### Manual Compilation

```bash
g++ -std=c++17 -Wall -Wextra -O2 -o cpp_shell cpp_shell.cpp
```

## Usage

### Starting the Shell

```bash
./cpp_shell
```

### Built-in Commands

| Command | Description |
|---------|-------------|
| `exit` | Exit the shell |
| `history` | Display command history |
| `pwd` | Print current working directory |
| `cd <dir>` | Change directory (supports ~ for home) |
| `clear` | Clear the terminal screen |
| `help` | Display available commands |

### External Commands

Any command not recognized as a built-in will be executed as an external system command:

```
╰─❯ ls -la
╰─❯ cat file.txt
╰─❯ grep pattern file.txt
```

## Architecture

The shell follows a modular design with separate functions for:

1. **Terminal Initialization** (`initialize_terminal`)
2. **Prompt Generation** (`display_prompt`)
3. **Command Parsing** (`parse_command`)
4. **Built-in Execution** (`execute_builtin`)
5. **External Execution** (`execute_external`)
6. **History Management** (`save_to_history`, `display_history`)

## Control Flow

```
Start
  ↓
Initialize Terminal (set title, clear screen)
  ↓
┌─→ Display Prompt
│     ↓
│   Read User Input
│     ↓
│   Parse Command
│     ↓
│   Is "exit"? ──Yes──→ Terminate Program
│     │No
│     ↓
│   Save to History
│     ↓
│   Is Built-in? ──Yes──→ Execute Built-in
│     │No                      ↓
│     ↓                   Display Output
│   Execute External           │
│     ↓                        │
│   Display Output/Error       │
│     ↓                        │
└─────────────────────────────←┘
```

## Example Session

```
╔════════════════════════════════════════════════════════════╗
║          C++ Terminal Emulator — cpp-shell                 ║
║          Type 'help' for available commands                ║
║          Type 'exit' to quit                               ║
╚════════════════════════════════════════════════════════════╝

╭─[john@cpp-shell]─(~/projects)
╰─❯ pwd
/home/john/projects

╭─[john@cpp-shell]─(~/projects)
╰─❯ cd ..

╭─[john@cpp-shell]─(~)
╰─❯ ls
Desktop  Documents  Downloads  projects

╭─[john@cpp-shell]─(~)
╰─❯ history
Command History:
────────────────
  1  pwd
  2  cd ..
  3  ls

╭─[john@cpp-shell]─(~)
╰─❯ exit
Goodbye! Exiting cpp-shell...
```

## License

This project is open source and available for educational purposes.

## Author

C++ Terminal Emulator Project
