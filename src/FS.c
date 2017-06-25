#include "FS.h"

#include "app_vcme_common_define.h"
#include "app_vcme_common_utils.h"

#if (1 == USE_ARM_NEON_OPT)
#include "arm_neon.h"
#endif

/* #include <stdio.h> */

static INT32U app_vcme_SAD_macroblock(INT08U *cur, INT08U *ref, INT16S dx, INT16S dy, INT16S x, INT16S y, INT32U macro, INT32U width, INT32U height);

INT32S app_vcme_fullsearch_sad(ST_APP_VCME_OUTPUT *pst_APP_VCME_OUTPUT, ST_APP_VCME_INPUT *pst_APP_VCME_INPUT)
{
	INT32S error_s32 = 0;
	INT32U i_u32, j_u32;
	INT16S dx_s16, dy_s16;
	INT16S tx_s16, ty_s16;
	INT16S getx_s16, gety_s16;
	INT32U sadmin_u32, sad_u32;

	for(j_u32 = 0; j_u32 < pst_APP_VCME_INPUT->numof_macroblocks_hei_u32; j_u32++)
	{
		for(i_u32 = 0; i_u32 < pst_APP_VCME_INPUT->numof_macroblocks_wid_u32; i_u32++)
		{
			dx_s16 = (INT16S)(i_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32);
			dy_s16 = (INT16S)(j_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32);
			sadmin_u32 = app_vcme_SAD_macroblock(pst_APP_VCME_INPUT->p_cur_u08, pst_APP_VCME_INPUT->p_ref_u08, dx_s16, dy_s16, i_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32, j_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32, pst_APP_VCME_INPUT->sizeof_macroblock_u32, pst_APP_VCME_INPUT->wid_u32, pst_APP_VCME_INPUT->hei_u32);
			getx_s16 = 0;
			gety_s16 = 0;
			for(ty_s16 = 1; ty_s16 < (INT16S)pst_APP_VCME_INPUT->search_range_u32; ty_s16++)
			{
				dx_s16 = (INT16S)(i_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32) - ty_s16;
				dy_s16 = (INT16S)(j_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32) - ty_s16;
				sad_u32 = app_vcme_SAD_macroblock(pst_APP_VCME_INPUT->p_cur_u08, pst_APP_VCME_INPUT->p_ref_u08, dx_s16, dy_s16, i_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32, j_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32, pst_APP_VCME_INPUT->sizeof_macroblock_u32, pst_APP_VCME_INPUT->wid_u32, pst_APP_VCME_INPUT->hei_u32);
				if(sad_u32 < sadmin_u32)
				{
					sadmin_u32 = sad_u32;
					getx_s16 = -ty_s16;
					gety_s16 = -ty_s16;
				}
				for(tx_s16 = 1; tx_s16 <= ty_s16 * 2; tx_s16++)
				{
					dx_s16 = (INT16S)(i_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32) - ty_s16 + tx_s16;
					dy_s16 = (INT16S)(j_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32) - ty_s16;
					sad_u32 = app_vcme_SAD_macroblock(pst_APP_VCME_INPUT->p_cur_u08, pst_APP_VCME_INPUT->p_ref_u08, dx_s16, dy_s16, i_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32, j_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32, pst_APP_VCME_INPUT->sizeof_macroblock_u32, pst_APP_VCME_INPUT->wid_u32, pst_APP_VCME_INPUT->hei_u32);
					if(sad_u32 < sadmin_u32)
					{
						sadmin_u32 = sad_u32;
						getx_s16 = -ty_s16 + tx_s16;
						gety_s16 = -ty_s16;
					}
				}
				for(tx_s16 = 1; tx_s16 <= ty_s16 * 2; tx_s16++)
				{
					dx_s16 = (INT16S)(i_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32) + ty_s16;
					dy_s16 = (INT16S)(j_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32) - ty_s16 + tx_s16;
					sad_u32 = app_vcme_SAD_macroblock(pst_APP_VCME_INPUT->p_cur_u08, pst_APP_VCME_INPUT->p_ref_u08, dx_s16, dy_s16, i_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32, j_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32, pst_APP_VCME_INPUT->sizeof_macroblock_u32, pst_APP_VCME_INPUT->wid_u32, pst_APP_VCME_INPUT->hei_u32);
					if(sad_u32 < sadmin_u32)
					{
						sadmin_u32 = sad_u32;
						getx_s16 = ty_s16;
						gety_s16 = -ty_s16 + tx_s16;
					}
				}
				for(tx_s16 = 1; tx_s16 <= ty_s16 * 2; tx_s16++)
				{
					dx_s16 = (INT16S)(i_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32) + ty_s16 - tx_s16;
					dy_s16 = (INT16S)(j_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32) + ty_s16;
					sad_u32 = app_vcme_SAD_macroblock(pst_APP_VCME_INPUT->p_cur_u08, pst_APP_VCME_INPUT->p_ref_u08, dx_s16, dy_s16, i_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32, j_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32, pst_APP_VCME_INPUT->sizeof_macroblock_u32, pst_APP_VCME_INPUT->wid_u32, pst_APP_VCME_INPUT->hei_u32);
					if(sad_u32 < sadmin_u32)
					{
						sadmin_u32 = sad_u32;
						getx_s16 = ty_s16 - tx_s16;
						gety_s16 = ty_s16;
					}
				}
				for(tx_s16 = 1; tx_s16 < ty_s16 * 2; tx_s16++)
				{
					dx_s16 = (INT16S)(i_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32) - ty_s16;
					dy_s16 = (INT16S)(j_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32) + ty_s16 - tx_s16;
					sad_u32 = app_vcme_SAD_macroblock(pst_APP_VCME_INPUT->p_cur_u08, pst_APP_VCME_INPUT->p_ref_u08, dx_s16, dy_s16, i_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32, j_u32 * pst_APP_VCME_INPUT->sizeof_macroblock_u32, pst_APP_VCME_INPUT->sizeof_macroblock_u32, pst_APP_VCME_INPUT->wid_u32, pst_APP_VCME_INPUT->hei_u32);
					if(sad_u32 < sadmin_u32)
					{
						sadmin_u32 = sad_u32;
						getx_s16 = -ty_s16;
						gety_s16 = ty_s16 - tx_s16;
					}
				}
			}
			/* printf("%u, %u: (%d, %d)\n", i_u32, j_u32, getx_s16, gety_s16); */
			pst_APP_VCME_OUTPUT->p_mv_s16[i_u32 * 2 + j_u32 * pst_APP_VCME_INPUT->numof_macroblocks_wid_u32 * 2] = getx_s16;
			pst_APP_VCME_OUTPUT->p_mv_s16[i_u32 * 2 + 1 + j_u32 * pst_APP_VCME_INPUT->numof_macroblocks_wid_u32 * 2] = gety_s16;
		}
	}

	return error_s32;
}

