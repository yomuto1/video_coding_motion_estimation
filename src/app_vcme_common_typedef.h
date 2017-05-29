/* */

#ifndef __APP_VCME_COMMON_TYPEDEF_H__
#define __APP_VCME_COMMON_TYPEDEF_H__

typedef char INT8S;
typedef signed char INT08S;
typedef unsigned char INT08U;
typedef signed short INT16S;
typedef unsigned short INT16U;
typedef signed int INT32S;
typedef unsigned int INT32U;
typedef signed long long int INT64S;
typedef unsigned long long int INT64U;
typedef float FP32;
typedef double FP64;

typedef struct _ST_APP_VCME_INPUT
{
	INT32U wid_u32;
	INT32U hei_u32;
	INT32U search_range_u32;
	INT32U sizeof_macroblock_u32;
	INT32U numof_frames_u32;
	INT32U numof_macroblocks_wid_u32;
	INT32U numof_macroblocks_hei_u32;

	INT08U *p_ref_u08;
	INT08U *p_ref_pad_u08;
	INT08U *p_cur_u08;
} ST_APP_VCME_INPUT;

typedef struct _ST_APP_VCME_OUTPUT
{
	INT16S *p_mv_s16;
} ST_APP_VCME_OUTPUT;

#endif

