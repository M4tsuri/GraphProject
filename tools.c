#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include "search.h"


int strcmp(const char *p1, const char *p2);


/* Compare S1 and S2, returning less than, equal to or
   greater than zero if S1 is lexicographically less than,
   equal to or greater than S2.  */
int strcmp (const char *p1, const char *p2) {

    const unsigned char *s1 = (const unsigned char *) p1;
    const unsigned char *s2 = (const unsigned char *) p2;
    unsigned char c1, c2;

    do {
        c1 = (unsigned char) *s1++;
        c2 = (unsigned char) *s2++;
        if (c1 == '\0')
            return c1 - c2;
    } while (c1 == c2);
    
    return c1 - c2;
}
/* from glibc 2.29 strcmp.c */

/* function for making a syscall link open and mmap */
inline __attribute__((always_inline))
long long syscall(long long n, 
                  long long a1, long long a2, 
                  long long a3, long long a4, 
                  long long a5, long long a6) {
    long long ret;
    register long long r8 __asm__("r8") = a5;
    register long long r9 __asm__("r9") = a6;
    register long long r10 __asm__("r10") = a4;
    __asm__ __volatile__ (
        "syscall\n\t"
        : "=a"(ret)
        : "a"(n),
          "D"(a1),
          "S"(a2),
          "d"(a3),
          "r"(r10),
          "r"(r8),
          "r"(r9)
        : "memory",
          "rcx",
          "r11"
    );
    return ret;
}


/* read three numbers from string and store it in a, b, c */
inline void readTri(char **line, int *a, int *b, int *c) {
    char *tmp = NULL;
    for (tmp = *line; *tmp != ' ' && *tmp != 0; ++tmp);
    *tmp = 0;
    *a = atoi(*line);
    *line = tmp + 1;
    for (tmp = *line; *tmp != ' ' && *tmp != 0; ++tmp);
    *tmp = 0;
    *b = atoi(*line);
    *line = tmp + 1;
    for (tmp = *line; *tmp != '\n' && *tmp != 0 && *tmp != ' '; ++tmp);
    *tmp = 0;
    *c = atoi(*line);
    *line = tmp + 1;
}



/* initialize an array with a specified value num */
inline void initArray(int *start, int length, int num) {
    for (int i = 0; i < length; ++i) {
        start[i] = num;
    }
}


void munmapFile(char *fileContent, int fd, size_t fileSize) {
    SYS_MUNMAP((long long)fileContent, fileSize);
    SYS_CLOSE(fd);
}

char *mmapFile(char filename[], int *fd, size_t *size) {
    /* open source file */
    *fd = SYS_OPEN((long long)filename, 0, 0);
    if (*fd == -1) {
        return NULL;
    }

    /* get file length */
    long long int fileSize = SYS_LSEEK(*fd, 0, 2);
    char *fileContent = (char *)SYS_MMAP(0, fileSize, 0x2, 0x2, *fd, 0);
    *size = fileSize;
    return fileContent;
}