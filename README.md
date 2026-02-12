<div align="center">
  <h1>📁 MCOPY</h1>
  <h3>Console utility for coping files and folders in Windows</h3>
  
  <p>
    <strong>Fast · Progress bar</strong>
  </p>
  
  <p>
    <img src="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white">
    <img src="https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white">
    <img src="https://img.shields.io/badge/Win32-API-00A98F?style=for-the-badge">
  </p>
</div>

---

## 🚀 Fast start

```bash
# Download 
git clone https://github.com/UnKnownHelp69/mcopy
cd mcopy

# Compile MinGW-w64
gcc mcopy.c cursor.c tools.c -o mcopy

# Compile MSVC
cl main.c cursor.c tools.c /Fe:mcopy.exe /O2 /W3

# add to path
set PATH=%PATH%;C:\path\to\mcopy
```

## Commands

```bash
# Using
mcopy [flags] <source> <destination>

# Copy file into folder
mcopy C:\photo.jpg D:\images

# Rewrite file
mcopy -r C:\file.txt D:\backup\copy.txt

# Copy folder to folder
mcopy D:\projects E:\backup

# Rewrite all files
mcopy -F C:\work D:\work_backup
```

## 🏷️ Flags

| Flag | Defenition | Example |
|:----:|---------|---------------------|
| 🟢 `-r` | **Replace existing files**<br><small>Overwrites files with the same name</small> | `mcopy -r file.txt D:\` |
| 🔴 `-R` | **Replace existing folders**<br><small>Deletes the folder before copying</small> | `mcopy -R folder D:\` |
| 🔵 `-A` | **Add to existing folders**<br><small>Copies new files without replacing existing ones</small> | `mcopy -A folder D:\` |
| ⚫ `-F` | **Force flag**<br><small>Ignore all = `-r` + `-R`</small> | `mcopy -F source D:\` |

---

### 💡 Combinig

```bash
# Several flags together are acceptable
mcopy -r -A C:\projects D:\backup
mcopy -R -r D:\data E:\archive
mcopy -F C:\work D:\work_backup
```

## 🔢 Error codes

| Code | Enum | Defenition |
|:---:|-----------|:----------:|
| `1` | `fileAccessError` | There is no access to an existing file |
| `2` | `readFileError` | Reading error |
| `3` | `writeFileError` | Writing error |
| `4` | `deleteFileError` | Couldn't delete the file |
| `5` | `getFileSizeError` | Couldn't get the size |
| `6` | `copyFiletoFileByValidPathesError` | Error copying by valid paths |
| `7` | `copyFileToFileError` | Error copying a file to a file |
| `8` | `getNameFileError` | Couldn't get the file name |
| `9` | `makeNewPathBySourcePathAndDestPathError` | Error forming the destination path |
| `10` | `copyFileToFolderError` | Error copying a file to a folder |
| `11` | `mallocError` | Insufficient memory |
| `12` | `getFolderSizeError` | Couldn't get folder size |
| `13` | `createFolderIfNotExistError` | Folder creation error |
| `14` | `checkSubFolderError` | Error checking the nesting |
| `15` | `copyFolderToFolderError` | Error copying folder to folder |
| `16` | `readSizeError` | File size reading error |
| `17` | `differentSizeOfCopiedFilesError` | The size of the original and the copy do not match |
| `18` | `inputError` | Invalid command line arguments |
| `19` | `undefinedError` | Unknown error |
| `20` | `deletFullFolderError` | Error deleting a folder completely |
| `21` | `infRecursionError` | Trying to copy a folder to itself |
| `22` | `deleteFolderError` | Empty folder deletion error |
