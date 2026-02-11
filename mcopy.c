#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>

#include "cursor.h"

#define KB_DISPLAY_THRESHOLD 5

// Procces signals
void signal_handler(int signal) {
    showCursor();
    exit(128 + signal);
}



// Print progress in cmd
void printfProgress() {

}

/* Return optimal buffer size
 * Take size of file in int64_t
 */
int64_t GetOptimalBufferSize(const int64_t fileSize) {
    int64_t size[] = {
        64 * 1024, // 64KB
        256 * 1024, // 256KB
        1 * 1024 * 1024, // 1MB
        4 * 1024 * 1024, // 4MB
        8 * 1024 * 1024, // 8MB
    };

    int64_t tresholds[] = {
        1 * 1024 * 1024, // Up to 1MB
        10 * 1024 * 1024, // Up to 10MB
        100 * 1024 * 1024, // Up to 100MB
        1024 * 1024 * 1024, // Up to 1GB
    };

    for (int i = 0; i < 4; ++i) {
        if (fileSize <= tresholds[i]) {
            return size[i];
        }
    }
    return size[4];
} 

/* Eval file size
 * totalSize is output size of file in bytes
 * Return 0 if success else error_code
 */
int getFileSize(const char *file_path, int64_t *totalSize) {
    HANDLE hFile = CreateFileA(file_path, GENERIC_READ, FILE_SHARE_READ, 
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        return 1;
    }

    LARGE_INTEGER liSize; // winAPI union for int (max 64-bit)
    if (!GetFileSizeEx(hFile, &liSize)) {
        CloseHandle(hFile);
        return 2;
    }

    CloseHandle(hFile);
    *totalSize = liSize.QuadPart;
    return 0;
}

/* Logic of copiyng one exactly valid file to valid dest
0 - if good, else error
*/
int copyFiletoFileByValidSources(const char *destination, const HANDLE hSource, 
    const HANDLE hDest, int64_t *copiedCurr, const int64_t totalSize) {
    
    int globPercent = -1;
    DWORD readSize, writtenSize;

    int64_t bufferSize = GetOptimalBufferSize(totalSize);
    BYTE *buffer = malloc(bufferSize * sizeof(BYTE)); // BYTE ~= uint8_t
    if (!buffer) {
        bufferSize = 64 * 1024;
        buffer = malloc(bufferSize * sizeof(BYTE));
        if (!buffer) {
            printf("Could not locate memory\n");
            CloseHandle(hSource);
            CloseHandle(hDest);
            return 1;
        }
    }
    
    while (ReadFile(hSource, buffer, (DWORD)bufferSize, &readSize, NULL) && readSize > 0) {
        // Write error
        if (!WriteFile(hDest, buffer, readSize, &writtenSize, NULL) || readSize != writtenSize) {
            printf("\nError: Copy failed at %lld/%lld bytes\n", *copiedCurr, totalSize);
            printf("Removing incompleted file: %s\n", destination);

            free(buffer);
            showCursor();
            CloseHandle(hSource);
            CloseHandle(hDest);
            if (!DeleteFileA(destination)) {
                printf("Could not delete incompleted file %s\n", destination);
                return 2;
            }
            return 3;
        }

        *copiedCurr += readSize;

        int currPercent = (int)((100LL * (*copiedCurr)) / totalSize);

        if (currPercent != globPercent) {
            printf("\rProgress: %d%%", currPercent);
            globPercent = currPercent;
        }
    }

    free(buffer);
    return 0;
}

/* 
 * Copy from file to file
 * source - path of file to copy
 * destination - path of target file
 * fileSize is size of successfuly copied file in bytes, 
 * if in KB rize KBytes, not rized - bytes
 * Returns 0 on success, non zero on error
 */
