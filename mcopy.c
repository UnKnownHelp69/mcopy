#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <signal.h>

#include "cursor.h"

// Procces signals
void signal_handler(int signal) {
    show_cursor();
    exit(128 + signal);
}

/* 
 * Copy from file to file
 * source - path of file to copy
 * destination - path of target file
 * Returns 0 on success, non zero on error
 */
int copyFileToFile(char *source, char *destination, int *file_size) {
    hide_cursor();

    for (int i = 1; i <= 5000; ++i) {
        int percent = (int)((i * 100) / 5000); 
        printf("\rProgress: %d%%", percent);
        fflush(stdout);
    }

    show_cursor();

    *file_size = 22;
    printf("\n");

    return 0;
}

/* 
 * Copy from file to folder
 * source - path of file to copy
 * destination - path of target folder
 * Returns 0 on success, non zero on error
 */
int copyFileToFolder(char *source, char *destination) {
    return 0;
}

/* 
 * Copy from folder to folder
 * source - path of folder to copy
 * destination - path of target folder
 * Returns 0 on success, non zero on error
 */
int copyFolderToFolder(char *source, char *destination) {
    return 0;
}


int main(int argc, char *argv[]) {
    // Add ctrl+break 
    SetConsoleCtrlHandler(NULL, FALSE);
    // Registr proccess signals
    signal(SIGINT, signal_handler);
    signal(SIGBREAK, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGABRT, signal_handler);
    
    if (argc != 3) {
        printf("Usage: mcopy <source> <destination>\n");
        return 1;
    } else {
        printf("Source: %s, Destination: %s\n", argv[1], argv[2]);
    }
    
    char *source = argv[1];
    char *destination = argv[2];

    DWORD source_type = GetFileAttributesA(source);
    DWORD destination_type = GetFileAttributesA(destination);

    if (source_type == INVALID_FILE_ATTRIBUTES) {
        printf("<source> is not available path\n");
        return 1;
    }
    
    if (destination_type == INVALID_FILE_ATTRIBUTES) {
        printf("<destination> is not available path\n");
        return 1;
    }
    
    if (source_type & FILE_ATTRIBUTE_DIRECTORY) {
        printf("<source> is a folder\n");
        if (destination_type & FILE_ATTRIBUTE_DIRECTORY) {
            printf("<destination> is a folder\n");
        } else {
            printf("It is impossible to copy folder to file\n");
            return 1;
        }
    } else {
        printf("<source> is a file\n");
        if (destination_type & FILE_ATTRIBUTE_DIRECTORY) {
            printf("<destination> is a folder\n");
        } else {
            printf("<destination> is a file\n");
            
            int file_size = 0;
            int status = copyFileToFile(source, destination, &file_size);
            
            if (status == 0) {
                printf("Successfuly copied %d\n", file_size);
            } else {
                printf("Exit with error code %d\n", status);
            }
        }
    }

    return 0;
}

