#include <stdio.h>
#include "stats.h"

int main(int argc, char *argv[]) {
    int m = numberOfEdges("dummy_test.txt");
    int n = numberOfVertices("dummy_test.txt");
    printf("%d\n%d\n", m, n);
    return 0;
}