#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "xtensa/tie/xt_hifi5.h"

#define ROWS 9
#define COLS 16

void matrix_vector_scalar(int8_t matrix[ROWS][COLS], int16_t *vec, int32_t *out) {
    for (int i = 0; i < ROWS; ++i) {
        out[i] = 0;
        for (int j = 0; j < COLS; ++j) {
            out[i] += matrix[i][j] * vec[j];
        }
    }
}

void matrix_vector_dsp(int8_t matrix[ROWS][COLS], int16_t *vec, int32_t *out) {
    int no_of_row_blocks = (ROWS / 4) * 4;
    int no_of_col_blocks = (COLS / 8) * 8;

    for (int i = 0; i < no_of_row_blocks; i += 4) {
        ae_int32x2 acc1 = AE_ZERO32();
        ae_int32x2 acc2 = AE_ZERO32();
        ae_int32x2 acc3 = AE_ZERO32();
        ae_int32x2 acc4 = AE_ZERO32();

        for (int j = 0; j < no_of_col_blocks; j += 8) {
            ae_int8x8 row0 = *((ae_int8x8 *)&matrix[i + 0][j]);
            ae_int8x8 row1 = *((ae_int8x8 *)&matrix[i + 1][j]);
            ae_int8x8 row2 = *((ae_int8x8 *)&matrix[i + 2][j]);
            ae_int8x8 row3 = *((ae_int8x8 *)&matrix[i + 3][j]);

            ae_int16x4 v0 = AE_L16X4_I((ae_int16x4 *)&vec[j], 0);
            ae_int16x4 v1 = AE_L16X4_I((ae_int16x4 *)&vec[j + 4], 0);

            AE_MULA8Q8X16(acc1, acc2, row0, row1, row2, row3, v0, v1);
        }
        out[i + 0] = AE_MOVAD32_H(acc1);
        out[i + 1] = AE_MOVAD32_L(acc1);
        out[i + 2] = AE_MOVAD32_H(acc2);
        out[i + 3] = AE_MOVAD32_L(acc2);
    }

    for (int i = no_of_row_blocks; i < ROWS; i++) {
		int32_t sum = 0;
		for (int j = 0; j < COLS; j++) {
			sum += matrix[i][j] * vec[j];
		}
		out[i] = sum;
	}

    for (int i = ROWS; i < ROWS; i++) {
		int32_t sum = 0;
		for (int j = 0; j < no_of_col_blocks; j++) {
			sum += matrix[i][j] * vec[j];
		}
		out[i] = sum;
	}
}

int main() {
    srand(time(0));

    int8_t matrix[ROWS][COLS];
    int16_t vec[COLS];
    int32_t out_dsp[ROWS] = {0};
    int32_t out_scalar[ROWS] = {0};

    printf("Matrix (%dx%d):\n", ROWS, COLS);
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            matrix[i][j] = rand() % 256 - 128;
            printf("%4d", matrix[i][j]);
        }
        printf("\n");
    }

    printf("\nVector (%dx1):\n", COLS);
    for (int i = 0; i < COLS; ++i) {
        vec[i] = rand() % 200 - 100;
        printf("%4d\n", vec[i]);
    }

    matrix_vector_scalar(matrix, vec, out_scalar);
    matrix_vector_dsp(matrix, vec, out_dsp);

    printf("\nRow |   DSP Output   |  Scalar Output  | Match\n");
    printf("----+----------------+-----------------+--------\n");
    for (int i = 0; i < ROWS; i++) {
        printf(" %2d | %14d | %15d |  %s\n",
               i, out_dsp[i], out_scalar[i],
               (out_dsp[i] == out_scalar[i]) ? "✅" : "❌");
    }
    return 0;
}
