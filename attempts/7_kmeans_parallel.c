#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <omp.h> 

#define DIMENSION 115
#define TOLERANCE 0.001

typedef struct {
    double coordinate[DIMENSION];
    int cluster_id;
} Point;


void print_points_array(Point *points_array, int n_line);
int get_index_factor(int n_line, int n_clusters);
int assign_cluster(Point point, Point *previous_centroids_array, int n_clusters);
double calc_euclidean_distance(Point point, Point cluster);
Point *calc_centroids(Point *points_array, int n_line, int n_clusters, int n_threads);


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
    double t, t_part;
    int n_threads, tid;

    if (argc != 3) {
        printf("USAGE: %s [input file] [n. cluster]\n", argv[0]);
        return 1;
    }

    int n_clusters = atoi(argv[2]);
    Point previous_centroids_array[n_clusters];
    Point actual_centroids_array[n_clusters];
    char newString[DIMENSION][512];
    char buf[16384];

    if (!(fin = fopen(argv[1], "r"))) {
        printf("the input file '%s' does not exist\n", argv[1]);
		return -1;	
	}

    while (fgets(buf, sizeof(buf), fin)) {
        j = 0;
        ctr = 0;
        for (i = 0; i <= (strlen(buf)); i++) {
        // if space or NULL found, assign NULL into newString[ctr]
            if (buf[i] == ',' || buf[i] == '\0') {
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
			if (points_array == NULL) {
				puts("ERROR: impossibile to dynamically allocate other memory");
				return 1;
			}
		}
	}

    fclose(fin);

    //Start chronometer
    //t = clock();
    t = omp_get_wtime();

    #pragma omp parallel shared(n_threads, tid)
    {
        tid = omp_get_thread_num();
        n_threads = omp_get_num_threads();
        if (tid == 0) {
            printf("\nNumber of threads: %d\n", n_threads);
            printf("\nInput points (from %d):\n", tid);
            //print_points_array(points_array, n_line);
            puts("");
        }
    }

    //puts("\nInput points:");
    //print_points_array(points_array, n_line);


    while (n_iteration < 1 || isOK < n_clusters) {

        //Take partial time
        t_part = omp_get_wtime() - t;
        printf("Iteration %d (partial time: %lf s)\n", n_iteration, t_part);

        double t_for;

        if (n_iteration == 0) {
            int centroid_index = 0;
            t_for = clock();
            for (i = 0; i < n_clusters; i++) {
                for (j = 0; j < DIMENSION; j++) {
                    actual_centroids_array[i].coordinate[j] = points_array[centroid_index].coordinate[j];
                }
                
                actual_centroids_array[i].cluster_id = i + 1;
                centroid_index += get_index_factor(n_line, n_clusters);
            }
            t_for = clock() - t_for;
            double t_for_taken = ((double)t_for)/CLOCKS_PER_SEC;
            printf("\tTime centroids calculation: %lf\n", t_for_taken);
        } else {
            t_for = omp_get_wtime();
            memcpy(actual_centroids_array, calc_centroids(points_array, n_line, n_clusters, n_threads), n_clusters * sizeof(Point));
            t_for = omp_get_wtime() - t_for;
            printf("\tTime centroids calculation: %lf\n", t_for);
        }
        
        t_for = omp_get_wtime();
        #pragma omp parallel shared(points_array)
        {
            #pragma omp for schedule(dynamic,1)
            for (i = 0; i < n_line; i++) {
                points_array[i].cluster_id = assign_cluster(points_array[i], actual_centroids_array, n_clusters);
            }
        }
        t_for = omp_get_wtime() - t_for;
        printf("\tTime clusters assignment: %lf\n", t_for);   //speedup up to 4 with 8 threads

        //puts("\n\tCentroids:");
        //print_points_array(actual_centroids_array, n_clusters);

        //puts("\n\tCluster assignment:");
        //print_points_array(points_array, n_line);


        isOK = 0;
        if (n_iteration > 0){    
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
    t = omp_get_wtime() - t;

    puts("\n------------------------------------------------------");
    puts("\nPROCESS ENDED SUCCESSFULLY!\n");
    printf("\tExecution time: %lf s\n", t);
    printf("\tNumber of iterations: %d\n", n_iteration - 1);
    printf("\tCentroids:\n");
    for (i = 0; i < n_clusters; i++) {
        printf("\t\t%d: ", actual_centroids_array[i].cluster_id);
        for (j = 0; j < DIMENSION; j++) {
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
        for (j = 0; j < DIMENSION; j++) {
            printf("%lf ", points_array[i].coordinate[j]);
            //printf("%.32f ", points_array[i].coordinate[j]);
        }
        printf("assigned to cluster %d\n", points_array[i].cluster_id);
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
    int i;
    double distance_2 = 0;
    for (i = 0; i < DIMENSION; i++) {
        distance_2 = distance_2 + pow((point.coordinate[i] - cluster.coordinate[i]), 2);
    }
    return sqrt(distance_2);
}



Point *calc_centroids(Point *points_array, int n_line, int n_clusters, int n_threads) {
    Point *centroids;
    centroids = (Point*)calloc(n_clusters, sizeof(Point));
    int n_points[n_clusters];
    memset(n_points, 0, sizeof(n_points));

    #pragma omp parallel
    {
        Point *part_centroids;
        part_centroids = (Point*)calloc(n_clusters, sizeof(Point));
        int part_n_points[n_clusters];
        memset(part_n_points, 0, sizeof(part_n_points));
        int centroid_index;
        #pragma omp for schedule(static) nowait
        for (int i = 0; i < n_line; i++) {
            centroid_index = points_array[i].cluster_id - 1;
            part_n_points[centroid_index]++;
            for (int j = 0; j < DIMENSION; j++) {
                part_centroids[centroid_index].coordinate[j] += points_array[i].coordinate[j];
            }
        }

        #pragma omp for schedule(static) collapse(2) ordered
        for (int i = 0; i < n_threads; i++) {
            for (int j = 0; j < n_clusters; j++) {
                for (int k = 0; k < DIMENSION; k++) {
                    #pragma omp ordered
                    centroids[j].coordinate[k] += part_centroids[j].coordinate[k];
                }
            }
        }
        for (int i = 0; i < n_clusters; i++) {
            #pragma omp critical
            n_points[i] += part_n_points[i];
        }
    }

    for (int i = 0; i < n_clusters; i++) {
        for (int j = 0; j < DIMENSION; j++) {
            centroids[i].coordinate[j] = centroids[i].coordinate[j] / (double)n_points[i];
        }
        centroids[i].cluster_id = i + 1;
    }

    return centroids;
}