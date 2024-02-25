#include "rand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int index = 1;
    int dist = atoi(argv[index++]);
	double a = atof(argv[index++]);
	double b = atof(argv[index++]);
    seed(clock());
    double res = ran_distri(dist, a, b);
    fprintf(stdout, "%d\n", (int)res);
}