// 4-bit matrix multiple
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "xtensa/tie/xt_hifi5.h"

#define ROWS 4
#define COLS 8

void pack_matrix_custom_layout(int8_t matrix[2][8], int8_t packed[4][8]) {
    int row = 0;
    int col = 0;

    for (int j = 7; j >= 0; --j) {
        uint8_t val = (uint8_t)matrix[0][j];
        packed[row][col++] = (val >> 4) & 0x0F;
        packed[row][col++] = val & 0x0F;
        if (col >= 8) { row++; col = 0; }
    }

    for (int j = 7; j >= 0; --j) {
        uint8_t val = (uint8_t)matrix[1][j];
        packed[row][col++] = (val >> 4) & 0x0F;
        packed[row][col++] = val & 0x0F;
        if (col >= 8) { row++; col = 0; }
    }
}
void matrix_vector_scalar(int8_t matrix[ROWS][COLS], int16_t *vec, int32_t *out) {
    for (int i = 0; i < 2; ++i) {
        out[i] = 0;
        for (int j = 0; j < 8; ++j) {
            out[i] += matrix[i][j] * vec[j];
        }
    }
}

void matrix_vector_dsp(int8_t matrix[ROWS][COLS], int16_t *vec, int32_t *out) {
    ae_int4x16 row0 = *((ae_int4x16 *)&matrix[0][0]);
    ae_int4x16 row1 = *((ae_int4x16 *)&matrix[2][0]);

    ae_int16x4 v0 = AE_L16X4_I((ae_int16x4 *)&vec[0], 0);
    ae_int16x4 v1 = AE_L16X4_I((ae_int16x4 *)&vec[4], 0);

    ae_int32x2 q0 = AE_ZERO32();
    ae_int32x2 q1 = AE_ZERO32();

    AE_MULA8Q4X16(q0, q1, row0, row1, v0, v1);

    out[0] = AE_MOVAD32_H(q0) + AE_MOVAD32_L(q0);
    out[1] = AE_MOVAD32_H(q1) + AE_MOVAD32_L(q1);
}

int main() {
	int8_t matrix[2][8] = {
	    {  23,  103,  125,  82, 116,   9,  53,   99 },
	    { 47,    1,  114,   52, 117,   90, 125,  82 }
	};

	int16_t vec[8] = { 3, 6, 11, 12, 7, 15, 12, 8 };
	int8_t packed[4][8] = {0};

	printf("Matrix (2x8):\n");
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 8; ++j) {
			printf("%4d", matrix[i][j]);
		}
		printf("\n");
	}

	printf("\nVector (8x1):\n");
	for (int i = 0; i < 8; ++i) {
		printf("%4d\n", vec[i]);
	}

	pack_matrix_custom_layout(matrix, packed);


	printf("\nPacked 4-bit matrix (4x8):\n");
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 8; ++j) {
			printf("%4d", packed[i][j]);
		}
		printf("\n");
	}

	int32_t out_scalar[2];
	int32_t out_dsp[2];

    matrix_vector_scalar(matrix, vec, out_scalar);
    matrix_vector_dsp(packed, vec, out_dsp);

    printf("\nRow |   DSP Output   |  Scalar Output  | Match\n");
    printf("----+----------------+-----------------+--------\n");
    for (int i = 0; i < 2; i++) {
        printf(" %2d | %14d | %15d |  %s\n",
               i, out_dsp[i], out_scalar[i],
               (out_dsp[i] == out_scalar[i]) ? "✅" : "❌");
    }
    return 0;
}
