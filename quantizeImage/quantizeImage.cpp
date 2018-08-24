// quantizeImage.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

/////////////////new here
/* median.c -- Anton Kruger, Copyright (c) Truda Software, 215 Marengo Rd,
**             #2, Oxford, IA 52322-9383
** Description: Contains an implementation of Heckbert's median-cut color
**             quantization algorithm.
** Compilers:  MSC 5.1, 6.0.
** Note: 1) Compile in large memory model. 2) Delete "#define FAST_REMAP"
**       statement below in order to deactivate fast remapping.
*/
#define  FAST_REMAP
#include <stdio.h>
#include <stddef.h>              /* for NULL                   */
#include <stdlib.h>              /* for "qsort"                */
#include <float.h>               /* for FLT_MAX, FLT_MIN       */
#define MAXCOLORS 256            /* maximum # of output colors */
#define HSIZE     32768          /* size of image histogram    */
typedef unsigned  char byte;     /* range 0-255                */
typedef unsigned  short word;    /* range 0-65,535             */
typedef unsigned  long dword;    /* range 0-4,294,967,295      */

/* Macros for converting between (r,g,b)-colors and 15-bit     */
/* colors follow.                                              */
#define RGB(r,g,b) (word)(((b)&~7)<<7)|(((g)&~7)<<2)|((r)>>3)
#define RED(x)     (byte)(((x)&31)<<3)
#define GREEN(x)   (byte)((((x)>>5)&255)<< 3)
#define BLUE(x)    (byte)((((x)>>10)&255)<< 3)

typedef  struct {       /* structure for a cube in color space */
	word  lower;         /* one corner's index in histogram     */
	word  upper;         /* another corner's index in histogram */
	dword count;         /* cube's histogram count              */
	int   level;         /* cube's level                        */

	byte  rmin, rmax;
	byte  gmin, gmax;
	byte  bmin, bmax;
} cube_t;

static cube_t list[MAXCOLORS];   /* list of cubes              */
static int longdim;              /* longest dimension of cube  */
static word HistPtr[HSIZE];      /* points to colors in "Hist" */

word myColor[HSIZE];

void Shrink(cube_t * Cube);
void InvMap(word * Hist, byte ColMap[][3], word ncubes);
int  compare(const void * a1, const void * a2);

