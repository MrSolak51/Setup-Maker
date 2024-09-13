#include "SetupMaker.h"

char* readFileToString(const char *fileName, size_t *outSize) {
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        perror("Dosya açılamadı");
        system("pause");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        perror("Bellek tahsisi başarısız");
        fclose(file);
        system("pause");
        exit(1);
    }

    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0'; 

    fclose(file);

    *outSize = fileSize;
    return buffer;
}


void createCFileFromData(const char *data[], size_t dataSizes[], size_t count, const char *cFileName,const char *fileNames[], char directories[][MAX_PATH_LENGTH_], size_t dirCount, const char *basePath,  const char *agreement,  const char *appName) {
    size_t basePathLen = strlen(basePath);
   
   FILE *fileh = fopen("Setup.h", "w");
    if (fileh == NULL) {
        perror("Yeni C dosyası oluşturulamadı");
        system("pause");
        exit(1);
    }

    fprintf(fileh, "#include <stdio.h>\n");
    fprintf(fileh, "#include <stdlib.h>\n");
    fprintf(fileh, "#include <windows.h>\n");
    fprintf(fileh, "#include <string.h>\n");
    fprintf(fileh, "#ifndef SETUP_H\n");
    fprintf(fileh, "#define SETUP_H\n");
    fprintf(fileh, "#define MAX_PATH_LENGTH 1024\n\n");

    fprintf(fileh, "static const char agreement[100000] = \"");
    while (*agreement) {
        if (*agreement == '\n') {
            fprintf(fileh, "\\n");
        } else if (*agreement == '\"') {
            fprintf(fileh, "\\\"");
        } else if (*agreement == '\r') {
            fprintf(fileh, "\\r");
        } else if (*agreement == '\"') {
            fprintf(fileh, "\\\"");
        } else {
            fputc(*agreement, fileh);
        }
        agreement++;
    }
    fprintf(fileh, "\";\n");

    fprintf(fileh, "static const char appName[100] = \"%s\";\n", appName);
    fprintf(fileh, "void CreateDesktopShortcut(char * exePath, char * exeName);\n");
    fprintf(fileh, "void CreateStartMenuShortcut(char * exePath, char * exeName);\n");
    fprintf(fileh, "int runSetup(char basePath[MAX_PATH_LENGTH], int desktopShortcut, int startMenuShortcut);\n");
    fprintf(fileh, "#endif // SETUP_H\n");

    fclose(fileh);
   
   
   
   
    FILE *file = fopen(cFileName, "w");
    if (file == NULL) {
        perror("Yeni C dosyası oluşturulamadı");
        system("pause");
        exit(1);
    }

    fprintf(file, "#include \"Setup.h\"\n\n");
    for (size_t i = 0; i < count; i++) {
        fprintf(file, "const unsigned char exeData%d[] = {\n", i);
        
        for (size_t j = 0; j < dataSizes[i]; j++) {
            if (j % 16 == 0 && j != 0) {
                fprintf(file, "\n");
            }
            fprintf(file, "0x%02X, ", (unsigned char)data[i][j]);
        }

        fprintf(file, "\n};\n");
    }
    fprintf(file, "void CreateDesktopShortcut(char * basePath, char * exePath, char * exeName){ \n");
    fprintf(file, "    FILE *file = fopen(\"createShortcut.ps1\", \"w\");\n");
    fprintf(file, "    fprintf(file, \"$TargetFile = \\\"%%s\\\"\\n\", exePath);\n");
    fprintf(file, "    fprintf(file, \"$ShortcutFile = \\\"C:\\\\Users\\\\$env:USERNAME\\\\Desktop\\\\%s.lnk\\\"\\n\");\n", appName);
    fprintf(file, "    fprintf(file, \"$WScriptShell = New-Object -ComObject WScript.Shell\\n\");\n");
    fprintf(file, "    fprintf(file, \"$Shortcut = $WScriptShell.CreateShortcut($ShortcutFile)\\n\");\n");
    fprintf(file, "    fprintf(file, \"$Shortcut.TargetPath = $TargetFile\\n\");\n");
    fprintf(file, "    fprintf(file, \"$shortcut.WorkingDirectory = \\\"%%s\\\"\\n\", basePath);\n");
    fprintf(file, "    fprintf(file, \"$Shortcut.Save()\\n\");\n");
    fprintf(file, "    fclose(file);\n");
    fprintf(file, "    system(\"powershell -Command \\\"Set-ExecutionPolicy RemoteSigned -Scope CurrentUser\\\"\");\n");
    fprintf(file, "    system(\"powershell -File \\\"createShortcut.ps1\\\"\");\n");
    fprintf(file, "    system(\"powershell -Command \\\"Set-ExecutionPolicy Restricted -Scope CurrentUser\\\"\");\n");
    fprintf(file, "}\n");
    fprintf(file, "void CreateStartMenuShortcut(char * basePath, char * exePath, char * exeName){\n");
    fprintf(file, "    FILE *file = fopen(\"createStartMenuIcon.ps1\", \"w\");\n");
    fprintf(file, "    fprintf(file, \"$TargetFile = \\\"%%s\\\"\\n\", exePath);\n");
    fprintf(file, "    fprintf(file, \"$ShortcutFile = \\\"$env:APPDATA\\\\Microsoft\\\\Windows\\\\Start Menu\\\\Programs\\\\%s.lnk\\\"\\n\");\n", appName);
    fprintf(file, "    fprintf(file, \"$WScriptShell = New-Object -ComObject WScript.Shell\\n\");\n");
    fprintf(file, "    fprintf(file, \"$Shortcut = $WScriptShell.CreateShortcut($ShortcutFile)\\n\");\n");
    fprintf(file, "    fprintf(file, \"$Shortcut.TargetPath = $TargetFile\\n\");\n");
    fprintf(file, "    fprintf(file, \"$shortcut.WorkingDirectory = \\\"%%s\\\"\\n\", basePath);\n");
    fprintf(file, "    fprintf(file, \"$Shortcut.Save()\\n\");\n");
    fprintf(file, "    fclose(file);\n");
    fprintf(file, "    system(\"powershell -Command \\\"Set-ExecutionPolicy RemoteSigned -Scope CurrentUser\\\"\");\n");
    fprintf(file, "    system(\"powershell -File \\\"createStartMenuIcon.ps1\\\"\");\n");
    fprintf(file, "    system(\"powershell -Command \\\"Set-ExecutionPolicy Restricted -Scope CurrentUser\\\"\");\n");
    fprintf(file, "}\n");
    fprintf(file, "int runSetup(char basePath[MAX_PATH_LENGTH], int desktopShortcut, int startMenuShortcut) {\n");

    fprintf(file, "char cmd[256];\n");
    fprintf(file, "sprintf(cmd, \"mkdir \\\"%%s\\\"\", basePath);\n");
    fprintf(file, "system(cmd);\n");
    char exeName[1000] = "    char exeName[] = \"";

    for (size_t i = 0; i < count; i++) {
        if (strstr(fileNames[i], ".exe") != NULL)
        {      
            strcat(exeName, fileNames[i] + basePathLen + 1);
        }  
    }

    strcat(exeName, "\";\n");
    fprintf(file, exeName);

    for(size_t i = 0; i < dirCount; i++){
        fprintf(file, "    char command%d[MAX_PATH_LENGTH + 50];\n", (int)i);
        fprintf(file, "    snprintf(command%d,sizeof(command%d), \"mkdir \\\"%%s\\\\%s\\\"\", basePath);\n", (int)i, (int)i, directories[i]);
        fprintf(file, "    system(command%d);\n", (int)i);
    }

    for (size_t i = 0; i < count; i++) {
        fprintf(file, "    char path%d[MAX_PATH_LENGTH]=\"\";\n", (int)i);
        fprintf(file, "    strcat(path%d, basePath);\n", (int)i);
        fprintf(file, "    strcat(path%d, \"/%s\");\n", (int)i, fileNames[i] + basePathLen + 1);

        fprintf(file, "    FILE *file%d = fopen(path%d, \"wb\");\n", (int)i, (int)i);
        fprintf(file, "    if (file%d == NULL) {\n", (int)i);
        fprintf(file, "        perror(\"Yeni EXE dosyası oluşturulamadı\");\n");
        fprintf(file, "        return 1;\n");
        fprintf(file, "    }\n");
        fprintf(file, "    fwrite(exeData%d, 1, sizeof(exeData%d), file%d);\n", (int)i, (int)i, (int)i);
        fprintf(file, "    fclose(file%d);\n", (int)i);
    }
    fprintf(file, "    char exePath[MAX_PATH_LENGTH] = \"\";\n");
    fprintf(file, "    snprintf(exePath, sizeof(exePath), \"%%s\\\\%%s\", basePath, exeName);\n");
    fprintf(file, "    if(desktopShortcut) CreateDesktopShortcut(exePath, exeName);\n");
    fprintf(file, "    if(startMenuShortcut) CreateStartMenuShortcut(exePath, exeName);\n");
    fprintf(file, "    return 0;\n");
    fprintf(file, "}\n");

    fclose(file);

    printf("Yeni C dosyası oluşturuldu: %s\n", cFileName);
    
}


