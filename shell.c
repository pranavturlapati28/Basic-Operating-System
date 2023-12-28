// PID: 730579095
// I pledge the COMP211 honor code.

// ----------------------------------------------
// These are the only libraries that can be
// used. Under no circumstances can additional
// libraries be included

#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shell.h"

const char *PATH_SEPARATOR = ":";


#define ONE 1
#define ZERO 0
#define TWO 2

// --------------------------------------------
// Currently only two builtin commands for this
// assignment exist
// --------------------------------------------
const char* BUILT_IN_COMMANDS[] = { "cd", "exit", NULL };


/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * Allocate memory for the command. The last element in `p_cmd->argv` should be
 * NULL.
 *
 * The length of `p_cmd->argv` should be `argc+ONE`: the first `argc` slots are
 * used to store the arguments and the last one is set to NULL.
 *
 * Arguments:
 *      p_cmd : pointer to the command need to allocate memory.
 *      argc :  the number of arguments.
 *
 * Return:
 *      None
 */
void alloc_mem_for_command(command_t* p_cmd, int argc) {
    p_cmd->argv = (char**)malloc((argc+ONE) * sizeof(char*));

    if (p_cmd -> argv == NULL) 
    {
        exit(EXIT_FAILURE);
    }

    p_cmd->argc = argc;

    for (int i = ZERO; i < argc; i++) {
        (p_cmd)->argv[i] = NULL;
    }
} // end alloc_mem_for_command function

/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * This function is used to free memory that may be malloc'd for the name and
 * argv fields in the command_t structure.  To be safe, you should also set the
 * pointer values to NULL.
 *
 * HINT(s): See man page for more information about free function
 *
 * Arguments:
 *         p_cmd : pointer to a command_t structure
 * Return: 
 *        None
 *
 */
void cleanup(command_t* p_cmd) {
    // TODO: COMPLETE YOUR CODE HERE.
    for (int i = ZERO; i < p_cmd->argc; i++) {
        if (p_cmd->argv[i]) {
            free(p_cmd->argv[i]);
        }
    }

    if (p_cmd->argv) {
        free(p_cmd->argv);
        p_cmd->argv = NULL;
        p_cmd->argc = ZERO;
    }
} // end cleanup function

/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * This function will parse the command (cmd for short) and its arguments
 * specified by the user.
 *
 * HINT(s): This function is "essentially" an exercise in string parsing.
 *
 *         EXAMPLE#ONE, if the user enters
 *
 *                                 cd /mnt/cdrom
 *
 *                 at the shell prompt, line would contain "cd /mnt/cdrom".
 *                 Using the space character as the delimiter, the fields in
 *                 the command struct would be:
 *
 *                                 - argc = 2
 *                                 - argv = {"cd", "/mnt/cdrom", NULL}
 *
 *         EXAMPLE#2, if the user enters
 *
 *                                 ls -la
 *
 *                 at the shell prompt, line would contain "ls -la". Using the
 *                 space character as the delimiter, the fields in the command
 *                 struct would be:
 *
 *                                 - argc = 2
 *                                 - argv = {"ls", "-la", NULL}
 *
 *         EXAMPLE#3, if the user enters nothing at the shell prompt (i.e.
 *         simply hits the return key), line would NULL, and the fields in
 *         the command struct would be:
 *
 *                                 - argc = ZERO
 *                                 - argv = {NULL}
 *
 *  Arguments:
 *      line: pointer to the string containing the cmd.
 *      p_cmd: pointer to the command_t structure
 *
 *  Return:
 *      N/A
 *
 */
void parse(char* line, command_t* p_cmd) {
    while (*line != '\0' && isspace(*line)) {
        line++;
    }

    int length = strlen(line);
    while (length > ZERO && isspace(line[length - ONE])) {
        length--;
    }

    alloc_mem_for_command(p_cmd, ZERO);

    char* delimiter;
    int argc = ZERO;

    char* line_copy = strndup(line, length);
    if (line_copy == NULL) {
        exit(EXIT_FAILURE);
    }

    delimiter = strtok(line_copy, " ");

    while (delimiter != NULL) {
        p_cmd->argv[argc] = strdup(delimiter);
        argc++;

        p_cmd->argv = realloc(p_cmd->argv, (argc + ONE) * sizeof(char*));
        if (p_cmd->argv == NULL) {
            exit(EXIT_FAILURE);
        }

        p_cmd->argv[argc] = NULL;
        delimiter = strtok(NULL, " ");
    }

    free(line_copy);
    p_cmd->argc = argc;
} // end parse function