#if (0 == USE_ARM_NEON_OPT)
static INT32U app_vcme_SAD_macroblock(INT08U *cur, INT08U *ref, INT16S dx, INT16S dy, INT16S x, INT16S y, INT32U macro, INT32U width, INT32U height)
{
	INT32U i, j;
	INT32U sad=0;

	if(dx<0)
		return 9999999;
	if(dx>=(INT16S)width-15)
		return 9999999;
	if(dy<0)
		return 9999999;
	if(dy>=(INT16S)height-15)
		return 9999999;

	for(j=0;j<macro;j++)
	{
		for(i=0;i<macro;i++)
		{
			sad += ABS(cur[x+i+(y+j)*width]-ref[dx+i+(dy+j)*width]);
		}
	}

	return sad;
}
#else
static INT32U app_vcme_SAD_macroblock(INT08U *cur, INT08U *ref, INT16S dx, INT16S dy, INT16S x, INT16S y, INT32U macro, INT32U width, INT32U height)
{
	INT32U i, j;
	INT32U sad=0;
	uint16x8_t sad_u16x8;
	uint32x4_t sad_u32x4;
	uint64x2_t sad_u64x2;
	uint8x8_t cur_u8x8, ref_u8x8;
	INT08U *p_cur_u08;
	INT08U *p_ref_u08;

	if(dx<0)
		return 9999999;
	if(dx>=(INT16S)width-15)
		return 9999999;
	if(dy<0)
		return 9999999;
	if(dy>=(INT16S)height-15)
		return 9999999;

	p_cur_u08 = &cur[x + y * width];
	p_ref_u08 = &ref[dx + dy * width];

	cur_u8x8 = vld1_u8(p_cur_u08);
	ref_u8x8 = vld1_u8(p_ref_u08);

	sad_u16x8 = vabdl_u8(cur_u8x8, ref_u8x8);

	p_cur_u08 += 8;
	p_ref_u08 += 8;

	for ( i = 0; i < macro / 8 - 1; i++ )
	{
		cur_u8x8 = vld1_u8(p_cur_u08);
		ref_u8x8 = vld1_u8(p_ref_u08);

		sad_u16x8 = vabal_u8(sad_u16x8, cur_u8x8, ref_u8x8);

		p_cur_u08 += 8;
		p_ref_u08 += 8;
	}

	for( j = 1; j < macro; j++ )
	{
		p_cur_u08 = &cur[x + (y + j) * width];
		p_ref_u08 = &ref[dx + (dy + j) * width];

		for ( i = 0; i < macro / 8; i++ )
		{
			cur_u8x8 = vld1_u8(p_cur_u08);
			ref_u8x8 = vld1_u8(p_ref_u08);

			sad_u16x8 = vabal_u8(sad_u16x8, cur_u8x8, ref_u8x8);

			p_cur_u08 += 8;
			p_ref_u08 += 8;
		}
	}

	sad_u32x4 = vpaddlq_u16(sad_u16x8);
	sad_u64x2 = vpaddlq_u32(sad_u32x4);

	sad = (INT32U)(vgetq_lane_u64(sad_u64x2, 0) + vgetq_lane_u64(sad_u64x2, 1));

	return sad;
}
#endif

