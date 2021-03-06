// quantizeImage.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "mediumCut.h"
#include "common.h"
#include "getExtremCol.h"
#include "octTree.h"

//this pro

const std::string path = "C:/MyGit/tempTexture/reverbank_do.bmp";
const std::string palletepath = "C:/MyGit/tempTexture/reverbank_do.bmp";

int main()
{	

	Image image;
	image.read("C:/MyGit/tempTexture/reverbank_do.tga");
	
	//extreme col
	//findImageExtream(image , 4, palletepath);
	
	// oct tree
	//findImageOctTreem(image, 4, palletepath);

	//medium cut
	medianCut(image, 4, palletepath);

    return 0;
}

