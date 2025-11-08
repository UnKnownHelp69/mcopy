#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <signal.h>
#include <stdint.h>

#include "cursor.h"

// Procces signals
void signal_handler(int signal) {
    show_cursor();
    exit(128 + signal);
}

/* Eval file size
 * totalSize is output size of file in bytes
 * Return 0 if success else error_code
 */
int getFileSize(char *file_path, int64_t *totalSize) {
    HANDLE hFile = CreateFileA(file_path, GENERIC_READ, 
                    FILE_SHARE_READ, NULL, OPEN_EXISTING, 
                    FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return 1;
    }

    LARGE_INTEGER liSize;
    if (!GetFileSizeEx(hFile, &liSize)) {
        CloseHandle(hFile);
        return 2;
    }

    CloseHandle(hFile);
    *totalSize = liSize.QuadPart;
    return 0;
}

/* 
 * Copy from file to file
 * source - path of file to copy
 * destination - path of target file
 * fileSize is size of successfuly copied file in bytes
 * Returns 0 on success, non zero on error
 */
int copyFileToFile(char *source, char *destination, int64_t *fileSize) {
    
    /*
    for (int i = 1; i <= 5000; ++i) {
        int percent = (int)((i * 100) / 5000); 
        printf("\rProgress: %d%%", percent);
        fflush(stdout);
    }*/
    
    int64_t totalSize = 0;
    int64_t copied = 0;
    int globPercent = -1;

    int status = getFileSize(source, &totalSize);

    if (status != 0) {
        return -1;
    }

    *fileSize = totalSize;
    hide_cursor();

    while (0) {
        // somehow copying
        int chunkSize = 0;
        copied += chunkSize;

        int currPercent = (int)((100 * copied) / totalSize);

        if (currPercent != globPercent) {
            printf("\rProgress: %d%%", currPercent);
            globPercent = currPercent;
        }
    }

    show_cursor();

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
            
            int64_t file_size = 0;
            int status = copyFileToFile(source, destination, &file_size);
            
            if (status == 0) {
                printf("Successfuly copied %d bytes\n", file_size);
            } else {
                printf("Exit with error code %d\n", status);
            }
        }
    }

    return 0;
}

