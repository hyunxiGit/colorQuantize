#include "stdafx.h"
#include "mediumCut.h"

word myColor[HSIZE];

int findColor_w(Image myImg, vector<RGB> & myCols, vector<word> & myCol_w)
{
	//image range normally is 256
	//int range = pow(2, image.modulusDepth());
	Magick::Color c;
	std::vector<RGB>::iterator itr;
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

			int r = c.quantumRed() / range;
			int g = c.quantumGreen() / range;
			int b = c.quantumBlue() / range;

			word _c = RGB_MEDIUM(r, g, b);
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

int medianCut(Image image)
{
	std::vector<RGB> cols;
	std::vector<RGB> result;

	std::vector<word> col_w;

	word _his[HSIZE];
	for (int i = 0; i < HSIZE; i++)
	{
		_his[i] = (word)0;
	}
	int _f = findColor_w(image, cols, col_w);
	std::vector<word>::iterator itr_w;
	int i = 0;
	for (itr_w = col_w.begin(); itr_w != col_w.end(); itr_w++)
	{
		_his[i] = 1;
		myColor[i] = *itr_w;
		i++;
	}

	//for (i = 0; i < _f; i++)
	//{
	//	std::cout << _his[i] << std::endl;
	//}

	byte myColMap[16][3];
	int c = MedianCut(_his, myColMap, myColor, 8);
	for (i = 0; i < c; i++)
	{
		RGB newCOL(myColMap[i][0], myColMap[i][1], myColMap[i][2]);
		cout << newCOL << endl;
		result.push_back(newCOL);
	}

	string p1 = "D:/work/quantizeImage/palette_medium.png";
	drawPallete(result, p1);

}
