//serial code for 'kmeans'

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define N_COORDINATES 115

struct centroid
{
    double coordinates_centroid[N_COORDINATES];
    double coordinates_previous_centroid[N_COORDINATES];
};

struct point
{
    int name_centroid;
    double coordinates_point[N_COORDINATES];
};

void centroid_void(struct centroid *c, int d, int i, int k, int n_row)
{
    c[d].coordinates_centroid[i]= c[d].coordinates_previous_centroid[i];
}

int euclidean_distance(double *coordinates, struct centroid *c, int k, int n_row)
{
    int cluster;
    double minimum;
    double coord_point[N_COORDINATES];
    double distance_point;
    
    for (int d = 0; d<k; d++)
    {
        distance_point = 0;
        for (int i = 0; i<N_COORDINATES; i++)
        {
            distance_point += pow((coordinates[i] - c[d].coordinates_centroid[i]), 2);
        }
        if (d == 0)
        {
            minimum = distance_point;
            cluster = d;

        }
        if(distance_point<minimum)
        {
            minimum = distance_point;
            cluster = d;
        }
    }
    return cluster;
}

void mean_coordinates(double *coordinates, struct centroid *c, struct point *p, int d, int k, int n_row)
{
    double mean[N_COORDINATES] = {0};
    int count = 0;
    for (int row = 0; row<n_row; row++)
    {
        if (p[row].name_centroid == d)
        {
            for (int i = 0; i<N_COORDINATES; i++)
            {
                mean[i] += p[row].coordinates_point[i];
            }
            count++;
        }
    }
    if (count != 0)
    {
        for (int i = 0; i<N_COORDINATES; i++)
        {
            coordinates[i] = mean[i]/count;
        }
    } else {
        for(int i = 0; i<N_COORDINATES; i++)
        {
            centroid_void(c, d, i, k, n_row);
        }
    }
}

int difference_centroids(struct centroid *c, int k, int n_row)
{
    double difference = 0;
    int count = 0;
    for (int d = 0; d<k; d++)
    {
        difference = 0;
        for (int i = 0; i<N_COORDINATES; i++)
        {
            difference += pow((c[d].coordinates_centroid[i] - c[d].coordinates_previous_centroid[i]), 2);
        }
        difference = sqrt(difference);
        if (difference > 0.001)
        {
            count ++;
        }
    }
    return count;
}

void selection_centroid(struct point *p, struct centroid *c, int k, int n_row)
{
  int count = n_row/k;

  for(int d = 0; d<k; d++)
  {
      for(int row = 0; row<n_row; row++)
      {
          for(int i = 0; i<N_COORDINATES; i++)
          {
              if(row == (d*count))
              {
                  c[d].coordinates_centroid[i] = p[row].coordinates_point[i];
              }
          }
      }
  }
}

int main(int argc, char *argv[]) {
    clock_t t;
    int d = 10;
    int index_1 = 0;
    int index_2 = 0;
    int index_3 = 0;
    int n_row = 0;
    char row[20000];
    char string[N_COORDINATES][512];

    //load the dataset
    FILE *stream;

    if(argc != 3){
        printf("error\n");
        return 1;
    }

    int k = atoi(argv[2]);

    //structs
    struct centroid centroids[k];
    struct point *points = malloc(d * sizeof(struct point));

    if ((stream = fopen(argv[1], "r")) == NULL)
    {
        printf("File can not be opened!");
        return -1;
    }

    t = clock();

    //store the values of each point

    while(fgets(row, sizeof(row), stream) != NULL)
    {
        index_2 = 0;
        index_3 = 0;

        for (index_1 = 0; index_1 <= strlen(row); index_1++){
            if (row[index_1] == ',' || row[index_1] == '\0'){
                string[index_3][index_2] = '\0';
                index_3++;
                index_2 = 0;
            } else {
                string[index_3][index_2] = row[index_1];
                index_2++;
            }
        }

        for(index_1 = 0; index_1<index_3; index_1++){
            points[n_row].coordinates_point[index_1] = atof(string[index_1]);
        }

        points[n_row].name_centroid = -1;

        n_row++;

        if (n_row +1 > d){
            d = d + 1;
            points = realloc(points, d * sizeof(struct point));
            if(points == NULL){
                printf("Error!\n");
                return 1;
            }
        }
    }

    fclose(stream);

    //choose the centroids
    selection_centroid(points, centroids, k, n_row);
    
    //loop with euclidean distance and new centroids
    int check = 0;
    int n_iterations = 0;
    do {
        for (int row = 0; row<n_row; row++)
        {
            points[row].name_centroid = euclidean_distance(points[row].coordinates_point, centroids, k, n_row);

        }
        for (int d=0; d<k; d++)
        {
            memcpy (centroids[d].coordinates_previous_centroid, centroids[d].coordinates_centroid, sizeof(centroids[d].coordinates_centroid));
            mean_coordinates(centroids[d].coordinates_centroid, centroids, points, d, k, n_row);
        }
        check = difference_centroids(centroids, k, n_row);
        n_iterations++;
        printf("number of iterations: %d\n", n_iterations);
    }
    while (check != 0);

    //computation of time
    t = clock() - t;

    double time_taken = ((double)t/CLOCKS_PER_SEC);
    printf("time: %lf\n", time_taken);

    for (int d = 0; d<k; d++)
    {
        printf("\n");
        for (int i = 0; i<N_COORDINATES; i++)
        {
            printf("centroid: %lf\n", centroids[d].coordinates_centroid[i]);
        }
   
        for (int row = 0; row<n_row; row++)
        {
            if (points[row].name_centroid == d)
            {
        }
        }
    }

    return 0;
}
