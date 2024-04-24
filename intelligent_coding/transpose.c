#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to generate a random square matrix
double** generate_random_matrix(int n) {
    double** matrix = (double**) malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double*) malloc(n * sizeof(double));
        for (int j = 0; j < n; j++) {
            matrix[i][j] = (double) rand() / RAND_MAX;
        }
    }
    return matrix;
}

// Function to free a matrix
void free_matrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Function to transpose a matrix using an additional array
void transpose_with_extra_array(double** src, double** dest, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dest[j][i] = src[i][j];
        }
    }
}

// Function to transpose a matrix in-place (only for square matrices)
void transpose_in_place(double** matrix, int n) {
    double temp;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            temp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp;
        }
    }
}

int main() {
    int matrix_sizes[] = { 4,  16,25, 32,45, 64, 84, 96,110, 128, 256, 512};
    int num_sizes = sizeof(matrix_sizes) / sizeof(matrix_sizes[0]);
    FILE *file = fopen("transpose_performance_data.txt", "w");
    fprintf(file, "Method,MatrixSize,TimeInMicroseconds\n");

    srand(time(NULL)); // Seed the random number generator

    for (int i = 0; i < num_sizes; i++) {
        int n = matrix_sizes[i];
        double** matrix = generate_random_matrix(n);
        double** transposed = generate_random_matrix(n); // used for extra array method

        clock_t start, end;
        double cpu_time_used;

        // Transpose using an additional array
        start = clock();
        transpose_with_extra_array(matrix, transposed, n);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC * 1000000;
        fprintf(file, "Extra Array,%d,%.0f\n", n, cpu_time_used);

        // Transpose in-place
        start = clock();
        transpose_in_place(matrix, n);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC * 1000000;
        fprintf(file, "In-place,%d,%.0f\n", n, cpu_time_used);

        // Free all matrices
        free_matrix(matrix, n);
        free_matrix(transposed, n);
    }

    fclose(file);
    printf("Transpose performance data has been written to transpose_performance_data.txt\n");

    return 0;
}
