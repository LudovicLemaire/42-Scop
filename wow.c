#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <unistd.h>

int main() {
    char *filename = "shaders/fragmentShader.frag";
    if (access(filename, F_OK ) != 0) {
        printf("\x1b[91mShader Path\x1b[0m: %s\n", filename);
        exit(-1);
    } else {
        printf("\x1b[92mShader Path\x1b[0m: %s\n", filename);
    }

    FILE *inputfile = fopen(filename, "r");
    size_t linesize = 0;
    char* linebuf = 0;
    ssize_t linelen = 0;
    struct stat file_info;
    lstat(filename, &file_info);
    int mallocText = file_info.st_size;
    char *completeText = calloc(mallocText, sizeof(char));

    while ((linelen = getline(&linebuf, &linesize, inputfile)) > 0) {
        strcat(completeText, linebuf);
        free(linebuf);
        linebuf = NULL;
    }
    free(linebuf);
    linebuf = NULL;

    printf("%s\n", completeText);
    return 0;
}