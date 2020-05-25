
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h> 

#define N_COORDINATES 115
#define TOLERANCE 0.001

typedef struct {
    double coordinate[N_COORDINATES];
    int cluster_id;
} Point;


int get_index_factor(int n_line, int n_clusters);
int assign_cluster(Point point, Point *previous_centroids_array, int n_clusters);
double calc_euclidean_distance(Point point, Point cluster);
Point *calc_centroids(Point *points_array, int n_line, int n_clusters);


int main(int argc, char *argv[]){
    FILE *fin;
    int d = 10;
    int i;
    int j;
    int ctr;
    int n_line = 0;
    int n_iteration = 0;
    int isOK = 0;
    Point *points_array = malloc(d * sizeof(Point));
    clock_t t, t_part;

    // Print the help
    if (argc != 3) {
        printf("USAGE: %s [input file] [n. cluster]\n", argv[0]);
        return 1;
    }

    int n_clusters = atoi(argv[2]);
    Point previous_centroids_array[n_clusters];
    Point actual_centroids_array[n_clusters];
    char newString[N_COORDINATES][512];
    char buf[16384];

    // Check if the input file exists
    if (!(fin = fopen(argv[1], "r"))) {
        printf("the input file '%s' does not exist\n", argv[1]);
		return -1;	
	}

    // Load the data from the specified CSV
    while (fgets(buf, sizeof(buf), fin)) {
        j = 0;
        ctr = 0;
        for (i = 0; i <= (strlen(buf)); i++) {
            if (buf[i] == ',' || buf[i] == '\0') {
                newString[ctr][j]='\0';
                ctr++;
                j = 0;
            } else{
                newString[ctr][j] = buf[i];
                j++;
            }
        }
        for (i = 0; i < ctr; i++) {
            points_array[n_line].coordinate[i] = atof(newString[i]);
        }

        points_array[n_line].cluster_id = -1;

        n_line++;
        
        if (n_line + 1 > d) {
			d = d + 1;
			points_array = realloc(points_array, d * sizeof(Point));
			if (points_array == NULL) {
				puts("ERROR: impossibile to dynamically allocate other memory");
				return 1;
			}
		}
	}

    fclose(fin);

    //Start the chronometer
    t = clock();


    while (n_iteration < 1 || isOK < n_clusters) {
        t_part = clock() - t;
        double partial_time_taken = ((double)t_part)/CLOCKS_PER_SEC;
        printf("Iteration %d (partial time: %lf s)\n", n_iteration, partial_time_taken);
        clock_t t_for;
        if (n_iteration == 0) {
            int centroid_index = 0;
            t_for = clock();
            for (i = 0; i < n_clusters; i++) {
                for (j = 0; j < N_COORDINATES; j++) {
                    actual_centroids_array[i].coordinate[j] = points_array[centroid_index].coordinate[j];
                }
                actual_centroids_array[i].cluster_id = i + 1;
                centroid_index += get_index_factor(n_line, n_clusters);
            }
            t_for = clock() - t_for;
            double t_for_taken = ((double)t_for)/CLOCKS_PER_SEC;
            printf("\tTime centroids calculation: %lf\n", t_for_taken);
        } else {
            t_for = clock();
            memcpy(actual_centroids_array, calc_centroids(points_array, n_line, n_clusters), n_clusters * sizeof(Point));
            t_for = clock() - t_for;
            double t_for_taken = ((double)t_for)/CLOCKS_PER_SEC;
            printf("\tTime centroids calculation: %lf\n", t_for_taken);
        }
        
        t_for = clock();
        for (i = 0; i < n_line; i++) {
            points_array[i].cluster_id = assign_cluster(points_array[i], actual_centroids_array, n_clusters);
        }
        t_for = clock() - t_for;
        double t_for_taken = ((double)t_for)/CLOCKS_PER_SEC;
        printf("\tTime clusters assignment: %lf\n", t_for_taken);

        isOK = 0;
        if (n_iteration > 0){
            for (i = 0; i < n_clusters; i++) {
                double distance = calc_euclidean_distance(previous_centroids_array[i], actual_centroids_array[i]);
                previous_centroids_array[i] = actual_centroids_array[i];
                if (distance <= TOLERANCE) {
                    isOK++;
                }
            }

        } else {
            for (i = 0; i < n_clusters; i++) {
                previous_centroids_array[i] = actual_centroids_array[i];
            }
        }
        n_iteration++;
    }

    //Stop the chronometer
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;

    puts("\n------------------------------------------------------");
    puts("\nPROCESS ENDED SUCCESSFULLY!\n");
    printf("\tExecution time: %lf s\n", time_taken);
    printf("\tNumber of iterations: %d\n", n_iteration - 1);
    printf("\tCentroids:\n");
    for (i = 0; i < n_clusters; i++) {
        printf("\t\t%d: ", actual_centroids_array[i].cluster_id);
        for (j = 0; j < N_COORDINATES; j++) {
            printf("%lf ", actual_centroids_array[i].coordinate[j]);
        }
        printf("\n");
    }

    puts("\n------------------------------------------------------\n");
    
	free(points_array);

    return 0;
}



/*
    Get the index that is used for selecting initial clusters
*/
int get_index_factor(int n_line, int n_clusters) {
    //printf("index factor: %d\n", n_line / n_clusters);
    return n_line / n_clusters;
}



/*
    Given a point, assign it to its cluster
*/
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



/*
    Calc the euclidean distance between a point and its cluster
*/
double calc_euclidean_distance(Point point, Point cluster) {
    int i;
    double distance_2 = 0;
    for (i = 0; i < N_COORDINATES; i++) {
        distance_2 = distance_2 + pow((point.coordinate[i] - cluster.coordinate[i]), 2);
    }
    return sqrt(distance_2);
}



/*
    Given the points array, calculate the new centroids
*/
Point *calc_centroids(Point *points_array, int n_line, int n_clusters) {
    int i;
    int j;
    Point *centroids = malloc(n_clusters * sizeof(Point));
    int n_points[n_clusters];
    int centroid_index;
    memset(n_points, 0, sizeof(n_points));
    
    for (i = 0; i < n_line; i++) {
        centroid_index = points_array[i].cluster_id - 1;
        for (j = 0; j < N_COORDINATES; j++) {
            if (n_points[centroid_index] == 0) {
                centroids[centroid_index].coordinate[j] = 0;
            }
            centroids[centroid_index].coordinate[j] += points_array[i].coordinate[j];
        }
        n_points[centroid_index]++;
    }

    for (i = 0; i < n_clusters; i++) {
        for (j = 0; j < N_COORDINATES; j++) {
            centroids[i].coordinate[j] = centroids[i].coordinate[j] / (double)n_points[i];
        }
        centroids[i].cluster_id = i + 1;
    }

    return centroids;
}