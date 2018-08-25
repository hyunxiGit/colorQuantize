#include "stdafx.h"
#include "medium.h"
word MedianCut(word Hist[], byte ColMap[][3], word myColor[] ,int maxcubes)
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
	Shrink(&Cube, myColor);
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
		Shrink(&CubeA, myColor);
		list[splitpos] = CubeA;               /* add in old slot */

		CubeB = Cube; CubeB.lower = median;
		CubeB.count = Cube.count - count;
		CubeB.level = Cube.level + 1;
		Shrink(&CubeB, myColor);
		list[ncubes++] = CubeB;               /* add in new slot */
		if ((ncubes % 10) == 0)
			fprintf(stderr, ".");               /* pacifier        */
	}
	std::cout << "enough cub" << std::endl;
	/* We have enough cubes, or we have split all we can. Now compute the color
	** map, inverse color map, and return number of colors in color map. */
	InvMap(Hist, ColMap, ncubes, myColor);
	return((word)ncubes);
}
void Shrink(cube_t * Cube, word myColor[])
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
		r = RED_MEDIUM(color);
		if (r > Cube->rmax) Cube->rmax = r;
		if (r < Cube->rmin) Cube->rmin = r;
		g = GREEN_MEDIUM(color);
		if (g > Cube->gmax) Cube->gmax = g;
		if (g < Cube->gmin) Cube->gmin = g;
		b = BLUE_MEDIUM(color);
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
void InvMap(word * Hist, byte ColMap[][3], word ncubes, word myColor[])
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

			r = RED_MEDIUM(color);
			rsum += (float)r;//*(float)Hist[color];
			g = GREEN_MEDIUM(color);
			gsum += (float)g;//*(float)Hist[color];
			b = BLUE_MEDIUM(color);
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
		std::cout << std::endl;
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
		C1 = RED_MEDIUM(color1), C2 = RED_MEDIUM(color2);
		break;
	case 1:
		C1 = GREEN_MEDIUM(color1), C2 = GREEN_MEDIUM(color2);
		break;
	case 2:
		C1 = BLUE_MEDIUM(color2), C2 = BLUE_MEDIUM(color2);
		break;
	}
	return ((int)(C1 - C2));
}
