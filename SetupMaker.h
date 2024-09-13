#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_PATH_LENGTH 1024
#define MAX_DIRECTORIES 100
#define MAX_PATH_LENGTH_ 260

#ifndef SETUPMAKER_H
#define SETUPMAKER_H

static const char agreement[100000] = "End User License Agreement (EULA)\r\n\r\n1. License Grant\r\n    Subject to the terms and conditions of this End User License Agreement (\"Agreement\"), Our Company (\"Licensor\") grants you a non-exclusive, non-transferable, revocable license to use the software application (\"Software\") solely for your personal or internal business purposes.\r\n\r\n2. Restrictions\r\n    You may not:\r\n        Distribute, sell, lease, or sublicense the Software to third parties.\r\n        Modify, decompile, disassemble, or reverse engineer the Software.\r\n        Use the Software to develop a competing product or service.\r\n        Remove or alter any copyright, trademark, or proprietary notices contained in or on the Software.\r\n3. Ownership\r\n    The Software is licensed, not sold, to you. Our Company and its licensors retain all rights, title, and interest in and to the Software, including all intellectual property rights.\r\n\r\n4. Termination\r\n    This Agreement is effective until terminated. Your rights under this Agreement will terminate automatically without notice from Licensor if you fail to comply with any term(s) of this Agreement. Upon termination, you must cease all use of the Software and destroy all copies in your possession.\r\n\r\n5. Disclaimer of Warranties\r\n    The Software is provided \"as is\" without warranty of any kind. Our Company disclaims all warranties, whether express or implied, including but not limited to the implied warranties of merchantability and fitness for a particular purpose. Our Company does not warrant that the Software will be error-free or uninterrupted.\r\n\r\n6. Limitation of Liability\r\n    In no event will Our Company be liable for any direct, indirect, incidental, special, consequential, or punitive damages, or any damages whatsoever, even if Our Company has been advised of the possibility of such damages, arising out of or in connection with the use of or inability to use the Software.\r\n\r\n7. Entire Agreement\r\n    This Agreement constitutes the entire agreement between you and Our Company regarding the Software and supersedes all prior or contemporaneous understandings and agreements, whether written or oral, relating to the Software.";

char* readFileToString(const char *fileName, size_t *outSize);
void createCFileFromData(const char *data[], size_t dataSizes[], size_t count, const char *cFileName,const char *fileNames[], char directories[][MAX_PATH_LENGTH_], size_t dirCount, const char *basePath,  const char *agreement,  const char *appName);
void listFilesRecursively(const char *basePath, char **fileList, int *index);
void listDirectories(const char *basePath, char directories[][MAX_PATH_LENGTH_], int *dirCount) ;
int runSetupMaker(char basePath_[MAX_PATH_LENGTH_], char appName[100], char agreement[100000]);

#endif // SETUPMAKER_H
