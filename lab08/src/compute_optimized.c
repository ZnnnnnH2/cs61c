#include "vector.h"
#include "compute.h"
#include <omp.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

// Flips the matrix in-place (not transpose) DO NOT EDIT
int flip(matrix_t *matrix) {
  int idx[8] = {7, 6, 5, 4, 3, 2, 1, 0};
  vector vidx = vec_load((vector *)idx);

  int num_elems = matrix->rows * matrix->cols;
  int loop_one_end = num_elems / 2 / 8 * 8;

  for (int x = 0; x < loop_one_end; x += 8) {
    vector a = vec_load((__m256i *)&matrix->data[x]);
    vector b = vec_load((__m256i *)&matrix->data[(num_elems - x - 8)]);
    a = _mm256_permutevar8x32_epi32(a, vidx);
    b = _mm256_permutevar8x32_epi32(b, vidx);
    vec_store((vector *)&matrix->data[x], b);
    vec_store((vector *)&matrix->data[(num_elems - x - 8)], a);
  }
  for (int x = loop_one_end; x < num_elems / 2; x++) {
    int temp = matrix->data[x];
    matrix->data[x] = matrix->data[(num_elems - x - 1)];
    matrix->data[(num_elems - x - 1)] = temp;
  }
  return 0;
}

// Computes the dot product of vec1 and vec2, both of size n
int dot(uint32_t n, int32_t *vec1, int32_t *vec2) {
  int sum = 0;
  for (int i = 0; i < n; i++) {
    sum += vec1[i] * vec2[i];
  }
  return sum;
}

// Computes the convolution of two matrices
int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
  
  // Flip matrix B
  if (flip(b_matrix))
    return -1;


  // Calculate the number of rows and columns in the output matrix
  int output_rows = a_matrix->rows - b_matrix->rows + 1;
  int output_cols = a_matrix->cols - b_matrix->cols + 1;

  // Allocate space for the output matrix
  *output_matrix = malloc(sizeof(matrix_t));
  if (!*output_matrix)
    return -1;

  
  // Update the rows and columns of the newly created output matrix
  (*output_matrix)->rows = output_rows;
  (*output_matrix)->cols = output_cols;

  // Allocate space for the output matrix data
  (*output_matrix)->data = malloc(sizeof(int32_t) * output_rows * output_cols);
  if (!(*output_matrix)->data)
    return -1;

  // Convolve
  /* DO NOT MODIFY ANYTHING ABOVE THIS LINE */
  
  // TODO: Optimize the convolution
  #pragma ________________________
  for (int o_row = 0; o_row < output_rows; o_row++) {
    for (int o_col = 0; o_col < output_cols; o_col++) {
      int result = 0;
      for (int b_row = 0; b_row < b_matrix->rows; b_row++) {

        vector vsum = _______________;
        for (int b_col = 0; b_col < ________________; b_col += _____________) {

          // Compute the current index in both matrices
          int a_idx = (o_row + b_row) * a_matrix->cols + (o_col + b_col);
          int b_idx = b_row * b_matrix->cols + b_col;

          vector a = vec_load(_____________________);
          vector b = vec_load(_____________________);
          
          vector mul = vec_mul(________, __________);
          vsum = vec_add(vsum, ____________________);
        }

        // reduce
        int sum = 0;
        int vresult[8];
        vec_store(__________, __________);
        for (int i = 0; i < 8; i++) {
            sum += vresult[i];
        }

        // tail case
        for (int b_col = ___________________; b_col < b_matrix->cols; b_col++) {
          
          // Compute the current index in both matrices
          int a_idx = (o_row + b_row) * a_matrix->cols + (o_col + b_col);
          int b_idx = b_row * b_matrix->cols + b_col;

          sum += a_matrix->data[a_idx] * b_matrix->data[b_idx];
        }
        result += sum 
      }
      int output_idx = o_row * output_cols + o_col;
      (*output_matrix)->data[output_idx] = _____________;
    }
  }
  return 0;
}
/* DO NOT MODIFY ANYTHING BELOW THIS LINE (in this function) */

// Executes a task
int execute_task(task_t *task) {
  matrix_t *a_matrix, *b_matrix, *output_matrix;

  char *a_matrix_path = get_a_matrix_path(task);
  if (read_matrix(a_matrix_path, &a_matrix))
    return -1;
  free(a_matrix_path);

  char *b_matrix_path = get_b_matrix_path(task);
  if (read_matrix(b_matrix_path, &b_matrix))
    return -1;
  free(b_matrix_path);

  clock_t begin = clock();
  if (convolve(a_matrix, b_matrix, &output_matrix))
    return -1;
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Runtime: %f\n", time_spent);

  char *output_matrix_path = get_output_matrix_path(task);
  if (write_matrix(output_matrix_path, output_matrix))
    return -1;
  free(output_matrix_path);

  free(a_matrix->data);
  free(b_matrix->data);
  free(output_matrix->data);
  free(a_matrix);
  free(b_matrix);
  free(output_matrix);
  return 0;
}
