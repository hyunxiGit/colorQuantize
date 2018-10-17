#pragma once

#include "medium.h"
#include <Magick++.h>
#include <vector>
#include "common.h"

int findColor_w(Magick::Image myImg, vector<RGB> & myCols, vector<word> & myCol_w);
int medianCut(Image image, short q , string palletepath);