#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

// #define COMPLEXITY

int wmacro;
int hmacro;
int width, height, sr, macro, macroblock, frame;
#ifdef COMPLEXITY
unsigned __int64 addition=0, multiplication=0, comparison=0, shift=0, abstraction=0, boolean=0;
#endif

int SAD_macroblock(unsigned char *cur, unsigned char *ref, int dx, int dy, int x, int y);
void PSNR(unsigned char **psnr_y1, unsigned char **psnr_y2, FILE *fpdata);

// hyuk, This FS.cpp provides the conceptual padding, not the real padding for motion estimation
// hyuk, argv[1]:input yuv file, argv[2]:output yuv file, argv[3]:mv file, argv[4]:data file, argv[5]: width, argv[6]: height, argv[7]: # of frame, argv[8]: search range, argv[9]: size of macroblock
int main(int argc, char *argv[])
{
	FILE *fp, *fpo, *fpmv, *fpdata; // hyuk, input file, output file, MV file, data file
	unsigned char *ref, *cur, *cur_made, *cur_C;
	unsigned char **psnr_y1, **psnr_y2;
	int i, j, m, getx=0, gety=0, min=9999999, spix=0, spiy=0, tx, ty, dx, dy, d;
	int sad, sadmin;
	char filename[100];

	width=atoi(argv[5]);
	height=atoi(argv[6]);
	frame=atoi(argv[7]);
	sr=atoi(argv[8]);
	macro=atoi(argv[9]);
	wmacro=width/macro;
	hmacro=height/macro;
	sprintf(filename, "%s", argv[3]);
	fpmv=fopen(filename, "w");
	sprintf(filename, "%s", argv[1]);
	fp=fopen(filename, "rb");
	if(fp==NULL)
	{
		printf("No source file\n");
		return -1;
	}
	ref=(unsigned char*)malloc(width*height*sizeof(unsigned char));
	cur=(unsigned char*)malloc(width*height*sizeof(unsigned char));
	cur_made=(unsigned char*)malloc(width*height*sizeof(unsigned char));
	cur_C=(unsigned char*)malloc((width*height/2)*sizeof(unsigned char));
	psnr_y1=(unsigned char**)malloc(frame*sizeof(unsigned char*));
	psnr_y2=(unsigned char**)malloc(frame*sizeof(unsigned char*));
	sprintf(filename, "%s", argv[2]);
	fpo=fopen(filename, "wb");
	fread(ref, sizeof(unsigned char), width*height, fp);
	fread(cur_C, sizeof(unsigned char), width*height/2, fp);
	fwrite(ref, sizeof(unsigned char), width*height, fpo);
	fwrite(cur_C, sizeof(unsigned char), width*height/2, fpo);
	for(j=0;j<frame;j++)
	{
		psnr_y1[j]=(unsigned char*)malloc(width*height*sizeof(unsigned char));
		psnr_y2[j]=(unsigned char*)malloc(width*height*sizeof(unsigned char));
	}
	memcpy(psnr_y1[0], ref, width*height*sizeof(unsigned char));
	memcpy(psnr_y2[0], ref, width*height*sizeof(unsigned char));

	clock_t time;
	time=clock();
	for(m=0;m<(frame-1);m++)
	{
		fread(cur, sizeof(unsigned char), width*height, fp);
		memcpy(psnr_y1[m+1], cur, width*height*sizeof(unsigned char));

		// hyuk, spiral search

		for(j=0;j<hmacro;j++)
		{
			for(i=0;i<wmacro;i++)
			{
				dx=i*macro;
				dy=j*macro;
				sadmin=SAD_macroblock(cur, ref, dx, dy, i*macro, j*macro);
				getx=0;
				gety=0;
				for(ty=1;ty<16;ty++)
				{
					dx=i*macro-ty;
					dy=j*macro-ty;
					sad=SAD_macroblock(cur, ref, dx, dy, i*macro, j*macro);
					if(sad<sadmin)
					{
						sadmin=sad;
						getx=-ty;
						gety=-ty;
					}
					for(tx=1;tx<=ty*2;tx++)
					{
						dx=i*macro-ty+tx;
						dy=j*macro-ty;
						sad=SAD_macroblock(cur, ref, dx, dy, i*macro, j*macro);
						if(sad<sadmin)
						{
							sadmin=sad;
							getx=-ty+tx;
							gety=-ty;
						}
					}
					for(tx=1;tx<=ty*2;tx++)
					{
						dx=i*macro+ty;
						dy=j*macro-ty+tx;
						sad=SAD_macroblock(cur, ref, dx, dy, i*macro, j*macro);
						if(sad<sadmin)
						{
							sadmin=sad;
							getx=ty;
							gety=-ty+tx;
						}
					}
					for(tx=1;tx<=ty*2;tx++)
					{
						dx=i*macro+ty-tx;
						dy=j*macro+ty;
						sad=SAD_macroblock(cur, ref, dx, dy, i*macro, j*macro);
						if(sad<sadmin)
						{
							sadmin=sad;
							getx=ty-tx;
							gety=ty;
						}
					}
					for(tx=1;tx<ty*2;tx++)
					{
						dx=i*macro-ty;
						dy=j*macro+ty-tx;
						sad=SAD_macroblock(cur, ref, dx, dy, i*macro, j*macro);
						if(sad<sadmin)
						{
							sadmin=sad;
							getx=-ty;
							gety=ty-tx;
						}
					}
				}
				for(tx=0;tx<macro;tx++)
				{
					for(d=0;d<macro;d++)
					{
						cur_made[i*macro+j*macro*width+d+tx*width]=ref[i*macro+j*macro*width+getx+gety*width+d+tx*width];
					}
				}
				fprintf(fpmv, "%d, %d:(%d, %d) ", i, j, getx, gety);
			}
			fprintf(fpmv, "\n");
		}
		printf("%d\n", m);
		fwrite(cur_made, sizeof(unsigned char), width*height, fpo);
		fread(cur_C, sizeof(unsigned char), width*height/2, fp);
		fwrite(cur_C, sizeof(unsigned char), width*height/2, fpo);
		memcpy(ref, cur, width*height*sizeof(unsigned char));
		memcpy(psnr_y2[m+1], cur_made, width*height*sizeof(unsigned char));
	}
	time=clock()-time;
	printf("%f\n",(double)time/CLOCKS_PER_SEC);
	sprintf(filename, "%s", argv[4]);
	fpdata=fopen(filename, "w");
	PSNR(psnr_y1, psnr_y2, fpdata);
	fprintf(fpdata, "\n%f",(double)time/CLOCKS_PER_SEC);
#ifdef COMPLEXITY
	fprintf(fpdata, "\n%I64u\n%I64u\n%I64u\n%I64u\n%I64u\n%I64u\n", addition, multiplication, comparison, shift, abstraction, boolean);
	printf("\n%I64u\n%I64u\n%I64u\n%I64u\n%I64u\n%I64u\n", addition, multiplication, comparison, shift, abstraction, boolean);
#endif
	fclose(fp);
	fclose(fpo);
	fclose(fpmv);
	fclose(fpdata);
	free(ref);
	free(cur);
	free(cur_made);
	free(cur_C);
	for(j=0;j<frame;j++)
	{
		free(psnr_y1[j]);
		free(psnr_y2[j]);
	}
	free(psnr_y1);
	free(psnr_y2);
	return 1;
}

int SAD_macroblock(unsigned char *cur, unsigned char *ref, int dx, int dy, int x, int y)
{
	int i, j;
	int sad=0;

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
			sad+=abs(cur[x+i+(y+j)*width]-ref[dx+i+(dy+j)*width]);
#ifdef COMPLEXITY
			addition+=2;
			abstraction++;
#endif
		}
	}

	return sad;
}

void PSNR(unsigned char **psnr_y1, unsigned char **psnr_y2, FILE *fpdata) // hyuk, calculate y-psnr
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
	printf("%f\n", psnr);
	fprintf(fpdata, "%f\n", psnr);
	printf("%I64u\n", ssd);
	fprintf(fpdata, "\n%I64u", ssd);
}
