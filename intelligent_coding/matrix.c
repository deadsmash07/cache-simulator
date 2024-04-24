#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to generate a random matrix
double **generate_random_matrix(int n)
{
    double **matrix = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++)
    {
        matrix[i] = (double *)malloc(n * sizeof(double));
        for (int j = 0; j < n; j++)
        {
            matrix[i][j] = rand() % 1001; // Random integer between 0 and 1000
        }
    }
    return matrix;
}
// Function to free a matrix
void free_matrix(double **matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

// Multiply matrices with loop order i-j-k
void multiply_ij_k(double **A, double **B, double **C, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < n; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Multiply matrices with loop order i-k-j
void multiply_i_k_j(double **A, double **B, double **C, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int k = 0; k < n; k++)
        {
            double r = A[i][k];
            for (int j = 0; j < n; j++)
            {
                C[i][j] += r * B[k][j];
            }
        }
    }
}

// Multiply matrices with loop order j-i-k
void multiply_j_i_k(double **A, double **B, double **C, int n)
{
    for (int j = 0; j < n; j++)
    {
        for (int i = 0; i < n; i++)
        {
            C[i][j] = 0;
            for (int k = 0; k < n; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Multiply matrices with loop order j-k-i
void multiply_j_k_i(double **A, double **B, double **C, int n)
{
    for (int j = 0; j < n; j++)
    {
        for (int k = 0; k < n; k++)
        {
            double r = B[k][j];
            for (int i = 0; i < n; i++)
            {
                C[i][j] += A[i][k] * r;
            }
        }
    }
}

// Multiply matrices with loop order k-i-j
void multiply_k_i_j(double **A, double **B, double **C, int n)
{
    for (int k = 0; k < n; k++)
    {
        for (int i = 0; i < n; i++)
        {
            double r = A[i][k];
            for (int j = 0; j < n; j++)
            {
                C[i][j] += r * B[k][j];
            }
        }
    }
}

// Multiply matrices with loop order k-j-i
void multiply_k_j_i(double **A, double **B, double **C, int n)
{
    for (int k = 0; k < n; k++)
    {
        for (int j = 0; j < n; j++)
        {
            double r = B[k][j];
            for (int i = 0; i < n; i++)
            {
                C[i][j] += A[i][k] * r;
            }
        }
    }
}

int main()
{
    int matrix_sizes[] = {2, 4, 8, 16,25, 32,40,50, 64,80};
    int num_sizes = sizeof(matrix_sizes) / sizeof(matrix_sizes[0]);
    FILE *file = fopen("performance_data.txt", "w");
    fprintf(file, "LoopOrder,MatrixSize,TimeInMicroseconds\n");

    srand(time(NULL)); // Seed the random number generator

    for (int i = 0; i < num_sizes; i++)
    {
        int n = matrix_sizes[i];
        double **A = generate_random_matrix(n);
        double **B = generate_random_matrix(n);
        double **C = generate_random_matrix(n);

        // Reset C to zero before each multiplication
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                C[i][j] = 0;
            }
        }

        // Time each function and write results
        clock_t start, end;
        double cpu_time_used;

        start = clock();
        multiply_ij_k(A, B, C, n);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000; // Convert to microseconds
        fprintf(file, "i-j-k,%d,%.0f\n", n, cpu_time_used);

        // Repeat for all other functions with different loop orders
        start = clock();
        multiply_i_k_j(A, B, C, n);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000;
        fprintf(file, "i-k-j,%d,%.0f\n", n, cpu_time_used);

        start = clock();
        multiply_j_i_k(A, B, C, n);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000;
        fprintf(file, "j-i-k,%d,%.0f\n", n, cpu_time_used);

        start = clock();
        multiply_j_k_i(A, B, C, n);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000;
        fprintf(file, "j-k-i,%d,%.0f\n", n, cpu_time_used);

        start = clock();
        multiply_k_i_j(A, B, C, n);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000;
        fprintf(file, "k-i-j,%d,%.0f\n", n, cpu_time_used);

        start = clock();
        multiply_k_j_i(A, B, C, n);
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000;
        fprintf(file, "k-j-i,%d,%.0f\n", n, cpu_time_used);

        // Free all matrices
        free_matrix(A, n);
        free_matrix(B, n);
        free_matrix(C, n);
    }

    fclose(file);
    printf("Performance data has been written to performance_data.txt\n");

    return 0;
}
