# Custom C-Shell Implementation

A feature-rich implementation of a custom shell in C, supporting various system commands, process management, I/O redirection, and more.

## Features

### Basic Shell Operations
- **Custom Prompt**: Displays username, system name, and current working directory
- **Command Processing**: Handles multiple commands using ';' and '&' operators
- **Background Process Management**: Supports running processes in background with '&'
- **Error Handling**: Provides clear error messages for invalid commands

### Built-in Commands

#### Directory Navigation (hop)
- Change directories with support for relative and absolute paths
- Special symbols: `.`, `..`, `~`, and `-`
- Multiple directory changes in sequence
- Displays full path after changing directory

#### File Operations (reveal)
- List files and directories in lexicographic order
- Supports flags:
  - `-l`: Detailed information display
  - `-a`: Show hidden files
- Color-coded output:
  - Green: Executable files
  - White: Regular files
  - Blue: Directories
- Supports relative/absolute paths and special symbols

#### Command History (log)
- Maintains history of last 15 commands
- Commands persist across sessions
- Supports:
  - `log`: Display command history
  - `log purge`: Clear history
  - `log execute <index>`: Execute command from history
- Intelligent command storage (no duplicates)

#### Process Information (proclore)
- Displays detailed process information:
  - PID
  - Process Status (R/R+/S/S+/Z)
  - Process Group
  - Virtual Memory
  - Executable Path

#### File Search (seek)
- Recursive file/directory search functionality
- Supports flags:
  - `-d`: Directory-only search
  - `-f`: File-only search
  - `-e`: Execute/navigate on single match
- Relative path output
- Permission handling

### Advanced Features

#### I/O Redirection
- Input redirection (`<`)
- Output redirection (`>`)
- Output append (`>>`)
- Handles file creation and permissions
- Error handling for missing input files

#### Pipe Implementation
- Supports multiple piped commands
- Handles command chaining
- Error detection for invalid pipe usage

#### Process Management
- **Activities**: Lists all running processes spawned by shell
- **Signal Handling**:
  - `ping <pid> <signal>`: Send signals to processes
  - Ctrl+C: Interrupt foreground process
  - Ctrl+D: Shell logout
  - Ctrl+Z: Stop foreground process
- **Process Control**:
  - `fg <pid>`: Bring background process to foreground
  - `bg <pid>`: Resume stopped background process

#### Configuration
- `.myshrc` support for shell customization
- Command aliases
- Custom function definitions

#### Network Features
- `iMan`: Fetch man pages from http://man.he.net/
- Display formatted man page content
- Error handling for invalid commands

## Usage

### Basic Command Syntax
```bash
# Basic command execution
<command> [arguments]

# Multiple commands
command1 ; command2

# Background execution
command &

# I/O redirection
command > output.txt
command < input.txt
command >> append.txt

# Pipe commands
command1 | command2 | command3
```

### Built-in Commands
```bash
# Directory navigation
hop [directory]
hop ..
hop ~
hop -

# List files
reveal [-l] [-a] [directory]

# Search files
seek [-d|-f] [-e] <name> [directory]

# Process information
proclore [pid]

# Command history
log
log purge
log execute <index>

# Process management
activities
ping <pid> <signal>
fg <pid>
bg <pid>

# Man page lookup
iMan <command>
```

## Technical Notes

### Process States
- R/R+: Running
- S/S+: Sleeping (interruptible wait)
- Z: Zombie
- '+' indicates foreground process

### File Permissions
- Output files created with 0644 permissions
- Handles read/write/execute permissions for seek command

### Error Handling
- Invalid commands
- File/directory not found
- Permission denied
- Invalid pipe usage
- Process not found

## Implementation Details

The shell is implemented in C and uses various system calls and library functions for its operation. Key implementation aspects include:

- Process creation using fork() and exec()
- Signal handling for process control
- File descriptors for I/O redirection
- Pipe implementation using pipe()
- Socket programming for man page fetching
- Command parsing and tokenization
- Background process management
- File and directory operations

## Prerequisites
- GCC Compiler
- Linux-based Operating System
- Internet connection (for iMan command)