// quantizeImage.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"


using namespace std;
using namespace Magick;


int q = 4;

const string path = "D:/work/quantizeImage/blendBase.jpg";

struct RGB
{
	RGB(float _r, float _g, float _b)
	{
		this->R = _r;
		this->G = _g;
		this->B = _b;
	};

	int R;
	int G;
	int B;
};

int main()
{	
	Image image;
	image.read(path);
	
	//image range normally is 256
	//int range = pow(2, image.modulusDepth());
	int range = 256 ;

	if (image.rows() > 512)
	{
		image.resize("512x512");
	}

	int h = min((int)(image.rows()), 512);
	int w = min((int)(image.columns()), 512);


	image.quantizeColors(4);
	image.quantize();

	Magick::Color c;
	vector<RGB> cols;
	vector<RGB>::iterator itr;

	int findNumber = 0;
	for (int row = 0; row < w; row++)
	{
		for (int column = 0; column < h ; column++)
		{
			bool find = false;
			c = image.pixelColor(row , column);
			int r = c.quantumRed();
			int g = c.quantumGreen();
			int b = c.quantumBlue();

			RGB myRGB(r, g , b);
			
			for (itr = cols.begin(); itr != cols.end(); itr++)
			{
				if (myRGB.R == itr->R && myRGB.G == itr-> G && myRGB.B == itr-> B)
				{
					find = true;
					break;
				}
			}

			if (!find)
			{
				cols.push_back(myRGB);
				findNumber++;
			}
		}
	}

	cout << "find number :" << findNumber << endl;

	for (itr = cols.begin(); itr != cols.end(); itr++)
	{
		cout << "(" << itr->R/255 << ", " << itr->G/255  << ", " << itr->B/255 << ")" << endl;
	}
	
	Image palette = Image("128x32", "white");
	
	int i = 0;
	for (itr = cols.begin(); itr != cols.end(); itr++)
	{
		
		//draw block
		palette.fillColor(Color(itr->R, itr->G, itr->B));
		palette.draw(DrawableRectangle(i*32, 0, i * 32 + 31, 31));

		//might not need export the value
		//itr->R /= range;
		//itr->G /= range;
		//itr->B /= range;
		//cout << "(" << itr->R << ", "<< itr->G << ", " << itr->B << ")" << endl;
		i++;
	}
	
	palette.write("D:/work/quantizeImage/new.png");

    return 0;
}

