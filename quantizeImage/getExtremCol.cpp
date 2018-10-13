#pragma once
#include "stdafx.h"
#include "common.h"
#include "getExtremCol.h"

int findExtrCol(vector <RGBCount> rgbCountVect, RGB midRGB, vector<RGB> & result)
{
	vector <RGBCount> ::iterator itr;

	RGB extrCol[8];
	for (itr = rgbCountVect.begin(); itr != rgbCountVect.end(); itr++)
	{
		int d = itr->first.length(midRGB);
		int q = itr->first.quadrant;
		switch (q)
		{
		case 0:
			if (d > extrCol[0].d)
			{
				//cout << "greater :" << endl;
				extrCol[0] = itr->first;
				//cout << itr->first << endl;
				//cout << extrCol[0] << endl;
			}
			break;
		case 1:
			//cout  << itr->first << endl;
			if (d > extrCol[1].d)
			{
				//cout << "greater :" << endl;
				extrCol[1] = itr->first;
				//cout << itr->first << endl;
				//cout << extrCol[1] << endl;
			}
			break;
		case 2:
			if (d > extrCol[2].d)
			{
				//cout << "greater :" << endl;
				extrCol[2] = itr->first;
				//cout << itr->first << endl;
				//cout << extrCol[2] << endl;
			}
			break;
		case 3:
			if (d > extrCol[3].d)
			{
				//cout << "greater :" << endl;
				extrCol[3] = itr->first;
				//cout << itr->first << endl;
				//cout << extrCol[3] << endl;
			}
			break;
		case 4:
			if (d > extrCol[4].d)
			{
				//cout << "greater :" << endl;
				extrCol[4] = itr->first;
				//cout << itr->first << endl;
				//cout << extrCol[4] << endl;
			}
			break;
		case 5:
			if (d > extrCol[5].d)
			{
				//cout << "greater :" << endl;
				extrCol[5] = itr->first;
				//cout << itr->first << endl;
				//cout << extrCol[5] << endl;
			}
			break;
		case 6:
			if (d > extrCol[6].d)
			{
				//cout << "greater :" << endl;
				extrCol[6] = itr->first;
				//cout << itr->first << endl;
				//cout << extrCol[6] << endl;
			}
			break;
		case 7:
			if (d > extrCol[7].d)
			{
				//cout << "greater :" << endl;
				extrCol[7] = itr->first;
				//cout << itr->first << endl;
				//cout << extrCol[7] << endl;
			}
			break;
		}
	}
	for (int i = 0; i < 8; i++)
	{
		result.push_back(extrCol[i]);
	}
	return 0;
}

RGB findmidCol(vector <RGBCount> rgbCountVect)
{
	cout << "rgbCountVect.size :" << rgbCountVect.size() << endl;
	RGB result(-1, -1, -1);
	unsigned long r, g, b;
	r = 0;
	g = 0;
	b = 0;
	unsigned int count = 0;
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

int findImageExtream(Image image, short q)
{
	vector <RGBCount> rgbCountVect;
	findColor(image, rgbCountVect);

	RGB myMidRGB = findmidCol(rgbCountVect);
	std::cout << "mid color :" << myMidRGB << endl;

	vector<RGB> extrColVec;

	findExtrCol(rgbCountVect, myMidRGB, extrColVec);
	for (int i = 0; i < 8; i++)
	{
		cout << extrColVec.at(i) << endl;
	}

	string p1 = "C:/MyGit/colorQuantize/GalileoTomb2.png";
	drawPallete(extrColVec, p1);
}