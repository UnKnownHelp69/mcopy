#include <stdbool.h>
#include <stdio.h>
#include "tools.h"

/* Init struct of commands */
void init() {
    commands.addToExistFold = false;
    commands.replExistFile = false;
    commands.replExistFold = false;
}

/* Print in cmd list of commands */
void help() {
    printf("Usage: mcopy -commands <source> <destination>\n\n");
    printf("List of commands: \n");
    printf("-F ignore all warnings, replace existing\n");
    printf("-A add new files to existing folders\n");
    printf("-R replace existing folders\n");
    printf("-r replace existing files\n");
}
