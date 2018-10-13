// quantizeImage.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "mediumCut.h"
#include "common.h"
#include "getExtremCol.h"
#include "octTree.h"



const std::string path = "C:/MyGit/colorQuantize/venice.jpg";
const std::string pathDDS2 = "C:/MyGit/colorQuantize/GalileoTomb2.png";
int main()
{	

	Image image;
	image.read(path);
	
	//extreme col
	//findImageExtream(image , 4);
	
	// oct tree
	//findImageOctTreem(image);

	//medium cut
	//medianCut(image);

	//write file
	//image.write(pathDDS2);

    return 0;
}

