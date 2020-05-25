# K-means
A simple multithread implementation of the n-dimensional K-means algorithm developed in C using OpenMP.


## Usage
Edit the constant `DIMENSION` with the _n_ dimension of the coordinates and `TOLERANCE` with a tollerance value of your choice.
The input CSV must contain only the numerical values of the coordinates and not the headers line.

### Linux
Execution:
```sh
$ gcc kmeans_serial.c -o kmeans_serial -lm -O3
$ gcc kmeans_parallel.c -o kmeans_parallel -lm -O3 -fopenmp
$ ./kmeans_serial <input csv> <number of clusters>
$ ./kmeans_parallel <input csv> <number of clusters>
```

### macOS
Install `gcc-8`:
```sh
$ brew install gcc@8
```
Execution:
```sh
$ gcc-8 kmeans_serial.c -o kmeans_serial -lm -O3
$ gcc-8 kmeans_parallel.c -o kmeans_parallel -lm -O3 -fopenmp
$ ./kmeans_serial <input csv> <number of clusters>
$ ./kmeans_parallel <input csv> <number of clusters>
```