word MedianCut(word Hist[], byte ColMap[][3], int maxcubes)
{
	/* Accepts "Hist", a 32,768-element array that contains 15-bit color counts
	** of input image. Uses Heckbert's median-cut algorithm to divide color
	** space into "maxcubes" cubes, and returns centroid (average value) of each
	** cube in ColMap. Hist is also updated so that it functions as an inverse
	** color map. MedianCut returns the actual number of cubes, which may be
	** less than "maxcubes". */
	byte        lr, lg, lb;
	word        i, median, color;
	dword       count;
	int         k, level, ncubes, splitpos;
	void        *base;
	size_t      num, width;
	cube_t      Cube, CubeA, CubeB;

	/* Create the initial cube, which is the whole RGB-cube. */
	ncubes = 0;
	Cube.count = 0;
	for (i = 0, color = 0; i <= HSIZE - 1; i++) {
		if (Hist[i] != 0) {
			HistPtr[color++] = i;
			Cube.count = Cube.count + Hist[i];
		}
	}
	Cube.lower = 0; Cube.upper = color - 1;
	Cube.level = 0;
	Shrink(&Cube);
	list[ncubes++] = Cube;

	/* Main loop follows. Search the list of cubes for next cube to split, which
	** is the lowest level cube. A special case is when a cube has only one
	** color, so that it cannot be split. */
	while (ncubes < maxcubes) {
		level = 255; splitpos = -1;
		for (k = 0; k <= ncubes - 1; k++) {
			if (list[k].lower == list[k].upper)
				;                            /* single color */
			else if (list[k].level < level) {
				level = list[k].level;
				splitpos = k;
			}
		}
		if (splitpos == -1)            /* no more cubes to split */
			break;

		/* Must split the cube "splitpos" in list of cubes. Next, find longest
		** dimension of cube, and update external variable "longdim" which is
		** used by sort routine so that it knows along which axis to sort. */
		Cube = list[splitpos];
		lr = Cube.rmax - Cube.rmin;
		lg = Cube.gmax - Cube.gmin;
		lb = Cube.bmax - Cube.bmin;
		if (lr >= lg && lr >= lb) longdim = 0;
		if (lg >= lr && lg >= lb) longdim = 1;
		if (lb >= lr && lb >= lg) longdim = 2;

		//std::cout << "longdim : " << longdim << std::endl;
	

		/* Sort along "longdim". This prepares for the next step, namely finding
		** median. Use standard lib's "qsort". */
		base = (void *)&HistPtr[Cube.lower];
		num = (size_t)(Cube.upper - Cube.lower + 1);
		width = (size_t)sizeof(HistPtr[0]);
		qsort(base, num, width, compare);

		/* Find median by scanning through cube, computing a running sum. When
		** running sum equals half the total for cube, median has been found. */
		count = 0;
		for (i = Cube.lower; i <= Cube.upper - 1; i++) {
			if (count >= Cube.count / 2) break;
			color = HistPtr[i];
			count = count + Hist[color];
		}
		median = i;


		/* Now split "Cube" at median. Then add two new cubes to list of cubes.*/
		CubeA = Cube; CubeA.upper = median - 1;
		CubeA.count = count;
		CubeA.level = Cube.level + 1;
		Shrink(&CubeA);
		list[splitpos] = CubeA;               /* add in old slot */

		CubeB = Cube; CubeB.lower = median;
		CubeB.count = Cube.count - count;
		CubeB.level = Cube.level + 1;
		Shrink(&CubeB);
		list[ncubes++] = CubeB;               /* add in new slot */
		if ((ncubes % 10) == 0)
			fprintf(stderr, ".");               /* pacifier        */
	}
	std::cout << "enough cub" << std::endl;
	/* We have enough cubes, or we have split all we can. Now compute the color
	** map, inverse color map, and return number of colors in color map. */
	InvMap(Hist, ColMap, ncubes);
	return((word)ncubes);
}
void Shrink(cube_t * Cube)
{
	/* Encloses "Cube" with a tight-fitting cube by updating (rmin,gmin,bmin)
	** and (rmax,gmax,bmax) members of "Cube". */
	byte        r, g, b;
	word        i, color;

	Cube->rmin = 255; Cube->rmax = 0;
	Cube->gmin = 255; Cube->gmax = 0;
	Cube->bmin = 255; Cube->bmax = 0;
	for (i = Cube->lower; i <= Cube->upper; i++) {
		color = myColor[HistPtr[i]];
		r = RED(color);
		if (r > Cube->rmax) Cube->rmax = r;
		if (r < Cube->rmin) Cube->rmin = r;
		g = GREEN(color);
		if (g > Cube->gmax) Cube->gmax = g;
		if (g < Cube->gmin) Cube->gmin = g;
		b = BLUE(color);
		if (b > Cube->bmax) Cube->bmax = b;
		if (b < Cube->bmin) Cube->bmin = b;

		//std::cout << "rgb : " << color <<" :"<< (int)r << "," << (int)g << "," << (int)b << std::endl;
		
	}
	//std::cout << "rgb : " << color  << std::endl;
	//std::cout << "rmax : " << (int)Cube->bmax << std::endl;
	//std::cout << "rmin : " << (int)Cube->rmin << std::endl;
	//std::cout << "gmax : " << (int)Cube->gmax << std::endl;
	//std::cout << "gmin : " << (int)Cube->gmin << std::endl;
	//std::cout << "bmax : " << (int)Cube->bmax << std::endl;
	//std::cout << "bmin : " << (int)Cube->bmin << std::endl;
}
void InvMap(word * Hist, byte ColMap[][3], word ncubes)
{
	/* For each cube in list of cubes, computes centroid (average value) of
	** colors enclosed by that cube, and loads centroids in the color map. Next
	** loads histogram with indices into the color map. A preprocessor directive
	** #define FAST_REMAP controls whether cube centroids become output color
	** for all the colors in a cube, or whether a "best remap" is followed. */
	byte        r, g, b;
	word        i, j, k, index, color;
	float       rsum, gsum, bsum;
	float       dr, dg, db, d, dmin;
	cube_t      Cube;

	for (k = 0; k <= ncubes - 1; k++) {
		Cube = list[k];
		rsum = gsum = bsum = (float)0.0;
		for (i = Cube.lower; i <= Cube.upper; i++) {
			color = myColor[HistPtr[i]];
			//std::cout << "k : " << k << ", "<< "color : " << (int)color <<std::endl;
			
			r = RED(color);
			rsum += (float)r;//*(float)Hist[color];
			g = GREEN(color);
			gsum += (float)g;//*(float)Hist[color];
			b = BLUE(color);
			bsum += (float)b;// *(float)Hist[color];

			//std::cout << "( " << (int)r << ", " << (int)g << "," << (int)b << ")" << std::endl;
		}

		/* Update the color map */
		ColMap[k][0] = (byte)(rsum / (float)Cube.count);
		ColMap[k][1] = (byte)(gsum / (float)Cube.count);
		ColMap[k][2] = (byte)(bsum / (float)Cube.count);
		std::cout << "r : " << rsum / (float)Cube.count << std::endl;
		std::cout << "g : " << gsum / (float)Cube.count << std::endl;
		std::cout << "b : " << bsum / (float)Cube.count << std::endl;
		std::cout  << std::endl;
		//std::cout << "( " << (int)r << ", " << (int)g << "," << (int)b << ")" << std::endl;
		//std::cout << "gmin : " << (int)Cube->gmin << std::endl;
		//std::cout << "bmax : " << (int)Cube->bmax << std::endl;
		//std::cout << "bmin : " << (int)Cube->bmin << std::endl;
	}
//
//#ifdef FAST_REMAP
//	/* Fast remap: for each color in each cube, load the corresponding slot
//	** in "Hist" with the centroid of the cube. */
//	for (k = 0; k <= ncubes - 1; k++) {
//		Cube = list[k];
//		for (i = Cube.lower; i <= Cube.upper; i++) {
//			color = HistPtr[i];
//			Hist[color] = k;
//		}
//
//		if ((k % 10) == 0) fprintf(stderr, ".");   /* pacifier    */
//	}
//#else
//	/* Best remap: for each color in each cube, find entry in ColMap that has
//	** smallest Euclidian distance from color. Record this in "Hist". */
//	for (k = 0; k <= ncubes - 1; k++) {
//		Cube = list[k];
//		for (i = Cube.lower; i <= Cube.upper; i++) {
//			color = HistPtr[i];
//			r = RED(color);  g = GREEN(color); b = BLUE(color);
//
//			/* Search for closest entry in "ColMap" */
//			dmin = (float)FLT_MAX;
//			for (j = 0; j <= ncubes - 1; j++) {
//				dr = (float)ColMap[j][0] - (float)r;
//				dg = (float)ColMap[j][1] - (float)g;
//				db = (float)ColMap[j][2] - (float)b;
//				d = dr * dr + dg * dg + db * db;
//				if (d == (float)0.0) {
//					index = j; break;
//				}
//				else if (d < dmin) {
//					dmin = d; index = j;
//				}
//			}
//			Hist[color] = index;
//		}
//		if ((k % 10) == 0) fprintf(stderr, ".");   /* pacifier    */
//	}
//#endif
	return;
}
int compare(const void * a1, const void * a2)
{
	/* Called by the sort routine in "MedianCut". Compares two
	** colors based on the external variable "longdim". */
	word        color1, color2;
	byte        C1, C2;

	color1 = (word)*(word *)a1;
	color2 = (word)*(word *)a2;
	switch (longdim) {

	case 0:
		C1 = RED(color1), C2 = RED(color2);
		break;
	case 1:
		C1 = GREEN(color1), C2 = GREEN(color2);
		break;
	case 2:
		C1 = BLUE(color2), C2 = BLUE(color2);
		break;
	}
	return ((int)(C1 - C2));
}


