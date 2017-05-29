/* */

#ifndef __APP_VCME_COMMON_UTILS_H__
#define __APP_VCME_COMMON_UTILS_H__

#include "app_vcme_common_typedef.h"

#define ABS(a)(((a) < (0))?-(a):(a))

FP64 PSNR(unsigned char **psnr_y1, unsigned char **psnr_y2, INT32U frame, INT32U width, INT32U height);

#endif

