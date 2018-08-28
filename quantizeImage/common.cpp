#pragma once
#include "stdafx.h"
#include "common.h"

int drawPallete(vector<RGB> & myPalette, string myPath)
{
	int size = myPalette.size() * 32;
	Geometry s = Geometry(size, 32);
	Image palette = Image(s, "white");
	vector<RGB>::iterator itr;
	int i = 0;
	for (itr = myPalette.begin(); itr != myPalette.end(); itr++)
	{
		//draw block
		palette.fillColor(Color(itr->R * range, itr->G * range, itr->B * range));
		palette.draw(DrawableRectangle(i * 32, 0, i * 32 + 31, 31));
		i++;
	}

	palette.write(myPath);
	return 1;
}

ostream& operator<<(ostream& os, const RGB& dt)
{
	os << "(" << dt.R << '/' << dt.G << '/' << dt.B << ")," << "( q =" << dt.quadrant << ", d = " << dt.d << ")";
	return os;
}

int findColor(Image myImg, vector <RGBCount> & rgbCountVect)
{
	int countThreshold = 3;
	//image range normally is 256 , otherwise use :int range = pow(2, image.modulusDepth());
	Magick::Color c;
	vector <RGBCount> temp;
	vector <RGBCount>::iterator itr;
	if (myImg.rows() > 512)
	{
		myImg.resize("512x512");
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

			short r = c.quantumRed() / range;
			short g = c.quantumGreen() / range;
			short b = c.quantumBlue() / range;


			if ((r < 5 && g < 5 && b < 5) || (r > 250 && g > 250 && b > 250)) {}
			else
			{
				RGB cRGB(r, g, b);
				for (itr = rgbCountVect.begin(); itr != rgbCountVect.end(); itr++)
				{

					if ((RGB)itr->first == cRGB)
					{
						find = true;
						itr->second++;
						//cout << " struct find"<<endl;
						break;
					}
				}

				if (!find)
				{
					rgbCountVect.push_back(RGBCount(cRGB, 1));
					findNumber++;
					//cout << "find :" << findNumber << endl;
				}
			}
		}
	}

	return findNumber;
}