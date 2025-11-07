#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

/* 
 * Copy from file to file
 * source - path of file to copy
 * destination - path of target file
 * Returns 0 on success, non zero on error
 */
int copyFileToFile(char *source, char *destination) {
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
        }
    }

    return 0;
}

