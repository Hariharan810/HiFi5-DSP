#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <xtensa/tie/xt_hifi5.h>
#define SIZE 8

int compare(int16_t input[SIZE][SIZE], int16_t output[SIZE][SIZE]){
	for(int i = 0; i < SIZE; i++){
		for(int j = 0; j < SIZE; j++){
			if(input[i][j] != output[i][j]){
				return 0;
			}
		}
	}
	return 1;
}

void transpose_scalar(int16_t input[SIZE][SIZE]){
	for(int i = 0; i < SIZE; i++){
		for(int j = i + 1; j < SIZE; j++){
			int temp = input[i][j];
			input[i][j] = input[j][i];
			input[j][i] = temp;
		}
	}
}

void transpose(int row, int col, int16_t input[SIZE][SIZE], int16_t output[SIZE][SIZE]){

	int no_of_row_blocks = (row/4) * 2;
	int no_of_col_blocks = (col/4) * 2;

	for(int i = 0; i < no_of_row_blocks; i += 4){
		for(int j = 0; j < no_of_col_blocks; j += 4){
			ae_int16x4 r0, r1, r2, r3;
			ae_int16x4 * ptr1 = (ae_int16x4 *)&input[i + 0][j];
			ae_int16x4 * ptr2 = (ae_int16x4 *)&input[i + 1][j];
			ae_int16x4 * ptr3 = (ae_int16x4 *)&input[i + 2][j];
			ae_int16x4 * ptr4 = (ae_int16x4 *)&input[i + 3][j];

			r0 = AE_L16X4_I(ptr1, 0);
			r1 = AE_L16X4_I(ptr2, 0);
			r2 = AE_L16X4_I(ptr3, 0);
			r3 = AE_L16X4_I(ptr4, 0);

			ae_int16x4 a,b,c,d,e,f,g,h;

			a = AE_SEL16_7362(r0, r2);
			b = AE_SEL16_5140(r0, r2);
			c = AE_SEL16_7362(r1, r3);
			d = AE_SEL16_5140(r1, r3);
			e = AE_SEL16_7362(a, c);
			f = AE_SEL16_5140(a, c);
			g = AE_SEL16_7362(b, d);
			h = AE_SEL16_5140(b, d);

			ae_int16x4 * out_ptr1 =(ae_int16x4 *)&output[i + 0][j];
			ae_int16x4 * out_ptr2 =(ae_int16x4 *)&output[i + 1][j];
			ae_int16x4 * out_ptr3 =(ae_int16x4 *)&output[i + 2][j];
			ae_int16x4 * out_ptr4 =(ae_int16x4 *)&output[i + 3][j];

			AE_S16X4_I(e, out_ptr1, 0);
			AE_S16X4_I(f, out_ptr2, 0);
			AE_S16X4_I(g, out_ptr3, 0);
			AE_S16X4_I(h, out_ptr4, 0);
		}
	}

	for(int i = 0; i < row; i++){
		for(int j = no_of_col_blocks; j < col; j++){
			output[j][i] = input[i][j];
		}
	}

	for(int i = no_of_row_blocks; i < row; i++){
		for(int j = 0; j < col; j++){
			output[j][i] = input[i][j];
		}
	}
}

int main(){

	int16_t input[SIZE][SIZE];
	srand(time(0));
	for(int i = 0; i < SIZE; i++){
		for(int j = 0; j < SIZE; j++){
			input[i][j] = rand() % 100;
		}
	}
	int16_t output[SIZE][SIZE];

	transpose(8, 8, input, output);
	transpose_scalar(input);
	int flag = compare(input, output);
	if(flag){
		printf("EQUAL!!!!");
	}else{
		printf("NOT EQUAL!!!");
	}

	return 0;
}
