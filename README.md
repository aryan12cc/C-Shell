
# C-Shell
A custom shell implementation in C with features similar to bash, including built-in commands, process management, and more.

## Features

### Core Functionality

- Custom shell prompt showing username, hostname, current directory, and execution times
- Command execution for both system commands and built-in commands
- Background process execution with & operator
- Pipes (|) for command chaining
- Input/output redirection (<, >, >>)
- Signal handling (SIGINT, SIGTSTP, SIGCHLD)
- Command history logging
- Support for command aliases and custom functions via .myshrc

### Built-in Commands
- hop - Directory navigation (similar to cd)
    - Supports relative and absolute paths
    - Special arguments: ., Projects, ~, - (previous directory)
- reveal - Lists directory contents (similar to ls)
    - -a flag to show hidden files
    - -l flag for detailed listing with permissions, owners, size, etc.
    - Color coding for different file types
- seek - Searches for files/directories
    - -d flag to search only for directories
    - -f flag to search only for files
    - -e flag to execute/open the file if exactly one match is found
- proclore - Displays process information
    - Shows PID, status, process group, virtual memory, and executable path
- activities - Shows all running/stopped background processes
- ping - Sends signals to processes
    - Usage: ping <pid> <signal>
- fg - Brings background processes to foreground
    - Usage: fg <pid>
- bg - Resumes stopped background processes
    - Usage: bg <pid>
- neonate - Displays the most recently created process ID
    - Usage: neonate -n <seconds>
    - Press 'x' to exit
- iMan - Fetches manual pages from the internet
    - Usage: iMan <command>
- log - Command history management
    - log to show command history
    - log purge to clear history
    - log execute <n> to execute nth command from history

### Advanced Features
- Execution time tracking (commands that take ≥2s appear in prompt)
- Color-coded output for different file types
- Custom aliases and functions defined in .myshrc
- Robust background process management
- Support for chaining multiple commands with pipes and I/O redirection


## Assumptions
- Maximum input size is 4096 characters.
- If no previous directory is present in hop -, it treats it as hop ..
- If multiple commands are given to hop, then hop - changes the current working directory to the directory referred to in the second last successful hop call.
- If reveal - is performed, but no directory changes have occurred, then reveal - will reveal the contents of the current working directory.
- At most 4096 entries can be present in a directory.
- reveal -l ; reveal - is the same as reveal -l ; reveal ..
- reveal -l color codes the entire line instead of just the file name.
- All commands that do not contain log are stored in log.txt.
- Before the code starts executing, log.txt should be present in the home directory of the shell.
- If a command like sleep 3 ; sleep 5 is given, the next prompt will contain ~ sleep : 3s;sleep : 5s>.
- If a background process is erroneous, a process is created, the shell identifies that the process is erroneous, and then the process is stopped.
- hop .. & treats & as another directory; background processes won't run on custom commands.
- The shell displays termination messages for background processes immediately when they terminate.
- Virtual memory in proclore is typically displayed in kB.
- seek <string> - checks in the same directory tree that hop - would go to.
- If just & is entered, a process will be created, run unsuccessfully, and exit normally. It will show (null) exited normally (<pid>).
- The aliases and functions are stored in .myshrc in the home directory.
- There are at max 256 aliases/functions with each having a maximum size of 256 characters.
- Each alias maps to a command, therefore, only the first word is checked. Aliases take higher priority than any command.
- If a foreground process is brought forward via fg, the next prompt will display fg instead of the process name if the execution time is ≥2 seconds.
Building and Running