/////////////////new here
//using namespace std;
using namespace Magick;


int q = 8;
const std::string texturePath = "D:/work/quantizeImage/riverside_d.tga";
const std ::string path = "D:/work/quantizeImage/blendBase.tga";

struct RGB_M
{
	RGB_M(float _r, float _g, float _b)
	{
		this->R = _r;
		this->G = _g;
		this->B = _b;
	};

	int R;
	int G;
	int B;
};




int drawPallete(std::vector<RGB_M> & myPalette , std::string myPath)
{
	
	int size = myPalette.size()*32;
	Geometry s = Geometry(size, 32);
	Image palette = Image(s, "white");
	std::vector<RGB_M>::iterator itr;
	int i = 0;
	for (itr = myPalette.begin(); itr != myPalette.end(); itr++)
	{

		//draw block
		palette.fillColor(Color(itr->R, itr->G, itr->B));
		palette.draw(DrawableRectangle(i * 32, 0, i * 32 + 31, 31));

		//might not need export the value
		//itr->R /= range;
		//itr->G /= range;
		//itr->B /= range;
		//cout << "(" << itr->R << ", "<< itr->G << ", " << itr->B << ")" << endl;

		i++;
	}

	palette.write(myPath);
	return 1;
}

int quant1(Image myImg , int target , std::vector<RGB_M> & myCols)
{
	//image range normally is 256
	//int range = pow(2, image.modulusDepth());
	int range = 256;
	Magick::Color c;
	std::vector<RGB_M>::iterator itr;
	if (myImg.rows() > 512)
	{
		myImg.resize("512x512");
	}

	int h = std::min((int)(myImg.rows()), 512);
	int w = std::min((int)(myImg.columns()), 512);
	
	myImg.quantizeColors(q);
	myImg.quantize();

	int findNumber = 0;
	for (int row = 0; row < w; row++)
	{
		for (int column = 0; column < h; column++)
		{
			bool find = false;
			c = myImg.pixelColor(row, column);
			int r = c.quantumRed();
			int g = c.quantumGreen();
			int b = c.quantumBlue();

			RGB_M myRGB(r, g, b);
			//cout << "(" << (float)(r) / 255  << ", " << (float)(g) / 255 << ", " << (float)(b) / 255  << ")" << endl;
			for (itr = myCols.begin(); itr != myCols.end(); itr++)
			{
				if (myRGB.R == itr->R && myRGB.G == itr->G && myRGB.B == itr->B)
				{
					find = true;
					break;
				}
			}

			if (!find)
			{
				myCols.push_back(myRGB);
				findNumber++;
			}
		}
	}

	std::cout << "find number :" << findNumber << std::endl;

	for (itr = myCols.begin(); itr != myCols.end(); itr++)
	{
		std::cout << "(" << (float)(itr->R) / 255 / 255 << ", " << (float)(itr->G) / 255 / 255 << ", " << (float)(itr->B) / 255 / 255 << ")" << std::endl;
	}

	return 1;
}

