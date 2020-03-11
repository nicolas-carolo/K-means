#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define N_COORDINATES 3
#define TOLERANCE 0

typedef struct {
    double coordinate[N_COORDINATES];
    int cluster_id;
} Point;


void print_points_array(Point *points_array, int n_line);
int get_index_factor(int n_lines, int n_cluster);
int assign_cluster(Point point, Point *previous_centroids_array, int n_clusters);
double calc_euclidean_distance(Point point, Point cluster);
Point calc_centroid(Point *points_array, int n_line, int cluster_id);


int main(int argc, char *argv[]){
    FILE *fin;
    char buf[512];
    //char line[512];
    int d = 10;
    int i;
    int n_line = 0;
    int n_iteration = 1;
    int isOK = 0;
    Point *points_array = malloc(d * sizeof(Point));
    //Point points_array[d];

    if (argc != 3) {
        printf("USAGE: %s [input file] [n. cluster]\n", argv[0]);
        return 1;
    }

    int n_clusters = atoi(argv[2]);
    int centroid_indexes[n_clusters];
    Point previous_centroids_array[n_clusters];
    Point actual_centroids_array[n_clusters];

    char newString[N_COORDINATES][512]; 
    int j;
    int ctr;

    // int error_array[n_clusters];

    if (!(fin = fopen(argv[1], "r"))) {
        printf("the input file '%s' does not exist\n", argv[1]);
		return -1;	
	}

    while (fgets(buf, sizeof(buf), fin)) {
		// sscanf(buf, "%s\n", line);
        //printf("%s", buf);
        j = 0;
        ctr = 0;
        for (i = 0; i <= (strlen(buf)); i++) {
        // if space or NULL found, assign NULL into newString[ctr]
            if (buf[i] == ' ' || buf[i] == '\0' || buf[i] == '\n') {
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

    puts("\nInput points:");
    print_points_array(points_array, n_line);


    while (n_iteration < 1 || isOK < n_clusters) {
    //while (n_iteration < 2) {

        printf("\n%d iteration:\n", n_iteration);

        puts("\n\tActual centroids:");
        if (n_iteration == 1) {
            int base_index_factor = get_index_factor(n_line, n_clusters);
            int index_factor = 0;
            for (i = 0; i < n_clusters; i++) {
                centroid_indexes[i] = index_factor;
                printf("\t%d: ", centroid_indexes[i]);
                for (j = 0; j < N_COORDINATES; j++) {
                    actual_centroids_array[i].coordinate[j] = points_array[centroid_indexes[i]].coordinate[j];
                    printf("%lf ", actual_centroids_array[i].coordinate[j]);
                }
                printf("\n");
                index_factor = index_factor + base_index_factor;
                actual_centroids_array[i].cluster_id = centroid_indexes[i];
            }
        } else {
            for (i = 0; i < n_clusters; i++) {
                Point centroid = calc_centroid(points_array, n_line, centroid_indexes[i]);
                actual_centroids_array[i] = centroid;
                //printf("\t%d: (%lf, %lf)\n", actual_centroids_array[i].cluster_id, actual_centroids_array[i].x, actual_centroids_array[i].y);
                printf("\t%d: ", centroid_indexes[i]);
                for (j = 0; j < N_COORDINATES; j++) {
                    printf("%lf ", actual_centroids_array[i].coordinate[j]);
                }
                printf("\n");
            }
        }
        

        for (i = 0; i < n_line; i++) {
            points_array[i].cluster_id = assign_cluster(points_array[i], actual_centroids_array, n_clusters);
        }

        puts("\n\tCluster assignment:");
        print_points_array(points_array, n_line);


        isOK = 0;
        if (n_iteration > 1){
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
    
	free(points_array);

    return 0;
}



void print_points_array(Point *points_array, int n_line) {
    int i;
    int j;
    for (i = 0; i < n_line; i++) {
        printf("\t%d: ", i);
        for (j = 0; j < N_COORDINATES; j++) {
            printf("%lf ", points_array[i].coordinate[j]);
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
    for (i = 0; i < N_COORDINATES; i++) {
        distance_2 = distance_2 + pow((point.coordinate[i] - cluster.coordinate[i]), 2);
    }
    return sqrt(distance_2);
}


Point calc_centroid(Point *points_array, int n_line, int cluster_id) {
    int i;
    int j;
    int n_points = 0;
    Point centroid;
    for (i = 0; i < n_line; i++) {
        if (points_array[i].cluster_id == cluster_id) {
            for (j = 0; j < N_COORDINATES; j++) {
                centroid.coordinate[j] = centroid.coordinate[j] + points_array[i].coordinate[j];
            }
            //centroid.x = centroid.x + points_array[i].x;
            //centroid.y = centroid.y + points_array[i].y;
            n_points++;
        }
    }
    for (i = 0; i < N_COORDINATES; i++) {
        centroid.coordinate[i] = centroid.coordinate[i] / n_points;
    }
    centroid.cluster_id = cluster_id;
    /*
    centroid.x = centroid.x / n_points;
    centroid.y = centroid.y /n_points;
    centroid.cluster_id = cluster_id;
    */
    return centroid;
}