int copyFileToFile(const char *source, const char *destination, int64_t *copiedSize, bool *KBytes) {
    int64_t totalSize = 0;

    int status = getFileSize(source, &totalSize);
    
    // getFileSize error
    if (status != 0) {
        printf("Could not get source file size: %s. With error: %d\n", source, status);
        return 1;
    }

    HANDLE hSource = CreateFileA(source, GENERIC_READ, FILE_SHARE_READ, 
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    // Invalid source handle value error
    if (hSource == INVALID_HANDLE_VALUE) {
        printf("Could not open file to read: %s\n", source);
        return 2;
    }

    HANDLE hDest = CreateFileA(destination, GENERIC_WRITE, 0, 
            NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    // Invalid destination handle value error
    if (hDest == INVALID_HANDLE_VALUE) {
        CloseHandle(hSource);
        printf("Could not open/make file to write: %s\n", destination);
        return 3;
    }
   
    int64_t copiedCurr = 0;
    
    hideCursor();
    printf("\rProgress: 0%%");
    
    status = copyFiletoFileByValidSources(destination, hSource, hDest, &copiedCurr, totalSize);
    if (status != 0) return 5; // if error fun cleans after itself ONLY in that fun.
    
    showCursor();
    CloseHandle(hSource);
    CloseHandle(hDest);
    printf("\n");

    // Smthg got wrong
    if (totalSize != copiedCurr) {
        printf("Error: Copy failed at %lld/%lld bytes\n", copiedCurr, totalSize);
        printf("Removing incompleted file: %s\n", destination);
        
        if (!DeleteFileA(destination)) {
            printf("Could not delete incompleted file %s\n", destination);
            return 6;
        }
        return 7;
    }

    if (totalSize / 1024 > KB_DISPLAY_THRESHOLD) {
        *KBytes = true;
        *copiedSize = totalSize / 1024;
    } else {
        *copiedSize = totalSize;
    }

    return 0;
}

/* Return 0 on success, non zero on error
 * Take path of path to extract name of file (after last "/")
 * Put in char **name name of file by pointer
 * TODO check if it is name of file non folder
 * But if we use ths fun we know that all pathes are correct
 * */
int getNameFile(const char *path, char **name) {
    int size = (int)strlen(path);
    const char * splitter = path + size;

    
    while (splitter > path) {
        if (*splitter == '\\' || *splitter == '/') {
            break;
        }
        --splitter;
    }
    *name = (char *)malloc((int)strlen(splitter) + 1);
    if (name == NULL){
        return 1;
    }
    strcpy(*name, splitter);
    return 0;
}

/* 
 * Copy from file to folder
 * source - path of file to copy
 * folderDestination - path of target folder
 * fileSize is size of successfuly copied file in bytes
 * rize KBytes if in KB, else in bytes
 * Returns 0 on success, non zero on error
 */
int copyFileToFolder(const char *source, const char *folderDestination, int64_t *copiedSize, bool *KBytes) {
    int64_t totalSize = 0;
    int globPercent = -1;

    int status = getFileSize(source, &totalSize);
    
    // getFileSize error
    if (status != 0) {
        printf("Could not get source file size: %s. With error: %d\n", source, status);
        return 1;
    }

    HANDLE hSource = CreateFileA(source, GENERIC_READ, FILE_SHARE_READ, 
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    // Invalid source handle value error
    if (hSource == INVALID_HANDLE_VALUE) {
        printf("Could not open file to read: %s\n", source);
        return 2;
    }
    
    char *name;
    status = getNameFile(source, &name);
    if (status != 0) {
        CloseHandle(hSource);
        printf("Could not find name of file to copy %s\n", source);
        return 3;
    }

    char *destination = (char *)malloc((int)strlen(folderDestination) + (int)strlen(name) + 2);
    if (destination == NULL) {
        CloseHandle(hSource);
        free(name);
        printf("Could not locate memory\n");
        return 4;
    }
    strcpy(destination, folderDestination);
    if (folderDestination[strlen(folderDestination) - 1] != '\\' &&
            folderDestination[strlen(folderDestination) - 1] != '/') {
        strcat(destination, "\\");
    }
    strcat(destination, name);
    free(name);

    HANDLE hDest = CreateFileA(destination, GENERIC_WRITE, 0, 
            NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    // Invalid destination handle value error
    if (hDest == INVALID_HANDLE_VALUE) {
        CloseHandle(hSource);
        printf("Could not open/make file to write: %s\n", destination);
        free(destination);
        return 5;
    }

    int64_t copiedCurr = 0;
    
    hideCursor();
    printf("\rProgress: 0%%");

    status = copyFiletoFileByValidSources(destination, hSource, hDest, &copiedCurr, totalSize);
    if (status != 0) {
        free(destination);
        return 7;
    }

    showCursor();
    CloseHandle(hSource);
    CloseHandle(hDest);
    printf("\n");

    // Smthg got wrong
    if (totalSize != copiedCurr) {
        printf("Error: Copy failed at %lld/%lld bytes\n", copiedCurr, totalSize);
        printf("Removing incompleted file: %s\n", destination);
        
        if (!DeleteFileA(destination)) {
            printf("Could not delete incompleted file %s\n", destination);
            free(destination);
            return 9;
        }
        free(destination);
        return 10;
    }
    free(destination);

    if (totalSize / 1024 > KB_DISPLAY_THRESHOLD) {
        *KBytes = true;
        *copiedSize = totalSize / 1024;
    } else {
        *copiedSize = totalSize;
    }

    return 0;
}

/* 
 * Copy from folder to folder
 * source - path of folder to copy
 * destination - path of target folder
 * Returns 0 on success, non zero on error
 */
int copyFolderToFolder(const char *source, const char *folderDestination, int64_t *copiedSize, bool *KBytes) {
    /* TODO
     * Check if source exists
     * Check if dest exists
     * Make new folder with the same name
     * Iterate by all files in folder
     * if file = copy file to folder
     * if folder = copy folder to folder
     * if != 0 any rec so rize error
     * else continue
     * so on
     * */

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

    DWORD source_type = GetFileAttributesA(source); //DWORD ~= uint32_t
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
            
            int64_t file_size = 0;
            bool KBtsOut = false;
            int status = copyFolderToFolder(source, destination, &file_size, &KBtsOut);

            if (status == 0) {
                if (KBtsOut) printf("Successfuly copied %lld KB\n", file_size);
                else printf("Successfuly copied %lld B\n", file_size);
            } else {
                printf("Exit with error code %d\n", status);
            }

        } else {
            printf("It is impossible to copy folder to file\n");
            return 1;
        }
    } else {
        printf("<source> is a file\n");
        if (destination_type & FILE_ATTRIBUTE_DIRECTORY) {
            printf("<destination> is a folder\n");

            int64_t file_size = 0;
            bool KBtsOut = 0;
            int status = copyFileToFolder(source, destination, &file_size, &KBtsOut);
            
            if (status == 0) {
                if (KBtsOut) printf("Successfuly copied %lld KB\n", file_size);
                else printf("Successfuly copied %lld B\n", file_size);
            } else {
                printf("Exit with error code %d\n", status);
            }

        } else {
            printf("<destination> is a file\n");
            
            int64_t file_size = 0;
            bool KBtsOut = false;
            int status = copyFileToFile(source, destination, &file_size, &KBtsOut);
            
            if (status == 0) {
                if (KBtsOut) printf("Successfuly copied %lld KB\n", file_size);
                else printf("Successfuly copied %lld B\n", file_size);
            } else {
                printf("Exit with error code %d\n", status);
            }
        }
    }

    return 0;
}

