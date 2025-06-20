#include <stdio.h>
#include <stdint.h>
#include "xtensa/tie/xt_hifi5.h"

void eltwiseMul(int16_t * arr1, int16_t * arr2, int16_t * result, int16_t size){
	ae_int16x4 * vec1_ptr = (ae_int16x4 *)&arr1[0];
	ae_int16x4 * vec2_ptr = (ae_int16x4 *)&arr2[0];
	ae_int16x4 * result_ptr = (ae_int16x4 *)&result[0];
	int count = 0;
	ae_int16x4 vec1, vec2, cast_pd;
	ae_int32x4 pd;
	for(int i = 0; i < size; i += 4){
		AE_L16X4_IP(vec1, vec1_ptr, 8);
		AE_L16X4_IP(vec2, vec2_ptr, 8);

		pd = AE_INT16X4_MUL_INT16X4(vec1, vec2);
		cast_pd = (ae_int16x4)pd;
		AE_S16X4_IP(cast_pd, result_ptr, 8);
		count++;
	}

	printf("count : %d \n", count);
}
