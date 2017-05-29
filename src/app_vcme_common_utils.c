#include <math.h>
#include <stdio.h>

#include "app_vcme_common_typedef.h"
#include "app_vcme_common_define.h"

FP64 PSNR(unsigned char **psnr_y1, unsigned char **psnr_y2, INT32U frame, INT32U width, INT32U height)
{
	int i, j;
	double psnr=0.;
	unsigned long long int ssd=0;
	for(j=0;j<frame;j++)
	{
		for(i=0;i<width*height;i++)
		{
			psnr+=(psnr_y1[j][i]-psnr_y2[j][i])*(psnr_y1[j][i]-psnr_y2[j][i]);
			ssd+=(psnr_y1[j][i]-psnr_y2[j][i])*(psnr_y1[j][i]-psnr_y2[j][i]);
		}
	}
	psnr=frame*width*height/psnr;
	psnr=10*log10(255*255*psnr);

	return psnr;
}

