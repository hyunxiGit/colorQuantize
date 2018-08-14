// quantizeImage.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include<Magick++.h>

using namespace std;
using namespace Magick;

int main()
{
	Image image;
	image.read("D:/work/quantizeImage/water.jpg");
	image.resize("1024x1024");
	image.quantizeColors(2);
	image.quantize(8);

	int range = pow(2, image.modulusDepth());

	Magick::Color c;

	c = image.pixelColor(0, 0);

	float b =(float) c.quantumBlue() / range;
	cout << b << endl;
	
	image.write("D:/work/quantizeImage/new.jpg");

	cout << "this is testing..." << endl;
    return 0;
}

