#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "xtensa/tie/xt_hifi5.h"

void mul_acc_scalar(int16_t * arr1, int16_t * arr2, int16_t size, int32_t *result){
	int acc = 0;
	for(int i = 0; i < size; i++){
		acc += arr1[i] * arr2[i];
	}
	*result = acc;
}

void mul_acc(int16_t * arr1, int16_t * arr2, int16_t size, int32_t * result){

	ae_int16x4 * vec_ptr1 = (ae_int16x4 *)&arr1[0];
	ae_int16x4 * vec_ptr2 = (ae_int16x4 *)&arr2[0];
	ae_int16x4 * result_ptr = (ae_int16x4 *)&result[0];

	ae_int16x4 vec1, vec2;
	ae_int32x2 acc;

	acc = AE_ZERO32();

	int i = 0;
	for(; i < size - 4; i+=4){
		AE_L16X4_IP(vec1, vec_ptr1, 8);
		AE_L16X4_IP(vec2, vec_ptr2, 8);
		ae_int32x2 p0, p1;
		AE_MUL16X4(p0, p1, vec1, vec2);
        acc = AE_ADD32S(acc, p0);
        acc = AE_ADD32S(acc, p1);
	}

	int32_t scalar_prod = 0;
	for(; i < size; i++){
		scalar_prod += arr1[i] * arr2[i];
	}

	int32_t vec_sum = AE_MOVAD32_L(acc) + AE_MOVAD32_H(acc);

	*result = vec_sum + scalar_prod;
}

int main () {

	srand(time(0));
	int size = 25;
	int16_t arr1[size];
	int16_t arr2[size];
	for(int i = 0; i < size; i++){
		arr1[i] = rand() % 100;
		arr2[i] = rand() % 100;
	}
	int32_t output = 0;
	int32_t output_scalar = 0;
	mul_acc(arr1, arr2, size, &output);
	mul_acc_scalar(arr1, arr2, size, &output_scalar);
	printf("Result : %d", output);
	printf("\nScalar Result : %d", output_scalar);
	if(output != output_scalar){
		printf("\nNOT EQUAL!!!!");
	}
	else{
		printf("\nEQUAL!!!!");
	}
	return 0;
}
