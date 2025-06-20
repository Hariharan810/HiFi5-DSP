#include <stdio.h>
#include <stdint.h>
#include "xtensa/tie/xt_hifi5.h"

void eltwiseAdd(int16_t * arr1, int16_t * arr2, int16_t * result, int16_t size){
	ae_int16x4 * vec1_ptr = (ae_int16x4 *)&arr1[0];
	ae_int16x4 * vec2_ptr = (ae_int16x4 *)&arr2[0];
	ae_int16x4 * result_ptr = (ae_int16x4 *)&result[0];

	ae_int16x4 vec1;
	ae_int16x4 vec2;

	for(int i = 0; i < size; i += 4){
		AE_L16X4_IP(vec1, vec1_ptr, 8);
		AE_L16X4_IP(vec2, vec2_ptr, 8);

		ae_int16x4 result_vec = AE_ADD16(vec1, vec2);
		AE_S16X4_XP(result_vec, result_ptr, 8);
	}
}


