#include <stdio.h>
#include "stats.h"

int main(int argc, char *argv[]) {
    int m = numberOfEdges("test.txt");
    int n = numberOfVertices("test.txt");
    printf("%d\n%d\n", m, n);
    return 0;
}