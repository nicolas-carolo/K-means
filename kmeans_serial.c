#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    double x;
    double y;
} Point;

void print_points_array(Point *points_array, int n_line);
int get_index_factor(int n_lines, int n_cluster);



int main(int argc, char *argv[]){
    FILE *fin;
    char buf[512];
    int d = 10;
    int n_line = 0;
    Point *points_array = malloc(d * sizeof(Point));

    if (argc != 3) {
        printf("USAGE: %s [input file] [n. cluster]\n", argv[0]);
        return 1;
    }

    int n_cluster = atoi(argv[2]);
    int cluster_indexes[n_cluster];

    if (!(fin = fopen(argv[1], "r"))) {
        printf("the input file '%s' does not exist\n", argv[1]);
		return -1;	
	}

    while (fgets(buf, sizeof(buf), fin)) {
		sscanf(buf, "%lf %lf", &points_array[n_line].x, &points_array[n_line].y);
		n_line++;
		if (n_line > d) {
			d = d + 2;
			points_array = realloc(points_array, d * sizeof(Point));
			if (points_array == NULL) {	//se non e' possibile riallocare la memoria il programma termina
				puts("ERROR: impossibile to dynamically allocate other memory");
				return 1;
			}
		}
	}

    puts("Input points:");
    print_points_array(points_array, n_line);


    puts("\nClusters:");
    int i;
    int base_index_factor = get_index_factor(n_line, n_cluster);
    int index_factor = 0;
    for (i = 0; i < n_cluster; i++) {
        cluster_indexes[i] = index_factor;
        index_factor = index_factor + base_index_factor;
    }


    for (i = 0; i < n_cluster; i++) {
        printf("%d\n", cluster_indexes[i]);
    }


    fclose(fin);
	free(points_array);

    return 0;
}


void print_points_array(Point *points_array, int n_line) {
    int i;
    for (i = 0; i < n_line; i++) {
        printf("(%lf, %lf)\n", points_array[i].x, points_array[i].y);
    }
}


int get_index_factor(int n_line, int n_cluster) {
    return n_line / n_cluster;
}