/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * This function is used determine if the named command (cmd for short) entered
 * by the user in the shell can be found in one of the folders defined in the
 * PATH environment (env or short) variable. If the file exists, then the name
 * of the executable at argv[ZERO] is replaced with the location (fully qualified
 * path) of the executable.
 *
 * For example,
 *
 * if command_t.argv[ZERO] is "ls", and "ls" is in the "/usr/bin" folder,
 * then command_t.argv[ZERO] would be changed to "/usr/bin/ls"
 *
 * HINT(s): Use getenv() system function to retrieve the folders defined in the
 *                 PATH variable. An small code segment is shown below that
 * demonstrates how to retrieve folders defined in your PATH.
 *
 *                         char* path_env_variable;
 *                         path_env_variable = getenv( "PATH" );
 *                         printf("PATH = %s\n", path_env_variable );
 *
 *                 The format of the PATH is very simple, the ':' character is
 * delimiter, i.e. used to mark the beginning and end of a folder defined in the
 * path.
 *
 *                 Write a loop that parses each folder defined in the path,
 * then uses this folder along with the stat function (see "File/Directory
 * Status" section in the assignment PDF).
 *
 *  Arguments:
 *         p_cmd: pointer to the command_t structure
 *  Return:
 *      true: if cmd is in the PATH
 *      false:  if not in the PATH.
 *
 */
bool find_fullpath(command_t* p_cmd) {
    // TODO: COMPLETE YOUR CODE HERE.
    char* path_env_variable = getenv("PATH");
    if (path_env_variable == NULL) {
        return false;
    }

    char* path_split = strdup(path_env_variable);
    if (path_split == NULL) {
        return false;
    }

    char* delimited = strtok(path_split, PATH_SEPARATOR);

    while (delimited != NULL) {
        char* path = malloc(strlen(delimited) + strlen(p_cmd->argv[ZERO]) + TWO);
        if (path == NULL) {
            free(path_split);
            return false;
        }

        sprintf(path, "%s/%s", delimited, p_cmd->argv[ZERO]);

        if (access(path, X_OK) == ZERO) {
            free(path_split);
            free(p_cmd->argv[ZERO]);
            p_cmd->argv[ZERO] = strdup(path);
            free(path);
            return true;
        }

        free(path);
        delimited = strtok(NULL, PATH_SEPARATOR);
    }

    free(path_split);
    return false; // temporary place holder
} // end find_fullpath function

/* ------------------------------------------------------------------------------
 *
 * YOU NEED TO COMPLETE THIS FUNCTION.
 *
 * This function will execute external commands (cmd for short).
 *
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structor
 *
 * Return:
 *      SUCCESS: successfully execute the command.
 *      ERROR: error occurred.
 *
 */
int execute(command_t* p_cmd) {
    int status = SUCCESS;
    int child_process_status;
    pid_t child_pid;

    if (is_builtin(p_cmd)) {
        return do_builtin(p_cmd);
    }

    if (!find_fullpath(p_cmd)) {
        printf("Command '%s' not found!\n", p_cmd->argv[ZERO]);
        return ERROR;
    }

    child_pid = fork();

    if (child_pid == -ONE) {
        return ERROR;
    }

    if (child_pid == ZERO) {
        execv(p_cmd->argv[ZERO], p_cmd->argv);
        exit(EXIT_FAILURE);
    } else {
        waitpid(child_pid, &child_process_status, ZERO);

        if (WIFEXITED(child_process_status)) {
            status = WEXITSTATUS(child_process_status);
        } else {
            status = ERROR;
        }
    }

    return status;
} // end execute function

/* ------------------------------------------------------------------------------
 *
 * This function will determine if command (cmd for short) entered in the shell
 * by the user is a valid builtin command.
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structure
 * Return:
 *      true:  the cmd is in array `valid_builtin_commands`.
 *      false: not in array `valid_builtin_commands`.
 *
 */
bool is_builtin(command_t* p_cmd) {
    int cnt = ZERO;

    while(BUILT_IN_COMMANDS[cnt] != NULL) {
        if (strcmp(p_cmd->argv[ZERO], BUILT_IN_COMMANDS[cnt]) == ZERO) {
            return true;
        }

        cnt++;
    }

    return false;
} // end is_builtin function

/* ------------------------------------------------------------------------------
 *
 * This function is used execute built-in commands such as change directory (cd)
 *
 * HINT(s): See man page for more information about chdir function
 *
 * Arguments:
 *      p_cmd: pointer to the command_t structure
 * Return:
 *      SUCCESS: no error occurred during chdir operation.
 *      ERROR: an error occured during chdir operation.
 *
 */
int do_builtin(command_t* p_cmd) {
    // If the command was 'cd', then change directories
    // otherwise, tell the program to EXIT

    struct stat buff;
    int status = SUCCESS;

    // exit
    if (strcmp( p_cmd->argv[ZERO], "exit") == ZERO) {
        exit(status);
    }

    // cd
    if (p_cmd->argc == ONE) {
        // -----------------------
        // cd with no arg
        // -----------------------
        // change working directory to that
        // specified in HOME environmental
        // variable

        status = chdir(getenv("HOME"));
    } else if ( (status = stat(p_cmd->argv[ONE], &buff)) == ZERO && (S_IFDIR & buff.st_mode) ) {
        // -----------------------
        // cd with one arg
        // -----------------------
        // only perform this operation if the requested
        // folder exists

        status = chdir(p_cmd->argv[ONE]);
    }

    return status;
} // end do_builtin function
