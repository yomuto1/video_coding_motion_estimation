#include "FS.h"

#include "app_vcme_common_utils.h"

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
			for(ty_s16 = 1; ty_s16 < pst_APP_VCME_INPUT->search_range_u32; ty_s16++)
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

static INT32U app_vcme_SAD_macroblock(INT08U *cur, INT08U *ref, INT16S dx, INT16S dy, INT16S x, INT16S y, INT32U macro, INT32U width, INT32U height)
{
	INT32U i, j;
	INT32U sad=0;

	if(dx<0)
		return 9999999;
	if(dx>=width-15)
		return 9999999;
	if(dy<0)
		return 9999999;
	if(dy>=height-15)
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

