# mini-project-1-template

## Assumptions

- **If no previous directory is present in `hop -`, it treats it as `hop .`.**
- Maximum number of characters input can handle is `4096` characters. `MAX_SIZE` is defined to `4100` in accordance to that.
- If multiple commands are given to `hop`, then `hop -` changes the current working directory to the directory referred to in the second last successful `hop` call. For example `hop .. xyz -`, where `xyz` is an invalid directory, `hop -` will change the working directory to the directory **before** `hop ..`.
- If `reveal -` is performed, but no directory changes have occurred, then `reveal -` will reveal the contents of the current working directory.
- At most `4096` entries can be present in a directory.
- `reveal -l ; reveal -` is the same as `reveal -l ; reveal .`
- `reveal -l` is color coding the entire line instead of just the file name.
- All commands that do not contain `log` are stored in `log.txt`.
- Before the code starts executing, `log.txt` should be present in the home directory of the shell.
- If a command like `sleep 3 ; sleep 5` is given, the next prompt will contain `~ sleep : 3s;sleep : 5s>`
- If a background process is errorneous, a process is created, the shell comes to know that the process is errorneous, and then the process is stopped. **The process exits normally**.
- `hop .. &` just treats `&` as another directory, thus background processes won't run on the custom commands implemented.
- The bash sends the message of termination of background process as soon as the background process is terminated.
- Currently there is no limit on the maximum amount of background processes. Just two children are spawned to take care of a single background process.
- In `proclore`, virtual memory is usually given in `kB`. This is also mentioned when `proclore` command is given as an input.
- `seek <enter string to match> -` checks in the same directory tree `hop -` would go to.
- If ` &` is entered, a process will be created, will run unsuccessfully, and then will exit normally. Since the process has no name, it will show `(null) exited normally (<pid>)`.
- The maximum number of processes that can execute are `200'000`.
- The aliases and functions will be stored in the file `.myshrc` in the home directory.
- There are at max 256 aliases / functions with each alias / function having a size of at max 256 characters.
- Each alias maps to a **command**, therefore, only the first word is checked. Furthermore, aliases take a higher priority than any command.
- If a foreground process is brought forward via `fg`, while displaying the next prompt, `fg` will be printed instead of the name of the foreground process if the time taken is >= 2 seconds.