////////////////////////////////////////////////////////////////
//
//			CFA denoise by wavelet transform, FT filtering
//
//	copyright (c) 2008-2012  Emil Martinec <ejmartin@uchicago.edu>
//
//
//  code dated: March 9, 2012
//
//	FTblockDN.cc is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////




#include <math.h>
#include <fftw3.h>
#include "../rtgui/threadutils.h"

//#include "bilateral2.h"
#include "gauss.h"

#include "rtengine.h"
#include "improcfun.h"
#include "LUT.h"
#include "array2D.h"
#include "iccmatrices.h"
#include "boxblur.h"
#include "rt_math.h"
#include "mytime.h"
#include "sleef.c"
#include "opthelper.h"

#ifdef _OPENMP
#include <omp.h>
#endif

#include "cplx_wavelet_dec.h"

//#define MIN(a,b) ((a) < (b) ? (a) : (b))
//#define MAX(a,b) ((a) > (b) ? (a) : (b))
//#define LIM(x,min,max) MAX(min,MIN(x,max))
//#define ULIM(x,y,z) ((y) < (z) ? LIM(x,y,z) : LIM(x,z,y))
//#define CLIP(x) LIM(x,0,65535)

#define TS 64		// Tile size
#define offset 25	// shift between tiles
#define fTS ((TS/2+1))	// second dimension of Fourier tiles
#define blkrad 1	// radius of block averaging

#define epsilon 0.001f/(TS*TS) //tolerance

#define PIX_SORT(a,b) { if ((a)>(b)) {temp=(a);(a)=(b);(b)=temp;} }

#define med3(a0,a1,a2,a3,a4,a5,a6,a7,a8,median) { \
pp[0]=a0; pp[1]=a1; pp[2]=a2; pp[3]=a3; pp[4]=a4; pp[5]=a5; pp[6]=a6; pp[7]=a7; pp[8]=a8; \
PIX_SORT(pp[1],pp[2]); PIX_SORT(pp[4],pp[5]); PIX_SORT(pp[7],pp[8]); \
PIX_SORT(pp[0],pp[1]); PIX_SORT(pp[3],pp[4]); PIX_SORT(pp[6],pp[7]); \
PIX_SORT(pp[1],pp[2]); PIX_SORT(pp[4],pp[5]); PIX_SORT(pp[7],pp[8]); \
PIX_SORT(pp[0],pp[3]); PIX_SORT(pp[5],pp[8]); PIX_SORT(pp[4],pp[7]); \
PIX_SORT(pp[3],pp[6]); PIX_SORT(pp[1],pp[4]); PIX_SORT(pp[2],pp[5]); \
PIX_SORT(pp[4],pp[7]); PIX_SORT(pp[4],pp[2]); PIX_SORT(pp[6],pp[4]); \
PIX_SORT(pp[4],pp[2]); median=pp[4];} //pp4 = median

#define med2(a0,a1,a2,a3,a4,median) { \
pp[0]=a0; pp[1]=a1; pp[2]=a2; pp[3]=a3; pp[4]=a4;  \
PIX_SORT(pp[0],pp[1]) ; PIX_SORT(pp[3],pp[4]) ; PIX_SORT(pp[0],pp[3]) ;\
PIX_SORT(pp[1],pp[4]) ; PIX_SORT(pp[1],pp[2]) ; PIX_SORT(pp[2],pp[3]) ;\
PIX_SORT(pp[1],pp[2]) ; median=pp[2] ;}

#define med5(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,a22,a23,a24,median) { \
pp[0]=a0; pp[1]=a1; pp[2]=a2; pp[3]=a3; pp[4]=a4; pp[5]=a5; pp[6]=a6; pp[7]=a7; pp[8]=a8; pp[9]=a9; pp[10]=a10; pp[11]=a11; pp[12]=a12; \
pp[13]=a13; pp[14]=a14; pp[15]=a15; pp[16]=a16; pp[17]=a17; pp[18]=a18; pp[19]=a19; pp[20]=a20; pp[21]=a21; pp[22]=a22; pp[23]=a23; pp[24]=a24; \
PIX_SORT(pp[0], pp[1]) ;   PIX_SORT(pp[3], pp[4]) ;   PIX_SORT(pp[2], pp[4]) ;\
PIX_SORT(pp[2], pp[3]) ;   PIX_SORT(pp[6], pp[7]) ;   PIX_SORT(pp[5], pp[7]) ;\
PIX_SORT(pp[5], pp[6]) ;   PIX_SORT(pp[9], pp[10]) ;  PIX_SORT(pp[8], pp[10]) ;\
PIX_SORT(pp[8], pp[9]) ;   PIX_SORT(pp[12], pp[13]) ; PIX_SORT(pp[11], pp[13]) ;\
PIX_SORT(pp[11], pp[12]) ; PIX_SORT(pp[15], pp[16]) ; PIX_SORT(pp[14], pp[16]) ;\
PIX_SORT(pp[14], pp[15]) ; PIX_SORT(pp[18], pp[19]) ; PIX_SORT(pp[17], pp[19]) ;\
PIX_SORT(pp[17], pp[18]) ; PIX_SORT(pp[21], pp[22]) ; PIX_SORT(pp[20], pp[22]) ;\
PIX_SORT(pp[20], pp[21]) ; PIX_SORT(pp[23], pp[24]) ; PIX_SORT(pp[2], pp[5]) ;\
PIX_SORT(pp[3], pp[6]) ;   PIX_SORT(pp[0], pp[6]) ;   PIX_SORT(pp[0], pp[3]) ;\
PIX_SORT(pp[4], pp[7]) ;   PIX_SORT(pp[1], pp[7]) ;   PIX_SORT(pp[1], pp[4]) ;\
PIX_SORT(pp[11], pp[14]) ; PIX_SORT(pp[8], pp[14]) ;  PIX_SORT(pp[8], pp[11]) ;\
PIX_SORT(pp[12], pp[15]) ; PIX_SORT(pp[9], pp[15]) ;  PIX_SORT(pp[9], pp[12]) ;\
PIX_SORT(pp[13], pp[16]) ; PIX_SORT(pp[10], pp[16]) ; PIX_SORT(pp[10], pp[13]) ;\
PIX_SORT(pp[20], pp[23]) ; PIX_SORT(pp[17], pp[23]) ; PIX_SORT(pp[17], pp[20]) ;\
PIX_SORT(pp[21], pp[24]) ; PIX_SORT(pp[18], pp[24]) ; PIX_SORT(pp[18], pp[21]) ;\
PIX_SORT(pp[19], pp[22]) ; PIX_SORT(pp[8], pp[17]) ;  PIX_SORT(pp[9], pp[18]) ;\
PIX_SORT(pp[0], pp[18]) ;  PIX_SORT(pp[0], pp[9]) ;   PIX_SORT(pp[10], pp[19]) ;\
PIX_SORT(pp[1], pp[19]) ;  PIX_SORT(pp[1], pp[10]) ;  PIX_SORT(pp[11], pp[20]) ;\
PIX_SORT(pp[2], pp[20]) ;  PIX_SORT(pp[2], pp[11]) ;  PIX_SORT(pp[12], pp[21]) ;\
PIX_SORT(pp[3], pp[21]) ;  PIX_SORT(pp[3], pp[12]) ;  PIX_SORT(pp[13], pp[22]) ;\
PIX_SORT(pp[4], pp[22]) ;  PIX_SORT(pp[4], pp[13]) ;  PIX_SORT(pp[14], pp[23]) ;\
PIX_SORT(pp[5], pp[23]) ;  PIX_SORT(pp[5], pp[14]) ;  PIX_SORT(pp[15], pp[24]) ;\
PIX_SORT(pp[6], pp[24]) ;  PIX_SORT(pp[6], pp[15]) ;  PIX_SORT(pp[7], pp[16]) ;\
PIX_SORT(pp[7], pp[19]) ;  PIX_SORT(pp[13], pp[21]) ; PIX_SORT(pp[15], pp[23]) ;\
PIX_SORT(pp[7], pp[13]) ;  PIX_SORT(pp[7], pp[15]) ;  PIX_SORT(pp[1], pp[9]) ;\
PIX_SORT(pp[3], pp[11]) ;  PIX_SORT(pp[5], pp[17]) ;  PIX_SORT(pp[11], pp[17]) ;\
PIX_SORT(pp[9], pp[17]) ;  PIX_SORT(pp[4], pp[10]) ;  PIX_SORT(pp[6], pp[12]) ;\
PIX_SORT(pp[7], pp[14]) ;  PIX_SORT(pp[4], pp[6]) ;   PIX_SORT(pp[4], pp[7]) ;\
PIX_SORT(pp[12], pp[14]) ; PIX_SORT(pp[10], pp[14]) ; PIX_SORT(pp[6], pp[7]) ;\
PIX_SORT(pp[10], pp[12]) ; PIX_SORT(pp[6], pp[10]) ;  PIX_SORT(pp[6], pp[17]) ;\
PIX_SORT(pp[12], pp[17]) ; PIX_SORT(pp[7], pp[17]) ;  PIX_SORT(pp[7], pp[10]) ;\
PIX_SORT(pp[12], pp[18]) ; PIX_SORT(pp[7], pp[12]) ;  PIX_SORT(pp[10], pp[18]) ;\
PIX_SORT(pp[12], pp[20]) ; PIX_SORT(pp[10], pp[20]) ; PIX_SORT(pp[10], pp[12]) ;\
median=pp[12];} 

#define ELEM_FLOAT_SWAP(a,b) { register float t=(a);(a)=(b);(b)=t; }


namespace rtengine {

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	/*
	 Structure of the algorithm:

	 1. Compute an initial denoise of the image via undecimated wavelet transform
	 and universal thresholding modulated by user input.
	 2. Decompose the residual image into TSxTS size tiles, shifting by 'offset' each step
	 (so roughly each pixel is in (TS/offset)^2 tiles); Discrete Cosine transform the tiles.
	 3. Filter the DCT data to pick out patterns missed by the wavelet denoise
	 4. Inverse DCT the denoised tile data and combine the tiles into a denoised output image.

	 */

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

extern const Settings* settings;

// Median calculation using quicksort
float fq_sort2(float arr[], int n) 
{
    int low, high ;
    int median;
    int middle, ll, hh;

    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;) {
        if (high <= low)
            return arr[median] ;
        if (high == low + 1) {
            if (arr[low] > arr[high])
                ELEM_FLOAT_SWAP(arr[low], arr[high]) ;
           return arr[median] ;
        }

    middle = (low + high) / 2;
    if (arr[middle] > arr[high]) ELEM_FLOAT_SWAP(arr[middle], arr[high]) ;
    if (arr[low] > arr[high]) ELEM_FLOAT_SWAP(arr[low], arr[high]) ;
    if (arr[middle] > arr[low]) ELEM_FLOAT_SWAP(arr[middle], arr[low]) ;

    ELEM_FLOAT_SWAP(arr[middle], arr[low+1]) ;
    ll = low + 1;
    hh = high;

    for (;;) {
        do ll++; while (arr[low] > arr[ll]) ;
        do hh--; while (arr[hh]  > arr[low]) ;

        if (hh < ll)
        break;

        ELEM_FLOAT_SWAP(arr[ll], arr[hh]) ;
   }

    ELEM_FLOAT_SWAP(arr[low], arr[hh]) ;

    if (hh <= median)
        low = ll;
        if (hh >= median)
        high = hh - 1;
    }
 }