int findColor(Image myImg, std::vector<RGB_M> & myCols, std::vector<word> & myCol_w)
{
	//image range normally is 256
	//int range = pow(2, image.modulusDepth());
	int range = 256;
	Magick::Color c;
	std::vector<RGB_M>::iterator itr;
	std::vector<word>::iterator itr_w;
	if (myImg.rows() > 512)
	{
		myImg.resize("256x256");
	}

	int h = std::min((int)(myImg.rows()), 512);
	int w = std::min((int)(myImg.columns()), 512);

	int findNumber = 0;
	for (int row = 0; row < w; row++)
	{
		for (int column = 0; column < h; column++)
		{
			bool find = false;
			c = myImg.pixelColor(row, column);

			int r = c.quantumRed()/ range;
			int g = c.quantumGreen()/ range;
			int b = c.quantumBlue()/ range;

			word _c = RGB(r, g, b);
			for (itr_w = myCol_w.begin(); itr_w != myCol_w.end(); itr_w++)
			{
				if (*itr_w == _c)
				{
					find = true;
					break;
				}
			}

			if (!find)
			{
				myCol_w.push_back(_c);
				findNumber++;
			}

			//RGB_M myRGB(r, g, b);
			/*for (itr = myCols.begin(); itr != myCols.end(); itr++)
			{
				if (myRGB.R == itr->R && myRGB.G == itr->G && myRGB.B == itr->B)
				{
					find = true;
					break;
				}
			}
*/
			/*if (!find)
			{
				myCols.push_back(myRGB);
				findNumber++;
			}*/
		}
	}

	std::cout << "find number :" << findNumber << std::endl;

	//for (itr = myCols.begin(); itr != myCols.end(); itr++)
	//{
	//	std::cout << "(" << (float)(itr->R) / 255 / 255 << ", " << (float)(itr->G) / 255 / 255 << ", " << (float)(itr->B) / 255 / 255 << ")" << std::endl;
	//}

	return findNumber;
}

int main()
{	
	Image image;
	image.read(path);
	

	std::vector<RGB_M> cols;
	std::vector<RGB_M> result;
	std::vector<word> col_w;
	word _his[HSIZE];
	for (int i = 0; i < HSIZE; i++)
	{
		_his[i] = (word)0;
	}
	int _f = findColor(texturePath, cols , col_w);
	std::vector<word>::iterator itr_w;
	int i = 0;
	for (itr_w = col_w.begin(); itr_w != col_w.end(); itr_w++)
	{
		_his[i] = 1;
		myColor[i] = *itr_w;
		i++;
	}

	for (i = 0; i < _f; i++)
	{
		std::cout << _his[i] << std::endl;
	}
	//quant1(image , q , cols);
	//std::string palletPath = "D:/work/quantizeImage/new.png";
	//drawPallete(cols, palletPath);
	//image.write("D:/work/quantizeImage/quantilize.png");

	byte myColMap[16][3];
	int c = MedianCut(_his, myColMap, 8);
	for (i = 0; i < c; i++)
	{
		RGB_M newCOL(myColMap[i][0] << 8, myColMap[i][1] << 8, myColMap[i][2] << 8);
		std::cout << (int)(myColMap[i][0]) <<"," <<(int)(myColMap[i][1]) << "," << (int)(myColMap[i][2]) << std :: endl;
		result.push_back(newCOL);
	}

	drawPallete(result, "D:/work/quantizeImage/palette.png");
    return 0;
}

