#include <math.h>
#include <stdio.h>

#include "app_vcme_common_typedef.h"
#include "app_vcme_common_define.h"

FP32 util_PSNR_one_frm(INT08U *p_psnr_1_u08, INT08U *p_psnr_2_u08, INT32U size_u32)
{
	INT32U i_u32;
	INT32U ssd_u32 = 0;
	FP64 psnr_f64;
	for ( i_u32 = 0 ; i_u32 < size_u32 ; i_u32++ )
	{
		ssd_u32 += (p_psnr_1_u08[i_u32] - p_psnr_2_u08[i_u32]) * (p_psnr_1_u08[i_u32] - p_psnr_2_u08[i_u32]);
	}
	psnr_f64 = (FP64)size_u32 / (FP64)ssd_u32;
	psnr_f64 = 10. * log10 ( 255. * 255. * psnr_f64 );

	return (FP32)(psnr_f64);
}