float media(float *elements, int N)
{
  int i,j,min;
  float temp;

  //   Order elements (only half of them)
   for (i = 0; i < (N >> 1) + 1; ++i)
   {
      //   Find position of minimum element
      min = i;
      for (j = i + 1; j < N; ++j)
         if (elements[j] < elements[min])
            min = j;
      //   Put found minimum element in its place
      temp = elements[i];
      elements[i] = elements[min];
      elements[min] = temp;
   }
   //   Get result - the middle element
   return elements[N >> 1];
}


	void ImProcFunctions::RGB_denoise(Imagefloat * src, Imagefloat * dst, bool isRAW, const procparams::DirPyrDenoiseParams & dnparams, const procparams::DefringeParams & defringe, const double expcomp)
	{
//#ifdef _DEBUG
	MyTime t1e,t2e;
	t1e.set();
//#endif
	
		static MyMutex FftwMutex;
		MyMutex::MyLock lock(FftwMutex);


		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		/*if (plistener) {
		 plistener->setProgressStr ("Denoise...");
		 plistener->setProgress (0.0);
		 }*/

//		volatile double progress = 0.0;

		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		const short int imheight=src->height, imwidth=src->width;

		if (dnparams.luma==0 && dnparams.chroma==0  && !dnparams.median ) {
            //nothing to do; copy src to dst or do nothing in case src == dst
			if(src != dst)
				memcpy(dst->data,src->data,dst->width*dst->height*3*sizeof(float));
			return;
		}
		
	if (dnparams.luma!=0 || dnparams.chroma!=0 || dnparams.methodmed=="Lab" || dnparams.methodmed=="Lonly" ) {
		perf=false;
		if(dnparams.dmethod=="RGB") perf=true;//RGB mode
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// gamma transform for input data
		float gam = dnparams.gamma;
		float gamthresh = 0.001f;
		if(!isRAW) {//reduce gamma under 1 for Lab mode ==> TIF and JPG
		 if(gam <1.9f) gam=1.f - (1.9f-gam)/3.f;//minimum gamma 0.7
		 else if (gam >= 1.9f && gam <= 3.f) gam=(1.4f/1.1f)*gam - 1.41818f;
		 }
		float gamslope = exp(log((double)gamthresh)/gam)/gamthresh;

		LUTf gamcurve(65536,0);
		if(perf) {
		for (int i=0; i<65536; i++) {
			gamcurve[i] = (Color::gamma((double)i/65535.0, gam, gamthresh, gamslope, 1.0, 0.0)) * 32768.0f;
		}
		}
		else  {
		for (int i=0; i<65536; i++) {
			gamcurve[i] = (Color::gamman((double)i/65535.0,gam)) * 32768.0f;
		}
		}

		// inverse gamma transform for output data
		float igam = 1.f/gam;
		float igamthresh = gamthresh*gamslope;
		float igamslope = 1.f/gamslope;

		LUTf igamcurve(65536,0);
		if(perf) {
		for (int i=0; i<65536; i++) {
			igamcurve[i] = (Color::gamma((float)i/32768.0f, igam, igamthresh, igamslope, 1.0, 0.0) * 65535.0f);
		}
		}
		else {
		for (int i=0; i<65536; i++) {
			igamcurve[i] = (Color::gamman((float)i/32768.0f,igam) * 65535.0f);
		}

		}
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


		//srand((unsigned)time(0));//test with random data

		const float gain = pow (2.0f, float(expcomp));
		float incr=1.f;
		float noisevar_Ldetail = SQR((float)(SQR(100.-dnparams.Ldetail) + 50.*(100.-dnparams.Ldetail)) * TS * 0.5f * incr);
		bool enhance_denoise = dnparams.enhance;
//		bool median_denoise = dnparams.median;
		int gamlab = settings->denoiselabgamma;//gamma lab essentialy for Luminance detail
		if(gamlab > 2) gamlab=2;
		if(settings->verbose) printf("Denoise Lab=%i\n",gamlab);

		array2D<float> tilemask_in(TS,TS);
		array2D<float> tilemask_out(TS,TS);

		const int border = MAX(2,TS/16);

#ifdef _OPENMP
#pragma omp parallel for
#endif
		for (int i=0; i<TS; i++) {
			float i1 = abs((i>TS/2 ? i-TS+1 : i));
			float vmask = (i1<border ? SQR(sin((M_PI*i1)/(2*border))) : 1.0f);
			float vmask2 = (i1<2*border ? SQR(sin((M_PI*i1)/(2*border))) : 1.0f);
			for (int j=0; j<TS; j++) {
				float j1 = abs((j>TS/2 ? j-TS+1 : j));
				tilemask_in[i][j] = (vmask * (j1<border ? SQR(sin((M_PI*j1)/(2*border))) : 1.0f)) + epsilon;

				tilemask_out[i][j] = (vmask2 * (j1<2*border ? SQR(sin((M_PI*j1)/(2*border))) : 1.0f)) + epsilon;

			}
		}
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// begin tile processing of image

		//output buffer
		Imagefloat * dsttmp = new Imagefloat(imwidth,imheight);
		for (int n=0; n<3*imwidth*imheight; n++) dsttmp->data[n] = 0;

		const int tilesize = 1024;
		const int overlap = 128;

		int numtiles_W, numtiles_H, tilewidth, tileheight, tileWskip, tileHskip;

		if (imwidth<tilesize) {
			numtiles_W = 1;
			tileWskip = imwidth;
			tilewidth = imwidth;
		} else {
			numtiles_W = ceil(((float)(imwidth))/(tilesize-overlap));
			tilewidth  = ceil(((float)(imwidth))/(numtiles_W))+overlap;
			tilewidth += (tilewidth&1);
			tileWskip = tilewidth-overlap;
		}
		if (imheight<tilesize) {
			numtiles_H = 1;
			tileHskip = imheight;
			tileheight = imheight;
		} else {
			numtiles_H = ceil(((float)(imheight))/(tilesize-overlap));
			tileheight = ceil(((float)(imheight))/(numtiles_H))+overlap;
			tileheight += (tileheight&1);
			tileHskip = tileheight-overlap;
		}
		//now we have tile dimensions, overlaps
		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        // According to FFTW-Doc 'it is safe to execute the same plan in parallel by multiple threads', so we now create 4 plans
        // outside the parallel region and use them inside the parallel region.

        // calculate max size of numblox_W.
        int max_numblox_W = ceil(((float)(MIN(imwidth,tilewidth)))/(offset))+2*blkrad;
        // calculate min size of numblox_W.
		int min_numblox_W = ceil(((float)((MIN(imwidth,((numtiles_W - 1) * tileWskip) + tilewidth) ) - ((numtiles_W - 1) * tileWskip)))/(offset))+2*blkrad;

        // these are needed only for creation of the plans and will be freed before entering the parallel loop
		float * Lbloxtmp;
		float * fLbloxtmp;
		Lbloxtmp  = (float*) fftwf_malloc(max_numblox_W*TS*TS*sizeof (float));
		fLbloxtmp = (float*) fftwf_malloc(max_numblox_W*TS*TS*sizeof (float));

		int nfwd[2]={TS,TS};

        //for DCT:
		const fftw_r2r_kind fwdkind[2] = {FFTW_REDFT10, FFTW_REDFT10};
		const fftw_r2r_kind bwdkind[2] = {FFTW_REDFT01, FFTW_REDFT01};

        fftwf_plan plan_forward_blox[2];
        fftwf_plan plan_backward_blox[2];

        // Creating the plans with FFTW_MEASURE instead of FFTW_ESTIMATE speeds up the execute a bit
		plan_forward_blox[0]  = fftwf_plan_many_r2r(2, nfwd, max_numblox_W, Lbloxtmp, NULL, 1, TS*TS, fLbloxtmp, NULL, 1, TS*TS, fwdkind, FFTW_MEASURE );
		plan_backward_blox[0] = fftwf_plan_many_r2r(2, nfwd, max_numblox_W, fLbloxtmp, NULL, 1, TS*TS, Lbloxtmp, NULL, 1, TS*TS, bwdkind, FFTW_MEASURE );
		plan_forward_blox[1]  = fftwf_plan_many_r2r(2, nfwd, min_numblox_W, Lbloxtmp, NULL, 1, TS*TS, fLbloxtmp, NULL, 1, TS*TS, fwdkind, FFTW_MEASURE );
		plan_backward_blox[1] = fftwf_plan_many_r2r(2, nfwd, min_numblox_W, fLbloxtmp, NULL, 1, TS*TS, Lbloxtmp, NULL, 1, TS*TS, bwdkind, FFTW_MEASURE );
		fftwf_free ( Lbloxtmp );
		fftwf_free ( fLbloxtmp );

#ifdef _OPENMP
        // Calculate number of tiles. If less than omp_get_max_threads(), then limit num_threads to number of tiles
        int numtiles = numtiles_W * numtiles_H;
        int numthreads = MIN(numtiles,omp_get_max_threads());
        if(options.rgbDenoiseThreadLimit > 0) numthreads = MIN(numthreads,options.rgbDenoiseThreadLimit);
        // Issue 1887, overide setting of 1, if more than one thread is available. This way the inner omp-directives should become inactive
        if(numthreads == 1 && omp_get_max_threads() > 1)
			numthreads = 2;
#pragma omp parallel num_threads(numthreads)
#endif
        {
		//DCT block data storage
		float * Lblox;
		float * fLblox;
    TMatrix wprof = iccStore->workingSpaceMatrix (params->icm.working);

    double wp[3][3] = {
        {wprof[0][0],wprof[0][1],wprof[0][2]},
		{wprof[1][0],wprof[1][1],wprof[1][2]},
		{wprof[2][0],wprof[2][1],wprof[2][2]}
    };

	TMatrix wiprof = iccStore->workingSpaceInverseMatrix (params->icm.working);
	//inverse matrix user select
	double wip[3][3] = {
		{wiprof[0][0],wiprof[0][1],wiprof[0][2]},
		{wiprof[1][0],wiprof[1][1],wiprof[1][2]},
		{wiprof[2][0],wiprof[2][1],wiprof[2][2]}
	};



#ifdef _OPENMP
#pragma omp critical
#endif
        {
		Lblox  = (float*) fftwf_malloc(max_numblox_W*TS*TS*sizeof(float));
		fLblox = (float*) fftwf_malloc(max_numblox_W*TS*TS*sizeof(float));
        }
        
        float * nbrwt = new float[TS*TS];
        float * blurbuffer = new float[TS*TS];
#ifdef _OPENMP
#pragma omp for schedule(dynamic) collapse(2)
#endif
		for (int tiletop=0; tiletop<imheight; tiletop+=tileHskip) {
			for (int tileleft=0; tileleft<imwidth; tileleft+=tileWskip) {

				int tileright = MIN(imwidth,tileleft+tilewidth);
				int tilebottom = MIN(imheight,tiletop+tileheight);
				int width  = tileright-tileleft;
				int height = tilebottom-tiletop;
				//input L channel
				array2D<float> Lin(width,height);
				//wavelet denoised image
				LabImage * labdn = new LabImage(width,height);

				//residual between input and denoised L channel
				array2D<float> Ldetail(width,height,ARRAY2D_CLEAR_DATA);
				//pixel weight
				array2D<float> totwt(width,height,ARRAY2D_CLEAR_DATA);//weight for combining DCT blocks

				//TODO: implement using AlignedBufferMP
				//fill tile from image; convert RGB to "luma/chroma"
				if (isRAW) {//image is raw; use channel differences for chroma channels
					if(!perf){//lab mode
							//modification Jacques feb 2013					
					for (int i=tiletop/*, i1=0*/; i<tilebottom; i++/*, i1++*/) {
						int i1 = i - tiletop;
						for (int j=tileleft/*, j1=0*/; j<tileright; j++/*, j1++*/) {
							int j1 = j - tileleft;
							float R_ = gain*src->r(i,j);
							float G_ = gain*src->g(i,j);
							float B_ = gain*src->b(i,j);
							//modify arbitrary data for Lab..I have test : nothing, gamma 2.6 11 - gamma 4 5 - gamma 5.5 10
							//we can put other as gamma g=2.6 slope=11, etc.
							// but noting to do with real gamma !!!: it's only for data Lab # data RGB
							//finally I opted fot gamma55 and with options we can change
							if (gamlab == 0) {// options 12/2013
							R_ = Color::igammatab_26_11[R_];
							G_ = Color::igammatab_26_11[G_];
							B_ = Color::igammatab_26_11[B_];
							}
							else if (gamlab == 1) {
							//other new gamma 4 5 
							R_ = Color::igammatab_4[R_];
							G_ = Color::igammatab_4[G_];
							B_ = Color::igammatab_4[B_];
							}
							else if (gamlab == 2) {							
							//new gamma 5.5 10 better for detail luminance..it is a compromise...which depends on the image (distribution BL, ML, HL ...)
							R_ = Color::igammatab_55[R_];
							G_ = Color::igammatab_55[G_];
							B_ = Color::igammatab_55[B_];
							}
							//apply gamma noise	standard (slider)
							R_ = R_<65535.0f ? gamcurve[R_] : (Color::gamman((double)R_/65535.0, gam)*32768.0f);
							G_ = G_<65535.0f ? gamcurve[G_] : (Color::gamman((double)G_/65535.0, gam)*32768.0f);
							B_ = B_<65535.0f ? gamcurve[B_] : (Color::gamman((double)B_/65535.0, gam)*32768.0f);
							//true conversion xyz=>Lab
							float L,a,b;
							float X,Y,Z;
							Color::rgbxyz(R_,G_,B_,X,Y,Z,wp);

							//convert to Lab
							Color::XYZ2Lab(X, Y, Z, L, a, b);
							labdn->L[i1][j1] = L;
							labdn->a[i1][j1] = a;
							labdn->b[i1][j1] = b;
							Lin[i1][j1] = L;
//							totwt[i1][j1] = 0;
						}
					}
					}
					else {//RGB mode
					for (int i=tiletop/*, i1=0*/; i<tilebottom; i++/*, i1++*/) {
						int i1 = i - tiletop;
						for (int j=tileleft/*, j1=0*/; j<tileright; j++/*, j1++*/) {
							int j1 = j - tileleft;

							float X = gain*src->r(i,j);
							float Y = gain*src->g(i,j);
							float Z = gain*src->b(i,j);

							X = X<65535.0f ? gamcurve[X] : (Color::gamma((double)X/65535.0, gam, gamthresh, gamslope, 1.0, 0.0)*32768.0f);
							Y = Y<65535.0f ? gamcurve[Y] : (Color::gamma((double)Y/65535.0, gam, gamthresh, gamslope, 1.0, 0.0)*32768.0f);
							Z = Z<65535.0f ? gamcurve[Z] : (Color::gamma((double)Z/65535.0, gam, gamthresh, gamslope, 1.0, 0.0)*32768.0f);

							labdn->L[i1][j1] = Y;
							labdn->a[i1][j1] = (X-Y);
							labdn->b[i1][j1] = (Y-Z);

//							Ldetail[i1][j1] = 0;
							Lin[i1][j1] = Y;
//							totwt[i1][j1] = 0;
						}
					}

					}
				} else {//image is not raw; use Lab parametrization
					for (int i=tiletop/*, i1=0*/; i<tilebottom; i++/*, i1++*/) {
						int i1 = i - tiletop;
						for (int j=tileleft/*, j1=0*/; j<tileright; j++/*, j1++*/) {
							int j1 = j - tileleft;
							float L,a,b;
							//use gamma sRGB, not good if TIF (JPG) Output profil not with gamma sRGB  (eg : gamma =1.0, or 1.8...)
							//very difficult to solve !
							// solution ==> save TIF with gamma sRGB and re open
							float rtmp = Color::igammatab_srgb[ src->r(i,j) ];
							float gtmp = Color::igammatab_srgb[ src->g(i,j) ];
							float btmp = Color::igammatab_srgb[ src->b(i,j) ];
							//modification Jacques feb 2013
							// gamma slider different from raw
							rtmp = rtmp<65535.0f ? gamcurve[rtmp] : (Color::gamman((double)rtmp/65535.0, gam)*32768.0f);
							gtmp = gtmp<65535.0f ? gamcurve[gtmp] : (Color::gamman((double)gtmp/65535.0, gam)*32768.0f);
							btmp = btmp<65535.0f ? gamcurve[btmp] : (Color::gamman((double)btmp/65535.0, gam)*32768.0f);


							float X,Y,Z;
							Color::rgbxyz(rtmp,gtmp,btmp,X,Y,Z,wp);

							//convert Lab
							Color::XYZ2Lab(X, Y, Z, L, a, b);
							labdn->L[i1][j1] = L;
							labdn->a[i1][j1] = a;
							labdn->b[i1][j1] = b;

//							Ldetail[i1][j1] = 0;
							Lin[i1][j1] = L;

//							totwt[i1][j1] = 0;
						}
					}
				}


				//initial impulse denoise
				if (dnparams.luma>0.01) {
					impulse_nr (labdn, float(MIN(50.0,dnparams.luma))/20.0f);
				}

				int datalen = labdn->W * labdn->H;

				//now perform basic wavelet denoise
				//last two arguments of wavelet decomposition are max number of wavelet decomposition levels;
				//and whether to subsample the image after wavelet filtering.  Subsampling is coded as
				//binary 1 or 0 for each level, eg subsampling = 0 means no subsampling, 1 means subsample
				//the first level only, 7 means subsample the first three levels, etc.
				float noisevarL	 = (float) (SQR((dnparams.luma/125.0)*(1.+ dnparams.luma/25.0)));
				//printf("noisL=%f \n",noisevarL);
				float interm_med= (float) dnparams.chroma/10.0;
				//printf("inter=%f\n",interm_med);
				float intermred, intermblue;
				if(dnparams.redchro > 0.) intermred=0.0014f* (float)SQR(dnparams.redchro); else intermred= (float) dnparams.redchro/7.0;//increase slower than linear for more sensit
				float intermred2=(float) dnparams.redchro/7.0;
				if(dnparams.bluechro > 0.) intermblue=0.0014f*(float) SQR(dnparams.bluechro); else intermblue= (float)dnparams.bluechro/7.0;//increase slower than linear		
				float intermblue2=(float) dnparams.bluechro/7.0;
				//adjust noise ab in function of sliders red and blue
				float realred = interm_med + intermred; if (realred < 0.f) realred=0.01f;
				float realred2 = interm_med + intermred2; if (realred2 < 0.f) realred2=0.01f;
				float noisevarab_r = SQR(realred);
				float realblue = interm_med + intermblue; if (realblue < 0.f) realblue=0.01f;
				float realblue2 = interm_med + intermblue2; if (realblue2 < 0.f) realblue2=0.01f;
				float noisevarab_b = SQR(realblue);
				bool execwavelet=true;
				if(noisevarL < 0.00007f && interm_med < 0.1f && dnparams.median && (dnparams.methodmed=="Lab" || dnparams.methodmed=="Lonly")) execwavelet=false;//do not exec wavelet if sliders luminance and chroma are very small and median need
				//we considered user don't want wavelet
				if(execwavelet) {//gain time if user choose only median  sliders L <=1  slider chrom master < 1
                { // enclosing this code in a block frees about 120 MB before allocating 20 MB after this block (measured with D700 NEF)
                wavelet_decomposition* Ldecomp;
                wavelet_decomposition* adecomp;
                wavelet_decomposition* bdecomp;

				int levwav=5;
				float maxreal = max(realred2, realblue2);
				//increase the level of wavelet if user increase much or very much sliders
				if( maxreal < 8.f) levwav=5;
				else if( maxreal < 10.f)levwav=6;
				else if( maxreal < 15.f)levwav=7;
				else levwav=8;//maximum ==> I have increase Maxlevel in cplx_wavelet_dec.h from 8 to 9
				
				
				//	if (settings->verbose) printf("levwavelet=%i  noisevarA=%f noisevarB=%f \n",levwav, noisevarab_r, noisevarab_b );
                Ldecomp = new wavelet_decomposition (labdn->data, labdn->W, labdn->H, levwav/*maxlevels*/, 0/*subsampling*/ );
                adecomp = new wavelet_decomposition (labdn->data+datalen, labdn->W, labdn->H,levwav, 1 );
                bdecomp = new wavelet_decomposition (labdn->data+2*datalen, labdn->W, labdn->H, levwav, 1 );

				if(enhance_denoise)	WaveletDenoiseAll_BiShrink(*Ldecomp, *adecomp, *bdecomp, noisevarL, noisevarab_r, noisevarab_b,labdn);//enhance mode
				else; WaveletDenoiseAll(*Ldecomp, *adecomp, *bdecomp, noisevarL, noisevarab_r, noisevarab_b,labdn);//

				Ldecomp->reconstruct(labdn->data);
				delete Ldecomp;
				adecomp->reconstruct(labdn->data+datalen);
				delete adecomp;
				bdecomp->reconstruct(labdn->data+2*datalen);
				delete bdecomp;
                }
				}
				//TODO: at this point wavelet coefficients storage can be freed
				//Issue 1680: Done now

				//second impulse denoise
				if (dnparams.luma>0.01) {
					impulse_nr (labdn, MIN(50.0f,(float)dnparams.luma)/20.0f);
				}
				//PF_correct_RT(dst, dst, defringe.radius, defringe.threshold);
				
				int metchoice=0;
				if(dnparams.methodmed=="Lonly") metchoice=1;
				else if(dnparams.methodmed=="Lab") metchoice=2;
				
				//median on Luminance Lab only
			if( (metchoice==1 || metchoice==2)   && dnparams.median) {
			//printf("Lab et Lonly \n");
			for(int iteration=1;iteration<=dnparams.passes;iteration++){
			  //printf("pas=%i\n",iteration);
					int wid=labdn->W;
					int hei=labdn->H;
					float** tmL;
					tmL = new float*[hei];
						for (int i=0; i<hei; i++)
							tmL[i] = new float[wid];
			
					int methmedL=0;
					int borderL = 1;
					if(dnparams.medmethod=="soft")
						methmedL=0;
					else if(dnparams.medmethod=="33")
						methmedL=1;
					else if(dnparams.medmethod=="55") {
						methmedL = 3;
						borderL = 2;
					}
					else if(dnparams.medmethod=="55soft") {
						methmedL = 2;
						borderL = 2;
					}	
					else if(dnparams.medmethod=="77") {
						methmedL = 4;
						borderL = 3;
					}	
				if (metchoice==1 || metchoice==2)
				{ /*printf("LONLY methmedL=%d\n", methmedL);*/
					
					if(methmedL < 2) {
						for (int i=1; i<hei-1; i++) {
							float pp[9],results[5],temp;
							if(methmedL == 0) 
								for (int j=1; j<wid-1; j++) {
									med2(labdn->L[i][j] ,labdn->L[i-1][j], labdn->L[i+1][j] ,labdn->L[i][j+1],labdn->L[i][j-1], tmL[i][j]);//3x3 soft
								}
							else
								for (int j=1; j<wid-1; j++) {//hybrid median filter ==> impluse denoise !I let here to get trace
									/*	pp[0] = labdn->L[i][j-1] ;
										pp[1] = labdn->L[i-1][j] ;
										pp[2] = labdn->L[i][j] ;
										pp[3] = labdn->L[i+1][j] ;
										pp[4] = labdn->L[i][j+1] ;
									//   Get median
										results[0] = media(pp, 5);
									//   Pick up x-window elements
										pp[0] = labdn->L[i-1][j-1] ;
										pp[1] = labdn->L[i+1][j-1] ;
										pp[2] = labdn->L[i][j] ;
										pp[3] = labdn->L[i-1][j+1] ;
										pp[4] = labdn->L[i+1][j+1] ;
										  //   Get median
										results[1] = media(pp, 5);
										//   Pick up leading element
										results[2] = labdn->L[i][j] ;;
										//   Get result
										tmL[i][j] = media(results, 3);
								*/
								med3(labdn->L[i][j] ,labdn->L[i-1][j], labdn->L[i+1][j] ,labdn->L[i][j+1],labdn->L[i][j-1], labdn->L[i-1][j-1],labdn->L[i-1][j+1],labdn->L[i+1][j-1],labdn->L[i+1][j+1],tmL[i][j]);//3x3 soft
								}
						}
					}
					else {
						for (int i=borderL; i<hei-borderL; i++) {
							float pp[49],temp;
						if(methmedL == 4)						
							for (int j=borderL; j<wid-borderL; j++) {
									int kk=0;
									for (int ii=-3;ii<=3;ii++) {
										for (int jj=-3;jj<=3;jj++){kk++;
										 pp[kk]=labdn->L[i+ii][j+jj];
										}
									}	
								fq_sort2(pp,49);
								tmL[i][j]=pp[24];//7x7
							}
						else if(methmedL == 3)	
							for (int j=2; j<wid-2; j++) {
								med5(labdn->L[i][j],labdn->L[i-1][j],labdn->L[i+1][j],labdn->L[i][j+1],labdn->L[i][j-1],labdn->L[i-1][j-1],labdn->L[i-1][j+1], labdn->L[i+1][j-1],labdn->L[i+1][j+1],
								labdn->L[i-2][j],labdn->L[i+2][j],labdn->L[i][j+2],labdn->L[i][j-2],labdn->L[i-2][j-2],labdn->L[i-2][j+2],labdn->L[i+2][j-2],labdn->L[i+2][j+2],	
								labdn->L[i-2][j+1],labdn->L[i+2][j+1],labdn->L[i-1][j+2],labdn->L[i-1][j-2],labdn->L[i-2][j-1],labdn->L[i+2][j-1],labdn->L[i+1][j+2],labdn->L[i+1][j-2],	
								tmL[i][j]);//5x5
							}
						else
							for (int j=2; j<wid-2; j++) {
								pp[0]=labdn->L[i][j];pp[1]=labdn->L[i-1][j]; pp[2]=labdn->L[i+1][j];pp[3]=labdn->L[i][j+1];pp[4]=labdn->L[i][j-1];pp[5]=labdn->L[i-1][j-1];pp[6]=labdn->L[i-1][j+1];
								pp[7]=labdn->L[i+1][j-1];pp[8]=labdn->L[i+1][j+1];pp[9]=labdn->L[i+2][j];pp[10]=labdn->L[i-2][j];pp[11]=labdn->L[i][j+2];pp[12]=labdn->L[i][j-2];
								fq_sort2(pp,13);
								tmL[i][j]=pp[6];//5x5 soft
							}
						}
					}

		
					for(int i = borderL; i < hei-borderL; i++ ) {
						for(int j = borderL; j < wid-borderL; j++) {
							labdn->L[i][j] = tmL[i][j];
						}
		 			}
             }    
				if(metchoice==2) {/*printf(" AB methmedL=%d\n", methmedL);*/
					if(methmedL < 2) {
						for (int i=1; i<hei-1; i++) {
							float pp[9],temp;
							if(methmedL == 0) 
								for (int j=1; j<wid-1; j++) {
									med2(labdn->a[i][j] ,labdn->a[i-1][j], labdn->a[i+1][j] ,labdn->a[i][j+1],labdn->a[i][j-1], tmL[i][j]);//3x3 soft
								}
							else
								for (int j=1; j<wid-1; j++) {
								med3(labdn->a[i][j] ,labdn->a[i-1][j], labdn->a[i+1][j] ,labdn->a[i][j+1],labdn->a[i][j-1], labdn->a[i-1][j-1],labdn->a[i-1][j+1],labdn->a[i+1][j-1],labdn->a[i+1][j+1],tmL[i][j]);//3x3 soft
								}
						}
					}
					else {
						for (int i=borderL; i<hei-borderL; i++) {
							float pp[49],temp;
						if(methmedL == 4)						
							for (int j=borderL; j<wid-borderL; j++) {
									int kk=0;
									for (int ii=-3;ii<=3;ii++) {
										for (int jj=-3;jj<=3;jj++){kk++;
										 pp[kk]=labdn->a[i+ii][j+jj];
										}
									}	
								fq_sort2(pp,49);
								tmL[i][j]=pp[24];//7x7
							}
						else if(methmedL == 3)
							for (int j=2; j<wid-2; j++) {						
								med5(labdn->a[i][j],labdn->a[i-1][j],labdn->a[i+1][j],labdn->a[i][j+1],labdn->a[i][j-1],labdn->a[i-1][j-1],labdn->a[i-1][j+1], labdn->a[i+1][j-1],labdn->a[i+1][j+1],
								labdn->a[i-2][j],labdn->a[i+2][j],labdn->a[i][j+2],labdn->a[i][j-2],labdn->a[i-2][j-2],labdn->a[i-2][j+2],labdn->a[i+2][j-2],labdn->a[i+2][j+2],	
								labdn->a[i-2][j+1],labdn->a[i+2][j+1],labdn->a[i-1][j+2],labdn->a[i-1][j-2],labdn->a[i-2][j-1],labdn->a[i+2][j-1],labdn->a[i+1][j+2],labdn->a[i+1][j-2],	
								tmL[i][j]);//5x5
								}
						else
							for (int j=2; j<wid-2; j++) {
								pp[0]=labdn->a[i][j];pp[1]=labdn->a[i-1][j]; pp[2]=labdn->a[i+1][j];pp[3]=labdn->a[i][j+1];pp[4]=labdn->a[i][j-1];pp[5]=labdn->a[i-1][j-1];pp[6]=labdn->a[i-1][j+1];
								pp[7]=labdn->a[i+1][j-1];pp[8]=labdn->a[i+1][j+1];pp[9]=labdn->a[i+2][j];pp[10]=labdn->a[i-2][j];pp[11]=labdn->a[i][j+2];pp[12]=labdn->a[i][j-2];
								fq_sort2(pp,13);
								tmL[i][j]=pp[6];//5x5 soft
							}
						/*	for (int j=3; j<wid-3; j++) {
									int kk=0;
									for (int ii=-2;ii<=2;ii++) {
										for (int jj=-2;jj<=2;jj++){kk++;
										 pp[kk]=labdn->a[i+ii][j+jj];
										}
										pp[kk+1]=labdn->a[i-3][j-3];pp[kk+2]=labdn->a[i-3][j+3];pp[kk+3]=labdn->a[i+3][j-3];pp[kk+4]=labdn->a[i+3][j+3];
										pp[kk+5]=labdn->a[i-3][j];pp[kk+6]=labdn->a[i+3][j];pp[kk+7]=labdn->a[i][j-3];pp[kk+8]=labdn->a[i][j+3];
										
									}	
								fq_sort2(pp,33);
								tmL[i][j]=pp[16];//7x7
							}
							*/
						}
					}

		
					for(int i = borderL; i < hei-borderL; i++ ) {
						for(int j = borderL; j < wid-borderL; j++) {
							labdn->a[i][j] = tmL[i][j];
						}
					}
					
					
//b
					if(methmedL < 2) {
						for (int i=1; i<hei-1; i++) {
							float pp[9],temp;
							if(methmedL == 0) 
								for (int j=1; j<wid-1; j++) {
									med2(labdn->b[i][j] ,labdn->b[i-1][j], labdn->b[i+1][j] ,labdn->b[i][j+1],labdn->b[i][j-1], tmL[i][j]);//3x3 soft
								}
							else
								for (int j=1; j<wid-1; j++) {
								med3(labdn->b[i][j] ,labdn->b[i-1][j], labdn->b[i+1][j] ,labdn->b[i][j+1],labdn->b[i][j-1], labdn->b[i-1][j-1],labdn->b[i-1][j+1],labdn->b[i+1][j-1],labdn->b[i+1][j+1],tmL[i][j]);//3x3 soft
								}
						}
					}
					else {
						for (int i=borderL; i<hei-borderL; i++) {
							float pp[49],temp;
						if(methmedL == 4)			
							for (int j=borderL; j<wid-borderL; j++) {
									int kk=0;
									for (int ii=-3;ii<=3;ii++) {
										for (int jj=-3;jj<=3;jj++){kk++;
										 pp[kk]=labdn->b[i+ii][j+jj];
										}
									}	
								fq_sort2(pp,49);
								tmL[i][j]=pp[24];//7x7
							
								
							}
						else if(methmedL == 3)			
							for (int j=2; j<wid-2; j++) {
								med5(labdn->b[i][j],labdn->b[i-1][j],labdn->b[i+1][j],labdn->b[i][j+1],labdn->b[i][j-1],labdn->b[i-1][j-1],labdn->b[i-1][j+1], labdn->b[i+1][j-1],labdn->b[i+1][j+1],
								labdn->b[i-2][j],labdn->b[i+2][j],labdn->b[i][j+2],labdn->b[i][j-2],labdn->b[i-2][j-2],labdn->b[i-2][j+2],labdn->b[i+2][j-2],labdn->b[i+2][j+2],	
								labdn->b[i-2][j+1],labdn->b[i+2][j+1],labdn->b[i-1][j+2],labdn->b[i-1][j-2],labdn->b[i-2][j-1],labdn->b[i+2][j-1],labdn->b[i+1][j+2],labdn->b[i+1][j-2],	
								tmL[i][j]);//5x5
							
							}
						else
							for (int j=2; j<wid-2; j++) {
							/*for (int j=3; j<wid-3; j++) {
									int kk=0;
									for (int ii=-2;ii<=2;ii++) {
										for (int jj=-2;jj<=2;jj++){kk++;
										 pp[kk]=labdn->b[i+ii][j+jj];
										}
										pp[kk+1]=labdn->b[i-3][j-3];pp[kk+2]=labdn->b[i-3][j+3];pp[kk+3]=labdn->b[i+3][j-3];pp[kk+4]=labdn->b[i+3][j+3];
										pp[kk+5]=labdn->b[i-3][j];pp[kk+6]=labdn->b[i+3][j];pp[kk+7]=labdn->b[i][j-3];pp[kk+8]=labdn->b[i][j+3];
										
									}	
								fq_sort2(pp,33);
								tmL[i][j]=pp[16];//7x7
							*/
							
							
								pp[0]=labdn->b[i][j];pp[1]=labdn->b[i-1][j]; pp[2]=labdn->b[i+1][j];pp[3]=labdn->b[i][j+1];pp[4]=labdn->b[i][j-1];pp[5]=labdn->b[i-1][j-1];pp[6]=labdn->b[i-1][j+1];
								pp[7]=labdn->b[i+1][j-1];pp[8]=labdn->b[i+1][j+1];pp[9]=labdn->b[i+2][j];pp[10]=labdn->b[i-2][j];pp[11]=labdn->b[i][j+2];pp[12]=labdn->b[i][j-2];
								fq_sort2(pp,13);
								tmL[i][j]=pp[6];//5x5 soft
							
							}
						}
					}

		
					for(int i = borderL; i < hei-borderL; i++ ) {
						for(int j = borderL; j < wid-borderL; j++) {
							labdn->b[i][j] = tmL[i][j];
						}
					}
				}			
			
					for (int i=0; i<hei; i++)
						delete [] tmL[i];
					delete [] tmL;
			}
			}
			
				//wavelet denoised L channel
				array2D<float> Lwavdn(width,height);
				float * Lwavdnptr = Lwavdn;
				memcpy (Lwavdnptr, labdn->data, width*height*sizeof(float));

				//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// now do detail recovery using block DCT to detect
				// patterns missed by wavelet denoise
				// blocks are not the same thing as tiles!



				// calculation for detail recovery blocks
				const int numblox_W = ceil(((float)(width))/(offset))+2*blkrad;
				const int numblox_H = ceil(((float)(height))/(offset))+2*blkrad;

				//const int nrtiles = numblox_W*numblox_H;
				// end of tiling calc
                {

				//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// Main detail recovery algorithm: Block loop
                AlignedBuffer<float> pBuf(width + TS + 2*blkrad*offset);
				for (int vblk=0; vblk<numblox_H; vblk++) {
					//printf("vblock=%d",vblk);

					int top = (vblk-blkrad)*offset;
					float * datarow = (float*)pBuf.data +blkrad*offset;

					//TODO: implement using AlignedBufferMP
					for (int i=0/*, row=top*/; i<TS; i++/*, row++*/) {
						int row = top + i;
						int rr = row;
						if (row<0) {
							rr = MIN(-row,height-1);
						} else if (row>=height) {
							rr = MAX(0,2*height-2-row);
						}

						for (int j=0; j<labdn->W; j++) {
							datarow[j] = (Lin[rr][j]-Lwavdn[rr][j]);
						}

						for (int j=-blkrad*offset; j<0; j++) {
							datarow[j] = datarow[MIN(-j,width-1)];
						}
						for (int j=width; j<width+TS+blkrad*offset; j++) {
							datarow[j] = datarow[MAX(0,2*width-2-j)];
						}//now we have a padded data row

						//now fill this row of the blocks with Lab high pass data
						//OMP here does not add speed, better handled on the outside loop
						for (int hblk=0; hblk<numblox_W; hblk++) {
							int left = (hblk-blkrad)*offset;
							int indx = (hblk)*TS;//index of block in malloc

							for (int j=0; j<TS; j++) {
								Lblox[(indx + i)*TS+j]  = tilemask_in[i][j]*datarow[left+j];// luma data
								if (top+i>=0 && top+i<height && left+j>=0 && left+j<width) {
									totwt[top+i][left+j] += tilemask_in[i][j]*tilemask_out[i][j];
								}
							}
						}
					}//end of filling block row

					//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					//fftwf_print_plan (plan_forward_blox);
					if(numblox_W == max_numblox_W)
                        fftwf_execute_r2r(plan_forward_blox[0],Lblox,fLblox);		// DCT an entire row of tiles
                    else
                        fftwf_execute_r2r(plan_forward_blox[1],Lblox,fLblox);		// DCT an entire row of tiles
					//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					// now process the vblk row of blocks for noise reduction
					for (int hblk=0; hblk<numblox_W; hblk++) {

						RGBtile_denoise (fLblox, hblk, noisevar_Ldetail, nbrwt, blurbuffer );

					}//end of horizontal block loop

					//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					//now perform inverse FT of an entire row of blocks
					if(numblox_W == max_numblox_W)
                        fftwf_execute_r2r(plan_backward_blox[0],fLblox,Lblox);	//for DCT
                    else
                        fftwf_execute_r2r(plan_backward_blox[1],fLblox,Lblox);	//for DCT

					int topproc = (vblk-blkrad)*offset;

					//add row of blocks to output image tile
					RGBoutput_tile_row (Lblox, Ldetail, tilemask_out, height, width, topproc );

					//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				}//end of vertical block loop
				//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

                }
				//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				for (int i=0; i<height; i++) {
					for (int j=0; j<width; j++) {
						//may want to include masking threshold for large hipass data to preserve edges/detail
						float hpdn = Ldetail[i][j]/totwt[i][j];//note that labdn initially stores the denoised hipass data

						labdn->L[i][j] = Lwavdn[i][j] + hpdn;

					}
				}

				//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				// transform denoised "Lab" to output RGB

				//calculate mask for feathering output tile overlaps
				float * Vmask = new float [height+1];
				float * Hmask = new float [width+1];

				for (int i=0; i<height; i++) {
					Vmask[i] = 1;
				}
				for (int j=0; j<width; j++) {
					Hmask[j] = 1;
				}
				for (int i=0; i<overlap; i++) {
					float mask = SQR(sin((M_PI*i)/(2*overlap)));
					if (tiletop>0) Vmask[i] = mask;
					if (tilebottom<imheight) Vmask[height-i] = mask;
					if (tileleft>0) Hmask[i] = mask;
					if (tileright<imwidth) Hmask[width-i] = mask;
				}

				//convert back to RGB and write to destination array
				if (isRAW) {
				if(!perf) {//Lab mode
					for (int i=tiletop; i<tilebottom; i++){
						int i1 = i-tiletop;
						float X,Y,Z,L,a,b;

						for (int j=tileleft; j<tileright; j++) {
							int j1=j-tileleft;
							//modification Jacques feb 2013
							//true conversion Lab==>xyz
							L = labdn->L[i1][j1];
							a = labdn->a[i1][j1];
							b = labdn->b[i1][j1];
							//convert XYZ
							Color::Lab2XYZ(L, a, b, X, Y, Z);
							//apply inverse gamma noise
							float r_,g_,b_;
							Color::xyz2rgb(X,Y,Z,r_,g_,b_,wip);
							//inverse gamma standard (slider)
							r_ = r_<32768.0f ? igamcurve[r_] : (Color::gamman((float)r_/32768.0f, igam) * 65535.0f);
							g_ = g_<32768.0f ? igamcurve[g_] : (Color::gamman((float)g_/32768.0f, igam) * 65535.0f);
							b_ = b_<32768.0f ? igamcurve[b_] : (Color::gamman((float)b_/32768.0f, igam) * 65535.0f);
							//readapt arbitrary gamma (inverse from beginning)
							if (gamlab == 0) {
							r_ = Color::gammatab_26_11[r_];
							g_ = Color::gammatab_26_11[g_];
							b_ = Color::gammatab_26_11[b_];
							}
							else if (gamlab == 1) {
							r_ = Color::gammatab_4[r_];
							g_ = Color::gammatab_4[g_];
							b_ = Color::gammatab_4[b_];
							}
							else if (gamlab == 2) {
							r_ = Color::gammatab_55[r_];
							g_ = Color::gammatab_55[g_];
							b_ = Color::gammatab_55[b_];
							}
							float factor = Vmask[i1]*Hmask[j1]/gain;

							dsttmp->r(i,j) += factor*r_;
							dsttmp->g(i,j) += factor*g_;
							dsttmp->b(i,j) += factor*b_;

						}
					}
					}
					else {//RGB mode
					for (int i=tiletop; i<tilebottom; i++){
						int i1 = i-tiletop;
						float X,Y,Z;
						for (int j=tileleft; j<tileright; j++) {
							int j1=j-tileleft;

							Y = labdn->L[i1][j1];
							X = (labdn->a[i1][j1]) + Y;
							Z = Y - (labdn->b[i1][j1]);

							X = X<32768.0f ? igamcurve[X] : (Color::gamma((float)X/32768.0f, igam, igamthresh, igamslope, 1.0, 0.0) * 65535.0f);
							Y = Y<32768.0f ? igamcurve[Y] : (Color::gamma((float)Y/32768.0f, igam, igamthresh, igamslope, 1.0, 0.0) * 65535.0f);
							Z = Z<32768.0f ? igamcurve[Z] : (Color::gamma((float)Z/32768.0f, igam, igamthresh, igamslope, 1.0, 0.0) * 65535.0f);

							float factor = Vmask[i1]*Hmask[j1]/gain;

							dsttmp->r(i,j) += factor*X;
							dsttmp->g(i,j) += factor*Y;
							dsttmp->b(i,j) += factor*Z;

						}
					}

					}
				} else {
					for (int i=tiletop; i<tilebottom; i++){
						int i1 = i-tiletop;
						float X,Y,Z,L,a,b;
						for (int j=tileleft; j<tileright; j++) {
							int j1=j-tileleft;
							//modification Jacques feb 2013
							L = labdn->L[i1][j1];
							a = labdn->a[i1][j1];
							b = labdn->b[i1][j1];
							Color::Lab2XYZ(L, a, b, X, Y, Z);

							float factor = Vmask[i1]*Hmask[j1];
							float r_,g_,b_;
							Color::xyz2rgb(X,Y,Z,r_,g_,b_,wip);
							//gamma slider is different from Raw
							r_ = r_<32768.0f ? igamcurve[r_] : (Color::gamman((float)r_/32768.0f, igam) * 65535.0f);
							g_ = g_<32768.0f ? igamcurve[g_] : (Color::gamman((float)g_/32768.0f, igam) * 65535.0f);
							b_ = b_<32768.0f ? igamcurve[b_] : (Color::gamman((float)b_/32768.0f, igam) * 65535.0f);

							dsttmp->r(i,j) += factor*r_;
							dsttmp->g(i,j) += factor*g_;
							dsttmp->b(i,j) += factor*b_;

						}
					}
				}

				//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				delete labdn;
			//	delete noiseh;

				delete[] Vmask;
				delete[] Hmask;



			}//end of tile row
		}//end of tile loop
#ifdef _OPENMP
#pragma omp critical
#endif
{
		fftwf_free ( Lblox);
		fftwf_free ( fLblox);
}
	delete [] nbrwt;
	delete [] blurbuffer;
        }
		//copy denoised image to output
		memcpy (dst->data, dsttmp->data, 3*dst->width*dst->height*sizeof(float));

		if (!isRAW) {//restore original image gamma
#ifdef _OPENMP
#pragma omp parallel for
#endif
			for (int i=0; i<3*dst->width*dst->height; i++) {
				dst->data[i] = Color::gammatab_srgb[ dst->data[i] ];
			}
		}

		delete dsttmp;
    // destroy the plans
	fftwf_destroy_plan( plan_forward_blox[0] );
	fftwf_destroy_plan( plan_backward_blox[0] );
	fftwf_destroy_plan( plan_forward_blox[1] );
	fftwf_destroy_plan( plan_backward_blox[1] );
	fftwf_cleanup();
	}


	//median 3x3 in complement on RGB
	if(dnparams.methodmed=="RGB" && dnparams.median) {
	//printf("RGB den\n");
		int wid=dst->width, hei=dst->height;
		float** tm;
		tm = new float*[hei];
		for (int i=0; i<hei; i++)
			tm[i] = new float[wid];
			
		Imagefloat *source;
		if (dnparams.luma==0 && dnparams.chroma==0)
			source = dst;
		else
			source = src;

		int methmed=0;
		int border = 1;
		if(dnparams.rgbmethod=="soft")
			methmed=0;
		else if(dnparams.rgbmethod=="33")
			methmed=1;
		else if(dnparams.rgbmethod=="55") {
			methmed = 3;
			border = 2;
		}
		else if(dnparams.rgbmethod=="55soft") {
			methmed = 2;
			border = 2;
		}

for(int iteration=1;iteration<=dnparams.passes;iteration++){

#pragma omp parallel
{
		if(methmed < 2) {
#pragma omp for	
			for (int i=1; i<hei-1; i++) {
				float pp[9],temp;
				if(methmed == 0)
					for (int j=1; j<wid-1; j++) {
						med2(source->r(i,j),source->r(i-1,j),source->r(i+1,j),source->r(i,j+1),source->r(i,j-1),tm[i][j]);//3x3 soft
						}
				else
					for (int j=1; j<wid-1; j++) {
						med3(source->r(i,j),source->r(i-1,j),source->r(i+1,j),source->r(i,j+1),source->r(i,j-1),source->r(i-1,j-1),source->r(i-1,j+1),source->r(i+1,j-1),source->r(i+1,j+1),tm[i][j]);//3x3
					}
			}
		} else {
#pragma omp for	
			for (int i=2; i<hei-2; i++) {
				float pp[25],temp;
				if(methmed == 3)			
					for (int j=2; j<wid-2; j++) {
						med5(source->r(i,j),source->r(i-1,j),source->r(i+1,j),source->r(i,j+1),source->r(i,j-1),source->r(i-1,j-1),source->r(i-1,j+1),source->r(i+1,j-1),source->r(i+1,j+1),
						source->r(i-2,j),source->r(i+2,j),source->r(i,j+2),source->r(i,j-2),source->r(i-2,j-2),source->r(i-2,j+2),source->r(i+2,j-2),source->r(i+2,j+2),	
						source->r(i-2,j+1),source->r(i+2,j+1),source->r(i-1,j+2),source->r(i-1,j-2),source->r(i-2,j-1),source->r(i+2,j-1),source->r(i+1,j+2),source->r(i+1,j-2),	
						tm[i][j]);//5x5
					}
				else
					for (int j=2; j<wid-2; j++) {
						pp[0]=source->r(i,j);pp[1]=source->r(i-1,j); pp[2]=source->r(i+1,j);pp[3]=source->r(i,j+1);pp[4]=source->r(i,j-1);pp[5]=source->r(i-1,j-1);pp[6]=source->r(i-1,j+1);
						pp[7]=source->r(i+1,j-1);pp[8]=source->r(i+1,j+1);pp[9]=source->r(i+2,j);pp[10]=source->r(i-2,j);pp[11]=source->r(i,j+2);pp[12]=source->r(i,j-2);
						fq_sort2(pp,13);
						tm[i][j]=pp[6];//5x5 soft
					}
				}
		}
#ifdef _OPENMP
#pragma omp for nowait
#endif
		for(int i = border; i < hei-border; i++ ) {
			for(int j = border; j < wid-border; j++) {
				dst->r(i,j) = tm[i][j];
			}
		}

		if(methmed < 2) {
#pragma omp for		
			for (int i=1; i<hei-1; i++) {
				float pp[9],temp;
				if(methmed == 0)
					for (int j=1; j<wid-1; j++) {
						med2(source->b(i,j),source->b(i-1,j),source->b(i+1,j),source->b(i,j+1),source->b(i,j-1),tm[i][j]);
					}
				else 
					for (int j=1; j<wid-1; j++) {
						med3(source->b(i,j),source->b(i-1,j),source->b(i+1,j),source->b(i,j+1),source->b(i,j-1),source->b(i-1,j-1),source->b(i-1,j+1),source->b(i+1,j-1),source->b(i+1,j+1),tm[i][j]);
					}
			}
		} else {
#pragma omp for	
			for (int i=2; i<hei-2; i++) {
				float pp[25],temp;
				if(methmed == 3)							
					for (int j=2; j<wid-2; j++) {
						med5(source->b(i,j),source->b(i-1,j),source->b(i+1,j),source->b(i,j+1),source->b(i,j-1),source->b(i-1,j-1),source->b(i-1,j+1),source->b(i+1,j-1),source->b(i+1,j+1),
						source->b(i-2,j),source->b(i+2,j),source->b(i,j+2),source->b(i,j-2),source->b(i-2,j-2),source->b(i-2,j+2),source->b(i+2,j-2),source->b(i+2,j+2),	
						source->b(i-2,j+1),source->b(i+2,j+1),source->b(i-1,j+2),source->b(i-1,j-2),source->b(i-2,j-1),source->b(i+2,j-1),source->b(i+1,j+2),source->b(i+1,j-2),	
						tm[i][j]);//5x5
					}
				else
					for (int j=2; j<wid-2; j++) {
						pp[0]=source->b(i,j);pp[1]=source->b(i-1,j); pp[2]=source->b(i+1,j);pp[3]=source->b(i,j+1);pp[4]=source->b(i,j-1);pp[5]=source->b(i-1,j-1);pp[6]=source->b(i-1,j+1);
						pp[7]=source->b(i+1,j-1);pp[8]=source->b(i+1,j+1);pp[9]=source->b(i+2,j);pp[10]=source->b(i-2,j);pp[11]=source->b(i,j+2);pp[12]=source->b(i,j-2);
						fq_sort2(pp,13);
						tm[i][j]=pp[6];//5x5 soft
					}
			}
		}

#ifdef _OPENMP
#pragma omp for nowait
#endif
		for(int i = border; i < hei-border; i++ ) {
			for(int j = border; j < wid-border; j++) {
				dst->b(i,j) = tm[i][j];	
			}
		}


		if(methmed < 2) {
#pragma omp for		
			for (int i=1; i<hei-1; i++) {
				float pp[9],temp;
				if(methmed == 0)
					for (int j=1; j<wid-1; j++) {
						med2(source->g(i,j),source->g(i-1,j),source->g(i+1,j),source->g(i,j+1),source->g(i,j-1),tm[i][j]);
					}
				else
					for (int j=1; j<wid-1; j++) {
						med3(source->g(i,j),source->g(i-1,j),source->g(i+1,j),source->g(i,j+1),source->g(i,j-1),source->g(i-1,j-1),source->g(i-1,j+1),source->g(i+1,j-1),source->g(i+1,j+1),tm[i][j]);
					}
			}
		} else {
#pragma omp for	
			for (int i=2; i<hei-2; i++) {
				float pp[25],temp;
				if(methmed == 3)											
					for (int j=2; j<wid-2; j++) {
						med5(source->g(i,j),source->g(i-1,j),source->g(i+1,j),source->g(i,j+1),source->g(i,j-1),source->g(i-1,j-1),source->g(i-1,j+1),source->g(i+1,j-1),source->g(i+1,j+1),
						source->g(i-2,j),source->g(i+2,j),source->g(i,j+2),source->g(i,j-2),source->g(i-2,j-2),source->g(i-2,j+2),source->g(i+2,j-2),source->g(i+2,j+2),	
						source->g(i-2,j+1),source->g(i+2,j+1),source->g(i-1,j+2),source->g(i-1,j-2),source->g(i-2,j-1),source->g(i+2,j-1),source->g(i+1,j+2),source->g(i+1,j-2),	
						tm[i][j]);//5x5
					}
				else
					for (int j=2; j<wid-2; j++) {
						pp[0]=source->g(i,j);pp[1]=source->g(i-1,j); pp[2]=source->g(i+1,j);pp[3]=source->g(i,j+1);pp[4]=source->g(i,j-1);pp[5]=source->g(i-1,j-1);pp[6]=source->g(i-1,j+1);
						pp[7]=source->g(i+1,j-1);pp[8]=source->g(i+1,j+1);pp[9]=source->g(i+2,j);pp[10]=source->g(i-2,j);pp[11]=source->g(i,j+2);pp[12]=source->g(i,j-2);
						fq_sort2(pp,13);
						tm[i][j]=pp[6];//5x5 soft
					}
			}
		}

#ifdef _OPENMP
#pragma omp for
#endif
		for(int i = border; i < hei-border; i++ ) {
			for(int j = border; j < wid-border; j++) {
				dst->g(i,j) = tm[i][j];
			}
		}
}
}		
                for (int i=0; i<hei; i++)
                    delete [] tm[i];
                delete [] tm;
			
}		
		//end median

//#ifdef _DEBUG
	if (settings->verbose) {
		t2e.set();
		printf("Denoise performed in %d usec:\n", t2e.etime(t1e));
	}
//#endif

	}//end of main RGB_denoise


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


