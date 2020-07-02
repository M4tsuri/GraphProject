#include<stdlib.h>
#include<stdio.h>
#include "stats.h"

int maxDegree(Graph *mainGraph)
{
    int *temp = (int)calloc(mainGraph ->_vertexNum, sizeof(int));
	int D;

    for (int i = 0; i < mainGraph->_edgeNum; i++) {
    int next = mainGraph->_edgeList[i].nextID;
    int now =  mainGraph->_edgeList[i].to;
    temp[now]++;
    temp[next]++;           
    } 

    int maxD = temp[0];  
    for (int i = 1; i < mainGraph->_vertexNum; i++) {
        if (D > maxD) {
            maxD = D;
        } 
    }

	return maxD;
}

float freemanNetworkCentrality(char name[])
{
    if (!mainGraph) {
        initGraph(name);
    }
    int max = maxDegree(mainGraph);
    int n = mainGraph->_vertexNum;
    int m = mainGraph->_edgeNum;
    float center = (n * max) / ((n - 1)(n - 2));
    return center;
}
