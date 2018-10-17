#pragma once
#include "common.h"
int findExtrCol(vector <RGBCount> rgbCountVect, RGB midRGB, vector<RGB> & result);
RGB findmidCol(vector <RGBCount> rgbCountVect);
int findImageExtream(Image image, short q , string palletepath);