SSEFUNCTION	void ImProcFunctions::RGBtile_denoise (float * fLblox, int hblproc, float noisevar_Ldetail, float * nbrwt, float * blurbuffer )	//for DCT
	{
		int blkstart = hblproc*TS*TS;

		boxabsblur(fLblox+blkstart, nbrwt, 3, 3, TS, TS, blurbuffer);//blur neighbor weights for more robust estimation	//for DCT
		
#ifdef __SSE2__
		__m128	tempv;
		__m128	noisevar_Ldetailv = _mm_set1_ps( noisevar_Ldetail );
		__m128	onev = _mm_set1_ps( 1.0f );
		for (int n=0; n<TS*TS; n+=4) {		//for DCT
			tempv  = onev - xexpf( -SQRV( LVFU(nbrwt[n]))/noisevar_Ldetailv);
			_mm_storeu_ps( &fLblox[blkstart+n], LVFU(fLblox[blkstart+n]) * tempv );
		}//output neighbor averaged result

#else
#pragma omp parallel for
		for (int n=0; n<TS*TS; n++) {		//for DCT
			fLblox[blkstart+n] *= (1-xexpf(-SQR(nbrwt[n])/noisevar_Ldetail));
		}//output neighbor averaged result
#endif

		//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		//printf("vblk=%d  hlk=%d  wsqave=%f   ||   ",vblproc,hblproc,wsqave);

	}//end of function tile_denoise


	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	void ImProcFunctions::RGBoutput_tile_row (float *bloxrow_L, float ** Ldetail, float ** tilemask_out, int height, int width, int top )
	{
		const int numblox_W = ceil(((float)(width))/(offset));
		const float DCTnorm = 1.0f/(4*TS*TS); //for DCT

		int imin = MAX(0,-top);
		int bottom = MIN( top+TS,height);
		int imax = bottom - top;

		//add row of tiles to output image
		for (int hblk=0; hblk < numblox_W; hblk++) {
			int left = (hblk-blkrad)*offset;
			int right  = MIN(left+TS, width);
			int jmin = MAX(0,-left);
			int jmax = right - left;
			int indx = hblk*TS;

			for (int i=imin; i<imax; i++)
				for (int j=jmin; j<jmax; j++) {	// this loop gets auto vectorized by gcc
					Ldetail[top+i][left+j] += tilemask_out[i][j]*bloxrow_L[(indx + i)*TS+j]*DCTnorm; //for DCT

				}
		}
	}

