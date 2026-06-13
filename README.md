# MyShell - Unix-like Shell in C

A lightweight Unix-style shell implemented in C as a systems programming project.

This project demonstrates core operating system concepts including process creation, program execution, inter-process communication, signal handling, file descriptor manipulation, command parsing, and process synchronization.

---

## Features

### Command Execution

Execute standard Linux commands using `fork()` and `execvp()`.

```bash
myshell> ls
myshell> pwd
myshell> date
```

---

### Built-in Commands

#### Change Directory

```bash
myshell> cd ..
myshell> cd /home/user/Desktop
```

#### Print Working Directory

```bash
myshell> pwd
```

#### View Command History

```bash
myshell> history
```

#### Exit Shell

```bash
myshell> exit
```

---

### Pipelines

Supports multi-stage pipelines using Unix pipes.

```bash
myshell> ls -l | grep .c
```

```bash
myshell> cat file.txt | sort | uniq
```

---

### Input Redirection

Redirect file contents into a command.

```bash
myshell> wc -l < file.txt
```

---

### Output Redirection

Redirect command output to a file.

```bash
myshell> ls > output.txt
```

---

### Append Redirection

Append output to an existing file.

```bash
myshell> echo hello >> log.txt
```

---

### Background Execution

Run processes without blocking the shell.

```bash
myshell> sleep 10 &
```

The shell immediately returns to the prompt while the process executes in the background.

---

### Signal Handling

The shell ignores `Ctrl+C` (`SIGINT`) while foreground child processes handle it normally.

Example:

```bash
myshell> sleep 100
```

Pressing:

```text
Ctrl+C
```

terminates the running command while keeping the shell active.

---

### Command History

Stores previously executed commands.

```bash
myshell> history
```

Example output:

```text
1 pwd
2 ls
3 cd ..
4 history
```

---

## System Calls and Functions Used

### Process Management

- `fork()`
- `execvp()`
- `waitpid()`

### Inter-Process Communication

- `pipe()`

### File Operations

- `open()`
- `close()`
- `dup2()`

### Directory Management

- `chdir()`
- `getcwd()`

### Signal Handling

- `signal()`

### String Processing

- `strtok()`
- `strcmp()`
- `strncmp()`
- `strcpy()`

---

## Compilation

Compile using GCC:

```bash
gcc myshell.c -o myshell
```

---

## Running

```bash
./myshell
```

---

## Example Session

```text
myshell> pwd
/home/user

myshell> ls | grep .c
main.c
shell.c

myshell> echo Hello > output.txt

myshell> cat output.txt
Hello

myshell> sleep 5 &

myshell> history
1 pwd
2 ls | grep .c
3 echo Hello > output.txt
4 cat output.txt
5 sleep 5 &
```

---

## Concepts Demonstrated

- Unix Process Creation
- Process Synchronization
- Program Execution
- Inter-Process Communication (IPC)
- File Descriptor Manipulation
- Command Parsing
- Input/Output Redirection
- Signal Handling
- Background Job Execution
- Shell Design Fundamentals

---

## Future Improvements

Potential extensions include:

- Job control (`jobs`, `fg`, `bg`)
- Tab completion
- Environment variable expansion
- Quoted string support
- Alias support
- Command scripting
- Improved command parser

---

## Author

Built as a systems programming project to explore the internal mechanisms of Unix-like shells and operating system process management.
