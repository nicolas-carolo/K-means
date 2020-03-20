
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h> 

#define N_COORDINATES 2
#define TOLERANCE 0.001

typedef struct {
    double coordinate[N_COORDINATES];
    int cluster_id;
} Point;


void print_points_array(Point *points_array, int n_line);
int assign_cluster(Point point, Point *previous_centroids_array, int n_clusters);
double calc_euclidean_distance(Point point, Point cluster);
Point *calc_centroids(Point *points_array, int n_line, int n_clusters);


int main(int argc, char *argv[]){
    FILE *fin;
    char buf[512];
    int d = 10;
    int i;
    int j;
    int ctr;
    int n_line = 0;
    int n_iteration = 0;
    int isOK = 0;
    Point *points_array = malloc(d * sizeof(Point));
    clock_t t;

    if (argc != 3) {
        printf("USAGE: %s [input file] [n. cluster]\n", argv[0]);
        return 1;
    }

    int n_clusters = atoi(argv[2]);
    Point previous_centroids_array[n_clusters];
    Point actual_centroids_array[n_clusters];
    char newString[N_COORDINATES][512]; 

    if (!(fin = fopen(argv[1], "r"))) {
        printf("the input file '%s' does not exist\n", argv[1]);
		return -1;	
	}

    while (fgets(buf, sizeof(buf), fin)) {
        j = 0;
        ctr = 0;
        for (i = 0; i <= (strlen(buf)); i++) {
        // if space or NULL found, assign NULL into newString[ctr]
            if (buf[i] == ',' || buf[i] == ' ' || buf[i] == '\0' || buf[i] == '\n') {
                newString[ctr][j]='\0';
                ctr++;  //for next word
                j = 0;    //for next word, init index to 0
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
			if (points_array == NULL) {	//se non e' possibile riallocare la memoria il programma termina
				puts("ERROR: impossibile to dynamically allocate other memory");
				return 1;
			}
		}
	}

    fclose(fin);

    //Start chronometer
    t = clock();

    puts("\nInput points:");
    print_points_array(points_array, n_line);


    while (n_iteration < 1 || isOK < n_clusters) {

        printf("\n%d iteration:\n", n_iteration);

        if (n_iteration == 0) {
            for (i = 0; i < n_clusters; i++) {
                memcpy(actual_centroids_array[i].coordinate, points_array[i].coordinate, N_COORDINATES * sizeof(double));
                actual_centroids_array[i].cluster_id = i + 1;
            }
        } else {
            memcpy(actual_centroids_array, calc_centroids(points_array, n_line, n_clusters), n_clusters * sizeof(Point));
        }
        

        for (i = 0; i < n_line; i++) {
            points_array[i].cluster_id = assign_cluster(points_array[i], actual_centroids_array, n_clusters);
        }

        puts("\n\tCentroids:");
        print_points_array(actual_centroids_array, n_clusters);

        puts("\n\tCluster assignment:");
        print_points_array(points_array, n_line);


        isOK = 0;
        if (n_iteration > 0){
            puts("\n\tNew-old centroids distances:");
            for (i = 0; i < n_clusters; i++) {
                double distance = calc_euclidean_distance(previous_centroids_array[i], actual_centroids_array[i]);
                printf("\t%d: error = %lf\n", actual_centroids_array[i].cluster_id, distance);
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

    //Stop chronometer
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



void print_points_array(Point *points_array, int n_line) {
    int i;
    int j;
    for (i = 0; i < n_line; i++) {
        printf("\t%d: ", i + 1);
        for (j = 0; j < N_COORDINATES; j++) {
            printf("%lf ", points_array[i].coordinate[j]);
        }
        printf("assigned to cluster %d\n", points_array[i].cluster_id);
    }
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
    int i;
    double distance_2 = 0;
    for (i = 0; i < N_COORDINATES; i++) {
        distance_2 = distance_2 + pow((point.coordinate[i] - cluster.coordinate[i]), 2);
    }
    return sqrt(distance_2);
}



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