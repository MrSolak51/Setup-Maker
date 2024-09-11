#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_PATH_LENGTH 1024
#define MAX_DIRECTORIES 100
#define MAX_PATH_LENGTH_ 260

#ifndef SETUPMAKER_H
#define SETUPMAKER_H

char* readFileToString(const char *fileName, size_t *outSize);
void createCFileFromData(const char *data[], size_t dataSizes[], size_t count, const char *cFileName,const char *fileNames[], char directories[][MAX_PATH_LENGTH_], size_t dirCount, const char *basePath,  const char *agreement,  const char *appName);
void listFilesRecursively(const char *basePath, char **fileList, int *index);
void listDirectories(const char *basePath, char directories[][MAX_PATH_LENGTH_], int *dirCount) ;
int runSetupMaker(char basePath_[MAX_PATH_LENGTH_], char appName[100], char agreement[100000]);

#endif // SETUPMAKER_H
