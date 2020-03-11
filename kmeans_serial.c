#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define error = 0.5

typedef struct {
    double x;
    double y;
    int cluster_id;
} Point;


void print_points_array(Point *points_array, int n_line);
int get_index_factor(int n_lines, int n_cluster);
int assign_cluster(Point point, Point *actual_centroids_array, int n_clusters);
double calc_euclidean_distance(Point point, Point cluster);
Point calc_centroid(Point *points_array, int n_line, int cluster_id);



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

    int n_clusters = atoi(argv[2]);
    int centroid_indexes[n_clusters];
    Point actual_centroids_array[n_clusters];
    Point new_centroids_array[n_clusters];

    if (!(fin = fopen(argv[1], "r"))) {
        printf("the input file '%s' does not exist\n", argv[1]);
		return -1;	
	}

    while (fgets(buf, sizeof(buf), fin)) {
		sscanf(buf, "%lf %lf", &points_array[n_line].x, &points_array[n_line].y);
        points_array[n_line].cluster_id = -1;
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
    int base_index_factor = get_index_factor(n_line, n_clusters);
    int index_factor = 0;
    for (i = 0; i < n_clusters; i++) {
        centroid_indexes[i] = index_factor;
        index_factor = index_factor + base_index_factor;
        actual_centroids_array[i].x = points_array[centroid_indexes[i]].x;
        actual_centroids_array[i].y = points_array[centroid_indexes[i]].y;
        actual_centroids_array[i].cluster_id = centroid_indexes[i];
        printf("%d: (%lf, %lf)\n", actual_centroids_array[i].cluster_id, actual_centroids_array[i].x, actual_centroids_array[i].y);
    }


    for (i = 0; i < n_line; i++) {
        points_array[i].cluster_id = assign_cluster(points_array[i], actual_centroids_array, n_clusters);
    }

    puts("\nFirst cluster assignment:");
    print_points_array(points_array, n_line);


    puts("\nNew centroids:");
    for (i = 0; i < n_clusters; i++) {
        Point centroid = calc_centroid(points_array, n_line, centroid_indexes[i]);
        new_centroids_array[i] = centroid;
        printf("%d: (%lf, %lf)\n", new_centroids_array[i].cluster_id, new_centroids_array[i].x, new_centroids_array[i].y);
    }


    puts("\nNew-old centroids distances:");
    for (i = 0; i < n_clusters; i++) {
        double distance = calc_euclidean_distance(actual_centroids_array[i], new_centroids_array[i]);
        printf("%d: distance = %lf\n", actual_centroids_array[i].cluster_id, distance);
    }


    fclose(fin);
	free(points_array);

    return 0;
}


void print_points_array(Point *points_array, int n_line) {
    int i;
    for (i = 0; i < n_line; i++) {
        printf("(%lf, %lf) assigned to cluster %d\n", points_array[i].x, points_array[i].y, points_array[i].cluster_id);
    }
}


int get_index_factor(int n_line, int n_clusters) {
    return n_line / n_clusters;
}


int assign_cluster(Point point, Point *actual_centroids_array, int n_clusters) {
    int i;
    double min_distance = calc_euclidean_distance(point, actual_centroids_array[0]);
    int cluster_id = actual_centroids_array[0].cluster_id;
    for (i = 1; i < n_clusters; i++) {
        double distance = calc_euclidean_distance(point, actual_centroids_array[i]);
        if (distance < min_distance) {
            min_distance = distance;
            cluster_id = actual_centroids_array[i].cluster_id;
        }
    }
    return cluster_id;
}


double calc_euclidean_distance(Point point, Point cluster) {
    return sqrt(pow((point.x - cluster.x), 2) + pow((point.y - cluster.y), 2));
}


Point calc_centroid(Point *points_array, int n_line, int cluster_id) {
    int i;
    int n_points = 0;
    Point centroid;
    for (i = 0; i < n_line; i++) {
        if (points_array[i].cluster_id == cluster_id) {
            centroid.x = centroid.x + points_array[i].x;
            centroid.y = centroid.y + points_array[i].y;
            n_points++;
        }
    }
    centroid.x = centroid.x / n_points;
    centroid.y = centroid.y /n_points;
    centroid.cluster_id = cluster_id;
    return centroid;
}