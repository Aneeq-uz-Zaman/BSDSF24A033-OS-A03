#include "shell.h"

int main() {
    char* cmdline;
    char** arglist;

    init_readline();

    // readline() prints the prompt, handles line editing, arrow keys and
    // Tab, and returns a malloc'd line (NULL on Ctrl+D).
    while ((cmdline = readline(PROMPT)) != NULL) {
        // !n must be handled before tokenizing and before saving to history:
        // the line is replaced by command n and then runs as if typed.
        if (cmdline[0] == '!') {
            char* expanded = expand_history(cmdline);
            free(cmdline);
            if (expanded == NULL) {
                continue;   // invalid or out of range; error already printed
            }
            cmdline = expanded;
            printf("%s\n", cmdline);   // show which command is being re-run
        }

        add_to_history(cmdline);       // our own list, used by "history" and !n
        if (cmdline[0] != '\0') {
            add_history(cmdline);      // readline's list, used by Up/Down arrows
        }

        if ((arglist = tokenize(cmdline)) != NULL) {
            // Built-ins (cd, exit, ...) run inside the shell process itself;
            // only external commands go through fork-exec-wait.
            if (handle_builtin(arglist) == 0) {
                execute(arglist);
            }

            // Free the memory allocated by tokenize()
            for (int i = 0; arglist[i] != NULL; i++) {
                free(arglist[i]);
            }
            free(arglist);
        }
        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}