#undef TS
#undef fTS
#undef offset
#undef epsilon


	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	float ImProcFunctions::MadMax(float * DataList, int & max, int datalen) {

		//computes Median Absolute Deviation and Maximum of DataList
		//DataList values should mostly have abs val < 65535

		int * histo = new int[65536];
		//memset(histo, 0, 65536*sizeof(histo));
		for (int i=0; i<65536; i++) histo[i]=0;

		//calculate histogram of absolute values of HH wavelet coeffs
		for (int i=0; i<datalen; i++) {
			histo[MAX(0,MIN(65535,abs((int)DataList[i])))]++;
		}

		//find median of histogram
		int median=0, count=0;
		while (count<datalen/2) {
			count += histo[median];
			median++;
		}

		//find max of histogram
		max=65535;
		while (histo[max]==0) {
			max--;
		}

		int count_ = count - histo[median-1];

		delete[] histo;

		// interpolate
		return (( (median-1) + (datalen/2-count_)/((float)(count-count_)) )/0.6745);

	}

	float ImProcFunctions::Mad( float *  RESTRICT DataList, int datalen, int * RESTRICT histo) {

		//computes Median Absolute Deviation
		//DataList values should mostly have abs val < 65535

		for (int i=0; i<65536; i++)
			histo[i]=0;

		//calculate histogram of absolute values of HH wavelet coeffs
		for (int i=0; i<datalen; i++) {
			histo[abs((int)DataList[i])&0xffff]++;
		}

		//find median of histogram
		int median=0, count=0;
		while (count<datalen/2) {
			count += histo[median];
			median++;
		}

		int count_ = count - histo[median-1];

		// interpolate
		return (( (median-1) + (datalen/2-count_)/((float)(count-count_)) )/0.6745);
	}


	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


