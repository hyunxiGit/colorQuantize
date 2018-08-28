#pragma once
#include <Magick++.h>
using namespace std;
using namespace Magick;

struct RGB
{
	RGB()
	{
		this->R = -1;
		this->G = -1;
		this->B = -1;
		this->d = -1;
	};
	RGB(short _r, short _g, short _b)
	{
		this->R = _r;
		this->G = _g;
		this->B = _b;
		this->d = -1;
	};

	short R;
	short G;
	short B;
	int d;
	short quadrant;

	bool operator == (RGB& value)
	{
		return(this->R == value.R && this->G == value.G && this->B == value.B);
	}

	RGB&  operator = (RGB& value)
	{
		this->R = value.R;
		this->G = value.G;
		this->B = value.B;
		this->d = value.d;
		this->quadrant = value.quadrant;
		return *this;
	}

	int length(RGB _c)
	{
		int _r = this->R - _c.R;
		int _g = this->G - _c.G;
		int _b = this->B - _c.B;
		d = pow(_r, 2) + pow(_g, 2) + pow(_b, 2);
		_r = _r > 0 ? 1 : 0;
		_g = _g > 0 ? 1 : 0;
		_b = _b > 0 ? 1 : 0;
		quadrant = (_r << 2) + (_g << 1) + _b;
		return d;
	}
};

ostream& operator<<(ostream& os, const RGB& dt);

typedef pair <RGB, int> RGBCount;

int drawPallete(vector<RGB> & myPalette, string myPath);

int findColor(Image myImg, vector <RGBCount> & rgbCountVect);