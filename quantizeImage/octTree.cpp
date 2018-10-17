#include "stdafx.h"
#include "common.h"
#include "octTree.h"

int octTreeQuant(Image myImg, int target, std::vector<RGB> & myCols)
{
	//image range normally is 256
	//int range = pow(2, image.modulusDepth());

	Magick::Color c;
	std::vector<RGB>::iterator itr;
	if (myImg.rows() > 512)
	{
		myImg.resize("512x512");
	}

	int h = std::min((int)(myImg.rows()), 512);
	int w = std::min((int)(myImg.columns()), 512);

	myImg.quantizeColors(target);
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

			RGB myRGB(r/range, g/range, b/range);
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

int findImageOctTreem(Image image , short q , string palletepath)
{
	vector<RGB> myCols;
	octTreeQuant(image, q, myCols);

	drawPallete(myCols, palletepath);

	return 0;
}
