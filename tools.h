#ifndef TOOLS_H
#define TOOLS_H

void (*parseArguments(int, char **))(void);
void readTri(char **line, int *a, int *b, int *c);
int strcmp(const char *p1, const char *p2);

long long syscall(long long n, long long a1, 
                  long long a2, long long a3, 
                  long long a4, long long a5, 
                  long long a6);


#define SYS_OPEN(filename, flags, mode) \
    syscall(2, (filename), (flags), (mode), 0, 0, 0);

#define SYS_LSEEK(fd, offset, origin) \
    syscall(8, (fd), (offset), (origin), 0, 0 ,0);

#define SYS_MMAP(addr, len, prot, flags, fd, off) \
    syscall(9, (addr), (len), (prot), (flags), (fd), (off))

#define SYS_MUNMAP(addr, len) \
    syscall(11, addr, len, 0, 0, 0, 0)

#define SYS_CLOSE(fd) \
    syscall(3, fd, 0, 0, 0, 0, 0)


#endif