// quantizeImage.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "medium.h"
/////////////////new here
/* median.c -- Anton Kruger, Copyright (c) Truda Software, 215 Marengo Rd,
**             #2, Oxford, IA 52322-9383
** Description: Contains an implementation of Heckbert's median-cut color
**             quantization algorithm.
** Compilers:  MSC 5.1, 6.0.
** Note: 1) Compile in large memory model. 2) Delete "#define FAST_REMAP"
**       statement below in order to deactivate fast remapping.
*/

word myColor[HSIZE];
/////////////////new here
using namespace std;
using namespace Magick;


int q = 8;
const std ::string path = "C:/MyGit/colorQuantize/riverside_d.tga";



struct RGB
{
	RGB(short _r, short _g, short _b)
	{
		this->R = _r;
		this->G = _g;
		this->B = _b;
	};

	short R;
	short G;
	short B;

	bool operator == (RGB& value)
	{
		return(this->R == value.R && this->G == value.G && this->B == value.B);
	}

	RGB&  operator = (RGB& value)
	{
		this->R = value.R;
		this->G = value.G;
		this->B = value.B;
		return *this;
	}
};


ostream& operator<<(ostream& os, const RGB& dt)
{
	os << dt.R << '/' << dt.G << '/' << dt.B;
	return os;
}

typedef pair<RGB, short> RGBCount;


int drawPallete(std::vector<RGB> & myPalette , std::string myPath)
{
	
	int size = myPalette.size()*32;
	Geometry s = Geometry(size, 32);
	Image palette = Image(s, "white");
	std::vector<RGB>::iterator itr;
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

int mgickQuant(Image myImg , int target , std::vector<RGB> & myCols)
{
	//image range normally is 256
	//int range = pow(2, image.modulusDepth());
	int range = 256;
	Magick::Color c;
	std::vector<RGB>::iterator itr;
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

			RGB myRGB(r, g, b);
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


int findColor_w(Image myImg, vector<RGB> & myCols, vector<word> & myCol_w)
{
	//image range normally is 256
	//int range = pow(2, image.modulusDepth());
	int range = 256;
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

			int r = c.quantumRed()/ range;
			int g = c.quantumGreen()/ range;
			int b = c.quantumBlue()/ range;

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


int findColor(Image myImg, vector <RGBCount> & rgbCountVect)
{
	//image range normally is 256 , otherwise use :int range = pow(2, image.modulusDepth());
	int range = 256;
	Magick::Color c;

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

			RGB cRGB (r, g, b);

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

	std::cout << "find number :" << findNumber << std::endl;

	//for (itr = rgbCountVect.begin(); itr != rgbCountVect.end(); itr++)
	//{
	//	std::cout << "(" << itr->first.R << ", " << itr->first.G << ", " << itr->first.B << ")" \
	//		<< ":"<< itr->second <<std::endl;
	//}

	return findNumber;
}

int medianCut()
{
	std::vector<RGB> cols;
	std::vector<RGB> result;

	std::vector<word> col_w;

	word _his[HSIZE];
	for (int i = 0; i < HSIZE; i++)
	{
		_his[i] = (word)0;
	}
	int _f = findColor_w(path, cols, col_w);
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
	int c = MedianCut(_his, myColMap, myColor, 8);
	for (i = 0; i < c; i++)
	{
		RGB newCOL(myColMap[i][0] << 8, myColMap[i][1] << 8, myColMap[i][2] << 8);
		cout << (int)(myColMap[i][0]) << "," << (int)(myColMap[i][1]) << "," << (int)(myColMap[i][2]) << endl;
		result.push_back(newCOL);
	}
}

RGB findmidCol(vector <RGBCount> rgbCountVect)
{
	cout << "rgbCountVect.size :" << rgbCountVect.size() << endl;
	RGB result(-1,-1,-1);
	unsigned long r , g , b ;
	r = 0;
	g = 0;
	b = 0;
	unsigned int count = 0 ;
	vector <RGBCount>::iterator itr;
	for (itr = rgbCountVect.begin(); itr != rgbCountVect.end(); itr++)
	{
		r += itr->first.R * itr->second;
		g += itr->first.G * itr->second;
		b += itr->first.B * itr->second;
		count += itr->second;
	}
	if (count > 0)
	{
		RGB _t((short)(r / count), (short)(g / count), (short)(b / count));
		result = _t;
	}
	return result;
}

int main()
{	
	Image image;
	image.read(path);
	
	vector <RGBCount> rgbCountVect;
	findColor(image, rgbCountVect);

	RGB myMidRGB = findmidCol(rgbCountVect);
	cout<< "mid color :" << myMidRGB <<endl;

	//drawPallete(result, "D:/work/quantizeImage/palette.png");
    return 0;
}