void listFilesRecursively(const char *basePath, char **fileList, int *index) {
    char path[MAX_PATH_LENGTH];
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    
    snprintf(path, sizeof(path), "%s/*", basePath);

    
    hFind = FindFirstFile(path, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Invalid directory path: %s\n", basePath);
        return;
    }

    do {
        
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
            snprintf(path, sizeof(path), "%s/%s", basePath, findFileData.cFileName);
            
            
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                
                listFilesRecursively(path, fileList, index);
            } else {
                
                strncpy(fileList[*index], path, MAX_PATH_LENGTH);
                (*index)++;
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}


void listDirectories(const char *basePath, char directories[][MAX_PATH_LENGTH_], int *dirCount) {
    char searchPath[MAX_PATH];
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;

    
    snprintf(searchPath, sizeof(searchPath), "%s/*", basePath);

    hFind = FindFirstFile(searchPath, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Dizin bulunamadı: %s\n", basePath);
        return;
    }

    do {
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                strncpy(directories[*dirCount], findFileData.cFileName, MAX_PATH_LENGTH);
                (*dirCount)++;
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}


int runSetupMaker(char basePath[MAX_PATH_LENGTH], char agreement[100000], char appName[100]) {
    char basePath_[MAX_PATH_LENGTH_];
    char directories[MAX_DIRECTORIES][MAX_PATH_LENGTH_];
    int dirCount = 0;
    char **fileList;
    
    int fileCount = 0;

    listDirectories(basePath, directories, &dirCount);
    fileList = (char **)malloc(MAX_PATH_LENGTH * sizeof(char *));
    if (fileList == NULL) {
        perror("Unable to allocate memory");
        system("pause");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < MAX_PATH_LENGTH; i++) {
        fileList[i] = (char *)malloc(MAX_PATH_LENGTH * sizeof(char));
        if (fileList[i] == NULL) {
            perror("Unable to allocate memory");
            return EXIT_FAILURE;
        }
    }
    listFilesRecursively(basePath, fileList, &fileCount);


    char *fileData[fileCount];
    size_t fileSizes[fileCount];

    for (size_t i = 0; i < fileCount; i++) {
        fileData[i] = readFileToString(fileList[i], &fileSizes[i]);
    }

    createCFileFromData((const char**)fileData, fileSizes, fileCount, "Setup.c", (const char**)fileList, directories, dirCount, basePath, agreement, appName);

    for (size_t i = 0; i < fileCount; i++) {
        free(fileData[i]);
    }

    for (int i = 0; i < MAX_PATH_LENGTH; i++) {
        free(fileList[i]);
    }
    free(fileList);
    return 0;
}