SSEFUNCTION	void ImProcFunctions::WaveletDenoiseAll_BiShrink(wavelet_decomposition &WaveletCoeffs_L, wavelet_decomposition &WaveletCoeffs_a,
													 wavelet_decomposition &WaveletCoeffs_b, float noisevar_L, float noisevar_abr, float noisevar_abb, LabImage * noi)
	{
		int maxlvl = WaveletCoeffs_L.maxlevel();
		const float eps = 0.01f;
//		int max;
//		float parfrac = 0.05;

		float madL[8][3], mada[8][3], madb[8][3];


		int * madHisto = new int[65536];
		for (int lvl=0; lvl<maxlvl; lvl++) {
			// compute median absolute deviation (MAD) of detail coefficients as robust noise estimator

			int Wlvl_L = WaveletCoeffs_L.level_W(lvl);
			int Hlvl_L = WaveletCoeffs_L.level_H(lvl);

			int Wlvl_ab = WaveletCoeffs_a.level_W(lvl);
			int Hlvl_ab = WaveletCoeffs_a.level_H(lvl);

			float ** WavCoeffs_L = WaveletCoeffs_L.level_coeffs(lvl);
			float ** WavCoeffs_a = WaveletCoeffs_a.level_coeffs(lvl);
			float ** WavCoeffs_b = WaveletCoeffs_b.level_coeffs(lvl);

			for (int dir=1; dir<4; dir++) {
				madL[lvl][dir-1] = SQR(Mad(WavCoeffs_L[dir], Wlvl_L*Hlvl_L, madHisto));
				mada[lvl][dir-1] = SQR(Mad(WavCoeffs_a[dir], Wlvl_ab*Hlvl_ab, madHisto));
				madb[lvl][dir-1] = SQR(Mad(WavCoeffs_b[dir], Wlvl_ab*Hlvl_ab, madHisto));
			}
		}
		delete [] madHisto;

		for (int lvl=maxlvl-1; lvl>=0; lvl--) {//for levels less than max, use level diff to make edge mask
			//for (int lvl=0; lvl<maxlvl; lvl++) {

			int Wlvl_L = WaveletCoeffs_L.level_W(lvl);
			int Hlvl_L = WaveletCoeffs_L.level_H(lvl);

			int Wlvl_ab = WaveletCoeffs_a.level_W(lvl);
			int Hlvl_ab = WaveletCoeffs_a.level_H(lvl);

			int skip_L = WaveletCoeffs_L.level_stride(lvl);
			int skip_ab = WaveletCoeffs_a.level_stride(lvl);
//			float skip_h;
			float ** WavCoeffs_L = WaveletCoeffs_L.level_coeffs(lvl);
			float ** WavCoeffs_a = WaveletCoeffs_a.level_coeffs(lvl);
			float ** WavCoeffs_b = WaveletCoeffs_b.level_coeffs(lvl);

			if (lvl==maxlvl-1) {
				ShrinkAll(WavCoeffs_L, WavCoeffs_a, WavCoeffs_b, lvl, Wlvl_L, Hlvl_L, Wlvl_ab, Hlvl_ab,
						  skip_L, skip_ab, noisevar_L, noisevar_abr, noisevar_abb, noi, mada[lvl], madb[lvl], madL[lvl], true);

			} else {

//				float ** WavPars_L = WaveletCoeffs_L.level_coeffs(lvl+1);
				//float ** WavPars_a = WaveletCoeffs_a.level_coeffs(lvl+1);
				//float ** WavPars_b = WaveletCoeffs_b.level_coeffs(lvl+1);

				//simple wavelet shrinkage
				float * sfave = new float[Wlvl_L*Hlvl_L];
				float * WavCoeffsLtemp = new float[Hlvl_ab*Wlvl_ab];

//				array2D<float> edge(Wlvl_L,Hlvl_L);

				//printf("\n level=%d  \n",lvl);

				for (int dir=1; dir<4; dir++) {
					float mad_L = madL[lvl][dir-1];
					float mad_a = noisevar_abr*mada[lvl][dir-1];
					float mad_b = noisevar_abb*madb[lvl][dir-1];


					//float mad_Lpar = madL[lvl+1][dir-1];
					//float mad_apar = mada[lvl+1][dir-1];
					//float mad_bpar = mada[lvl+1][dir-1];

					//float skip_ab_ratio = WaveletCoeffs_a.level_stride(lvl+1)/skip_ab;
//					float skip_L_ratio =  WaveletCoeffs_L.level_stride(lvl+1)/skip_L;

					if (noisevar_abr>0.01f  || noisevar_abb>0.01f) {
						for(int i=0;i<Hlvl_ab;i++)
							for(int j=0;j<Wlvl_ab;j++)
								WavCoeffsLtemp[i*Wlvl_ab+j] = WavCoeffs_L[dir][((i*skip_L)/skip_ab)*Wlvl_L + ((j*skip_L)/skip_ab)];
#ifdef __SSE2__
						int j;
						__m128 onev = _mm_set1_ps(1.f);
						__m128 mad_Lm9v = _mm_set1_ps(mad_L * 9.f);
						__m128 mad_av = _mm_set1_ps(mad_a);
						__m128 mad_bv = _mm_set1_ps(mad_b);
						__m128 epsv = _mm_set1_ps(eps);
						__m128 mag_Lv, mag_av, mag_bv;
						__m128 tempav, tempbv;
						for (int i=0; i<Hlvl_ab; i++) {
							int coeffloc_ab = i*Wlvl_ab;
							for (j=0; j<Wlvl_ab-3; j+=4, coeffloc_ab+=4) {
								tempav = LVFU(WavCoeffs_a[dir][coeffloc_ab]);
								tempbv = LVFU(WavCoeffs_b[dir][coeffloc_ab]);
								mag_Lv = LVFU(WavCoeffsLtemp[coeffloc_ab]);
								mag_av = SQRV(tempav)+epsv;
								mag_bv = SQRV(tempbv)+epsv;
								mag_Lv = SQRV(mag_Lv) + epsv;
								_mm_storeu_ps(&WavCoeffs_a[dir][coeffloc_ab], tempav * SQRV((onev-xexpf(-(mag_av/mad_av)-(mag_Lv/mad_Lm9v)))));
								_mm_storeu_ps(&WavCoeffs_b[dir][coeffloc_ab], tempbv * SQRV((onev-xexpf(-(mag_bv/mad_bv)-(mag_Lv/mad_Lm9v)))));
							}
							for (; j<Wlvl_ab; j++,coeffloc_ab++) {
								float mag_L = SQR(WavCoeffsLtemp[coeffloc_ab ])+eps;
								float mag_a = SQR(WavCoeffs_a[dir][coeffloc_ab])+eps;
								float mag_b = SQR(WavCoeffs_b[dir][coeffloc_ab])+eps;
								WavCoeffs_a[dir][coeffloc_ab] *= SQR(1.f-xexpf(-(mag_a/mad_a)-(mag_L/(9.f*mad_L)))/*satfactor_a*/);
								WavCoeffs_b[dir][coeffloc_ab] *= SQR(1.f-xexpf(-(mag_b/mad_b)-(mag_L/(9.f*mad_L)))/*satfactor_b*/);
							}
						}//now chrominance coefficients are denoised
#else
						//printf("  dir=%d  mad_L=%f		mad_a=%f		mad_b=%f	\n",dir,sqrt(mad_L),sqrt(mad_a),sqrt(mad_b));

						for (int i=0; i<Hlvl_ab; i++) {
							for (int j=0; j<Wlvl_ab; j++) {

								int coeffloc_ab = i*Wlvl_ab+j;
								//int coeffloc_abpar = (MAX(0,i-skip_ab)*Wlvl_ab+MAX(0,j-skip_ab))/skip_ab_ratio;

//								int coeffloc_L	= ((i*skip_L)/skip_ab)*Wlvl_L + ((j*skip_L)/skip_ab);

								float mag_L = SQR(WavCoeffsLtemp[coeffloc_ab ])+eps;
								float mag_a = SQR(WavCoeffs_a[dir][coeffloc_ab])+eps;
								float mag_b = SQR(WavCoeffs_b[dir][coeffloc_ab])+eps;

								//float edgefactor = 1-exp(-mag_L/(9*mad_L));// * exp(-mag_a/(4*mad_a)) * exp(-mag_b/(4*mad_b));

								//float coeff_a = sqrt(SQR(WavCoeffs_a[dir][coeffloc_ab])/mad_a+SQR(parfrac*WavPars_a[dir][coeffloc_abpar])/mad_apar);
								//float coeff_b = sqrt(SQR(WavCoeffs_b[dir][coeffloc_ab])/mad_b+SQR(parfrac*WavPars_b[dir][coeffloc_abpar])/mad_bpar);

								// 'firm' threshold of chroma coefficients
								//WavCoeffs_a[dir][coeffloc_ab] *= edgefactor*(coeff_a>2 ? 1 : (coeff_a<1 ? 0 : (coeff_a - 1)));
								//WavCoeffs_b[dir][coeffloc_ab] *= edgefactor*(coeff_b>2 ? 1 : (coeff_b<1 ? 0 : (coeff_b - 1)));

								//float satfactor_a = mad_a/(mad_a+0.5*SQR(WavCoeffs_a[0][coeffloc_ab]));
								//float satfactor_b = mad_b/(mad_b+0.5*SQR(WavCoeffs_b[0][coeffloc_ab]));

								WavCoeffs_a[dir][coeffloc_ab] *= SQR(1.f-xexpf(-(mag_a/mad_a)-(mag_L/(9.f*mad_L)))/*satfactor_a*/);
								WavCoeffs_b[dir][coeffloc_ab] *= SQR(1.f-xexpf(-(mag_b/mad_b)-(mag_L/(9.f*mad_L)))/*satfactor_b*/);

							}
						}//now chrominance coefficients are denoised
#endif
					}

					if (noisevar_L>0.01f) {
						mad_L *= noisevar_L*5.f/(lvl+1);
#ifdef __SSE2__
						int j;
						__m128 mad_Lv = _mm_set1_ps(mad_L);
						__m128 mad_Lm9v = _mm_set1_ps(mad_L * 9.f);
						__m128 epsv = _mm_set1_ps(eps);
						__m128 mag_Lv;
						for (int i=0; i<Hlvl_L; i++) {
							int coeffloc_L = i*Wlvl_L;
							for (j=0; j<Wlvl_L-3; j+=4,coeffloc_L+=4) {
								mag_Lv = SQRV(LVFU(WavCoeffs_L[dir][coeffloc_L]));
								_mm_storeu_ps(&sfave[coeffloc_L], mag_Lv / ( mag_Lv + mad_Lv * xexpf(-mag_Lv/mad_Lm9v )+ epsv));
							}
							for (; j<Wlvl_L; j++, coeffloc_L++) {
								float mag_L = SQR(WavCoeffs_L[dir][coeffloc_L]);
								sfave[coeffloc_L] = mag_L/(mag_L+mad_L*xexpf(-mag_L/(9.f*mad_L))+eps);
							}
						}
#else
						for (int i=0; i<Hlvl_L; i++)
							for (int j=0; j<Wlvl_L; j++) {

								int coeffloc_L = i*Wlvl_L+j;
//								int coeffloc_Lpar = (MAX(0,i-skip_L)*Wlvl_L+MAX(0,j-skip_L))/skip_L_ratio;

								float mag_L = SQR(WavCoeffs_L[dir][coeffloc_L]);
								//float mag_Lpar = SQR(parfrac*WavPars_L[dir][coeffloc_Lpar]);
								//float sf_L = SQR(1-expf(-(mag_L/mad_L)-(mag_Lpar/mad_L)));
								float sf_L = mag_L/(mag_L+mad_L*xexpf(-mag_L/(9.f*mad_L))+eps);

								sfave[coeffloc_L] = sf_L;

								//edge[i][j] = (WavCoeffs_L[dir][coeffloc_L] - WavPars_L[dir][coeffloc_Lpar]);
							}
#endif
						//blur edge measure
						//gaussHorizontal<float> (edge, edge, buffer, Wlvl_L, Hlvl_L, 1<<(lvl+1), false /*multiThread*/);
						//gaussVertical<float>   (edge, edge, buffer, Wlvl_L, Hlvl_L, 1<<(lvl+1), false);

						boxblur(sfave, sfave, lvl+2, lvl+2, Wlvl_L, Hlvl_L);//increase smoothness by locally averaging shrinkage

#ifdef __SSE2__
						__m128 tempLv;
						__m128 tempL2v;
						__m128 sf_Lv;

						for (int i=0; i<Hlvl_L; i++) {
							int coeffloc_L = i*Wlvl_L;
							for (j=0; j<Wlvl_L-3; j+=4,coeffloc_L+=4) {
								tempLv = LVFU(WavCoeffs_L[dir][coeffloc_L]);
								mag_Lv = SQRV(tempLv);
								tempL2v = LVFU(sfave[coeffloc_L]);
								sf_Lv = mag_Lv/(mag_Lv + mad_Lv*xexpf(-mag_Lv/mad_Lm9v)+epsv);
								_mm_storeu_ps(&WavCoeffs_L[dir][coeffloc_L], tempLv * (SQRV(tempL2v)+SQRV(sf_Lv))/(tempL2v+sf_Lv+epsv));
								//use smoothed shrinkage unless local shrinkage is much less

							}//now luminance coeffs are denoised
							for (; j<Wlvl_L; j++,coeffloc_L++) {
								float mag_L = SQR(WavCoeffs_L[dir][coeffloc_L]);
								float sf_L = mag_L/(mag_L + mad_L*xexpf(-mag_L/(9.f*mad_L))+eps);

								//use smoothed shrinkage unless local shrinkage is much less
								WavCoeffs_L[dir][coeffloc_L] *= (SQR(sfave[coeffloc_L])+SQR(sf_L))/(sfave[coeffloc_L]+sf_L+eps);

							}//now luminance coeffs are denoised
						}

#else
						for (int i=0; i<Hlvl_L; i++)
							for (int j=0; j<Wlvl_L; j++) {

								int coeffloc_L = i*Wlvl_L+j;

								float mag_L = SQR(WavCoeffs_L[dir][coeffloc_L]);
								//float sf_L = SQR(1-expf(-(mag_L/mad_L)-(mag_Lpar/mad_L)));

								float edgefactor = 1;//expf(-SQR(edge[i][j])/mad_L);

								float sf_L = mag_L/(mag_L + edgefactor*mad_L*xexpf(-mag_L/(9.f*mad_L))+eps);

								//use smoothed shrinkage unless local shrinkage is much less
								WavCoeffs_L[dir][coeffloc_L] *= (SQR(edgefactor*sfave[coeffloc_L])+SQR(sf_L))/(edgefactor*sfave[coeffloc_L]+sf_L+eps);

							}//now luminance coeffs are denoised
#endif

					}
				}
				delete[] WavCoeffsLtemp;
				delete[] sfave;

			}
		}
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	void ImProcFunctions::WaveletDenoiseAll(wavelet_decomposition &WaveletCoeffs_L, wavelet_decomposition &WaveletCoeffs_a,
											wavelet_decomposition &WaveletCoeffs_b, float noisevar_L, float noisevar_abr, float noisevar_abb, LabImage * noi)//mod JD

	{
		int maxlvl = WaveletCoeffs_L.maxlevel();
//       printf("maxlevel = %d\n",maxlvl);
//omp_set_nested(true);
		for (int lvl=0; lvl<maxlvl; lvl++) {

			int Wlvl_L = WaveletCoeffs_L.level_W(lvl);
			int Hlvl_L = WaveletCoeffs_L.level_H(lvl);

			int Wlvl_ab = WaveletCoeffs_a.level_W(lvl);
			int Hlvl_ab = WaveletCoeffs_a.level_H(lvl);


			int skip_L = WaveletCoeffs_L.level_stride(lvl);
			int skip_ab = WaveletCoeffs_a.level_stride(lvl);

			float ** WavCoeffs_L = WaveletCoeffs_L.level_coeffs(lvl);
			float ** WavCoeffs_a = WaveletCoeffs_a.level_coeffs(lvl);
			float ** WavCoeffs_b = WaveletCoeffs_b.level_coeffs(lvl);

       //     printf("Hab : %d\n", Hlvl_ab);
        //    printf("Wab : %d\n", Wlvl_ab);
			ShrinkAll(WavCoeffs_L, WavCoeffs_a, WavCoeffs_b, lvl, Wlvl_L, Hlvl_L, Wlvl_ab, Hlvl_ab,
					  skip_L, skip_ab, noisevar_L, noisevar_abr, noisevar_abb, noi);

		}
//omp_set_nested(false);
	}

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SSEFUNCTION	void ImProcFunctions::ShrinkAll(float ** WavCoeffs_L, float ** WavCoeffs_a, float ** WavCoeffs_b, int level,
									int W_L, int H_L, int W_ab, int H_ab,int skip_L, int skip_ab, float noisevar_L, float noisevar_abr, float noisevar_abb, LabImage * noi,
									float * madaa, float * madab, float * madaL, bool madCalculated)

									{
		//simple wavelet shrinkage
		const float eps = 0.01f;
		float * sfave = new float[W_L*H_L];
		float * sfavea = new float[W_L*H_L];
		float * sfaveb = new float[W_L*H_L];
		float * WavCoeffsLtemp = new float[H_ab*W_ab];

//		int max;

		//printf("\n level=%d  \n",level);

		for (int dir=1; dir<4; dir++) {
			float mada, madb, madL;
			if(madCalculated) {
				mada = madaa[dir-1];
				madb = madab[dir-1];
				madL = madaL[dir-1] ;
			} else {
				int * madHisto = new int[65536];
				mada = SQR(Mad(WavCoeffs_a[dir], W_ab*H_ab, madHisto));
				madb = SQR(Mad(WavCoeffs_b[dir], W_ab*H_ab, madHisto));
				madL = SQR(Mad(WavCoeffs_L[dir], W_L*H_L, madHisto));
				delete [] madHisto;
			}

		//	printf("  dir=%d  mad_L=%f	mad_a=%f	mad_b=%f  skip_ab=%i	\n",dir,sqrt(madL),sqrt(mada),sqrt(madb), skip_ab);

			float mad_L = madL*noisevar_L*5/(level+1);
			float mad_a = mada*noisevar_abr;  // noisevar_abr between 0..2.25=default 100=middle value  ==> 582=max
			float mad_b = madb*noisevar_abb;

			if (noisevar_abr>0.01f  ||  noisevar_abb>0.01f) {
				for(int i=0;i<H_ab;i++)
					for(int j=0;j<W_ab;j++)
						WavCoeffsLtemp[i*W_ab+j] = WavCoeffs_L[dir][((i*skip_L)/skip_ab)*W_L + ((j*skip_L)/skip_ab)];

#ifdef __SSE2__
				int j;
				__m128 onev = _mm_set1_ps(1.f);
				__m128 madLm9v = _mm_set1_ps(madL * 9.f);
				__m128 mad_av = _mm_set1_ps(mad_a);
				__m128 mad_bv = _mm_set1_ps(mad_b);
				__m128 epsv = _mm_set1_ps(eps);
				__m128 mag_Lv, mag_av, mag_bv;
				for (int i=0; i<H_ab; i++) {
					int coeffloc_ab = i*W_ab;
					for (j=0; j<W_ab-3; j+=4, coeffloc_ab+=4) {
						mag_Lv = LVFU(WavCoeffsLtemp[coeffloc_ab]);
						mag_av = SQRV(LVFU(WavCoeffs_a[dir][coeffloc_ab]))+epsv;
						mag_bv = SQRV(LVFU(WavCoeffs_b[dir][coeffloc_ab]))+epsv;
						mag_Lv = SQRV(mag_Lv) + epsv;
						_mm_storeu_ps(&sfavea[coeffloc_ab], (onev-xexpf(-(mag_av/mad_av)-(mag_Lv/madLm9v))));
						_mm_storeu_ps(&sfaveb[coeffloc_ab], (onev-xexpf(-(mag_bv/mad_bv)-(mag_Lv/madLm9v))));
					}
					for (; j<W_ab; j++,coeffloc_ab++) {
						float mag_L = SQR(WavCoeffsLtemp[coeffloc_ab])+eps;
						float mag_a = SQR(WavCoeffs_a[dir][coeffloc_ab])+eps;
						float mag_b = SQR(WavCoeffs_b[dir][coeffloc_ab])+eps;
						sfavea[coeffloc_ab] = (1.f-xexpf(-(mag_a/mad_a)-(mag_L/(9.f*madL))));
						sfaveb[coeffloc_ab] = (1.f-xexpf(-(mag_b/mad_b)-(mag_L/(9.f*madL))));
					}
				}//now chrominance coefficients are denoised
#else
				for (int i=0; i<H_ab; i++) {
					for (int j=0; j<W_ab; j++) {
						int coeffloc_ab = i*W_ab+j;
						int coeffloc_L	= ((i*skip_L)/skip_ab)*W_L + ((j*skip_L)/skip_ab);
						float mag_L = SQR(WavCoeffs_L[dir][coeffloc_L ])+eps;
						float mag_a = SQR(WavCoeffs_a[dir][coeffloc_ab])+eps;
						float mag_b = SQR(WavCoeffs_b[dir][coeffloc_ab])+eps;
						sfavea[coeffloc_ab] = (1.f-xexpf(-(mag_a/mad_a)-(mag_L/(9.f*madL))));
						sfaveb[coeffloc_ab] = (1.f-xexpf(-(mag_b/mad_b)-(mag_L/(9.f*madL))));
						// 'firm' threshold of chroma coefficients
						//WavCoeffs_a[dir][coeffloc_ab] *= (1-exp(-(mag_a/mad_a)-(mag_L/(9*madL))));//(coeff_a>2*thresh_a ? 1 : (coeff_a<thresh_a ? 0 : (coeff_a/thresh_a - 1)));
						//WavCoeffs_b[dir][coeffloc_ab] *= (1-exp(-(mag_b/mad_b)-(mag_L/(9*madL))));//(coeff_b>2*thresh_b ? 1 : (coeff_b<thresh_b ? 0 : (coeff_b/thresh_b - 1)));
					}
				}//now chrominance coefficients are denoised
#endif

				boxblur(sfavea, sfavea, level+2, level+2, W_ab, H_ab);//increase smoothness by locally averaging shrinkage
				boxblur(sfaveb, sfaveb, level+2, level+2, W_ab, H_ab);//increase smoothness by locally averaging shrinkage

#ifdef __SSE2__
				__m128 sfav, sfbv;
				__m128 sfaveav, sfavebv;
				for (int i=0; i<H_ab; i++) {
					int coeffloc_ab = i*W_ab;
					for (j=0; j<W_ab-3; j+=4,coeffloc_ab+=4) {
						mag_Lv = LVFU(WavCoeffsLtemp[coeffloc_ab]);
						mag_av = SQRV(LVFU(WavCoeffs_a[dir][coeffloc_ab]))+epsv;
						mag_bv = SQRV(LVFU(WavCoeffs_b[dir][coeffloc_ab]))+epsv;
						mag_Lv = SQRV(mag_Lv) + epsv;
						sfav = (onev - xexpf(-(mag_av/mad_av)-(mag_Lv/madLm9v)));
						sfbv = (onev - xexpf(-(mag_bv/mad_bv)-(mag_Lv/madLm9v)));
						sfaveav = LVFU(sfavea[coeffloc_ab]);
						sfavebv = LVFU(sfaveb[coeffloc_ab]);
						//use smoothed shrinkage unless local shrinkage is much less
						_mm_storeu_ps( &WavCoeffs_a[dir][coeffloc_ab], LVFU(WavCoeffs_a[dir][coeffloc_ab]) * (SQRV(sfaveav)+SQRV(sfav))/(sfaveav+sfav+epsv));
						_mm_storeu_ps( &WavCoeffs_b[dir][coeffloc_ab], LVFU(WavCoeffs_b[dir][coeffloc_ab]) * (SQRV(sfavebv)+SQRV(sfbv))/(sfavebv+sfbv+epsv));

					}//now chrominance coefficients are denoised
					for (; j<W_ab; j++,coeffloc_ab++) {
							//modification Jacques feb 2013
						float mag_L = SQR(WavCoeffsLtemp[coeffloc_ab])+eps;

						float mag_a = SQR(WavCoeffs_a[dir][coeffloc_ab])+eps;
						float mag_b = SQR(WavCoeffs_b[dir][coeffloc_ab])+eps;

						float sfa = (1.f-xexpf(-(mag_a/mad_a)-(mag_L/(9.f*madL))));
						float sfb = (1.f-xexpf(-(mag_b/mad_b)-(mag_L/(9.f*madL))));

						//use smoothed shrinkage unless local shrinkage is much less
						WavCoeffs_a[dir][coeffloc_ab] *= (SQR(sfavea[coeffloc_ab])+SQR(sfa))/(sfavea[coeffloc_ab]+sfa+eps);
						WavCoeffs_b[dir][coeffloc_ab] *= (SQR(sfaveb[coeffloc_ab])+SQR(sfb))/(sfaveb[coeffloc_ab]+sfb+eps);

					}//now chrominance coefficients are denoised
				}
#else
				for (int i=0; i<H_ab; i++) {
					for (int j=0; j<W_ab; j++) {

						int coeffloc_ab = i*W_ab+j;
//						int coeffloc_L	= ((i*skip_L)/skip_ab)*W_L + ((j*skip_L)/skip_ab);
						float mag_L = SQR(WavCoeffsLtemp[coeffloc_ab])+eps;

//						float mag_L = SQR(WavCoeffs_L[dir][coeffloc_L ])+eps;
						float mag_a = SQR(WavCoeffs_a[dir][coeffloc_ab])+eps;
						float mag_b = SQR(WavCoeffs_b[dir][coeffloc_ab])+eps;

						float sfa = (1.f-xexpf(-(mag_a/mad_a)-(mag_L/(9.f*madL))));
						float sfb = (1.f-xexpf(-(mag_b/mad_b)-(mag_L/(9.f*madL))));

						//use smoothed shrinkage unless local shrinkage is much less
						WavCoeffs_a[dir][coeffloc_ab] *= (SQR(sfavea[coeffloc_ab])+SQR(sfa))/(sfavea[coeffloc_ab]+sfa+eps);
						WavCoeffs_b[dir][coeffloc_ab] *= (SQR(sfaveb[coeffloc_ab])+SQR(sfb))/(sfaveb[coeffloc_ab]+sfb+eps);

					}//now chrominance coefficients are denoised
				}
#endif
			}

			if (noisevar_L>0.01f) {
#ifdef __SSE2__
				__m128	magv;
				__m128  mad_Lv = _mm_set1_ps( mad_L );
				__m128	ninev = _mm_set1_ps( 9.0f );
				__m128 	epsv = _mm_set1_ps( eps );
				for (int i=0; i<W_L*H_L-3; i+=4) {
					magv = SQRV(LVFU(WavCoeffs_L[dir][i]));
					_mm_storeu_ps( &sfave[i], magv / (magv + mad_Lv*xexpf(-magv/(ninev * mad_Lv)) + epsv));
				}
				for (int i=(W_L*H_L)-((W_L*H_L)%4); i<W_L*H_L; i++) {
					float mag = SQR(WavCoeffs_L[dir][i]);
					sfave[i] = mag/(mag+mad_L*xexpf(-mag/(9*mad_L))+eps);
				}
#else
				for (int i=0; i<W_L*H_L; i++) {

					float mag = SQR(WavCoeffs_L[dir][i]);
					float shrinkfactor = mag/(mag+mad_L*xexpf(-mag/(9*mad_L))+eps);

					//WavCoeffs_L[dir][i] *= shrinkfactor;
					sfave[i] = shrinkfactor;
				}
#endif
				boxblur(sfave, sfave, level+2, level+2, W_L, H_L);//increase smoothness by locally averaging shrinkage
#ifdef __SSE2__
				__m128	sfv;
				for (int i=0; i<W_L*H_L-3; i+=4) {
					magv = SQRV( LVFU(WavCoeffs_L[dir][i]));
					sfv = magv/(magv + mad_Lv * xexpf( -magv / (ninev * mad_Lv)) + epsv );
					//use smoothed shrinkage unless local shrinkage is much less
					_mm_storeu_ps( &WavCoeffs_L[dir][i], _mm_loadu_ps( &WavCoeffs_L[dir][i]) * (SQRV( LVFU(sfave[i] )) + SQRV(sfv)) / (LVFU(sfave[i])+sfv+epsv));
				}
				for (int i=(W_L*H_L)-((W_L*H_L)%4); i<W_L*H_L; i++) {
					float mag = SQR(WavCoeffs_L[dir][i]);
					float sf = mag/(mag+mad_L*xexpf(-mag/(9*mad_L))+eps);

					//use smoothed shrinkage unless local shrinkage is much less
					WavCoeffs_L[dir][i] *= (SQR(sfave[i])+SQR(sf))/(sfave[i]+sf+eps);
				}//now luminance coefficients are denoised

#else
				for (int i=0; i<W_L*H_L; i++) {
					float mag = SQR(WavCoeffs_L[dir][i]);
					float sf = mag/(mag+mad_L*xexpf(-mag/(9.f*mad_L))+eps);

					//use smoothed shrinkage unless local shrinkage is much less
					WavCoeffs_L[dir][i] *= (SQR(sfave[i])+SQR(sf))/(sfave[i]+sf+eps);

				}//now luminance coefficients are denoised
#endif
			}


		}
		delete[] sfave;
		delete[] sfavea;
		delete[] sfaveb;
		delete[] WavCoeffsLtemp;
	}



	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


}

