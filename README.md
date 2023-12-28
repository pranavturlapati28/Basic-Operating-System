
# Simple Shell Project

## Overview

This project explores process control and environment variables in C by building a basic command-line shell. The shell can execute built-in commands like cd and exit, as well as external programs such as ls and pwd.

## Key Features

Memory Management: Employs dynamic memory allocation to store user input and parsed command structures, ensuring efficient resource usage.
Command Parsing: Accurately interprets user input, separating commands and arguments for proper execution.
Process Control: Seamlessly handles both built-in and external commands using fork, exec, and wait system calls, demonstrating a deep understanding of process management.
Environment Variable Access: Leverages getenv to retrieve the system's PATH variable, enabling the shell to locate and execute programs within specified directories.
Code Style and Readability: Adheres to consistent formatting and meaningful variable names, prioritizing code clarity and maintainability.
## Usage

Compile the code using make.
Run the shell by executing ./project.
Enter commands as you would in a standard shell.
Supported commands:
cd: Change directories
exit: Terminate the shell
External programs (e.g., ls, pwd, echo)
## Testing

Use the provided test cases: ./project < test_data/case{0,1,2,3}/input
Compare output with expected results in test_data/case{0,1,2,3}/output
Run all tests automatically: make followed by ./run_tests.sh
