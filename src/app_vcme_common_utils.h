/* */

#ifndef __APP_VCME_COMMON_UTILS_H__
#define __APP_VCME_COMMON_UTILS_H__

#include "app_vcme_common_typedef.h"

#define ABS(a)(((a) < (0))?-(a):(a))

FP32 util_PSNR_one_frm(INT08U *p_psnr_1_u08, INT08U *p_psnr_2_u08, INT32U size_u32);

#endif

