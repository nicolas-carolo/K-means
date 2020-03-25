//serial code for 'kmeans'

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define k 10
#define number_points 49548
#define number_coordinates 115

struct centroid
{
    double coordinates_centroid[number_coordinates];
    double coordinates_previous_centroid[number_coordinates];
};

struct centroid centroids[k];

struct point
{
    int name_centroid;
    double coordinates_point[number_coordinates];
};

struct point points[number_points];

void centroid_void(struct centroid *c, int d, int i)
{
    c[d].coordinates_centroid[i]= c[d].coordinates_previous_centroid[i];
}

int euclidean_distance(double *coordinates, struct centroid *c)
{
    int cluster;
    double minimum;
    double coord_point[number_coordinates];
    double distance_point;
    
    for (int d = 0; d<k; d++)
    {
        distance_point = 0;
        for (int i = 0; i<number_coordinates; i++)
        {
            //printf("%lf\n", coordinates[i]);
            //printf("%lf\n", c[d].coordinates_centroid[i]);
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
        //printf("distance %lf\n", distance_point);
        //printf("minimum %lf\n", minimum);
        //printf("%d\n", cluster);
    }
    //printf("%d\n", cluster);
    return cluster;
}

void mean_coordinates(double *coordinates, struct point *p, int d)
{
    double mean[number_coordinates] = {0};
    int count = 0;
    for (int row = 0; row<number_points; row++)
    {
        if (p[row].name_centroid == d)
        {
            for (int i = 0; i<number_coordinates; i++)
            {
                mean[i] += p[row].coordinates_point[i];
                //printf("maoooooooooooooooooooooooooooo %lf\n", mean[i]);
            }
            count++;
        }
    }
    if (count != 0)
    {
        for (int i = 0; i<number_coordinates; i++)
        {
            //printf("maoooooooooooooooooooooooooooo %lf\n", mean[i]);
            //printf("%d\n", count);
            coordinates[i] = mean[i]/count;
            //printf("provaaaaaaaaaaaaa %lf\n", coordinates[i]);
        }
    } else {
        for(int i = 0; i<number_coordinates; i++)
        {
            printf("ERRORRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR\n");
            centroid_void(centroids, d, i);
        }
    }
}

int difference_centroids(struct centroid *c)
{
    double difference = 0;
    int count = 0;
    for (int d = 0; d<k; d++)
    {
        difference = 0;
        for (int i = 0; i<number_coordinates; i++)
        {
            difference += pow((c[d].coordinates_centroid[i] - c[d].coordinates_previous_centroid[i]), 2);
        }
        difference = sqrt(difference);
        //printf("ddddddd %lf\n", difference);
        if (difference > 0.001)
        {
            count ++;
        }
    }
    //printf("cccccc %d\n", count);
    //return count;
    return count;
}

void selection_centroid(struct point *p, struct centroid *c)
{
  int count = number_points/k;
  //printf("%d\n", count);

  for(int d = 0; d<k; d++)
  {
      for(int row = 0; row<number_points; row++)
      {
          for(int i = 0; i<number_coordinates; i++)
          {
              if(row == (d*count))
              {
                  c[d].coordinates_centroid[i] = p[row].coordinates_point[i];
                  //printf("bbbbbbbbbbbbbbbbbbb %lf\n", c[d].coordinates_centroid[i]);
              }
          }
      }
  }
}

int main() {
    clock_t t;

    //load the dataset
    FILE *stream;
    stream = fopen("dataset6.txt", "r");

    if ((stream = fopen("dataset6.txt", "r")) == NULL)
    {
        printf("File can not be opened!");
        exit(1);
    }

    stream = fopen("dataset6.txt", "r");

    t = clock();

    //store the values of each point
    for (int row = 0; row<number_points; row++)
    {
        for (int i = 0; i<number_coordinates; i++)
        {
            fscanf(stream, "%lf,", &points[row].coordinates_point[i]);
            //printf("%lf\n", points[row].coordinates_point[i]);
        }
    }

    fclose(stream);

    //choose the centroids
    selection_centroid(points, centroids);
    
    //loop with euclidean distance and new centroids
    int check = 0;
    int n_iterations = 0;
    do {
        for (int row = 0; row<number_points; row++)
        {
            points[row].name_centroid = euclidean_distance(points[row].coordinates_point, centroids);
            //printf("new name %d\n", points[row].name_centroid);
        }
        for (int d=0; d<k; d++)
        {
            memcpy (centroids[d].coordinates_previous_centroid, centroids[d].coordinates_centroid, sizeof(centroids[d].coordinates_centroid));
            mean_coordinates(centroids[d].coordinates_centroid, points, d);
        }
        check = difference_centroids(centroids);
        n_iterations++;
        printf("ooooooooooooooo %d\n", n_iterations);
    }
    while (check != 0);

    //computation of time
    t = clock() - t;

    double time_taken = ((double)t/CLOCKS_PER_SEC);
    printf("timeeeeeeeee %lf\n", time_taken);

    for (int d = 0; d<k; d++)
    {
        printf("\n");
        for (int i = 0; i<number_coordinates; i++)
        {
            printf("centroide %lf\n", centroids[d].coordinates_centroid[i]);
        }
   
        for (int row = 0; row<number_points; row++)
        {
            if (points[row].name_centroid == d)
            {
            //printf("%lf %lf\n", points[row].coordinates_point[0], points[row].coordinates_point[1]);
        }
        }
    }

    return 0;
}
