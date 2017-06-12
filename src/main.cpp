#include "app_vcme_common_typedef.h"
#include "app_vcme_common_define.h"
#include "app_vcme_common_utils.h"

#include "FS.h"

#include <stdio.h>
#include <stdlib.h>
#if ((1 == DEBUG_INFO) || (1 == TIME_CHECK))
#include <string.h>
#endif
#if (1 == TIME_CHECK)
#include <time.h>
#endif

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>

using namespace cv;

/* hyuk, This FS.cpp provides the conceptual padding, not the real padding for motion estimation */
/* hyuk, argv[1]:input yuv file, argv[2]:output yuv file, argv[3]:data file, argv[4]:algorithm, argv[5]: width, argv[6]: height, argv[7]: # of frame, argv[8]: search range, argv[9]: size of macroblock */
INT32S main(INT32S argc, INT8S *argv[])
{
	FILE *fp, *fpo, *fpdata;
	static INT8S sa_filename_s8[128];
	INT08U *p_cur_made_u08, *p_cur_C_u08;
	static ST_APP_VCME_INPUT sst_APP_VCME_INPUT;
	static ST_APP_VCME_OUTPUT sst_APP_VCME_OUTPUT;
#if (1 == TIME_CHECK)
	clock_t time_init;
	clock_t time_sum = 0;
	clock_t *time;
#endif
	INT32S error_s32;
	INT32U frame_u32;
	size_t fread_return_u32;
	FP64 psnr_f64 = 0.;
	INT32U i_u32, j_u32;
	INT32U tx, d;
	static INT8S sa_name_s8[256];

#if (1 == TIME_CHECK)
	time_init = clock();
	time_init = clock() - time_init;
#endif

	/* input parameter setting */
	sst_APP_VCME_INPUT.wid_u32 = atoi(argv[5]);
	sst_APP_VCME_INPUT.hei_u32 = atoi(argv[6]);
	sst_APP_VCME_INPUT.numof_frames_u32 = atoi(argv[7]);
	sst_APP_VCME_INPUT.search_range_u32 = atoi(argv[8]);
	sst_APP_VCME_INPUT.sizeof_macroblock_u32 = atoi(argv[9]);
	sst_APP_VCME_INPUT.numof_macroblocks_wid_u32 = sst_APP_VCME_INPUT.wid_u32 / sst_APP_VCME_INPUT.sizeof_macroblock_u32;
	sst_APP_VCME_INPUT.numof_macroblocks_hei_u32 = sst_APP_VCME_INPUT.hei_u32 / sst_APP_VCME_INPUT.sizeof_macroblock_u32;

#if (1 == DEBUG_INFO)
	printf("input parameter setting: wid %u, hei %u, # of frames %u, SR %u, sizeof MB %u, MB wid %u, MB hei %u\n", sst_APP_VCME_INPUT.wid_u32, sst_APP_VCME_INPUT.hei_u32, sst_APP_VCME_INPUT.numof_frames_u32, sst_APP_VCME_INPUT.search_range_u32, sst_APP_VCME_INPUT.sizeof_macroblock_u32, sst_APP_VCME_INPUT.numof_macroblocks_wid_u32, sst_APP_VCME_INPUT.numof_macroblocks_hei_u32);
#endif

	Mat grayScaleImage(sst_APP_VCME_INPUT.hei_u32, sst_APP_VCME_INPUT.wid_u32, CV_8UC1);

	/* file control */
	error_s32 = sprintf(sa_filename_s8, "%s", argv[3]);
	if(0 == error_s32)
	{
#if (1 == DEBUG_INFO)
		printf("sprintf 1st error\n");
#endif
	}
	fpdata = fopen(sa_filename_s8, "w");
	if(fpdata==NULL)
	{
#if (1 == DEBUG_INFO)
		printf("open fpdata error\n");
#endif
		return -1;
	}
	error_s32 = sprintf(sa_filename_s8, "%s", argv[1]);
	if(0 == error_s32)
	{
#if (1 == DEBUG_INFO)
		printf("sprintf 2nd error\n");
#endif
	}
	fp = fopen(sa_filename_s8, "rb");
	if(fp==NULL)
	{
#if (1 == DEBUG_INFO)
		printf("No source file\n");
#endif
		return -1;
	}
	error_s32 = sprintf(sa_filename_s8, "%s", argv[2]);
	if(0 == error_s32)
	{
#if (1 == DEBUG_INFO)
		printf("sprintf 3rd error\n");
#endif
	}
	fpo = fopen(sa_filename_s8, "wb");
	if(fpo==NULL)
	{
#if (1 == DEBUG_INFO)
		printf("output yuv data open error\n");
#endif
		return -1;
	}

#if (1 == DEBUG_INFO)
	printf("file control finished\n");
#endif

	/* dynamic memory allocation */
	sst_APP_VCME_INPUT.p_ref_u08 = (INT08U *)malloc(sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32 * sizeof(INT08U));
	sst_APP_VCME_INPUT.p_ref_pad_u08 = (INT08U *)malloc((sst_APP_VCME_INPUT.wid_u32 + 2 * sst_APP_VCME_INPUT.search_range_u32) * (sst_APP_VCME_INPUT.hei_u32 + 2 * sst_APP_VCME_INPUT.search_range_u32) * sizeof(INT08U));
	sst_APP_VCME_INPUT.p_cur_u08 = (INT08U *)malloc(sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32 * sizeof(INT08U));
	p_cur_made_u08 = (INT08U*)malloc(sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32 * sizeof(INT08U));
	p_cur_C_u08 = (INT08U *)malloc((sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32 / 2) * sizeof(INT08U));
	sst_APP_VCME_OUTPUT.p_mv_s16 = (INT16S *)malloc(sst_APP_VCME_INPUT.numof_macroblocks_wid_u32 * sst_APP_VCME_INPUT.numof_macroblocks_hei_u32 * 2 * sizeof(INT16S));
#if (1 == TIME_CHECK)
	time = (clock_t *)malloc((sst_APP_VCME_INPUT.numof_frames_u32 - 1) * sizeof(clock_t));
#endif

#if (1 == DEBUG_INFO)
	printf("dynamic memory allocation finished\n");
#endif


	/* read first frame and set it as reference frame */
	fread_return_u32 = fread(sst_APP_VCME_INPUT.p_ref_u08, sizeof(INT08U), sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32, fp);
	if(0 == fread_return_u32)
	{
#if (1 == DEBUG_INFO)
		printf("1st fread for Y error\n");
#endif
	}
	fread_return_u32 = fread(p_cur_C_u08, sizeof(INT08U), sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32 / 2, fp);
	if(0 == fread_return_u32)
	{
#if (1 == DEBUG_INFO)
		printf("1st fread for C error\n");
#endif
	}
	fwrite(sst_APP_VCME_INPUT.p_ref_u08, sizeof(INT08U), sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32, fpo);
	fwrite(p_cur_C_u08, sizeof(INT08U), sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32 / 2, fpo);

#if (1 == DEBUG_INFO)
	printf("read 1st frame finished\n");
#endif

	/* -1 since one frame shall be used as first reference frame */
	for(frame_u32 = 0; frame_u32 < (sst_APP_VCME_INPUT.numof_frames_u32 - 1); frame_u32++)
	{
#if (1 == DEBUG_INFO)
		printf("frame number: %u\n", frame_u32);
#endif
		fread_return_u32 = fread(sst_APP_VCME_INPUT.p_cur_u08, sizeof(INT08U), sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32, fp);
		if(0 == fread_return_u32)
		{
#if (1 == DEBUG_INFO)
			printf("%u th fread error\n", frame_u32);
#endif
		}

		/* consider padding, early stop, etc. */
#if (1 == TIME_CHECK)
		time[frame_u32] = clock();
#endif

		error_s32 = app_vcme_fullsearch_sad(&sst_APP_VCME_OUTPUT, &sst_APP_VCME_INPUT);

#if (1 == TIME_CHECK)
		time[frame_u32] = clock() - time[frame_u32] - time_init;
		time_sum += time[frame_u32];
#endif

		for(j_u32 = 0; j_u32 < sst_APP_VCME_INPUT.numof_macroblocks_hei_u32; j_u32++)
		{
			for(i_u32 = 0; i_u32 < sst_APP_VCME_INPUT.numof_macroblocks_wid_u32; i_u32++)
			{
#if (1 == DEBUG_INFO)
				printf("%u frame, i %u, j %u, (%d, %d)\n", frame_u32, i_u32, j_u32, sst_APP_VCME_OUTPUT.p_mv_s16[2 * i_u32 + j_u32 * sst_APP_VCME_INPUT.numof_macroblocks_wid_u32 * 2], sst_APP_VCME_OUTPUT.p_mv_s16[2 * i_u32 + 1 + j_u32 * sst_APP_VCME_INPUT.numof_macroblocks_wid_u32 * 2]);
#endif
				for(tx=0;tx<sst_APP_VCME_INPUT.sizeof_macroblock_u32;tx++)
				{
					for(d=0;d<sst_APP_VCME_INPUT.sizeof_macroblock_u32;d++)
					{
						/* printf("%u, %u, %u, %u, %d, %d - (%d, %d)\n", i_u32, j_u32, d, tx, sst_APP_VCME_OUTPUT.p_mv_s16[2 * i_u32 + j_u32 * sst_APP_VCME_INPUT.numof_macroblocks_wid_u32 * 2], sst_APP_VCME_OUTPUT.p_mv_s16[2 * i_u32 + 1 + j_u32 * sst_APP_VCME_INPUT.numof_macroblocks_wid_u32 * 2],  i_u32*sst_APP_VCME_INPUT.sizeof_macroblock_u32+j_u32*sst_APP_VCME_INPUT.sizeof_macroblock_u32*sst_APP_VCME_INPUT.wid_u32+d+tx*sst_APP_VCME_INPUT.wid_u32, i_u32*sst_APP_VCME_INPUT.sizeof_macroblock_u32+j_u32*sst_APP_VCME_INPUT.sizeof_macroblock_u32*sst_APP_VCME_INPUT.wid_u32+sst_APP_VCME_OUTPUT.p_mv_s16[2 * i_u32 + j_u32 * sst_APP_VCME_INPUT.numof_macroblocks_wid_u32 * 2] + sst_APP_VCME_OUTPUT.p_mv_s16[2 * i_u32 + 1 + j_u32 * sst_APP_VCME_INPUT.numof_macroblocks_wid_u32 * 2] * sst_APP_VCME_INPUT.wid_u32+d+tx*sst_APP_VCME_INPUT.wid_u32); */
						p_cur_made_u08[i_u32*sst_APP_VCME_INPUT.sizeof_macroblock_u32+j_u32*sst_APP_VCME_INPUT.sizeof_macroblock_u32*sst_APP_VCME_INPUT.wid_u32+d+tx*sst_APP_VCME_INPUT.wid_u32]=sst_APP_VCME_INPUT.p_ref_u08[i_u32*sst_APP_VCME_INPUT.sizeof_macroblock_u32+j_u32*sst_APP_VCME_INPUT.sizeof_macroblock_u32*sst_APP_VCME_INPUT.wid_u32 + sst_APP_VCME_OUTPUT.p_mv_s16[2 * i_u32 + j_u32 * sst_APP_VCME_INPUT.numof_macroblocks_wid_u32 * 2] + sst_APP_VCME_OUTPUT.p_mv_s16[2 * i_u32 + 1 + j_u32 * sst_APP_VCME_INPUT.numof_macroblocks_wid_u32 * 2] * sst_APP_VCME_INPUT.wid_u32+d+tx*sst_APP_VCME_INPUT.wid_u32];
					}
				}
			}
		}

		fwrite(p_cur_made_u08, sizeof(INT08U), sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32, fpo);
		memcpy(grayScaleImage.data, p_cur_made_u08, sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32 * sizeof(unsigned char));
		sprintf(sa_name_s8, "./results/%s_%4u.jpg", argv[2], frame_u32);
		imwrite(sa_name_s8, grayScaleImage);
		psnr_f64 += util_PSNR_one_frm(grayScaleImage.data, sst_APP_VCME_INPUT.p_cur_u08, sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32);
		fread_return_u32 = fread(p_cur_C_u08, sizeof(unsigned char), sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32 / 2, fp);
		if(0 == fread_return_u32)
		{
#if (1 == DEBUG_INFO)
			printf("%u th fread for C error\n", frame_u32);
#endif
		}
		fwrite(p_cur_C_u08, sizeof(unsigned char), sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32 / 2, fpo);
		memcpy(sst_APP_VCME_INPUT.p_ref_u08, sst_APP_VCME_INPUT.p_cur_u08, sst_APP_VCME_INPUT.wid_u32 * sst_APP_VCME_INPUT.hei_u32 * sizeof(unsigned char));
	}
	printf("avg PSNR: %f\n", psnr_f64 / (FP64)(sst_APP_VCME_INPUT.numof_frames_u32 - 1));
#if (1 == TIME_CHECK)
	printf("sum time: %f\n", ((FP32)time_sum) / CLOCKS_PER_SEC);
#endif
	fclose(fp);
	fclose(fpo);
	fclose(fpdata);
	free(sst_APP_VCME_INPUT.p_ref_u08);
	free(sst_APP_VCME_INPUT.p_ref_pad_u08);
	free(sst_APP_VCME_INPUT.p_cur_u08);
	free(sst_APP_VCME_OUTPUT.p_mv_s16);
	free(p_cur_made_u08);
	free(p_cur_C_u08);
	free(time);

	return 1;
}

